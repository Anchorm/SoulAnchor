/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#include "parwindow.h"

ParWindow::ParWindow(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setWindowTitle("Parallel Window - SoulAnchor");
    setObjectName("ParallelWindow");
    setWindowIcon(anchorIcon);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    bookName = "";

    int maxH = 28;
    int minW = 50;

    cb_select->setMaximumHeight(maxH);
    cb_select->setMinimumWidth(minW);
    cb_select->setMaximumSize(130, maxH);

    leInput->setPlaceholderText("bk ch:vs");
    leInput->setMinimumWidth(minW);
    leInput->setMaximumSize(170, maxH);
    leInput->setTextMargins(2, 0, 0, 0);

    btn_ok->setText(tr("Go"));
    btn_ok->setMaximumSize(70, maxH);
    btn_ok->setMinimumWidth(30);
    btn_ok->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    btn_next->setText(tr("next"));
    btn_next->setMaximumSize(100, maxH);
    btn_next->setMinimumWidth(minW);

    btn_prev->setText(tr("previous"));
    btn_prev->setMaximumSize(100, maxH);
    btn_prev->setMinimumWidth(minW);

    flowLayout->setContentsMargins(3,0,0,0);

    hbox_2->setSpacing(5);
    hbox_2->setAlignment(Qt::AlignLeft);
    hbox_2->setContentsMargins(3,0,0,0);

    vbox->setContentsMargins(3,3,3,3);

    cb_select->addItem("none");

    QString getDisLang = "SELECT DISTINCT language FROM version_info ORDER BY language;";
    QSqlQuery getL(getDisLang, dbH.bibleDb);

    while (getL.next()) {
        cb_select->addItem(getL.value(0).toString());
    }

    cb_select->addItem("all");

    connect(cb_select, &QComboBox::currentTextChanged, this, &ParWindow::checkTls);

    hbox_2->addWidget(cb_select);
    hbox_2->addWidget(leInput);
    hbox_2->addWidget(btn_ok);
    hbox_2->addWidget(btn_prev);
    hbox_2->addWidget(btn_next);

    //create checkboxes for translations
    QString sqlCb = "SELECT abbreviation, language, version "
                    "FROM version_info ORDER BY abbreviation ASC";
    QSqlQuery getCb(sqlCb, dbH.bibleDb);
    QString abbr;
    QString lang;
    QString desc;

    while ( getCb.next() ) {
        abbr = getCb.value(0).toString().toUpper();
        lang = getCb.value(1).toString();
        desc = getCb.value(2).toString();

        QCheckBox *chkB = new QCheckBox(abbr);
        chkB->setToolTip(desc);
        chkB->setProperty("langName", lang);
        chkBoxes.append(chkB);
    }

    for (QCheckBox *chkB: qAsConst(chkBoxes)) {
        flowLayout->addWidget(chkB);
    }

    te->setReadOnly(true);
    te->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(te, &QTextEdit::customContextMenuRequested, this, &ParWindow::ccMenuParW);
    connect(leInput, &QLineEdit::returnPressed, this, &ParWindow::printRequest);
    connect(btn_ok, &QPushButton::clicked, this, [this] () {
        te->clear();
        printRequest();
    });

    new QShortcut(Qt::EnterKeyReturn, this, SLOT(printScriptures()));

    connect(btn_next, &QPushButton::clicked, this, &ParWindow::nextChapter);   
    new QShortcut(Qt::Key_Plus, this, SLOT(nextChapter()));
    new QShortcut(Qt::Key_Right, this, SLOT(nextChapter()));
    connect(btn_prev, &QPushButton::clicked, this, &ParWindow::prevChapter);
    new QShortcut(Qt::Key_Minus, this, SLOT(prevChapter()));
    new QShortcut(Qt::Key_Left, this, SLOT(prevChapter()));

    new QShortcut(Qt::Key_F11, this, [this] () {
        isFullScreen() ? showNormal() : showFullScreen(); });
    new QShortcut(Qt::Key_Escape, this, [this](){
        if ( isFullScreen() ) showNormal(); });

    vbox->addItem(flowLayout);
    vbox->addItem(hbox_2);
    vbox->addWidget(te);
    setLayout(vbox);

    createOtNtMenus();
}

void ParWindow::setStyle(const QHash<QString, QString> &clrScheme)
{
    scheme = clrScheme;
    QString css = "background-color:" + clrScheme["bgClr"];
    te->setStyleSheet(css);
}

