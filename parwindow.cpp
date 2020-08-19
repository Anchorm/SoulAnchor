#include "parwindow.h"

ParWindow::ParWindow(QWidget *parent) : QWidget(parent, Qt::Window)
{
    setWindowTitle("Parallel Window - SoulAnchor");
    setWindowIcon(anchorIcon);
    setMinimumSize(500, 500);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    bookName = "";

    int bHeight = 26;
    cb_select->setFixedHeight(bHeight);
    leInput->setFixedHeight(bHeight);
    btn_ok->setFixedHeight(bHeight);
    btn_next->setFixedHeight(bHeight);
    btn_prev->setFixedHeight(bHeight);

    leInput->setPlaceholderText("bk ch:vs");
    leInput->setFixedWidth(200);
    leInput->setTextMargins(2, 0, 0, 0);

    btn_ok->setText(tr("Go"));
    btn_prev->setText(tr("previous"));
    btn_next->setText(tr("next"));

    hbox_1->setSpacing(5);
    hbox_1->setAlignment(Qt::AlignLeft);
    hbox_2->setSpacing(5);
    hbox_2->setAlignment(Qt::AlignLeft);

    cb_select->addItem("none");

    QString getDisLang = "SELECT DISTINCT language FROM BIBLE_VERSION_KEY ORDER BY language;";
    QSqlQuery getL(getDisLang, dbH.bibleDb);

    while (getL.next()) {
        cb_select->addItem(getL.value(0).toString());
    }

    cb_select->addItem("all");

    connect(cb_select, &QComboBox::currentTextChanged, this, &ParWindow::checkTls);

    hbox_1->addWidget(cb_select);
    hbox_1->addItem(spacer);

    hbox_1->addWidget(leInput);
    hbox_1->addWidget(btn_ok);
    hbox_1->addItem(spacer2);
    hbox_1->addWidget(btn_prev);
    hbox_1->addWidget(btn_next);
    hbox_1->addItem(spacer3);

    vbox->setContentsMargins(3, 3, 3, 3);

    //create checkboxes for translations
    QString sqlCb = "SELECT abbreviation, language, version "
                    "FROM bible_version_key ORDER BY abbreviation ASC";
    QSqlQuery getCb(sqlCb, dbH.bibleDb);
    QString abbr;
    QString lang;
    QString desc;

    while ( getCb.next() ) {
        abbr = getCb.value(0).toString();
        lang = getCb.value(1).toString();
        desc = getCb.value(2).toString();

        QCheckBox *chkB = new QCheckBox(abbr);
        chkB->setToolTip(desc);
        chkB->setProperty("langName", lang);
        chkBoxes.append(chkB);
    }

    for (QCheckBox *chkB: qAsConst(chkBoxes)) {
        hbox_2->addWidget(chkB);
    }

    te->setStyleSheet(docStyle);
    te->setReadOnly(true);
    te->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(te, &QTextEdit::customContextMenuRequested, this, &ParWindow::ccMenuParW);
    connect(leInput, &QLineEdit::returnPressed, this, &ParWindow::printRequest);
    connect(btn_ok, &QPushButton::clicked, this, [this] () {
        te->clear();
        printRequest();
    });

    new QShortcut(QKeySequence(Qt::EnterKeyReturn), this, SLOT(printScriptures()));
    connect(btn_next, &QPushButton::clicked, this, &ParWindow::nextChapter);
    new QShortcut(QKeySequence(Qt::Key_Plus), this, SLOT(nextChapter()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(nextChapter()));
    connect(btn_prev, &QPushButton::clicked, this, &ParWindow::prevChapter);
    new QShortcut(QKeySequence(Qt::Key_Minus), this, SLOT(prevChapter()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(prevChapter()));

    new QShortcut(QKeySequence(Qt::Key_F11), this, SLOT(toggleFullscreen()));
    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapeKey()));

    vbox->addItem(hbox_2);
    vbox->addItem(hbox_1);
    vbox->addWidget(te);
    setLayout(vbox);

    createOtNtMenus();
}