void ParWindow::setTlandJob(const QString &tlAbbr, const QHash<QString, int> &job)
{
    bool tlChecked = false;

    for (QCheckBox *cb: qAsConst(chkBoxes)) {
        if (cb->isChecked()) {
            tlChecked = true;
            break;
        }
    }

    if (!tlChecked) {
        for (QCheckBox *cb: qAsConst(chkBoxes)) {
            if (cb->text() == tlAbbr) {
                cb->setCheckState(Qt::Checked);
            } else {
                cb->setCheckState(Qt::Unchecked);
            }
        }
    }

    te->clear();
    history.clear();
    history.enqueue(job);
    centerWindow();
}

void ParWindow::nextChapter()
{
    if (lastBookNumber == 0 || lastChapNumber == 0) {
        return;
    } else {
        int finChap = dbH.getChapterCount(lastBookNumber);
        int chapter = lastChapNumber + 1;

        if (chapter <= finChap ) {
            QHash<QString, int> job;
            job["bk"] = lastBookNumber;
            job["c1"] = chapter;
            printQ.enqueue(job);
            history.clear();
            history.enqueue(job);
            te->clear();
            printScriptures();
        }
    }
}

void ParWindow::prevChapter()
{
    if (lastBookNumber == 0 || lastChapNumber == 0) {
        return;
    } else {
        int chapter = lastChapNumber - 1;

        if (chapter >= 1) {
            QHash<QString, int> job;
            job["bk"] = lastBookNumber;
            job["c1"] = chapter;
            printQ.enqueue(job);
            history.clear();
            history.enqueue(job);
            te->clear();
            printScriptures();
        }
    }
}

void ParWindow::popupChapters(int bkNr)
{
    // called from the customcontextmenu
    if (bkNr == 0){
        te->setHtml("let us select a book first");
        return;
    }

    QString bookName = ::g_bookNames[bkNr];
    int finalChapter = dbH.getChapterCount(bkNr);
    QMenu chapMenu(bookName, this);
    QAction *title = chapMenu.addAction(bookName);
    title->setEnabled(false);

    for (int i = 1; i <= finalChapter  ; ++i){
        chapMenu.addAction(QString("%1").arg(i));
    }

    QAction *action = chapMenu.exec(QCursor().pos());
    if (action != nullptr){
        int chapNr = action->text().toInt();
        QHash<QString, int> job = { {"bk", bkNr}, {"c1", chapNr} };
        printQ.enqueue(job);
        history.clear();
        history.enqueue(job);
        te->clear();
        printScriptures();
    }
}

void ParWindow::createOtNtMenus()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString lang = settings.value("booknameLanguage", "english").toString();

    QString sqlQ = "SELECT book_nr, testament, name_" + lang + " FROM number_name";
    QSqlQuery query(sqlQ, dbH.bibleDb);

    while (query.next()) {
        int bkNr = query.value(0).toInt();
        QString bkName = query.value(2).toString();
        QAction *bookAction = new QAction(scrollIcon, bkName, this);
        connect(bookAction, &QAction::triggered, this, [bkNr, this]() {
            this->popupChapters(bkNr); });

        if (query.value(1) == "OT") {
            otMenu->addAction(bookAction);
        } else if (query.value(1) == "NT"){
            ntMenu->addAction(bookAction);
        }
    }
}

void ParWindow::ccMenuParW()
{
    QMenu ccMenu(this);
    ccMenu.addMenu(otMenu);
    ccMenu.addMenu(ntMenu);

    QAction *chapAction = ccMenu.addAction(tr("chapters"));
    QAction *nextAction = ccMenu.addAction(tr("next"));
    nextAction->setIcon(nextIcon);
    QAction *prevAction = ccMenu.addAction(tr("previous"));
    prevAction->setIcon(prevIcon);
    ccMenu.addSeparator();

    QAction *selectAction = ccMenu.addAction(tr("select all"));
    selectAction->setShortcut(QKeySequence("Ctrl+a"));
    selectAction->setIcon(selectIcon);
    QAction *copyAction = ccMenu.addAction(tr("copy"));
    copyAction->setShortcut(QKeySequence("Ctrl+c"));
    copyAction->setIcon(copyIcon);
    ccMenu.addSeparator();

    QAction *closeAction = ccMenu.addAction(tr("close"));
    closeAction->setIcon(closeIcon);

    QAction *action = ccMenu.exec(QCursor().pos());
    if (action == chapAction){
        popupChapters(lastBookNumber);
    } else if (action == nextAction){
        nextChapter();
    } else if (action == prevAction){
        prevChapter();
    } else if (action == selectAction){
        te->selectAll();
    } else if (action == copyAction){
        te->copy();
    } else if (action == closeAction){
        hide();
    }

}

void ParWindow::centerWindow()
{
    if (parentWidget())
    {
        QScreen* activeScreen = parentWidget()->screen();
        if (activeScreen != nullptr)
        {
            int width = (parentWidget()->width() / 100) * 80;
            int height = (parentWidget()->height() / 100) * 80;
            resize(width, height);

            auto winGeo = frameGeometry();
            auto parentGeoCenter = parentWidget()->geometry().center();
            winGeo.moveCenter(parentGeoCenter);
            move(winGeo.topLeft());
        }
    }
}

void ParWindow::checkTls()
{
    // set QCheckBoxes checked state
    QString selection = cb_select->currentText();

    if (selection == "none") {
        for (QCheckBox *i: qAsConst(chkBoxes)){
            i->setCheckState(Qt::Unchecked);
        }
    } else if (selection == "all") {
        for (QCheckBox *i: qAsConst(chkBoxes)){
            i->setCheckState(Qt::Checked);
        }
    } else {
        for (QCheckBox *i: qAsConst(chkBoxes)) {
            if (selection == i->property("langName"))
                i->setCheckState(Qt::Checked);
            else
                i->setCheckState(Qt::Unchecked);
        }
    }
}

void ParWindow::printRequest()
{
    QString request = leInput->text();

    if (request.isEmpty() and history.isEmpty()) {
        te->clear();
        return;
    } else if ( request.isEmpty() and not history.isEmpty() ) {
        te->clear();
        printScriptures();
        return;
    }

    QRegularExpressionMatchIterator gmatch = re->globalMatch(request.trimmed());
    if (gmatch.hasNext()) {
        this->te->clear();
    }
    QString testBook;
    int bkNr = 0, chNr1 = 0 , chNr2 = 0, vsNr1 = 0, vsNr2 = 0;

    QRegularExpressionMatch match;
    while (gmatch.hasNext()) {
        testBook.clear();

        match = gmatch.next();
        bkNr = 0; chNr1 = 0 ; chNr2 = 0; vsNr1 = 0; vsNr2 = 0;
        if (match.hasMatch()) {
            if (!match.captured("prt").isEmpty()){
                QString part = match.captured("prt");
                testBook.append(part + " ");
            }
            if (!match.captured("bk").isEmpty()){
                QString book = match.captured("bk");
                testBook.append(book.toUpper());
            }
            if (!match.captured("ch1").isEmpty()){
                QString chapter1 = match.captured("ch1");
                chNr1 = chapter1.toInt();
            }
            if (!match.captured("ch2").isEmpty()){
                QString chapter2 = match.captured("ch2");
                chNr2 = chapter2.toInt();
            }
            if (!match.captured("vs1").isEmpty()){
                QString verse1 = match.captured("vs1");
                vsNr1 = verse1.toInt();
            }
            if (!match.captured("vs2").isEmpty()){
                QString verse2 = match.captured("vs2");
                 vsNr2 = verse2.toInt();
            }
        }

        // get the book number
        QSqlQuery query("SELECT abbr_en, book_nr, abbr_nl FROM abbr", dbH.bibleDb);

        while (query.next()) {
            if (testBook == query.value(0).toString().toUpper()){
                bkNr = query.value(1).toInt();
                break;
            } else if (testBook == query.value(2).toString().toUpper()){
                bkNr = query.value(1).toInt();
                break;
            }
        }

        // get chapter number(s)
        QHash<QString, int> job;
        if (bkNr > 0) {
            job["bk"] = bkNr;
            int fCh = dbH.getChapterCount(bkNr);

            if (chNr1 <= 0) {
                chNr1 = 1;
                job["c1"] = chNr1;
            } else if (chNr1 > fCh) {
                chNr1 = fCh;
                job["c1"] = chNr1;
            } else {
                job["c1"] = chNr1;
            }

            // chapter range
            if (chNr2 > chNr1 and chNr2 <= fCh) {
                printQ.enqueue(job);
                while (chNr1 < chNr2) {
                    chNr1 += 1;
                    job["c1"] = chNr1;
                    printQ.enqueue(job);
                }

            // single chapter, perhaps with verse(s)
            } else if (vsNr1 > 0){
                if (vsNr2 > 0 and vsNr2 > vsNr1){
                    job["v1"] = vsNr1;
                    job["v2"] = vsNr2;
                } else {
                    job["v1"] = vsNr1;
                }
                printQ.enqueue(job);
            } else {
                printQ.enqueue(job);
            }

            history.clear();
            history.enqueue(job);
        }
    }

    // printScriptures removes a job from the queue
    while (not printQ.isEmpty()) {
        printScriptures();
    }
}