void ParWindow::setTlandJob(const QString &tlAbbr, const QHash<QString, int> &job) {
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

void ParWindow::toggleFullscreen(){
    isFullScreen() ? showNormal() : showFullScreen();
}

void ParWindow::escapeKey(){
    if ( isFullScreen() ) showNormal();
}

void ParWindow::nextChapter(){
    if (lastBookNumber == 0 || lastChapNumber == 0) {
        return;
    } else {
        int finChap = dbH.getFinalChapter(lastBookNumber);
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

void ParWindow::prevChapter(){
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

void ParWindow::popupChapters(int bkNr) {
    // called from the customcontextmenu
    if (bkNr == 0){
        te->setHtml("<span style='color:brown'>  let us select a book first</span>");
        return;
    }

    QString bookName = ::g_bookNames[bkNr];
    int finalChapter = dbH.getFinalChapter(bkNr);
    QMenu chapMenu(bookName);
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

void ParWindow::createOtNtMenus() {  
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString lang = settings.value("bknLanguage", "english").toString();

    QString sqlQ = "SELECT book_nr, testament, name_" + lang + " FROM number_name";
    QSqlQuery query(sqlQ, dbH.bibleDb);

    while (query.next()) {
        int bkNr = query.value(0).toInt();
        QString bkName = query.value(2).toString();
        QAction *bookAction = new QAction(bookIcon, bkName, this);
        connect(bookAction, &QAction::triggered, this, [bkNr, this]() {
            this->popupChapters(bkNr); });

        if (query.value(1) == "OT") {
            otMenu->addAction(bookAction);
        } else if (query.value(1) == "NT"){
            ntMenu->addAction(bookAction);
        }
    }
}

void ParWindow::ccMenuParW(){
    QMenu ccMenu;
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

void ParWindow::centerWindow(){
    if (parentWidget())
    {
        QScreen* activeScreen = parentWidget()->screen();
        if (activeScreen != nullptr)
        {
            auto winGeo = frameGeometry();
            auto parentGeoCenter = parentWidget()->geometry().center();
            winGeo.moveCenter(parentGeoCenter);
            move(winGeo.topLeft());
        }
    }
}

void ParWindow::checkTls() {
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

void ParWindow::printRequest() {
    QString request = leInput->text();

    if (request.isEmpty() and history.isEmpty()) {
        te->clear();
        return;
    } else if ( request.isEmpty() and not history.isEmpty() ) {
        te->clear();
        printScriptures();
        return;
    }

    QString pattern =
            "\\s*(?<prt>[1-3]?)"
            "\\s*(?<bk>[a-zA-Zëüï]+)"
            "\\s*(?<ch1>\\d?\\d?\\d?)"
            "-?(?<ch2>\\d?\\d?\\d?)"
            ":?(?<vs1>\\d?\\d?\\d?)"
            "-?(?<vs2>\\d?\\d?\\d?)";

    QRegularExpression re(pattern);
    QRegularExpressionMatchIterator gmatch = re.globalMatch(request);
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
            int fCh = dbH.getFinalChapter(bkNr);

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

void ParWindow::printScriptures(){
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
//    qDebug() << "bk:" << bk << "c1:" << c1 << "v1:" << v1 << "v2:" << v2;

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
    1. make a QVector of queries based on active translations
    ------------------------------------------------------*/
    bool haveTl = false; // do we have a translation selected?
    QVector< QVector<QString> > queries;

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

            QVector<QString> query;
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
    QVector< QMap<int, QString> > rootC; // root container

    for (QVector<QString> qV: queries) {
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
    QVector<int> containerSizes;
    for (const auto &subC: rootC) {
        containerSizes.append(subC.size());
    }

    int* iL;
    iL = std::max_element(containerSizes.begin(), containerSizes.end() );

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
    QString bigString = "<table width='100%' cellspacing='1' cellpadding='5'>";

    bigString.append("<tr>");
    for (auto subC: rootC) {
        QString tlName = subC[0];
        QString header = QString("<td> "
                                 "<center><span style='%1'>%2</span></center><br>"
                                 "<span style='%3'>%4 %5</span>"
                                 "</td>").arg(tlStyle, tlName, headerStyle, bookName, cStr);
        bigString.append(header);
    }
    bigString.append("</tr>");


    QString bg = bg1;
    QString txtStyle = textStyle1;
    QString nr;
    QString txt;

    QList prefCKeys = prefC.keys();

    for (auto &key : qAsConst(prefCKeys)) {
        bigString.append( QString("<tr style='%1'>").arg(bg) );

        for (const auto &subC: rootC) {
            nr = QString::number(key);

            txt = subC.value(key);

                if ( txt.isEmpty() ) {
                    txt = "(verse not found)";
                }

                bigString.append("<td>");
                bigString.append(QString("<span style='%1'>%2</span> ").arg(nrStyle, nr) );
                bigString.append(QString("<span style='%1'>%2</span>").arg(txtStyle, txt) );
                bigString.append("</td>");
        }

        bigString.append("</tr>");
        bg == bg1 ? bg = bg2 : bg = bg1 ;
        txtStyle == textStyle1 ? txtStyle = textStyle2 : txtStyle = textStyle1;
    }

    bigString.append("</table>");
    te->insertHtml(bigString);
}