void ParWindow::printScriptures()
{
    QHash<QString, int > job;

    if (not printQ.isEmpty()) {
        job = printQ.dequeue();
    } else if (not history.isEmpty()) {
        job = history.first();
    } else {
        return;
    }

    int bk = job["bk"];
    QString bookName = ::g_bookNames[bk];
    int c1 = job["c1"];
    QString cStr = QString::number(c1);
    int v1 = job["v1"];
    int v2 = job["v2"];

    if(bk == 0) {
        return;
    }
    if(c1 == 0) {
        c1 = 1;
    }

    lastBookNumber = bk;
    lastChapNumber = c1;

    // i don't know if there is a SQLITE query that can do the right kind of join, so code it is

    /*------------------------------------------------------
    1. make a QList of queries based on active translations
    ------------------------------------------------------*/
    bool haveTl = false; // do we have a translation selected?
    QList< QList<QString> > queries;

    for (QCheckBox* cb: qAsConst(chkBoxes)) {
        if (cb->isChecked()) {
            // selected translations
            haveTl = true;
            QString tl = cb->text();

            QString chapterSql;
            QString verseSql;

            chapterSql = QString("= %1").arg(c1);
            if (v2) {
                verseSql = QString("AND v BETWEEN %1 AND %2").arg(v1).arg(v2);
            } else if (v1) {
                verseSql = QString("AND v = %1").arg(v1);
            }

            QString sql = QString("SELECT v, t from t_%1 where b = %2 and c %3 ")
                    .arg(tl, QString().number(bk), chapterSql);

            sql.append(verseSql);

            QList<QString> query;
            query.append(tl);
            query.append(sql);
            queries.append(query);
        }
    }

    if (!haveTl) {
        te->setHtml("<span style='color:brown'>  let us select a translation first</span>");
        return;
    }

    /*------------------------------------------------------
    2. do a query for each tl and add a QString to a QMap <--  items are always sorted by key
    ------------------------------------------------------*/
    // need int for key to sort ascending
    QList< QMap<int, QString> > rootC; // root container

    for (auto &qV: qAsConst(queries)) {
        // sub container
        QMap<int, QString> subC;

        // tl name for header
        subC.insert(0, qV[0]);

        int nr;
        QString txt;

        QSqlQuery query(qV[1], dbH.bibleDb);

        while (query.next()) {
            nr = query.value(0).toInt();
            txt = query.value(1).toString();
            subC.insert(nr, txt);
        }

        rootC.append(subC);
    }

    /*------------------------------------------------------
    3. determine which container's size is greatest
    for the case that a translation has less verses
    to keep displaying the verses in parallel
    ------------------------------------------------------*/
    QList<qint64> containerSizes;
    for (const auto &subC: rootC) {
        containerSizes.append(subC.size());
    }

    qint64* iL;
    iL = &*std::max_element(containerSizes.begin(), containerSizes.end() );

    // choose a preferred container with the max number of verses
    QMap<int, QString> prefC;

    for (const auto &subC: rootC) {
        if (subC.size() == *iL) {
            prefC = subC;
            break;
        }
    }

    // since this container is used to iterate the verses, the header is not wanted
    prefC.remove(0);

    /*------------------------------------------------------
    4. create one big html formatted string and append the tl's
    header first, QMap 0
    ------------------------------------------------------*/
    QString bigString = "<table width='100%' cellspacing='0' cellpadding='5'>";

    bigString.append("<tr>");
    for (auto subC: rootC) {
        QString tlName = subC[0];
        QString header = QString("<td> "
                        "<center><span style='font-weight:bold;color:%1'>%2</span></center><br>"
                        "<span style='font-weight:bold;color:%3'><small>%4 %5</small></span>"
                        "</td>")
                .arg(scheme["titleClr"], tlName, scheme["titleClr"], bookName, cStr);
        bigString.append(header);
    }
    bigString.append("</tr>");

    QString bg = scheme["bgClr"];
    QString bg1 = scheme["bgClr"];
    QString bg2 = scheme["bg2Clr"];
    QString nr;
    QString txt;

    QList prefCKeys = prefC.keys();

    for (auto &key : qAsConst(prefCKeys)) {
        bigString.append( QString("<tr style='background-color:%1'>").arg(bg) );

        for (const auto &subC: rootC) {
            nr = QString::number(key);

            txt = subC.value(key);

                if ( txt.isEmpty() ) {
                    txt = "(verse not found)";
                }

                bigString.append("<td>");
                bigString.append(QString("<span style='color:%1'><small>%2</small></span> ")
                                 .arg(scheme["nrClr"], nr) );
                bigString.append(QString("<span style='color:%1'>%2</span>")
                                 .arg(scheme["txtClr"], txt) );
                bigString.append("</td>");
        }

        bigString.append("</tr>");
        bg == bg1 ? bg = bg2 : bg = bg1 ;
    }

    bigString.append("</table>");
    te->insertHtml(bigString);
}
