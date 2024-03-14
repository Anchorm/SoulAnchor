/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);

    // restore window state or set a default state of 80% of the screen
    QByteArray geo = settings.value("window/geometry").toByteArray();
    QByteArray state = settings.value("window/windowState").toByteArray();
    if (!geo.isEmpty() && !state.isEmpty()) {
        restoreGeometry(geo);
        restoreState(state);
    } else {
        int width80 = (screen()->geometry().width() / 100) * 80;
        int height80 = (screen()->geometry().height() / 100) * 80;
        resize(width80, height80);
        centerApp();
    }

    // make sure the correct framesize is shown on startup
    QSize appSize(this->geometry().width(), this->geometry().height());
    QResizeEvent *resEvent = new QResizeEvent(appSize, appSize);
    qApp->postEvent(this, resEvent );

    tab = settings.value("tab", "today").toString().toLower();
    startup = settings.value("startup", "nothing").toString().toLower();
    guiLanguage = settings.value("guiLanguage", "en").toString().toLower();
    bknLanguage = settings.value("booknameLanguage", "en").toString().toLower();
    scripLayout = settings.value("layout", "table").toString().toLower();
    rosterRead = settings.value("rosters/rosterRead", "false").toBool();
    docMargin = settings.value("margin", "14").toInt();
    frameWidth = settings.value("width", "10").toInt(); // range 2-10
    showMaps = settings.value("showMaps").toBool();
    filtersMenuVisible = settings.value("filters/menuVisible", "false").toBool();

    ui->info_frame->hide();
    ui->find_frame->hide();
    textBrowser = ui->tb_scriptures;

    if (tab == "contents")
        ui->tabwidget->setCurrentIndex(0);
    else if (tab == "search")
        ui->tabwidget->setCurrentIndex(1);
    else
        ui->tabwidget->setCurrentIndex(2);

    // tab widget less space than background frame
    ui->splitter_central->setStretchFactor(0,0);
    ui->splitter_central->setStretchFactor(1,1);

    // bible frame vs info frame
    ui->splitter_background->setStretchFactor(0,1);
    ui->splitter_background->setStretchFactor(1,1);

    // chapters, scriptures
    ui->splitter_bibleframe->setStretchFactor(0,1);
    ui->splitter_bibleframe->setStretchFactor(1,19);

    // give info frame more space than strongs frame
    ui->splitter_info->setStretchFactor(0,3);
    ui->splitter_info->setStretchFactor(1,2);

    connect(ui->action_quit, &QAction::triggered,
            this, &MainWindow::exitApp, Qt::QueuedConnection);
    ui->action_quit->setShortcut(QKeySequence("Ctrl+q"));

    connect(ui->action_toggle_tab, &QAction::triggered,
            this, &MainWindow::toggleTabW);
    ui->action_toggle_tab->setShortcut(Qt::Key_F8);

    connect(ui->action_toggle_bible, &QAction::triggered,
            this, &MainWindow::toggleBible);
    ui->action_toggle_bible->setShortcut(Qt::Key_F9);

    connect(ui->action_toggle_info, &QAction::triggered,
            this, &MainWindow::toggleInfo);
    ui->action_toggle_info->setShortcut(Qt::Key_F10);

    connect(ui->action_fullscreen, &QAction::triggered, this, [this] () {
        isFullScreen() ? showNormal() : showFullScreen(); });
    ui->action_fullscreen->setShortcut(Qt::Key_F11);

    new QShortcut(Qt::Key_F12, this, SLOT(toggleMenu()));

    connect(ui->action_import, &QAction::triggered, this, [this] () {
        !impW->isVisible() ? impW->show() : impW->raise(); } );
    impW->centerWindow();

    connect(ui->action_make_roster, &QAction::triggered, this, [this] () {
        !rosterW->isVisible() ? rosterW->show() : rosterW->raise(); } );

    connect(ui->action_open_parallel, &QAction::triggered,
            this, &MainWindow::openParW);
    ui->action_open_parallel->setShortcut(QKeySequence("Ctrl+p"));

    connect(ui->action_open_maps, &QAction::triggered, this, [this] () {
        imgW->changeMap(); } );

    connect(ui->action_modifications, &QAction::triggered,
            this, &MainWindow::modifications);

    connect(ui->action_about, &QAction::triggered, this, [this] () {
        aboutW->centerWindow();
        !aboutW->isVisible() ? aboutW->show() : aboutW->raise(); });

    connect(ui->action_settings, &QAction::triggered, this, [this] () {
        settingsW->centerWindow();
        !settingsW->isVisible() ? settingsW->show() : settingsW->raise(); });

    connect(ui->action_lord_prayer, &QAction::triggered, this,
            &MainWindow::theLordsPrayer);
    connect(ui->action_breaking_bread, &QAction::triggered,
            this, &MainWindow::breakingBread);
    connect(ui->action_immersion, &QAction::triggered, this, &MainWindow::immersion);
    connect(ui->action_salvation, &QAction::triggered, this, &MainWindow::salvation);
    connect(ui->action_worries, &QAction::triggered, this, &MainWindow::worries);
    connect(ui->action_topical_index, &QAction::triggered, this, &MainWindow::showTopics);
    connect(ui->action_cross_references, &QAction::triggered,
            this, &MainWindow::makeCrossRefs);
    connect(ui->action_strongify, &QAction::triggered, this, &MainWindow::strongify);

    connect(ui->action_overview, &QAction::triggered, this, [this] () {
        showEncPic(":/data/img/overview.jpg"); } );
    connect(ui->action_shortcuts, &QAction::triggered, this, &MainWindow::showShortcuts);
    connect(ui->action_emergency, &QAction::triggered, this, [this] () {
        showEncPic(":/data/img/emergency.jpg"); } );
    connect(ui->action_empire, &QAction::triggered, this, &MainWindow::showEmpireMaps);

    new QShortcut(Qt::Key_Plus, this, SLOT(nextChapter()));
    new QShortcut(Qt::Key_Right, this, SLOT(nextChapter()));
    new QShortcut(Qt::Key_Minus, this, SLOT(prevChapter()));
    new QShortcut(Qt::Key_Left, this, SLOT(prevChapter()));

    // find in page - current textdocument
    connect(ui->cb_find_loc, &QComboBox::currentIndexChanged,
            this, &MainWindow::setFindInPageLocation);
    new QShortcut(QKeySequence("Ctrl+f"), this, SLOT(showFindFrame()));
    connect(ui->btn_find_next, &QToolButton::clicked, this, [this]() {
        findInPage("next"); });
    connect(ui->btn_find_prev, &QToolButton::clicked, this, [this]() {
        findInPage("prev"); });
    connect(ui->lineEdit_find, &QLineEdit::textEdited, this, &MainWindow::startFind);
    connect(ui->lineEdit_find, &QLineEdit::returnPressed,
            this, &MainWindow::highlightMatches);
    ui->btn_find_prev->setShortcut(Qt::Key_F2);
    ui->btn_find_next->setShortcut(Qt::Key_F3);
    connect(ui->chkBox_find_case, &QCheckBox::stateChanged,
            this, &MainWindow::modifyFindInPageFlags);
    connect(ui->chkBox_find_whole, &QCheckBox::stateChanged,
            this, &MainWindow::modifyFindInPageFlags);

    new QShortcut(Qt::Key_Escape, this, SLOT(escapeKey()));

    connect(new QShortcut(QKeySequence("Alt+1"),  this), &QShortcut::activated,
            ui->tabwidget, [=]() {ui->tabwidget->setCurrentIndex(0); });
    connect(new QShortcut(QKeySequence("Alt+2"),  this), &QShortcut::activated,
            ui->tabwidget, [=]() {ui->tabwidget->setCurrentIndex(1); });
    connect(new QShortcut(QKeySequence("Alt+3"),  this), &QShortcut::activated,
            ui->tabwidget, [=]() {ui->tabwidget->setCurrentIndex(2); });

    connect(ui->background_frame, &QTextEdit::customContextMenuRequested,
            this, &MainWindow::ccMenuBackground);
    connect(ui->bible_frame, &QFrame::customContextMenuRequested,
            this, &MainWindow::ccMenuBibleFrame);
    connect(ui->info_tb, &QFrame::customContextMenuRequested,
            this, &MainWindow::ccMenuInfo);
    connect(ui->strongs_tb, &QFrame::customContextMenuRequested,
            this, &MainWindow::ccMenuStrongs);

    connect(ui->lbl_book_title , &ClickableLabel::clicked,
            this, &MainWindow::showAboutBook);

    connect(ui->lw_books, &QListWidget::itemClicked,
            this, &MainWindow::bookSelected);
    connect(ui->lw_books, &QListWidget::itemActivated,
            this, &MainWindow::bookSelected);
    connect(ui->lw_chapters, &QListWidget::itemClicked,
            this, &MainWindow::chapterSelected);
    connect(ui->lw_chapters, &QListWidget::itemActivated,
            this, &MainWindow::chapterSelected);
    connect(ui->lineEdit_select, &QLineEdit::returnPressed, this, [this] () {
        printRequest(ui->lineEdit_select->text()); });

    populateSearchCbs();

    connect(ui->search_le_what, &QLineEdit::returnPressed,
            this, &MainWindow::searchScriptures);
    connect(ui->search_btn_find, &QToolButton::clicked,
            this, &MainWindow::searchScriptures);
    connect(ui->search_le_what, &QLineEdit::textChanged, this, [this](){
        if(ui->search_le_what->text().isEmpty()) ui->search_tb->clear(); });

    connect(ui->action_daily_proverb, &QAction::triggered,
            this, &MainWindow::todaysProverb);
    connect(ui->btn_daily_proverb, &QPushButton::clicked,
            this, &MainWindow::todaysProverb);
    connect(ui->btn_daily_psalm, &QPushButton::clicked,
            this, &MainWindow::todaysPsalm);
    connect(ui->action_daily_psalm, &QAction::triggered,
            this, &MainWindow::todaysPsalm);
    connect(ui->btn_daily_letter, &QPushButton::clicked,
            this, &MainWindow::todaysLetter);
    connect(ui->action_letter, &QAction::triggered,
            this, &MainWindow::todaysLetter);

    connect(ui->btn_daily_mbm, &QPushButton::clicked, this, [this](){
        MainWindow::morningAndEvening("mbm"); });
    connect(ui->action_morning, &QAction::triggered, this, [this](){
        MainWindow::morningAndEvening("mbm"); });
    connect(ui->btn_daily_ebe, &QPushButton::clicked, this, [this](){
        MainWindow::morningAndEvening("ebe"); });
    connect(ui->action_evening, &QAction::triggered, this, [this](){
        MainWindow::morningAndEvening("ebe"); });

    connect(ui->btn_roster, &QPushButton::clicked,
            this, &MainWindow::readingPlan);
    connect(ui->action_show_roster, &QAction::triggered,
            this, &MainWindow::loadRoster);
    ui->frame_roster_btns->hide();

    updateCbTranslations();

    activeTl = settings.value("translation", "net").toString().toLower();
    if (checkTableExists(activeTl) == false) exit(1);

    int tlIndex = ui->cb_select_translation->findData(activeTl);
    if (tlIndex == -1) tlIndex = 0;

    setHasNotes();
    ui->cb_select_translation->setCurrentIndex(tlIndex);

    ui->search_lbl_tl->setText(ui->cb_select_translation->currentText());

    connect(ui->cb_select_translation, &QComboBox::currentTextChanged,
            this, &MainWindow::setTranslation);

    activeSubh = settings.value("subheadings", "net").toString().toLower();

    getBooksAbbr();

    makeMusicMenu();
    mplayer->setAudioOutput(audioOutput);
    connect(ui->action_stop_media, &QAction::triggered,
            this, [this] () { mplayer->stop(); });
    connect(mplayer, &QMediaPlayer::playbackStateChanged,
            this, &MainWindow::stopPlayer);
    connect(ui->action_play_random, &QAction::triggered,
            this, &MainWindow::playRandom);

    makeTextMenuItems();

    // signals
    QObject::connect(rosterW, &Roster::rosterCreated,
                     this, &MainWindow::addRostersToMenu);

    QObject::connect(settingsW, &SettingsWindow::fontChanged,
                     this, &MainWindow::applyFont);
    QObject::connect(settingsW, &SettingsWindow::schemeChanged,
                     this, &MainWindow::applyScheme);

    QObject::connect(settingsW, &SettingsWindow::booknameLangChanged,
                     this, &MainWindow::updateBooksWidget);
    QObject::connect(settingsW, &SettingsWindow::booknameLangChanged,
                     this, &MainWindow::createOtNtMenus);

    QObject::connect(settingsW, &SettingsWindow::subheadingsChanged,
                     this, &MainWindow::updateTLandSubh);
    QObject::connect(settingsW, &SettingsWindow::showMapsChanged, this, [this]
                    (bool showM)
                    {showM ? showMaps = true : showMaps = false;
    });

    QObject::connect(settingsW, &SettingsWindow::showFiltersChanged, this, [this]
                     (bool showF)
                     {showF ? ui->menu_bar->addAction(ui->menu_filters->menuAction()) :
                        ui->menu_bar->removeAction(ui->menu_filters->menuAction());});

    QObject::connect(this, &MainWindow::parOpened,
                     parW, &ParWindow::setTlandJob);
    QObject::connect(this, &MainWindow::setParwStyle,
                     parW, &ParWindow::setStyle);

    QObject::connect(this, &MainWindow::setImgWindowPixmap,
                     imgW, &ImageWindow::setPixmap);

    addRostersToMenu();
    setActiveRoster();
    buildBookmarkMenu();

    iFilter = settings.value("filters/immersionFilter").toBool();
    jFilter = settings.value("filters/judeansFilter").toBool();
    ui->action_immersion_filter->setChecked(iFilter);
    ui->action_judeans_filter->setChecked(jFilter);

    connect(ui->action_immersion_filter, &QAction::triggered, this, &MainWindow::setFilters );
    connect(ui->action_judeans_filter, &QAction::triggered, this, &MainWindow::setFilters );
    connect(ui->action_about_filters, &QAction::triggered, this, &MainWindow::aboutFilters );

    connect(ui->strongs_le, &QLineEdit::returnPressed, this, [this]() {
        getStrongs(ui->strongs_le->text()); });
    connect(ui->strongs_pb, &QPushButton::clicked, this, [this]() {
        getStrongs(ui->strongs_le->text()); });
    connect(ui->strongs_le, &QLineEdit::textChanged, this, [this]() {
        if(ui->strongs_le->text().isEmpty()) ui->strongs_tb->clear(); });

    connect(ui->dict_le, &QLineEdit::returnPressed, this, [this](){
        getDictSug(""); });
    connect(ui->dict_le, &QLineEdit::textChanged, this, [this](){
        if(ui->dict_le->text().isEmpty()) ui->info_tb->clear(); });

    connect(ui->action_add_bookmark, &QAction::triggered, this, &MainWindow::addBookmark);
    connect(ui->action_rename_bookmark, &QAction::triggered, this, &MainWindow::renameBookmark);
    connect(ui->action_delete_bookmark, &QAction::triggered, this, &MainWindow::deleteBookmark);

    rosterRead ? ui->cb_roster_read->setChecked(true)
               : ui->cb_roster_read->setChecked(false);
    strongTl = "strongs_akjv";

    ui->bible_frame->layout()->setAlignment(Qt::AlignHCenter);

    settingsW->cancelSettings();

    setEncPic();
    changeEncPic();
    setEncTxt();
    changeEncTxt();

    if (startup == "psalm") todaysPsalm();
    else if (startup == "proverb") todaysProverb();
    else if (startup == "letter") todaysLetter();
    else if (startup == "reading plan") readingPlan();
    else ui->bible_frame->hide();

    ui->tb_scriptures->moveCursor(QTextCursor::Start);
}

MainWindow::~MainWindow()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("window/geometry", saveGeometry());
    settings.setValue("window/windowState", saveState());
    delete ui;
}

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange)
        ui->retranslateUi(this);

    if (event->type() == QEvent::PaletteChange) {
        if (activeScheme == "system")
            applyScheme("system");
    }
    event->ignore();
}

bool MainWindow::checkTableExists(const QString &activeTl)
{
    // check if table exists, if it does not, app will crash
    // might happen when changing conf file or removing table
    QString sqlCheck = QString("SELECT name FROM sqlite_master "
               "WHERE type='table' AND name='t_%1';").arg(activeTl);
    QSqlQuery query(sqlCheck, bibleDb);
    QString tableTest = "";

    if (Utilities::handleQuery(query)) {
        if (query.next())
            tableTest = query.value(0).toString();
    }

    if (tableTest.isEmpty()) {
        ::sout << "Cannot find table: t_" + activeTl <<  Qt::endl;
        ::sout << "- check config file for valid translation name" <<  Qt::endl;
        ::sout << "- or remove soulanchor.conf, "
                  "a default version will then be used" <<  Qt::endl;
        ::sout << "- or check bibles.db database for valid table name" <<  Qt::endl;
        return false;
    } else {
        return true;
    }
}

void MainWindow::modifications()
{
    ui->info_tb->clear();
    ui->info_lbl_title->setText("");
    QFile *modFile = new QFile (QString(APP_DATADIR_PREFIX) +
                                "/share/doc/soulanchor/mod.html");
    if (modFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        ui->info_tb->setHtml(modFile->readAll());
    } else {
        ui->info_tb->setHtml(QString(APP_DATADIR_PREFIX)
                             + "/share/doc/soulanchor/mod.html not found");
    }
    ui->info_frame->show();
}

void MainWindow::printMsg(const QString message)
{
    ui->tb_scriptures->setHtml(QString("<br><h3 style='text-align:center;color:%1;'>"
                               "%2</h3>").arg(scheme["nrClr"], message));
}

void MainWindow::popupMsg(const QString message)
{
    QMessageBox msgBox(QMessageBox::Information, "info", message, QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::strongify()
{
    // make it strong
    QString nrStyle = "color:" + scheme["nrClr"];
    QString txtStyle = "padding: 5px; color:" + scheme["txtClr"];
    QString christStyle = "color:" + scheme["titleClr"];
    QString strongStyle = "font-weight: normal; text-decoration: none; color:" + scheme["nrClr"];

    QHash<QString, int> job;
    if (!printHistory.isEmpty())
        job = printHistory.last();
    else
        return;

    int bk = job["bk"];
    int c1 = job["c1"];

    const QString bkStr = QString::number(bk);
    const QString cStr = QString::number(c1);

    QString sql = QString("SELECT b, c, v, t from %1 where b = %2 and c = %3 ")
            .arg(strongTl, bkStr, cStr);
    QSqlQuery query(sql, bibleDb);

    QString v, t;
    QString bookName = ::g_bookNames[bk];
    QString header = QString(tr("showing %1 %2 with Strong's numbers using the AKJV"))
            .arg(bookName, cStr);

    QString strongified = QString("<table>"
            "<th style='color:%1'>" + header + "</th><br>").arg(scheme["titleClr"]);

    while (query.next()) {
        v = query.value(2).toString();
        t = query.value(3).toString();
        strongified +=  "<tr>"
                        "<td style='" + nrStyle + "'>" + "<sub>" + v + "</sub>" +"</td>"
                        + " <td style='" + txtStyle + "'>" + t +"</td>"
                        "</tr>";
    }

    strongified += "</>";

    QString strongNr;
    QString hebrewsOrGreek;

    if (bk < 40)
        hebrewsOrGreek = "H";
    else
        hebrewsOrGreek = "G";

    // look for the words of the Lord tags and replace and change style
    strongified.replace("<J>","<span style='" + christStyle + "'>");
    strongified.replace("</J>","</span>");

    // use regex to change strong tags into anchors
    QRegularExpressionMatchIterator gmatch = strongRegex->globalMatch(strongified);

    QString startAnchor;

    QRegularExpressionMatch match;

    while (gmatch.hasNext())
    {
        match = gmatch.next();
        strongNr = hebrewsOrGreek;
        strongNr += match.captured("strongNr");
        startAnchor = "<a style='" + strongStyle + "' href='strongs:";
        startAnchor += strongNr + "'>";

        strongified.replace(match.captured(), startAnchor + " <sup>" + strongNr + "</sup></a>");
    }

    ui->tb_scriptures->setHtml(strongified);
}

void MainWindow::getTWOT(QString twot)
{
    // i think there's still copyright on the TWOT so for those
    // who own the TWOT, they can add a table: name: twot: columns: topic, definition

    QSqlQuery chkTWOT("SELECT name FROM sqlite_master WHERE type = 'table' "
                           "and name = 'twot' ", dictDb);

    if (not chkTWOT.next()) {
        ui->strongs_tb->moveCursor(QTextCursor().Start);
        ui->strongs_tb->insertHtml("<p style='color:brown;'>This database is not included.</p>");
        return;
    }

    QRegularExpressionMatch match = twotRegex->match(twot);

    // twot topic number
    QString twotNr;
    if (match.hasMatch()) {
        twotNr = match.captured();
    } else {
        ui->strongs_tb->setHtml("TWOT entry not found");
        return;
    }

    if (twotNr.length() == 1)
        twotNr.prepend("000");
    else if (twotNr.length() == 2)
        twotNr.prepend("00");
    else if (twotNr.length() == 3)
        twotNr.prepend("0");

    QString sql_twot = QString("SELECT topic, definition "
                          "FROM twot WHERE topic LIKE '%1%' ").arg(twotNr);

    QSqlQuery query_twot(sql_twot, dictDb);
    QString result;

    while (query_twot.next()) {
        result = query_twot.value(1).toString();
    }

    if (result.isEmpty()) {
        ui->strongs_lbl->clear();
        ui->strongs_tb->setHtml("no information");
        return;
    } else {
        if (!strongsHistory.isEmpty()) {
            if ("T" + twotNr != strongsHistory[0]) {
                strongsHistory.prepend("T" + twotNr);
            }
        } else {
            strongsHistory.prepend("T" + twotNr);
        }
        if (strongsHistory.size() > 10) strongsHistory.removeLast();
    }

    ui->strongs_lbl->setText("TWOT: " + twotNr);

    QString newLink = QString("<a style='text-decoration:none;color:%1' href")
                        .arg(scheme["nrClr"]);
    result.replace("<a href", newLink);

    ui->strongs_tb->setHtml(result);
}

void MainWindow::getStrongs(QString strongs)
{
    if (strongs.isEmpty()) {
        ui->strongs_tb->clear();
        return;
    }

    QString numberRequest, numberResult;
    QString desc, sql_strong, sql_bdbt;

    QRegularExpressionMatch match = getStrongRegex->match(strongs);

    // strongs number
    if (match.hasMatch()) {
        numberRequest = match.captured().toUpper();
        sql_strong = QString("SELECT topic, definition, lexeme "
                              "FROM sece WHERE topic = '%1' ").arg(numberRequest);
        sql_bdbt = QString("SELECT topic, definition, lexeme "
                              "FROM bdbt WHERE topic = '%1' ").arg(numberRequest);
    // strongs word
    } else {
        sql_strong = QString("SELECT topic, definition, lexeme "
                              "FROM sece WHERE lexeme = '%1' ").arg(strongs);
        sql_bdbt = QString("SELECT topic, definition, lexeme "
                              "FROM bdbt WHERE lexeme = '%1' ").arg(strongs);
    }

    QSqlQuery query_strong(sql_strong, dictDb);
    QString result;

    while (query_strong.next()) {
        numberResult = query_strong.value(0).toString();
        desc = query_strong.value(1).toString();
    }

    if (numberResult.isEmpty()) {
        ui->strongs_lbl->clear();
        ui->strongs_tb->setHtml("no results<br>");
        return;
    } else {
        if (!strongsHistory.isEmpty()) {
            if (numberResult != strongsHistory[0]) {
                strongsHistory.prepend(numberResult);
            }
        } else {
            strongsHistory.prepend(numberResult);
        }
        if (strongsHistory.size() > 10) strongsHistory.removeLast();
    }

    result += "<br>SECE:<br><br>";
    result += desc;

    result += "<br><br>BDB-T:<br><br>";
    QSqlQuery query_bdbt(sql_bdbt, dictDb);
    while (query_bdbt.next()) {
        result += query_bdbt.value(1).toString();
    }

    QString newClr = QString("<font color='%1'>").arg(scheme["titleClr"]);
    QString newLink = QString("<a style='text-decoration:none;color:%1' href")
                        .arg(scheme["nrClr"]);
    result.replace("<p/>", "<br>");
    result.replace("<p />", "<br>");
    result.replace("<font color='1'>", newClr);
    result.replace("<font color='2'>", newClr);
    result.replace("<font color='3'>", newClr);
    result.replace("<font color='4'>", newClr);
    result.replace("<font color='5'>", newClr);
    result.replace("<a href", newLink);

    ui->info_frame->show();
    ui->strongs_lbl->setText(numberResult);
    ui->strongs_tb->setHtml(result);
}

void MainWindow::on_dict_pb_go_clicked()
{
    getDictSug();
}

void MainWindow::on_dict_pb_index_clicked()
{
    // show all topics columns from all 'non-strong' dictionaries
    ui->info_lbl_title->setText(tr("dictionaries index"));
    QString dictsLinks = tr("Showing results for: ");

    // only the first time it's empty
    if (!dictIndex.isEmpty()) {
        ui->info_tb->setHtml(tr("<b>working...</b>"));
        ui->info_tb->repaint();
        ui->info_tb->setHtml(dictIndex);
        return;
    }

    ui->info_tb->setHtml(tr("<b>this will take a moment...</b>"));
    ui->info_tb->repaint();

    QStringList dictTables;
    QSqlQuery query_get_tables("SELECT name FROM info WHERE strong = 'false' "
                               "ORDER BY name ASC", dictDb);

    while (query_get_tables.next())
        dictTables.append(query_get_tables.value(0).toString());

    QString aStyle = "text-decoration:none;font-weight:normal;color:" + scheme["txtClr"];
    QString topicHref;
    QString topicTitle;
    QString sql_get_topics;
    QSqlQuery query_get_topics(dictDb);

    for (const QString &table : dictTables) {
        dictsLinks += QString("<a style='text-decoration:none;color:%1' "
                        "href='dict:%2'>%2</a> ")
                        .arg(scheme["nrClr"], table);
        dictIndex.append("<br><h3 id='" + table + "'>" + table + ":</h3><br>");
        sql_get_topics = QString("SELECT topic FROM '%1'").arg(table.toLower());
        query_get_topics.exec(sql_get_topics);

        while (query_get_topics.next()) {
            topicHref = query_get_topics.value(0).toString();
            topicTitle = query_get_topics.value(0).toString();
            topicTitle.insert(1,"</b>");
            topicTitle.prepend("<b>");
            dictIndex += QString(" <a style='%1' href='dictword:%2'>%3</a> ")
                    .arg(aStyle, topicHref, topicTitle);
        }
    }

    dictIndex.prepend(dictsLinks);
    ui->info_tb->setHtml(dictIndex);
}

void MainWindow::getDictWord(QString word)
{
    // retrieve the word from ALL dictionaries after a link has been clicked
    ui->info_lbl_title->setText(word);
    QString result;
    QString desc;
    QString activeDicts = tr("Showing results for: ");

    QStringList dictTables;
    QSqlQuery query_get_tables("SELECT name FROM info WHERE strong = 'false' "
                               "ORDER BY name ASC", dictDb);
    while (query_get_tables.next())
        dictTables.append(query_get_tables.value(0).toString());

    QString sql_dict;
    QSqlQuery dict_query(dictDb);

    word.replace("'", "_");

    for (const QString &table : dictTables) {
        desc.clear();
        sql_dict = QString("SELECT topic, definition "
                           "FROM %1 WHERE topic LIKE '%2' ").arg(table.toLower(), word);

        dict_query.exec(sql_dict);
        while (dict_query.next())
            desc = dict_query.value(1).toString();

        if (not desc.isEmpty()) {
            activeDicts += QString("<a style='text-decoration:none;color:%2' "
                "href='dict:%1'>%1</a> ").arg(table, scheme["nrClr"]);
            result.append(QString("  <h2 id='%1' style='color:%2;"
                "font-family:serif;'>%1:</h2><br>").arg(table, scheme["titleClr"]));
            result += desc + "<br><br>";
        }
    }

    // if there are no results then use LIKE with a % to give more possible matches
    if (!result.isEmpty()) {
        ui->info_tb->setHtml(result);
    } else {
        for (const QString &table : dictTables) {
            desc.clear();
            sql_dict = QString("SELECT topic, definition "
                               "FROM %1 WHERE topic LIKE '%2%' ")
                                .arg(table.toLower(), word);

            dict_query.exec(sql_dict);
            while (dict_query.next())
                desc = dict_query.value(1).toString();

            if (not desc.isEmpty()) {
                activeDicts += QString("<a style='text-decoration:none;color:%2' "
                    "href='dict:%1'>%1</a> ").arg(table, scheme["nrClr"]);
                result.append(QString("  <h2 id='%1' style='color:%2;"
                    "font-family:serif;'>%1:</h2><br>").arg(table, scheme["titleClr"]));
                result += desc + "<br><br>";
            }
        }
    }

    if (!result.isEmpty()) {
        result.replace("<p/>", "<br><br>");
        result.replace("<p />", "<br><br>");

        QString anchor = QString("<a style='text-decoration:none;color:%1'")
                            .arg(scheme["nrClr"]);
        result.replace("<a", anchor);

        result.replace("href='S:", "href='dictword:");
        result.prepend(activeDicts + "<br>");

        ui->info_tb->setHtml(result);

        if (!dictwordHistory.isEmpty()) {
            if (word != dictwordHistory[0])
                dictwordHistory.prepend(word);
        } else {
            dictwordHistory.prepend(word);
        }
        if (dictwordHistory.size() > 10)
            dictwordHistory.removeLast();

    } else {
        ui->info_tb->setHtml( word + " not found in dictionaries");
    }
}

void MainWindow::getDictSug(QString word)
{
    // find word suggestions in dictionaries
    if (word.isEmpty())
        word = ui->dict_le->text();

    if (word.isEmpty() || word.length() < 1) {
        ui->info_lbl_title->clear();
        ui->info_tb->clear();
        return;
    }

    QString result;
    QString links;
    QString topic;
    QString aStyle = "text-decoration:none;font-weight:normal;color:" + scheme["nrClr"];
    ui->info_lbl_title->setText("index for: " + word);

    QStringList dictTables;
    QSqlQuery query_get_tables("SELECT name FROM info WHERE strong = 'false' "
                               "ORDER BY name ASC", dictDb);
    while (query_get_tables.next())
        dictTables.append(query_get_tables.value(0).toString());

    QString sql_dict;
    QSqlQuery dict_query(dictDb);
    for (const QString &table : dictTables) {
        sql_dict = QString("SELECT topic, definition "
                          "FROM '%1' WHERE topic like '%2%'").arg(table.toLower(), word);
        links.clear();

        dict_query.exec(sql_dict);
        while (dict_query.next()) {
            topic = dict_query.value(0).toString();
            links += QString("<a style='%1' href='dictword:%2'>%2</a><br>")
                    .arg(aStyle, topic);
        }

        if (not links.isEmpty()) {
            result.append("<big>" + table + ":</big><br><br>");
            result.append(links);
            result.append("<br><br>");
        }
    }

    if (!result.isEmpty())
        ui->info_tb->setHtml(result);
    else
        ui->info_tb->setHtml( word + " not found in dictionaries");
}

void MainWindow::addBookmark()
{
    QHash<QString, int> job;
    if (!printHistory.isEmpty())
        job = printHistory.last();
    else
        return;

    int bk = job["bk"];
    int c1 = job["c1"];
    int c2 = job["c2"];
    int v1 = job["v1"];
    int v2 = job["v2"];

    QString bookName = ::g_bookNames[bk];
    QString bookmarkName = QString("%1 %2").arg(bookName).arg(c1);

    if (c2 > c1)
        bookmarkName += QString("-%1").arg(c2);
    else if (v1 > 0) {
        bookmarkName += QString(":%1").arg(v1);
        if (v2 > v1)
            bookmarkName += QString("-%1").arg(v2);
    }

    QString sqlAdd =
            QString("INSERT INTO 'bookmarks' "
            "(name, book, chapter1, chapter2, verse1, verse2) "
            "VALUES ('%1', %2, %3, %4, %5, %6)")
            .arg(bookmarkName).arg(bk).arg(c1).arg(c2).arg(v1).arg(v2);
    QSqlQuery queryAdd(sqlAdd, bookmarksDb);
    queryAdd.exec();

    buildBookmarkMenu();
}

void MainWindow::renameBookmark()
{
    QStringList bookmarks;
    QSqlQuery getNames("SELECT name FROM 'bookmarks'", bookmarksDb);

    while (getNames.next()) {
        QString bookmarkName = getNames.value(0).toString();
        bookmarks.append(bookmarkName);
    }

    bool ok = false;
    QString oldName = QInputDialog::getItem(this, tr("rename bookmark"),
                        tr("Choose bookmark to rename:"), bookmarks, 0, false, &ok);

    if (ok && !oldName.isEmpty()) {
        ok = false;
        QString newName = QInputDialog::getText(this, tr("rename bookmark"),
                            tr("And the new name is:"), QLineEdit::Normal,"", &ok);

        if (ok && !newName.isEmpty()) {
            QString sqlUpdate = QString("UPDATE 'bookmarks' SET name = '%1' "
                                        "WHERE name = '%2' ").arg(newName, oldName);
            QSqlQuery updateQuery(sqlUpdate, bookmarksDb);

            if (updateQuery.exec()) {
                QList bkmAcs = ui->menu_bookmarks->actions();
                for (QAction *ac : bkmAcs)
                    if (ac->text() == oldName)
                        ac->deleteLater();

                buildBookmarkMenu();
            }
        }
    }
}

void MainWindow::deleteBookmark()
{
    QStringList bookmarks;
    QSqlQuery getNames("SELECT name FROM 'bookmarks'", bookmarksDb);
    while (getNames.next()) {
        QString bookmarkName = getNames.value(0).toString();
        bookmarks.append(bookmarkName);
    }

    bool ok = false;
    QString bookmarkName = QInputDialog::getItem(this, tr("delete bookmark"),
                            tr("bookmark to remove:"), bookmarks, 0, false, &ok);

    if (ok && !bookmarkName.isEmpty() ) {
        QString sqlDelete = QString("DELETE FROM 'bookmarks' "
                                    "WHERE name = '%1' ").arg(bookmarkName);
        QSqlQuery deleteQuery(sqlDelete, bookmarksDb);
        deleteQuery.exec();

        QList bkmAcs = ui->menu_bookmarks->actions();
        for (QAction *ac : bkmAcs)
            if (ac->text() == bookmarkName)
                ac->deleteLater();

        buildBookmarkMenu();
    }
}

void MainWindow::buildBookmarkMenu()
{
    QString sqlCreateTable = QString("CREATE TABLE IF NOT EXISTS 'bookmarks' "
                                     "(name TEXT NOT NULL UNIQUE, "
                                     "book INTEGER NOT NULL, "
                                     "chapter1 INTEGER NOT NULL, "
                                     "chapter2 INTEGER NOT NULL, "
                                     "verse1 INTEGER NOT NULL, "
                                     "verse2 INTEGER NOT NULL) "
                                     );
    QSqlQuery queryCreateTable(sqlCreateTable, bookmarksDb);
    queryCreateTable.exec();

    QSqlQuery getBookmarks("SELECT name FROM 'bookmarks'", bookmarksDb);
    QList bookmarkActions = ui->menu_bookmarks->actions();
    QStringList bookmarkNames;

    for (QAction *action : bookmarkActions )
        bookmarkNames.append(action->text());

    while (getBookmarks.next()) {
        QString bookmarkName = getBookmarks.value(0).toString();

        if (!bookmarkNames.contains(bookmarkName)) {
            QAction *bookmarkAction = new QAction(bookmarkName, this);
            bookmarkAction->setIcon(bookmarkIcon);
            connect(bookmarkAction, &QAction::triggered, this, [this, bookmarkName] () {
                    printBookmark(bookmarkName); } );
            ui->menu_bookmarks->addAction(bookmarkAction);
        }
    }
}

void MainWindow::printBookmark(const QString &bookmarkName)
{
    int bk = 0; int c1 = 0; int c2 = 0; int v1 = 0; int v2 = 0;

    QString sqlGet = QString("SELECT * FROM 'bookmarks' "
                             "WHERE name = '%1' ").arg(bookmarkName);
    QSqlQuery queryGet(sqlGet, bookmarksDb);

    while (queryGet.next() ){
        bk = queryGet.value(1).toInt();
        c1 = queryGet.value(2).toInt();
        c2 = queryGet.value(3).toInt();
        v1 = queryGet.value(4).toInt();
        v2 = queryGet.value(5).toInt();
    }

    if (bk == 0 || c1 == 0) return;

    QHash<QString, int> job;
    job["bk"] = bk;
    job["c1"] = c1;
    job["c2"] = 0;
    job["v1"] = 0;
    job["v2"] = 0;

    if (c2 > 0) {
        job["c2"] = c2;
    } else {
        if (v1 > 0) {
            job["v1"] = v1;

            if (v2 > 0) {
                job["v2"] = v2;
            }
        }
    }

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(c1 - 1);
    ui->tb_scriptures->clear();
    printQ.enqueue(job);
    printScriptures();
}

void MainWindow::on_cb_roster_read_clicked()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    bool checked = ui->cb_roster_read->isChecked();

    if(checked) {
        ui->tb_scriptures->clear();
        ui->lw_chapters->clear();
        ui->lbl_book_title->setText("");
        rosterRead = true;
        settings.setValue("rosters/rosterRead", "true");
    } else {
        rosterRead = false;
        settings.setValue("rosters/rosterRead", "false");
        loadRoster();
    }
}

void MainWindow::on_btn_next_session_clicked()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    ui->cb_roster_read->setChecked(false);
    rosterRead = false;
    settings.setValue("rosters/rosterRead", "false");

    QString rosterName = settings.value("rosters/activeRoster").toString();
    if ( rosterName.isEmpty() ) {
        return;
    }

    int finalSession = 0;
    QString sqlGetFinal = QString("SELECT COUNT(DISTINCT SESSION) FROM '%1' ").arg(rosterName);
    QSqlQuery getFin(sqlGetFinal, rosterDb);
    while (getFin.next() ) {
        finalSession = getFin.value(0).toInt();
    }

    int currentSession = 0;
    QString sqlGetCur = QString("SELECT SESSION FROM '%1' WHERE CURRENT = 'true'"
                                "ORDER BY SESSION DESC LIMIT 1 ").arg(rosterName);
    QSqlQuery getCur(sqlGetCur, rosterDb);
    while (getCur.next() ) {
        currentSession = getCur.value(0).toInt();
    }

    int nextSession = currentSession + 1;
    if (nextSession > finalSession) {
        printMsg("You have reached the end of the reading plan.");
        return;
    } else {
        updateSession(rosterName, nextSession);
        printSession(rosterName);
    }
}

void MainWindow::on_btn_prev_session_clicked()
{   
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    ui->cb_roster_read->setChecked(false);
    rosterRead = false;
    settings.setValue("rosters/rosterRead", "false");

    QString rosterName = settings.value("rosters/activeRoster").toString();
    if ( rosterName.isEmpty() ) {
        return;
    }

    int currentSession = 0;
    QString sqlGetCur = QString("SELECT SESSION FROM '%1' WHERE CURRENT = 'true'"
                                "ORDER BY SESSION DESC LIMIT 1 ").arg(rosterName);
    QSqlQuery getCur(sqlGetCur, rosterDb);
    while (getCur.next() ) {
        currentSession = getCur.value(0).toInt();
    }

    int prevSession = currentSession - 1;
    if (prevSession <= 0) {
        return;
    } else {
        updateSession(rosterName, prevSession);
        printSession(rosterName);
    }
}

void MainWindow::updateSession(const QString &rosterName, int session)
{
    // update current session in roster.db
    QString sqlClear = QString("UPDATE '%1' SET CURRENT = NULL "
                             "WHERE CURRENT = 'true' ").arg(rosterName);
    QSqlQuery clearCurrent(sqlClear, rosterDb);
    clearCurrent.exec();

    QString sqlSet = QString("UPDATE '%1' SET CURRENT = 'true' "
                             "WHERE SESSION = %2;").arg(rosterName).arg(session);
    QSqlQuery setCurrent(sqlSet, rosterDb);
    setCurrent.exec();
}

void MainWindow::printSession(const QString &rosterName)
{
    // print current reading plan session
    QString sql = QString("SELECT SESSION, BOOK, CHAPTER "
                          "from '%1' where CURRENT = 'true' ").arg(rosterName);
    QSqlQuery getSession(sql, rosterDb);
    int bk = 0, ch = 0;
    QString session;
    QStringList fullRosterName = rosterName.split(" ");
    QString shortRosterName = fullRosterName[0];

    QString finalSession;
    QString sqlGetFinal = QString("SELECT COUNT(DISTINCT SESSION) FROM '%1' ")
            .arg(rosterName);
    QSqlQuery getFin(sqlGetFinal, rosterDb);
    while (getFin.next() ) {
        finalSession = getFin.value(0).toString();
    }

    QHash<QString, int> job;
    QString rosterInfo = "<small>";
    QString info;
    QString bookname;

    while (getSession.next()) {
        session = getSession.value(0).toString();
        bk = getSession.value(1).toInt();
        ch = getSession.value(2).toInt();
        bookname = ::g_booksAbbr[bk];

        info = bookname + " " + getSession.value(2).toString() + " - " ;
        rosterInfo.append(info);

        job["bk"] = bk;
        job["c1"] = ch;
        printQ.enqueue(job);
    }

    // remove dash
    rosterInfo.remove(-2 ,3);

    rosterInfo.append("</small>");
    ui->lbl_roster_info->setText(rosterInfo);
    QString sessionTitle = QString("%1 %2/%3").arg(shortRosterName, session, finalSession);
    ui->lbl_roster_name->setText(sessionTitle);

    setBookTitle(shortRosterName);
    ui->lw_chapters->clear();
    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);

    if (rosterRead)
        printQ.clear();
    else
        processPrintQueue();
}

void MainWindow::on_action_delete_roster_triggered()
{
    if (rosterGroup->actions().isEmpty()) return;

    QStringList rosters;
    QList roGrAc = rosterGroup->actions();
    for (QAction *ac : roGrAc ) {
        rosters.append(ac->text());
    }

    bool ok = false;
    QString rosterName = QInputDialog::getItem(this, tr("remove roster"),
                                       tr("Roster to remove:"), rosters, 0, false, &ok);
    if (ok && !rosterName.isEmpty() ){
        QString sqlDrop = QString("DROP TABLE IF EXISTS '%1' ").arg(rosterName);
        QSqlQuery queryDrop(sqlDrop, rosterDb);
        queryDrop.exec();

        QSettings settings(settingsFile.fileName(), QSettings::IniFormat);

        if (rosterName == settings.value("rosters/activeRoster")) {
            settings.setValue("rosters/activeRoster", "");
            settings.setValue("rosters/rosterRead", "false");
            rosterRead = false;
            if (ui->frame_roster_btns->isVisible()) ui->frame_roster_btns->hide();
        }
        addRostersToMenu();

        QString msg = QString("%1 has been removed").arg(rosterName);
        printMsg(msg);
    }
}

void MainWindow::on_action_reset_roster_triggered()
{
    if (rosterGroup->actions().isEmpty())
        return;

    QString rosterName;
    QList rosAcList = rosterGroup->actions();

    for (QAction *ac : rosAcList) {
        if (ac->isChecked()) {
            rosterName = ac->text();
            break;
        }
    }

    if (rosterName.isEmpty()) return;

    QString msg = QString(tr("Set roster %1 to the first session?")).arg(rosterName);
    QMessageBox msgBox(QMessageBox::Question, tr("reset roster"),
                       msg, QMessageBox::Ok | QMessageBox::Cancel);
    int ret = msgBox.exec();

    if (ret == 1024) {
        updateSession(rosterName, 1);
        printSession(rosterName);
        rosterRead = false;
        QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
        settings.setValue("rosters/rosterRead", "false");
        ui->cb_roster_read->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::addRostersToMenu()
{
    QList rgActions = rosterGroup->actions();
    for ( QAction *ac : rgActions ) {
        delete ac;
    }

    QList rmActions = ui->menu_roster_select->actions();
    for ( QAction *ac : rmActions ) {
        delete ac;
    }

    QSqlQuery getRosters("SELECT name FROM sqlite_master WHERE type = 'table' ", rosterDb);

    while (getRosters.next()) {
        QString rosterName = getRosters.value(0).toString();
        QAction *rosterAction = new QAction(rosterName, rosterGroup );
        connect(rosterAction, &QAction::triggered, this, [this, rosterName]()
            { rosterActionTriggered(rosterName); });
        rosterAction->setCheckable(true);
    }
    ui->menu_roster_select->addActions(rosterGroup->actions());

    setActiveRoster();
}

void MainWindow::rosterActionTriggered(QString rosterName)
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("rosters/activeRoster", rosterName);
    setActiveRoster();
    loadRoster();
}

void MainWindow::setActiveRoster()
{
    //set active roster in menu
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString activeRoster = settings.value("rosters/activeRoster").toString();
    if ( activeRoster.isEmpty() ) {
        return;
    }

    QList rgActions = rosterGroup->actions();
    for (QAction *ac : rgActions ) {
        if( ac->text() == activeRoster) {
            ac->setChecked(true);
            break;
        }
    }
}

void MainWindow::loadRoster()
{
    // select the active roster
    if (!ui->bible_frame->isVisible()) ui->bible_frame->show();

    QString noPlan = tr("No reading plan available.<br> "
            "Go to menu <i>Daily</i> and create a plan.<br> "
                        "And select it in the same menu. ");

    if (ui->menu_roster_select->actions().isEmpty()) {
        printMsg(noPlan);
        ui->frame_roster_btns->hide();
        return;
    }

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString rosterName = settings.value("rosters/activeRoster").toString();
    if ( rosterName.isEmpty() ) {
        printMsg(noPlan);
        ui->frame_roster_btns->hide();
        return;
    } else {
        rosterRead ? ui->cb_roster_read->setChecked(true) :
                     ui->cb_roster_read->setChecked(false);
        ui->frame_roster_btns->show();
        ui->tabwidget->setCurrentIndex(2);
        ui->tb_scriptures->clear();
        ui->lbl_book_title->setText("");
        ui->lw_chapters->clear();

        printSession(rosterName);
    }
}

void MainWindow::readingPlan()
{
    if (ui->frame_roster_btns->isVisible()){
        ui->frame_roster_btns->hide();
        ui->tb_scriptures->clear();
        ui->lbl_book_title->setText("");
    } else {
        loadRoster();
    }
}

void MainWindow::openParW()
{
    QHash<QString, int> job;
    if(!printHistory.empty())
        job = printHistory.last();
    else
        job = { {"bk", 1}, {"c1", 1} };

    emit setParwStyle(scheme);
    emit parOpened(activeTl.toUpper(), job);
    !parW->isVisible() ? parW->show() : parW->raise();
}

void MainWindow::openImgW(const QString &imgName)
{
    QPixmap mapPix {};
    QString sqlGetImg {QString("select name, content from img_data where "
                               "name is '%1'").arg(imgName)};
    QSqlQuery queryImg(sqlGetImg, varDb);

    if (queryImg.next())
        mapPix.loadFromData(queryImg.value(1).toByteArray());

    emit setImgWindowPixmap(mapPix, imgName);
}

void MainWindow::applyFont(
        const QString &font, const QString &fontS,
        const int &margin, const int &width,
        const bool scrCheck, const bool bkCheck, const bool chCheck)
{
    // apply changes from settings window, font, margin, width
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    int ps;

    if (font.isEmpty() or fontS.isEmpty()) {
        *scripFont = settings.value("font/font", "sans").toString();
        ps = settings.value("font/fontsize", "12").toInt();
        scripFont->setPointSize(ps);
    } else {
        *scripFont = font;
        ps = fontS.toInt();
        scripFont->setPointSize(ps);
    }

    // a custom font or the default app font which is def os font
    scrCheck ? ui->tb_scriptures->document()->setDefaultFont(*scripFont) :
               ui->tb_scriptures->document()->setDefaultFont(QApplication::font());
    bkCheck ? ui->lw_books->setFont(*scripFont) :
              ui->lw_books->setFont(QApplication::font());
    chCheck ? ui->lw_chapters->setFont(*scripFont) :
              ui->lw_chapters->setFont(QApplication::font());

    ui->tb_scriptures->document()->setDocumentMargin(margin);
    ui->info_tb->document()->setDocumentMargin(10);
    ui->strongs_tb->document()->setDocumentMargin(10);

    int maxWidth = 16777215;
    int bgfWidth = ui->background_frame->width();
    if (width >= 10 or width <= 1) {
        maxWidth = 16777215;
        ui->splitter_central->setHandleWidth(0);
    } else {
        frameWidth = width;
        ui->splitter_central->setHandleWidth(1);
        maxWidth = (bgfWidth / 10) * width;
    }
    ui->bible_frame->setMaximumWidth(maxWidth);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    // set the width of the bible frame on every resize of the window
    int maxWidth = 16777215;
    int bgfWidth = ui->background_frame->width();

    if (frameWidth >= 10 or frameWidth <= 1) {
        ui->splitter_central->setHandleWidth(0);
    } else {
        ui->splitter_central->setHandleWidth(1);
        maxWidth = (bgfWidth / 10) * frameWidth;
    }
    ui->bible_frame->setMaximumWidth(maxWidth);

    event->accept();
}

void MainWindow::spokenWord()
{
    // this only works if audio bible mp3's are available, check menu help-modifications"
    QString sBk = ui->lw_books->currentIndex().data(0x0100).toString();
    QString sCh = ui->lw_chapters->currentIndex().data(0x0100).toString();

    if (sBk.length() == 1)
        sBk.prepend("0");

    if (sCh.length() == 1)
        sCh.prepend("00");
    else if (sCh.length() == 2)
        sCh.prepend("0");

    QStringList files = QDir(::userDataDir.path()+ "/audio-bible").entryList();

    QString pattern = QString("^%1.*%2.mp3").arg(sBk,sCh);
    audioBibleRegex->setPattern(pattern);

    QFileInfo audioBibleBook;
    bool hasMatch = false;

    for (const QString &file : files) {
        QRegularExpressionMatch match = audioBibleRegex->match(file);
        if (match.hasMatch()) {
            audioBibleBook.setFile(::userDataDir.path()+ "/audio-bible/" + file);
            hasMatch = true;
            break;
        }
    }

    if (hasMatch) {
        QString absPath = audioBibleBook.absoluteFilePath();
        QString basename = audioBibleBook.baseName();
        playMusic(absPath, basename);
    } else {
        printMsg("could not find an mp3 file for this chapter, "
            "see menu help-modifications to learn how to add mp3's");
    }
}

void MainWindow::playRandom()
{
    if(!musicList.isEmpty()) {
        std::shuffle(musicList.begin(), musicList.end(),
                     std::default_random_engine(std::random_device()()));
        QString absPath = musicList[0][0];
        QString basename = musicList[0][1];
        playMusic(absPath, basename);
    } else {
        printMsg("no music found, "
            "see menu help-modifications to learn how to add music");
    }
}

void MainWindow::stopPlayer()
{
    if (!mplayer->isPlaying()) {
        ui->action_stop_media->setText(tr("Stop playing"));
        ui->action_stop_media->setEnabled(false);
    }
}

bool MainWindow::compareFunctionR(QAction *a, QAction *b)
{
    // to sort the psalms by nr for easy display in the music menu
    static QRegularExpression re("\\s*(?<ps>[Pp]salm)"
                                 "\\s*(?<ch>\\d?\\d?\\d?)");
    QString aNr, bNr;
    QRegularExpressionMatch matchA = re.match(a->text());
    QRegularExpressionMatch matchB = re.match(b->text());
    aNr = matchA.captured("ch");
    bNr = matchB.captured("ch");

    return aNr.toInt() < bNr.toInt();
}

bool MainWindow::compareFunctionS(QAction *a, QAction *b)
{
    return a->text() < b->text();
}

void MainWindow::showText(const QString &filepath, const QString &filename)
{
    // show a contemplation text, markdown format
    ui->info_tb->clear(); ui->info_frame->show();
    ui->info_lbl_title->setText(filename);
    QString atext;

    QFile textFile(filepath);
    if (!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    atext = textFile.readAll();
    atext.replace("<a href", QString("<a style='text-decoration:none;color:%1' href")
                        .arg(scheme["nrClr"]));

    ui->info_tb->setMarkdown(atext);
}

void MainWindow::makeMusicMenu()
{
    QDirIterator it(::userDataDir.path()+ "/music", QStringList()
                    << "*.mp3", QDir::NoFilter, QDirIterator::Subdirectories);

    QList<QAction*> psalmActions;
    QList<QAction*> hymnActions;
    QList<QAction*> divActions;

    while (it.hasNext()) {
        QFileInfo f(it.next());
        QAction *musAction = new QAction(musIcon, f.baseName(), this);
        connect(musAction, &QAction::triggered, this, [this, f]() {
            playMusic( f.absoluteFilePath(), f.baseName() );
        });

        if (f.path() == ::userDataDir.path()+ "/music/psalms")
            psalmActions.append(musAction);
        else if (f.path() == ::userDataDir.path()+ "/music/hymns")
            hymnActions.append(musAction);
        else
            divActions.append(musAction);

        QList<QString> info = {f.absoluteFilePath(), f.baseName()};
        musicList.append(info);
    }

    std::sort(psalmActions.begin(), psalmActions.end(), compareFunctionR );
    std::sort(hymnActions.begin(), hymnActions.end(), compareFunctionS );
    std::sort(divActions.begin(), divActions.end(), compareFunctionS);

        for (QAction *ac : psalmActions)
            ui->menu_psalms->addAction(ac);
        for (QAction *ac : hymnActions)
            ui->menu_hymns->addAction(ac);
        for (QAction *ac : divActions)
            ui->menu_music_word->addAction(ac);
}

void MainWindow::makeTextMenuItems()
{
    // add text about filters and populate notes menu
    QList<QAction*> txtActions;

    QAction *immAction = new QAction(docIcon, tr("about Immersion"), this);
    QAction *judAction = new QAction(docIcon, tr("about Judeans"), this);
    connect(immAction, &QAction::triggered, this, [this]() {
                showText(":/data/txt/immersion.md", "immersion"); });
    connect(judAction, &QAction::triggered, this, [this]() {
                showText(":/data/txt/judeans.md", "Judeans"); });

    ui->menu_filters->addSeparator();
    ui->menu_filters->addAction(immAction);
    ui->menu_filters->addAction(judAction);

    if (!filtersMenuVisible)
        ui->menu_bar->removeAction(ui->menu_filters->menuAction());

    QDirIterator it(::userDataDir.path()+ "/notes", QStringList() <<
                    "*.md" << "*.txt", QDir::NoFilter, QDirIterator::Subdirectories);

    if (!it.hasNext()) {
            QAction *txtAction = new QAction(docIcon, "see menu help-modifications "
                                                      "on how to add notes", this);
            txtAction->setDisabled(true);
            txtActions.append(txtAction);
    } else {
        while (it.hasNext()) {
            QFileInfo f(it.next());
            QAction *txtAction = new QAction(docIcon, f.baseName(), this);
            connect(txtAction, &QAction::triggered, this, [this, f]() {
                showText(f.absoluteFilePath(), f.baseName());
            });
            txtActions.append(txtAction);
        }
    }

    std::sort(txtActions.begin(), txtActions.end(), compareFunctionS);

    for (QAction *ac: txtActions)
        ui->menu_text_word->addAction(ac);
}

void MainWindow::playMusic(QString filepath, QString filename)
{
    mplayer->stop(); audioOutput->setVolume(20);
    mplayer->setSource(QUrl::fromLocalFile(filepath));
    mplayer->play();

    ui->action_stop_media->setEnabled(true);
    ui->action_stop_media->setText(tr("Stop playing: ") + filename);

    QRegularExpressionMatch psalmMatch = psalmRegex->match(filename);
    QRegularExpressionMatch hymnMatch = hymnRegex->match(filepath);
    QRegularExpressionMatch scripMatch = scripRegex->match(filename);

    if (psalmMatch.hasMatch()) {
        int ch;
        QString chS;
        chS = psalmMatch.captured("ch");
        ch = chS.toInt();

        ui->lw_books->setCurrentRow(19 - 1);
        setBookTitle();
        updateChapterWidget();
        ui->lw_chapters->setCurrentRow(ch - 1);

        QHash<QString, int> job;
        job["bk"] = 19;
        job["c1"] = ch;
        printQ.enqueue(job);
        processPrintQueue();
    } else if (hymnMatch.hasMatch()) {
        ui->info_tb->clear(); ui->info_frame->show();

        QFile hymnFile(QString(::userDataDir.path()
                               + "/music/hymns/%1.txt").arg(filename));
        if (!hymnFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        ui->info_lbl_title->setText(filename);

        QString hymnText;
        while (!hymnFile.atEnd())
            hymnText += hymnFile.readLine() + "<br>";

        ui->info_tb->setHtml(QString("<br><div style='%1'>%2</div>")
                             .arg(devStyle, hymnText) );

    } else if (scripMatch.hasMatch() and !filepath.contains("audio-bible")) {
        printRequestSingle(filename);
    }
}

void MainWindow::theLordsPrayer()
{
    printQ.enqueue( { {"bk", 40}, {"c1", 6}, {"v1", 5}, {"v2", 15} });
    printQ.enqueue( { {"bk", 42}, {"c1", 11}, {"v1", 1}, {"v2", 13} });

    setBookTitle(tr("The Lord's Prayer"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::breakingBread()
{
    printQ.enqueue( { {"bk", 40}, {"c1", 26}, {"v1", 26}, {"v2", 29} });
    printQ.enqueue( { {"bk", 41}, {"c1", 14}, {"v1", 22}, {"v2", 25} });
    printQ.enqueue( { {"bk", 42}, {"c1", 22}, {"v1", 7}, {"v2", 20} });
    printQ.enqueue( { {"bk", 44}, {"c1", 2}, {"v1", 42} });
    printQ.enqueue( { {"bk", 46}, {"c1", 10}, {"v1", 16}, {"v2", 17} });
    printQ.enqueue( { {"bk", 46}, {"c1", 11}, {"v1", 20}, {"v2", 32} });

    setBookTitle(tr("Breaking Bread"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::immersion()
{
    printQ.enqueue( { {"bk", 40}, {"c1", 3} });
    printQ.enqueue( { {"bk", 41}, {"c1", 1}, {"v1", 1}, {"v2", 8} });
    printQ.enqueue( { {"bk", 42}, {"c1", 3}, {"v1", 7}, {"v2", 18} });
    printQ.enqueue( { {"bk", 43}, {"c1", 3}, {"v1", 22}, {"v2", 36} });
    printQ.enqueue( { {"bk", 44}, {"c1", 1}, {"v1", 4}, {"v2", 5} });
    printQ.enqueue( { {"bk", 44}, {"c1", 2}, {"v1", 36}, {"v2", 42} });
    printQ.enqueue( { {"bk", 44}, {"c1", 8}, {"v1", 12} });
    printQ.enqueue( { {"bk", 44}, {"c1", 8}, {"v1", 35}, {"v2", 38} });
    printQ.enqueue( { {"bk", 44}, {"c1", 9}, {"v1", 17}, {"v2", 18} });
    printQ.enqueue( { {"bk", 44}, {"c1", 10}, {"v1", 34}, {"v2", 48} });
    printQ.enqueue( { {"bk", 44}, {"c1", 16 }, {"v1", 14}, {"v2", 15} });
    printQ.enqueue( { {"bk", 44}, {"c1", 16 }, {"v1", 30}, {"v2", 34} });
    printQ.enqueue( { {"bk", 44}, {"c1", 18 }, {"v1", 7}, {"v2", 8} });
    printQ.enqueue( { {"bk", 44}, {"c1", 19 }, {"v1", 1}, {"v2", 6} });
    printQ.enqueue( { {"bk", 45}, {"c1", 6 } });
    printQ.enqueue( { {"bk", 48}, {"c1", 3 }, {"v1", 27} });

    setBookTitle(tr("Immersion"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::salvation()
{
    printQ.enqueue({ {"bk", 21}, {"c1", 12}, {"v1", 13}, {"v2", 14} });
    printQ.enqueue({ {"bk", 40}, {"c1", 22}, {"v1", 36}, {"v2", 40} });
    printQ.enqueue({ {"bk", 41}, {"c1", 12}, {"v1", 28}, {"v2", 34} });

    printQ.enqueue({ {"bk", 43}, {"c1", 14}, {"v1", 15}, {"v2", 21} });
    printQ.enqueue({ {"bk", 44}, {"c1", 2}, {"v1", 36}, {"v2", 42} });
    printQ.enqueue({ {"bk", 44}, {"c1", 4}, {"v1", 8}, {"v2", 12} });
    printQ.enqueue({ {"bk", 45}, {"c1", 6}, {"v1", 1}, {"v2", 14} });
    printQ.enqueue({ {"bk", 60}, {"c1", 3}, {"v1", 18}, {"v2", 22} });
    printQ.enqueue({ {"bk", 59}, {"c1", 2}, {"v1", 14}, {"v2", 26} });
    printQ.enqueue({ {"bk", 66}, {"c1", 14}, {"v1", 12} });

    setBookTitle(tr("salvation"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::worries()
{
    printQ.enqueue({ {"bk", 40}, {"c1", 6}, {"v1", 20}, {"v2", 34} });
    printQ.enqueue({ {"bk", 42}, {"c1", 21}, {"v1", 34}, {"v2", 36} });
    printQ.enqueue({ {"bk", 44}, {"c1", 20}, {"v1", 24} });
    printQ.enqueue({ {"bk", 46}, {"c1", 7}, {"v1", 29}, {"v2", 35} });
    printQ.enqueue({ {"bk", 60}, {"c1", 5}, {"v1", 5}, {"v2", 11} });

    setBookTitle(tr("worries"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::showEmpireMaps()
{
    ui->tb_scriptures->clear();
    ui->lw_books->clearSelection();
    ui->lw_chapters->clear();
    ui->lbl_book_title->setText("");

    QString sqlGetMaps {"select name from img_data where name like '%empire%' "
                        "or name like '%nations%' or name like '%world%'"};
    QSqlQuery querygetMaps(sqlGetMaps, varDb);
    QString mapName {};

    while (querygetMaps.next()) {
        mapName = querygetMaps.value(0).toString();
        ui->tb_scriptures->insertHtml(QString(
                            "<a href='img:%1' style='text-decoration:none;"
                            "color:%2;font-size:large;'>"
                            "<img width='28' height='28' "
                            "src=':/data/img/map.png'>&nbsp;<i>%1</i>&nbsp;&nbsp;"
                            "</a><br>"
                            ).arg(mapName, scheme.value("titleClr")));
    }

    ui->bible_frame->show();
}

void MainWindow::showShortcuts()
{
    ui->info_tb->clear(); ui->info_lbl_title->clear(); ui->info_frame->show();

    QString info;
    info.append("<style>table {margin:20px}; td {padding-right:10px}; tr {margin-top:300px}</style>"
                "<table>"
                "<tr><td>next chapter</td> <td>plus</td><td>right arrow</td></tr>"
                "<tr><td>previous chapter</td> <td>minus</td><td>left arrow</td></tr>"
                "<tr></tr>"
                "<tr><td>contents tab</td> <td>alt + 1</td></tr>"
                "<tr><td>search tab</td> <td>alt + 2</td></tr>"
                "<tr><td>today tab</td> <td>alt + 3</td></tr>"
                "<tr></tr>"
                "<tr><td>parallel window</td> <td>ctrl + p</td></tr>"
                "<tr></tr>"
                "<tr><td>search in page</td> <td>ctrl + f</td></tr>"
                "<tr><td>find next, previous</td> <td>f3</td><td>f2</td></tr>"
                "<tr></tr>"
                "<tr><td>toggle tab widget</td> <td>F8</td></tr>"
                "<tr><td>toggle bible frame</td> <td>F9</td></tr>"
                "<tr><td>toggle info frame</td> <td>F10</td></tr>"
                "<tr><td>toggle fullscreen</td> <td>F11</td></tr>"
                "<tr><td>toggle menu</td> <td>F12</td></tr>"
                "<tr><td>(disables many shortcuts)</td></tr>"
                "<tr></tr>"
                "<tr><td>quit</td> <td>ctrl + q</td></tr>"
                "</table>"
                "<br>Hints:<br><br>"
                "Hold ctrl to select more chapters in the chapter list<br>"
                "Hold shift to select a range<br><br>"
                "The lineEdit in the content tab accepts more than one entry<br>"
                "e.g. ps 1-5 act 2:38<br><br>"
                "The following frames can be collapsed by dragging the splitter's handle:<br>"
                "chapter frame, info frame, strongs frame<br>"
                );
    ui->info_tb->setHtml(info);
    ui->info_lbl_title->setText(tr("shortcuts"));
}

void MainWindow::showEncPic(const QString &fileName)
{
    ui->bible_frame->hide(); ui->info_frame->hide();
    ui->background_frame->setStyleSheet(
                QString("#background_frame {background-image: url(%1); "
                "background-repeat: no-repeat; background-position: center}").arg(fileName));
}

void MainWindow::changeEncPic()
{
    if (!encPics.isEmpty()) {
        std::shuffle(encPics.begin(), encPics.end(),
                     std::default_random_engine(std::random_device()()));
        QString fileName(encPics[0]);
        QPixmap encPic(fileName);
        QPixmap encPicS(encPic.scaledToWidth(230));

        if(encPicS.height() > 200)
            encPicLbl->setPixmap(encPicS.scaledToHeight(200));
        else
            encPicLbl->setPixmap(encPicS);

        ui->pb_enc_img->disconnect(); // remove old signals else weird things happen!
        connect(ui->pb_enc_img, &QPushButton::clicked, this, [this, fileName]() {
            showEncPic(fileName);});
        connect(ui->pb_enc_img, &QPushButton::customContextMenuRequested,
                this, &MainWindow::changeEncPic);
    }
}

void MainWindow::setEncPic()
{
    ui->pb_enc_img->setText("");

    QDir dir(::dataDir.path() + "/img");
    if (!dir.exists()) {
        ::sout << "Cannot find the img directory in app data dir" <<  Qt::endl;
        return;
    }

    QStringList filters;
    filters << "*.png" << "*.jpg";
    dir.setNameFilters(filters);

    if (!dir.isEmpty()) {
        QDirIterator it(dir, QDirIterator::NoIteratorFlags);
        while (it.hasNext())
            encPics.append(it.next());

        QVBoxLayout *encPicLay = new QVBoxLayout();
        encPicLay->setContentsMargins(1,1,1,1);
        encPicLay->setSizeConstraint(QLayout::SetMinAndMaxSize);

        encPicLay->addWidget(encPicLbl);
        ui->pb_enc_img->setLayout(encPicLay);
    } else {
        ::sout << "no files in the images directory";
    }
}

void MainWindow::printEncTxt(int bk, int ch, int vs, const QString &verse)
{
    QHash<QString, int> job = { {"bk", bk}, {"c1", ch} };
    printQ.enqueue(job);

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(ch - 1);

    processPrintQueue();

    ui->tb_scriptures->moveCursor(QTextCursor().Start);
    if (not ui->tb_scriptures->find(verse))
        ui->tb_scriptures->find(QString::number(vs) + " ");
}

void MainWindow::changeEncTxt()
{
    std::shuffle(encScrip.begin(), encScrip.end(),
                 std::default_random_engine(std::random_device()()));
    int bk = encScrip[0][0], ch = encScrip[0][1], vs = encScrip[0][2];
    QString verse = getVerse(bk, ch, vs);

    // remove html tags since QLabel can't handle them
    verse.remove(*tagRegex);
    encTxtLbl->setText(verse);

    ui->pb_enc_txt->disconnect(); // remove old signals else weird things happen!
    connect(ui->pb_enc_txt, &QPushButton::clicked, this, [this, bk, ch, vs, verse]() {
        printEncTxt(bk, ch, vs, verse);});
    connect(ui->pb_enc_txt, &QPushButton::customContextMenuRequested,
            this, &MainWindow::changeEncTxt);
}

void MainWindow::setEncTxt()
{
    encTxtLbl->setWordWrap(true);
    encTxtLbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    encTxtLbl->setFixedWidth(220);

    QVBoxLayout *encTxtLay = new QVBoxLayout();
    encTxtLay->setSizeConstraint(QLayout::SetMinAndMaxSize);
    encTxtLay->addWidget(encTxtLbl);  
    ui->pb_enc_txt->setLayout(encTxtLay);
}

void MainWindow::escapeKey()
{
    if (this->isFullScreen())
        this->showNormal();
    else if (ui->menu_bar->isHidden())
        ui->menu_bar->show();
    else if (ui->tabwidget->isHidden())
        ui->tabwidget->show();
    else if (ui->find_frame->isVisible())
        ui->find_frame->hide();
}

void MainWindow::todaysProverb()
{
    ui->frame_roster_btns->hide();
    int bk = 20;
    int ch = ui->calendar->selectedDate().day();

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(ch - 1);

    QHash<QString, int> job;
    job["bk"] = bk;
    job["c1"] = ch;
    printQ.enqueue(job);
    processPrintQueue();
}

void MainWindow::todaysPsalm()
{
    ui->frame_roster_btns->hide();
    int today = ui->calendar->selectedDate().day();

    int bk = 19;
    ui->lw_books->setCurrentRow(bk - 1);
    int lastCh = (today * 5) + 1;
    int firstCh;

    if ( today == 1 )
        firstCh = 1;
    else
        firstCh = lastCh - 4;

    int ch;
    QList<int> psalms;

    if ( today == 31 ) {
        ch  = 119;
    } else {
        for ( int i = firstCh; i <= lastCh; ++i ) {
            if ( i != 119 and i != 151 )
                psalms.append(i);
        }

        std::shuffle(psalms.begin(), psalms.end(),
                     std::default_random_engine(std::random_device()() ));
        ch = psalms.first();
    }

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(ch - 1);

    QHash<QString, int> job;
    job["bk"] = bk;
    job["c1"] = ch;
    printQ.enqueue(job);
    processPrintQueue();
}

void MainWindow::morningAndEvening(const QString &morningOrEvening)
{
    int today = ui->calendar->selectedDate().dayOfYear();
    QString devotion;
    QString desc =  " by C.H. Spurgeon";
    if (morningOrEvening == "mbm")
        desc.prepend("Morning by morning");
    else
        desc.prepend("Evening by evening");

    QString sql = QString("select day, devotion from %1 where day = %2")
            .arg(morningOrEvening).arg(today);
    QSqlQuery query(sql, varDb);

    while (query.next())
        devotion.append(query.value(1).toString());

    devotion.replace("<p/>", "<br><br>");
    devotion.replace("<p />", "<br><br>");
    devotion.replace("<a href", QString("<a style='color:%1' href").arg(scheme["nrClr"]));

    ui->info_lbl_title->setText(desc);
    breakItUp(devotion);
    ui->info_tb->setHtml(QString("<div style='%1'>%2</div>")
                         .arg(devStyle, devotion));

    QRegularExpressionMatch urlMatch = SpurgeonUrlRegex->match(devotion);
    if (urlMatch.hasMatch()) {
        QString match = urlMatch.captured("scrip");
        QUrl mbmUrl("b:" + match);
        on_info_tb_anchorClicked(mbmUrl);
    }

    ui->info_frame->show();
}

void MainWindow::todaysLetter()
{
    ui->frame_roster_btns->hide();
    QList<int> letters = {45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65};
    std::shuffle(letters.begin(), letters.end(),
                 std::default_random_engine(std::random_device()() ));
    int bk = letters[0];

    int finalChapter = Utilities::getChapterCount(bk);

    QList<int> chapters;
    for (int ch = 1; ch <= finalChapter; ++ch)
        chapters.append(ch);

    std::shuffle(chapters.begin(), chapters.end(),
                 std::default_random_engine(std::random_device()() ));
    int ch = chapters[0];

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(ch - 1);

    QHash<QString, int> job;
    job["bk"] = bk;
    job["c1"] = ch;
    printQ.enqueue(job);
    processPrintQueue();
}

void MainWindow::on_search_tb_anchorClicked(const QUrl &url)
{
    QString url_string = url.toString(QUrl::RemoveScheme);
    QStringList url_list = url_string.split("/");

    int bk = url_list[0].toInt();
    int ch = url_list[1].toInt();
    int vs = url_list[2].toInt();
    QHash<QString, int> job = { {"bk", bk}, {"c1", ch} };
    printQ.enqueue(job);

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(ch - 1);

    processPrintQueue();

    QFlags<QTextDocument::FindFlag> flags = QTextDocument().FindWholeWords;
    ui->tb_scriptures->moveCursor(QTextCursor().Start);
    // this will not find the correct verse if there are multiple verses with the same content,
    // eg lev 6:8 will just highlight 6:1
    if (not ui->tb_scriptures->find(getVerse(bk, ch, vs), flags))
        ui->tb_scriptures->find(url_list[2]);
}

QString MainWindow::getVerse(const int bk, const int ch, const int vs)
{
    QString verse;
    QString sql = QString("SELECT t from t_%1 where b = %2 and c = %3 and v = %4 ")
            .arg(activeTl).arg(bk).arg(ch).arg(vs);

    QSqlQuery query(sql, bibleDb);
    while (query.next())
        verse = query.value(0).toString();
    return verse;
}

void MainWindow::on_info_tb_anchorClicked(const QUrl &url)
{
    // url links: cross-refs, strongs, topics, mbm ebe, dictionary
    QString sUrl = url.toString(QUrl::RemoveScheme);
    QString bookName;
    QString goodUrl;

    // cross-references, topical
    if (url.scheme() == "bible") {
        printRequest(sUrl, "en");
    // code follows database href tags which is a bit awkward perhaps
    } else if (url.scheme() == "strongs" || url.scheme() == "s" ) {
        getStrongs(sUrl);
        QString check = ui->strongs_tb->toPlainText();

        if (check == "no results\n\ntry a valid strongs number or word\n") {
            ui->strongs_tb->clear();
            getDictWord(sUrl);
        }
    } else if (url.scheme() == "b") {
            QStringList wrongUrl = sUrl.split(" ");
            QString sql = "select normal from convert where weird = " + wrongUrl[0];

            QSqlQuery query(sql, dictDb);
            while (query.next()) {
                bookName = ::g_bookNames[query.value(0).toInt()];
            }
            if (!bookName.isEmpty()) {
                goodUrl = bookName + " " + wrongUrl[1];
                printRequest(goodUrl);
            } else
                return;
    } else if (url.scheme() == "dictword") {
        getDictWord(sUrl);
    } else if (url.scheme() == "dict") {
        ui->info_tb->scrollToAnchor(sUrl);
    } else if (sUrl.startsWith("#")) {
        ui->info_tb->scrollToAnchor(sUrl.sliced(1));
    } else if (url.scheme() == "topic") {
        getTopic(sUrl);
    } else if (url.scheme() == "topical-index") {
        showTopics();
    }
}

void MainWindow::on_strongs_tb_anchorClicked(const QUrl &url)
{
    QString sUrl = url.toString(QUrl::RemoveScheme);

    if (url.scheme() == "twot") {
        getTWOT(sUrl);
    } else if (url.scheme() == "b") {
        QString bookName;
        QString goodUrl;
        QStringList urlList = sUrl.split(" ");
        QString sql = "select normal from convert where weird = " + urlList[0];

        QSqlQuery query(sql, dictDb);
        while (query.next())
            bookName = ::g_bookNames[query.value(0).toInt()];

        if (!bookName.isEmpty()) {
            goodUrl = bookName + " " + urlList[1];
            printRequest(goodUrl);
        } else
            return;
    } else if (url.scheme() == "ln" or url.scheme() == "oshl") {
        ui->strongs_tb->moveCursor(QTextCursor().Start);
        ui->strongs_tb->insertHtml("<p style='color:brown;'>This database is not included.</p>");
        return;
    } else {
        getStrongs(sUrl);
    }
}

void MainWindow::on_tb_scriptures_anchorClicked(const QUrl &url)
{
    // show a footnote or process a strongs nr
    QString sUrl = url.toString(QUrl::RemoveScheme);

    if (url.scheme() == "note") {
        QList urlList = sUrl.split(" ");
        QString bk = urlList[0];
        QString ch = urlList[1];
        QString vs = urlList[2];
        QString marker = "[" + urlList[3] + "]";
        QString note = "";
        QString limited =
                "<br>This free version has the full NET Bible text and limited notes "
                "(the first chapter of every book (66 chapters) and the remaining "
                "chapters display notes in the first three verses).<br>";

        // table name always starts with notes_
        QString sql = QString("select book, chapter, "
                            "verse, marker, note "
                            "from %1 where book is %2 and "
                            "chapter is %3 and "
                            "verse is %4 and "
                            "marker is '%5'").arg(activeTl.toLower(),
                                                  bk, ch, vs, marker);
        QSqlQuery query(sql, varDb);
        if (query.next()) {
            note = query.value(4).toString();
            ui->strongs_tb->clear();
            if (note.startsWith("tn")) {
                ui->strongs_lbl->setText(marker);
                note.append("<br><br><small>Translator’s Note - explains the "
                            "rationale "
                             "for the translation and gives alternative "
                             "translations, interpretive options, and other "
                             "technical information.</small><br><br>");
            } else if (note.startsWith("sn")) {
                ui->strongs_lbl->setText(marker);
                note.append("<br><br><small>Study Note - includes comments about "
                            "historical "
                             "or cultural background, explanation of obscure "
                             "phrases or brief discussions of context, discussions "
                             "of the theological point made by the biblical author, "
                             "cross references and references to Old Testament "
                             "quotations or allusions in the New Testament, "
                             "or other miscellaneous information helpful to the "
                             "modern reader.</small><br><br>");
            } else if (note.startsWith("tc")) {
                ui->strongs_lbl->setText(marker);
                note.append("<br><br><small>Text Critical Note - "
                            "discusses alternate (variant) "
                             "readings found in the various manuscripts and "
                             "groups of manuscripts of the Hebrew Old Testament "
                             "and Greek New Testament.</small><br><br>");
            } else if (note.startsWith("map")) {
                ui->strongs_lbl->setText(marker);
                note.append("<br><br><small>Map Note - gives map coordinates for "
                            "site within "
                             "the two map sections, “The Journeys of Paul” and "
                             "“The Holy Land from the Heavens.”</small><br><br>");
            } else {
                ui->strongs_lbl->setText("");
            }

            note.prepend("<br>");
            note.replace("<a href=",
                         QString("<a style='text-decoration:none;color:%1;' href=")
                         .arg(scheme["nrClr"]));

            breakItUp(note);
            ui->strongs_tb->setHtml(note);
            ui->info_frame->show();
        } else {
            ui->strongs_lbl->setText("limited footnotes");
            ui->strongs_tb->setHtml(limited);
            ui->info_frame->show();
        }
    } else if (url.scheme() == "img") {
        openImgW(sUrl);
    } else {
        getStrongs(sUrl);
    }
}

void MainWindow::breakItUp(QString &textwall)
{
    // check for end of linechar and lenght, since we want
    // linebreaks for better readability
    QList<QString> endChar = {".", "!", "?"};
    QList<int> breaks;
    int textL {0};
    QChar testChar {};

    for (int i = 0; i < textwall.length() - 1; i++) {
        textL += 1;
        testChar = textwall.at(i);
        if (endChar.contains(testChar) && textL > 280) {
            if (textwall.at(i+1).isSpace()){
                breaks.append(i);
                textL = 0;
            }
        }
    }

    // insert break after end of line character + space
    // and adjust for new size caused by the insert
    int correction = 2;
    for(int pos : std::as_const(breaks)) {
        textwall.insert(pos + correction, "<br><br>");
        correction += 8;
    }
}

void MainWindow::versesWithStrongNumber(const QString &strongs, const QString &where)
{
    ui->search_tb->clear();
    int counter = 0;
    QString lookWhere;
    QString StrongsPrefix = strongs.at(0);

    // we only search for a number without a G/H prefix (which does not exist in db)
    // and  we have to make sure we only allow a query if the location is valid (OT or NT)

    if (where.startsWith("in")) { // genre
        QString themeStr = where.at(where.length() - 2);
        int themeNr = themeStr.toInt();

        if (StrongsPrefix == "H" and themeNr < 5)
            lookWhere = where;
        else if (StrongsPrefix == "G" and themeNr > 4)
            lookWhere = where;
        else
            lookWhere = "0";

    } else if (StrongsPrefix == "H" and where == "between 1 and 66") { // Hebrew word
        lookWhere = "between 1 and 39";
    } else if (StrongsPrefix == "H" and where == "between 40 and 66") { // not OT
        lookWhere = "0";
    } else if (StrongsPrefix == "H" and where != "between 1 and 39") { // one book
        lookWhere = where;
    } else if (StrongsPrefix == "H") {
        QString bookNrStr = where.last(2);
        int bookNrInt = bookNrStr.toInt();
        if (bookNrInt > 39)
            lookWhere = "0";
        else
            lookWhere = where;

    } else if (StrongsPrefix == "G" and where == "between 1 and 66") { // Greek word
        lookWhere = "between 40 and 66";
    } else if (StrongsPrefix == "G" and where == "between 1 and 39") { // not NT
        lookWhere = "0";
    } else if (StrongsPrefix == "G" and where != "between 40 and 66") { // one book
        QString bookNrStr = where.last(2);
        int bookNrInt = bookNrStr.toInt();
        if (bookNrInt < 40)
            lookWhere = "0";
        else
            lookWhere = where;

    } else {
        lookWhere = where;
    }

    QString sql_findStrongNr = QString("select b, c, v, t from %1 "
                               "where t like '%<S>%2</S>%' "
                               "and b %3").arg(strongTl, strongs.mid(1), lookWhere);
    QSqlQuery query(sql_findStrongNr, bibleDb);

    int bk;
    QString bkS;
    while (query.next()) {
        bk = query.value(0).toInt();
        bkS = query.value(0).toString();
        counter += 1;
        QString bookname = ::g_bookNames[bk];
        QString c1 = query.value(1).toString();
        QString v1 = query.value(2).toString();
        QString txt = query.value(3).toString();
        txt.replace("<S>" + strongs.mid(1) + "</S>", " <b>" + strongs.mid(1) + "</b> ");
        txt.replace("<S>", " ");
        txt.replace("</S>", " ");

        QString link = QString("%1/%2/%3").arg(bkS, c1, v1) ;
        QString match = QString(
                "<h3><a style='color:%6' href='%5'> %1 %2:%3 </a></h3>%4<br><br>")
                .arg(bookname, c1, v1, txt, link, scheme["nrClr"]);
        ui->search_tb->insertHtml(match);
    }

    QString intro;
    if (counter == 0)
        intro = QString("<center><h3>'%1'<br>was not found</h3></center>")
                .arg(strongs);
    else if (counter == 1)
        intro = QString("<center><h3>'%1'<br>was found in one verse</h3></center>")
                .arg(strongs);
    else
        intro = QString("<center><h3>'%1'<br>was found in %2 verses</h3></center>")
                .arg(strongs, QString::number(counter));

    ui->search_tb->moveCursor(QTextCursor().Start);
    intro += "<br><br>";
    ui->search_tb->insertHtml(intro);
}

void MainWindow::searchScriptures()
{
    QString what = ui->search_le_what->text().trimmed();
    QString cbWhere = ui->search_cb_where->currentData(0x0100).toString();
    QString where;

    if (cbWhere == "this") {
        where = "= " + ui->lw_books->currentIndex().data(0x0100).toString();
    } else if (cbWhere == "all") {
        where = "between 1 and 66";
    } else if (cbWhere == "O.T.") {
        where = "between 1 and 39";
    } else if (cbWhere == "N.T.") {
        where = "between 40 and 66";
    } else {
        where = "in (";

        QSqlQuery getBookNrs("select book_nr "
                            "from books_info "
                            "where genre_nr = "
                            + cbWhere, booksDb);
        while (getBookNrs.next())
            where.append(getBookNrs.value(0).toString() + ",");

        where.chop(1);
        where.append(")");
    }

    QRegularExpressionMatch match = getStrongRegex->match(what);
    if (match.hasMatch()) {
        QString strongs;
        strongs = match.captured().toUpper();
        versesWithStrongNumber(strongs, where);
        return;
    }

    QString tl = "t_" + activeTl;

    QString globOrLike; // sql glob or like
    QString astOrPer; // asterisk or percentage operator

    if (ui->search_chkb_case->isChecked()) {
        globOrLike = "GLOB";
        astOrPer = "*";
    }
    else {
        globOrLike = "LIKE";
        astOrPer = "%";
    }

    if (what.length() > 1) {
        ui->search_tb->setHtml("<h3 align='center'>Patience is a virtue....</h3>");
        ui->search_tb->repaint();
    } else {
        ui->search_tb->setHtml("<h3 align='center'>Search input is empty "
                               "or less than 2 characters.</h3>");
        return;
    }

    QString first_word;
    QString whereClause;
    QStringList searchList = what.split(" ");
    QString anyExact = ui->search_cb_anyExact->currentData(0x0100).toString();

    // build the where clause of the SQL query
    // LIKE = case-insensitive (for ASCII characters)
    // can't do [,.!;:?] with LIKE
    // GLOB = case-sensitive
    if (anyExact == "any") {
        first_word = searchList[0];
        // whole words, use () to separate the words
        if (ui->search_chkb_whole->isChecked()) {
            whereClause = QString("( t %1 '%2 %3 %2' AND b %4 "
                                "OR t %1 '%3 %2' AND b %4 "
                                "OR t %1 '%2 %3' AND b %4 "
                                "OR t %1 '%2 %3,%2' AND b %4 "
                                "OR t %1 '%2 %3.%2' AND b %4 "
                                "OR t %1 '%2 %3!%2' AND b %4 "
                                "OR t %1 '%2 %3;%2' AND b %4 "
                                "OR t %1 '%2 %3:%2' AND b %4 "
                                "OR t %1 '%2 %3?%2' AND b %4 ) ")
                .arg(globOrLike, astOrPer, first_word, where);

            if (searchList.length() > 1) {
                for (QString &word : searchList.mid(1, -1)) {
                    whereClause += QString("AND ( t %1 '%2 %3 %2' AND b %4 "
                                        "OR t %1 '%3 %2' AND b %4 "
                                        "OR t %1 '%2 %3' AND b %4 "
                                        "OR t %1 '%2 %3,%2' AND b %4 "
                                        "OR t %1 '%2 %3.%2' AND b %4 "
                                        "OR t %1 '%2 %3!%2' AND b %4 "
                                        "OR t %1 '%2 %3;%2' AND b %4 "
                                        "OR t %1 '%2 %3:%2' AND b %4 "
                                        "OR t %1 '%2 %3?%2' AND b %4 ) ")
                        .arg(globOrLike, astOrPer, word, where);
                }
            }

        // can be part of a word
        } else {
            whereClause = QString("( t %1 '%2%3%2' AND b %4 "
                                "OR t %1 '%3%2' AND b %4 "
                                "OR t %1 '%2%3' AND b %4 "
                                "OR t %1 '%2%3,%2' AND b %4 "
                                "OR t %1 '%2%3.%2' AND b %4 "
                                "OR t %1 '%2%3!%2' AND b %4 "
                                "OR t %1 '%2%3;%2' AND b %4 "
                                "OR t %1 '%2%3:%2' AND b %4 "
                                "OR t %1 '%2%3?%2' AND b %4 ) ")
                .arg(globOrLike, astOrPer, first_word, where);

            if (searchList.length() > 1) {

                for (QString &word : searchList.mid(1, -1)) {
                    whereClause += QString("AND ( t %1 '%2%3%2' AND b %4 "
                                           "OR t %1 '%3%2' AND b %4 "
                                           "OR t %1 '%2%3' AND b %4 "
                                           "OR t %1 '%2%3,%2' AND b %4 "
                                           "OR t %1 '%2%3.%2' AND b %4 "
                                           "OR t %1 '%2%3!%2' AND b %4 "
                                           "OR t %1 '%2%3;%2' AND b %4 "
                                           "OR t %1 '%2%3:%2' AND b %4 "
                                           "OR t %1 '%2%3?%2' AND b %4 ) ")
                           .arg(globOrLike, astOrPer, word, where);
                }
            }
        }
    // exact match, no AND clauses
    } else {
        whereClause = QString("t %1 '%2 %3 %2' AND b %4 "
                              "OR t %1 '%3 %2' AND b %4 "
                              "OR t %1 '%2 %3' AND b %4 "
                              "OR t %1 '%2 %3,%2' AND b %4 "
                              "OR t %1 '%2 %3.%2' AND b %4 "
                              "OR t %1 '%2 %3!%2' AND b %4 "
                              "OR t %1 '%2 %3;%2' AND b %4 "
                              "OR t %1 '%2 %3:%2' AND b %4 "
                              "OR t %1 '%2 %3?%2' AND b %4 ")
                          .arg(globOrLike, astOrPer, what, where);
    }

    QString selectStr = QString("SELECT b,c,v,t FROM %1 WHERE %2").arg(tl, whereClause);
    QSqlQuery query(selectStr, bibleDb);
    int counter = 0; // count the matches per verse
    ui->search_tb->clear();

    while (query.next()) {
        counter += 1;
        QString bookname = ::g_bookNames[query.value(0).toInt()];
        QString bk = query.value(0).toString();
        QString c1 = query.value(1).toString();
        QString v1 = query.value(2).toString();
        QString txt = query.value(3).toString();
        QString link = QString("%1/%2/%3").arg(bk, c1, v1) ;
        QString match = QString(
                    "<h3><a style='color:%1;text-decoration:underline;' "
                    "href='%2'>  %3 %4:%5 </a></h3>"
                    "%6<br><br>")
                    .arg(scheme["nrClr"], link, bookname, c1, v1, txt);
        ui->search_tb->insertHtml(match);
    }

    // lets highlight all matches and count the results
    QTextCharFormat boldF;
    boldF.setFontWeight(QFont::Bold);
    QFlags<QTextDocument::FindFlag> ourFlags;

    if (ui->search_chkb_whole->isChecked())
        ourFlags = ourFlags | QTextDocument::FindWholeWords;

    if (ui->search_chkb_case->isChecked())
        ourFlags = ourFlags | QTextDocument::FindCaseSensitively;


    if (anyExact == "any order") {
        for (const QString &word : std::as_const(searchList)) {
            ui->search_tb->moveCursor(QTextCursor().Start);

            while (ui->search_tb->find(word, ourFlags))
                ui->search_tb->textCursor().mergeCharFormat(boldF);
        }
    } else {
        ui->search_tb->moveCursor(QTextCursor().Start);

        while (ui->search_tb->find(what, ourFlags))
            ui->search_tb->textCursor().mergeCharFormat(boldF);
    }

    QString intro;
    if (counter == 0) {
        intro = QString("<center><h3>%1<br>was not found</h3></center><br>")
                .arg(what);
    } else if (counter == 1) {
        intro = QString("<center><h3>%1<br>was found in one verse</h3></center><br>")
                .arg(what);
    } else {
        QString count = QString::number(counter);
        intro = QString("<center><h3>%1<br>was found in %2 verses</h3></center><br>")
                .arg(what, count);
    }

    ui->search_tb->moveCursor(QTextCursor().Start);
    ui->search_tb->insertHtml(intro);
}

void MainWindow::printRequestSingle(const QString &request)
{
    // called by playMusic() Do english lookup only. For one chapter.

    int bkNr = 0, chNr1 = 0 , vsNr1 = 0, vsNr2 = 0;
    QRegularExpressionMatch match = scripRegex->match(request);

    if (match.hasMatch()) {
        ui->tb_scriptures->clear();
        QString testBook;
        testBook.clear();

        bkNr = 0; chNr1 = 0 ; vsNr1 = 0; vsNr2 = 0;

            if (!match.captured("prt").isEmpty()) {
                QString part = match.captured("prt");
                testBook.append(part + " ");
            }
            if (!match.captured("bk").isEmpty()) {
                QString book = match.captured("bk");
                testBook.append(book.toUpper());
            }
            if (!match.captured("ch1").isEmpty()) {
                QString chapter1 = match.captured("ch1");
                chNr1 = chapter1.toInt();
            }
            if (!match.captured("vs1").isEmpty()) {
                QString verse1 = match.captured("vs1");
                vsNr1 = verse1.toInt();
            }
            if (!match.captured("vs2").isEmpty()) {
                QString verse2 = match.captured("vs2");
                 vsNr2 = verse2.toInt();
            }

        // get the book number
        QSqlQuery query("SELECT book_nr, name FROM lang_en", booksDb);

        while (query.next()) {
            if (testBook == query.value(1).toString().toUpper()) {
                bkNr = query.value(0).toInt();
                break;
            }
        }

        if (bkNr == 0) {
            QSqlQuery query("SELECT abbreviation, book_nr FROM lang_en", booksDb);

            while (query.next()) {
                if (testBook == query.value(0).toString().toUpper()) {
                    bkNr = query.value(1).toInt();
                    break;
                }
            }
        }

        // get chapter number(s)
        if (bkNr > 0) {
            int finalChapter = Utilities::getChapterCount(bkNr);
            if (chNr1 <= 0) chNr1 = 1;
            if (chNr1 > finalChapter) chNr1 = finalChapter;

                // single chapter, perhaps with verse(s)
            if (vsNr1 > 0) {
                if (vsNr2 > 0){
                    QHash<QString, int> job = { {"bk", bkNr}, {"c1", chNr1},
                                                {"v1", vsNr1}, {"v2", vsNr2} };
                    printQ.enqueue(job);

                } else {
                    QHash<QString, int> job = { {"bk", bkNr}, {"c1", chNr1}, {"v1", vsNr1} };
                    printQ.enqueue(job);
                }
            } else {
                QHash<QString, int> job = { {"bk", bkNr}, {"c1", chNr1} };
                printQ.enqueue(job);
            }
        bkNr < 40 ? ui->lw_books->setCurrentRow(bkNr - 1) : ui->lw_books->setCurrentRow(bkNr);
        setBookTitle();
        updateChapterWidget();
        ui->lw_chapters->setCurrentRow(chNr1 - 1);
        }
    }
    processPrintQueue();
}

void MainWindow::printRequest(const QString &request, const QString &language)
{
    // scan a string and see if scriptures can be found and printed

    // some request are english language only, eg topical and crossref
    QString bookLang;
    if (language == "en")
        bookLang = "en";
    else
        bookLang = bknLanguage;

    if (request.simplified().length() < 2 ) {
        ui->lw_books->clearSelection();
        ui->lw_chapters->clear();
        ui->lbl_book_title->setText("");
        if (!ui->bible_frame->isVisible()) ui->bible_frame->show();
        printMsg("input too short");
        return;
    }

    QRegularExpressionMatchIterator gmatch = scripRegex->globalMatch(request.simplified());
    QRegularExpressionMatch match;
    QString testBook, testBookNoSpace;
    int bkNr = 0, chNr1 = 0 , chNr2 = 0, vsNr1 = 0, vsNr2 = 0;

    while (gmatch.hasNext()) {
        testBook.clear();
        testBookNoSpace.clear();

        match = gmatch.next();
        bkNr = 0; chNr1 = 0 ; chNr2 = 0; vsNr1 = 0; vsNr2 = 0;
        if (match.hasMatch()) {
            if (!match.captured("prt").isEmpty()) {
                QString part = match.captured("prt");
                testBook.append(part + " ");
            }
            if (!match.captured("bk").isEmpty()) {
                QString book = match.captured("bk");
                testBook.append(book);
            }
            if (!match.captured("ch1").isEmpty()) {
                QString chapter1 = match.captured("ch1");
                chNr1 = chapter1.toInt();
            }
            if (!match.captured("ch2").isEmpty()){
                QString chapter2 = match.captured("ch2");
                chNr2 = chapter2.toInt();
            }
            if (!match.captured("vs1").isEmpty()) {
                QString verse1 = match.captured("vs1");
                vsNr1 = verse1.toInt();
            }
            if (!match.captured("vs2").isEmpty()) {
                QString verse2 = match.captured("vs2");
                 vsNr2 = verse2.toInt();
            }
        }

        testBookNoSpace = testBook = testBook.toUpper();
        testBookNoSpace.replace(" ", "");
        // try to get the book number
        QSqlQuery query("SELECT book_nr, name, abbreviation FROM lang_" + bookLang, booksDb);

        while (query.next()) {
            if (testBook == query.value(1).toString().simplified().toUpper()) {
                // compare name
                bkNr = query.value(0).toInt();
                break;
            } else if (testBook == query.value(2).toString().toUpper()) {
                // compare abbreviation
                bkNr = query.value(0).toInt();
                break;
            } else if (testBookNoSpace == query.value(2).toString().toUpper()) {
                // compare abbreviation without spaces eg 1 Jn -> 1Jn
                bkNr = query.value(0).toInt();
                break;
            }
            else if (query.value(1).toString().toUpper().startsWith(testBook)) {
                // compare partial name
                bkNr = query.value(0).toInt();
                break;
            }
        }

        // get chapter and verse number(s)
        if (bkNr > 0) {
            int finalChapter = Utilities::getChapterCount(bkNr);
            if (chNr1 <= 0)
                chNr1 = 1;
            if (chNr1 > finalChapter)
                chNr1 = finalChapter;

            QHash<QString, int> job = {
                { "bk", bkNr },
                { "c1", chNr1 }, { "c2", chNr2},
                { "v1", vsNr1 }, { "v2", vsNr2}
            };

            printQ.append(job);
        }

    } //  endof while gmatch.hasNext()

    // if at least one book has been found
    if (bkNr > 0) {
        bkNr < 40 ? ui->lw_books->setCurrentRow(bkNr - 1) : ui->lw_books->setCurrentRow(bkNr);
        setBookTitle();
        updateChapterWidget();
        ui->lw_chapters->setCurrentRow(chNr1 - 1);
        processPrintQueue();
    } else {
        ui->lw_books->clearSelection();
        ui->lw_chapters->clear();
        ui->lbl_book_title->setText("");
        if (!ui->bible_frame->isVisible()) ui->bible_frame->show();
        printMsg("no book found");
    }
}

void MainWindow::centerApp()
{
    auto appGeo = frameGeometry();
    auto screenGeoCenter = screen()->geometry().center();
    appGeo.moveCenter(screenGeoCenter);
    move(appGeo.topLeft());
}

void MainWindow::nextChapter()
{
    if (ui->lw_chapters->count() == 0) {return;}

    if (ui->lw_chapters->selectedItems().length() > 0 ) {
        int curChap = ui->lw_chapters->currentItem()->data(0x0100).toInt();
        ui->lw_chapters->clearSelection();
        int finChap = ui->lw_chapters->count();
        if ( (curChap + 1) <= finChap) {
            ui->lw_chapters->setCurrentRow(curChap);
            chapterSelected();
        }
    } else {
            ui->lw_chapters->setCurrentRow(0);
            chapterSelected();
    }
}

void MainWindow::prevChapter()
{
    if (ui->lw_chapters->count() == 0) {return;}

    if (ui->lw_chapters->selectedItems().length() > 0 ) {
        int curChap = ui->lw_chapters->currentItem()->data(0x0100).toInt();
        ui->lw_chapters->clearSelection();
        if ( (curChap - 1) > 0) {
            ui->lw_chapters->setCurrentRow(curChap - 2);
            chapterSelected();
        }
    } else {
            ui->lw_chapters->setCurrentRow(0);
            chapterSelected();
    }
}

void MainWindow::createOtNtMenus()
{
    otMenu->clear();
    ntMenu->clear();
    QString sql = QString(
              "SELECT books_info.book_nr, name, testament "
              "FROM lang_%1 "
              "INNER JOIN books_info "
              "ON books_info.book_nr = lang_%1.book_nr "
              ).arg(bknLanguage);

    QSqlQuery query(sql, booksDb );

    while (query.next()) {
        int bkNr = query.value(0).toInt();
        QString bkName = query.value(1).toString();
        QAction *bookAction = new QAction(scrollIcon, bkName, this);
        connect(bookAction, &QAction::triggered, this, [bkNr, this]() {
            popupChapters(bkNr); });

        if (query.value(2) == "OT")
            otMenu->addAction(bookAction);
        else if (query.value(2) == "NT")
            ntMenu->addAction(bookAction);
    }
}

void MainWindow::ccMenuBackground()
{
    //custom context menu for background frame
    QMenu ccMenu(this);
    QAction *parAction = ccMenu.addAction(bookOpenIcon, tr("Open Parallel Window"));
    QAction *showBibleAction = ccMenu.addAction(bookOpenIcon, tr("Show Bible Frame"));
    QAction *showInfoAction = ccMenu.addAction(docIcon, tr("Show Info Frame"));

    QAction *action = ccMenu.exec(QCursor().pos());
    if (action == parAction)
        openParW();
    else if (action == showBibleAction)
        ui->bible_frame->show();
    else if (action == showInfoAction)
        ui->info_frame->show();
}

void MainWindow::ccMenuInfo()
{
    //custom context menu for info_frame
    QMenu ccMenu(this);
    QMenu *histMenu = ccMenu.addMenu(tr("history"));

    for (const QString &histItem : std::as_const(dictwordHistory)) {
        QAction *histAction = new QAction(histItem);
        connect(histAction, &QAction::triggered, this, [this, histItem] () {
                getDictWord(histItem) ;});
        histMenu->addAction(histAction);
    }

    QAction *findAction = ccMenu.addAction(findIcon, tr("find"));
    ccMenu.addSeparator();
    QAction *selectAction = ccMenu.addAction(selectIcon, tr("select all"));
    QAction *copyAction = ccMenu.addAction(copyIcon, tr("copy"));
    ccMenu.addSeparator();
    QAction *closeAction = ccMenu.addAction(closeIcon, tr("close"));
    QAction *action = ccMenu.exec(QCursor().pos());

    if (action == closeAction)
        closeInfo();
    else if (action == findAction)
        showFindFrame();
    else if (action == selectAction)
        ui->info_tb->selectAll();
    else if (action == copyAction)
        ui->info_tb->copy();
}

void MainWindow::ccMenuStrongs()
{
    //custom context menu for info_frame
    QMenu ccMenu(this);
    QMenu *histMenu = ccMenu.addMenu(tr("history"));

    for (QString histItem : std::as_const(strongsHistory)) {
        QAction *histAction = new QAction(histItem);
        if (histItem[0] == QString("T")) {
            connect(histAction, &QAction::triggered, this, [this, histItem] () {
                getTWOT(histItem) ;});
        } else {
            connect(histAction, &QAction::triggered, this, [this, histItem] () {
                getStrongs(histItem) ;});
        }
        histMenu->addAction(histAction);
    }

    QAction *findAction = ccMenu.addAction(findIcon, tr("find"));
    ccMenu.addSeparator();
    QAction *selectAction = ccMenu.addAction(selectIcon, tr("select all"));
    QAction *copyAction = ccMenu.addAction(copyIcon, tr("copy"));
    ccMenu.addSeparator();
    QAction *closeAction = ccMenu.addAction(closeIcon, tr("close"));
    QAction *action = ccMenu.exec(QCursor().pos());

    if (action == closeAction)
        closeInfo();
    else if (action == findAction)
        showFindFrame();
    else if (action == selectAction)
        ui->strongs_tb->selectAll();
    else if (action == copyAction)
        ui->strongs_tb->copy();
}

void MainWindow::ccMenuBibleFrame()
{
    QMenu ccMenu(this);

    QAction *nextAction = ccMenu.addAction(nextIcon, tr("next"));
    QAction *prevAction = ccMenu.addAction(prevIcon, tr("previous"));
    QAction *chapAction = ccMenu.addAction(tr("chapters"));
    QAction *otPopupAction = ccMenu.addAction("OT");
    QAction *ntPopupAction = ccMenu.addAction("NT");
    ccMenu.addSeparator();

    QAction *parAction = ccMenu.addAction(bookOpenIcon, tr("Open Parallel Window"));
    QAction *strongAction = ccMenu.addAction(strongIcon, tr("Strongify"));
    QAction *crossrefAction = ccMenu.addAction(tr("show cross references"));

    ccMenu.addSeparator();

    QAction *audioAction = ccMenu.addAction(playIcon, tr("play this chapter"));
    QAction *stopAudioAction = ccMenu.addAction(stopIcon, tr("stop playing"));
    ccMenu.addSeparator();

    QAction *selectAction = ccMenu.addAction(selectIcon, tr("select all"));
    QAction *copyAction = ccMenu.addAction(copyIcon, tr("copy"));
    QAction *findAction = ccMenu.addAction(findIcon, tr("find"));

    ccMenu.addSeparator();
    QString thisBook("this book");
    if (ui->lw_books->currentRow() > 0) {
        int bkNr = ui->lw_books->currentItem()->data(0x0100).toInt();
        thisBook = ::g_bookNames[bkNr];
    }
    QAction *aboutBookAction = ccMenu.addAction(docIcon,tr("about ") + thisBook);
    QAction *aboutTlAction = ccMenu.addAction(docIcon, tr("about ") + activeTl);
    ccMenu.addSeparator();
    QAction *viewAction = ccMenu.addAction(tr("toggle table/book layout"));
    ccMenu.addSeparator();
    QAction *hideAction = ccMenu.addAction(closeIcon, tr("close"));

    QAction *action = ccMenu.exec(QCursor().pos());

    if (action == parAction)
        openParW();
    else if (action == strongAction)
        strongify();
    else if (action == crossrefAction)
        makeCrossRefs();
    else if (action == otPopupAction)
        otMenu->exec(QCursor().pos());
    else if (action == ntPopupAction)
        ntMenu->exec(QCursor().pos());
    else if (action == chapAction)
        popupChapters();
    else if (action == nextAction)
        nextChapter();
    else if (action == prevAction)
        prevChapter();

    else if (action == selectAction)
        ui->tb_scriptures->selectAll();
    else if (action == copyAction)
        ui->tb_scriptures->copy();
    else if (action == findAction)
        showFindFrame();
    else if (action == aboutBookAction)
        showAboutBook();
    else if (action == aboutTlAction)
        showAboutTl();

    else if (action == audioAction)
        spokenWord();
    else if (action == stopAudioAction)
        mplayer->stop();

    else if (action == hideAction)
        ui->bible_frame->hide();
    else if (action == viewAction) {
        scripLayout == "table" ? scripLayout = "book" : scripLayout = "table";
        ui->tb_scriptures->clear();
        if (not ui->menu_history->actions().isEmpty())
            ui->menu_history->actions().constLast()->activate(QAction::Trigger);
    }
}

void MainWindow::showTopics()
{
    // topical bible references, show all topic names
    ui->info_tb->clear();
    ui->info_lbl_title->setText("topical references");
    ui->info_frame->show();

    if (topicalIndex.isEmpty()) {
        QString topicsSql = QString("SELECT DISTINCT Topic FROM topical;");
        QSqlQuery topicsQuery = QSqlQuery(topicsSql, varDb);
        QString top;

        while (topicsQuery.next()) {
            top = topicsQuery.value(0).toString();
            topicalIndex.append(QString("<a style='padding-left:10px;text-decoration:none;"
                                        "color:%2' href='topic:%1'>%1</a><br>")
                                        .arg(top, scheme["txtClr"]));
        }
    }

    ui->info_tb->setHtml(topicalIndex);
}

void MainWindow::getTopic(const QString &topic)
{
    // topical bible references, show one topic with references
    ui->info_tb->clear();
    ui->info_lbl_title->setText(topic);
    ui->info_frame->show();

    QString topicSql = QString("SELECT Verse FROM topical WHERE Topic = '%1' "
                                "ORDER BY Votes DESC").arg(topic);
    QSqlQuery topicQuery = QSqlQuery(topicSql, varDb);
    QString results;
    QString clr = "black";
    results.append(QString(
                    "<center><br><a style='font-family:serif;"
                    "text-decoration:underline;font-weight:normal;color:%1' "
                    "href='topical-index:'>index</a></center><br>")
                    .arg(scheme["titleClr"]));

    bool firstClr = true;
    QString res;

    while (topicQuery.next()) {
        res = topicQuery.value(0).toString();

        //  change the bible reference format to a format soulanchor uses
        res.replace(*bkRegex, "\\1 \\2:\\3");
        // a chapter range, keep chapter nr
        res.replace(*chRegex, "-\\3");

        if (firstClr) {
            clr = scheme["nrClr"];
            firstClr = false;
        } else {
            clr = scheme["txtClr"];
            firstClr = true;
        }

        results.append(QString("<center><a style='text-decoration:none;color:%2' "
                                "href='bible:%1'>  %1</a></center><br>").arg(res, clr));
    }

    ui->info_tb->setHtml(results);
}

void MainWindow::makeCrossRefs()
{
    // create cross references and show on the info pane
    QHash<QString, int> job;
    if (!printHistory.isEmpty()){
        job = printHistory.last();
    } else {
        return;
    }

    int bk = job["bk"];
    int ch = job["c1"];
    const QString bkStr = QString::number(bk);
    const QString cStr = QString::number(ch);

    // convert to abbreviation that the database uses
    QString bkAbbr = crossrefDict[bkStr];

    if (bkAbbr.isEmpty()) {
        ui->info_tb->clear();
        ui->info_lbl_title->clear();
        return;
    } else {
        ui->info_frame->show();
        ui->info_tb->clear();
        ui->info_lbl_title->setText("cross-references");
    }

    // get total number of verses
    int numberOfVerses = 0;

    QString getN = QString("SELECT count(v) FROM t_%1 WHERE b = %2 and c = %3 ")
            .arg(activeTl, bkStr, cStr);
    QSqlQuery getNumberQ(getN, bibleDb);

    while (getNumberQ.next()) {
        numberOfVerses = getNumberQ.value(0).toInt();
    }

    QStringList results;
    QString verse;
    QString verseQ;
    QString headerVerse1;
    QString headerVerse2;
    QSqlQuery verseQuery;

    // iterate over every verse to get the votes per verse
    for (int v = 1; v <= numberOfVerses  ; ++v) {
        verse = QString().number(v);
        // db format = bookname.chapterNr.verseNr
        verseQ = QString("SELECT FromVerse, ToVerse, Votes FROM crossrefs "
                    "WHERE FromVerse = '%1.%2.%3' ORDER BY Votes DESC")
                    .arg(bkAbbr, cStr, verse);
        verseQuery = QSqlQuery(verseQ, varDb);

        while (verseQuery.next()) {
            headerVerse1 = verseQuery.value(0).toString();
            if (headerVerse1 != headerVerse2) {
                results.append("header" + headerVerse1);
                headerVerse2 = headerVerse1;
            }
            results.append(verseQuery.value(1).toString());
        }
    }

    // process the list, print a verse header and then the refs
    QString res, header, clrs;
    bool firstClr = true;

    while (!results.isEmpty()) {
        res = results.takeFirst();
        if (res.startsWith("header")) {
            res.remove(0,6);
            res.replace(*bkRegex, "\\1 \\2:\\3");
            ui->info_tb->insertHtml(
                        QString("<br><h4><a style='color:%1;text-decoration:none' "
                            "href='bible:%2'>%2</a> </h4>")
                            .arg(scheme["titleClr"], res));
            header = res;
            firstClr = true;
        } else {
            // change the bible reference format to a format soulanchor uses
            res.replace(*bkRegex, "\\1 \\2:\\3");

            // a chapter range, keep chapter nr
            res.replace(*chRegex
                        , "-\\3");

            if (firstClr) {
                clrs = scheme["nrClr"];
                firstClr = false;
            } else {
                clrs = scheme["txtClr"];
                firstClr = true;
            }
            ui->info_tb->insertHtml(QString("<a style='text-decoration:none;color:%3' "
                                "href='bible:%1 %2'>%2</a> ").arg(header, res, clrs));
        }
    }
}

void MainWindow::showAboutBook()
{
    ui->info_tb->clear(); ui->info_lbl_title->clear();

    if (!ui->info_frame->isVisible()) {
        ui->info_frame->show();
        QPropertyAnimation *animation = new QPropertyAnimation(ui->info_frame, "maximumWidth");
        animation->setDuration(500);
        animation->setStartValue(0);
        animation->setEndValue(2000);
        animation->start(QPropertyAnimation::DeleteWhenStopped);
        ui->info_frame->setMaximumWidth(16777215);
    }

    ui->info_tb->setCurrentCharFormat(emptyFormat);

    if (ui->lw_books->selectedItems().count() == 0) {
        ui->info_lbl_title->setText(tr("no book selected?"));
        return;
    }

    QString bkNr = ui->lw_books->currentItem()->data(0x0100).toString();
    QString title;

    // default is english dictionaries only
    QString sql = QString("SELECT book_nr, name FROM lang_EN "
                          "WHERE book_nr = %1").arg(bkNr);
    QSqlQuery query(sql, booksDb);
    while (query.next())
        title  = query.value(1).toString();

    title.remove(*nrRegex);
    getDictSug(title);
}

void MainWindow::showAboutTl(){
    // show information about the active translation from the database
    ui->info_tb->clear(); ui->info_lbl_title->clear(); ui->info_frame->show();
    ui->info_tb->setCurrentCharFormat(emptyFormat);

    QString sql = QString("SELECT info FROM version_info "
                          "WHERE abbreviation IS '%1'").arg(activeTl);
    QSqlQuery query(sql, bibleDb );
    QString info;

    while (query.next())
        info = query.value(0).toString();

    ui->info_lbl_title->setText(activeTl);
    if (info.isEmpty())
        ui->info_tb->setText("no information available");
    else
        ui->info_tb->setText(info);
}

void MainWindow::popupChapters(int bkNr)
{
    // show a chapters menu, activated from the custom-context-menu

    if (bkNr == 0) {
        if (ui->lw_books->selectedItems().count() == 0) {
            printMsg("What book?");
            return;
        } else {
            bkNr = ui->lw_books->currentItem()->data(0x0100).toInt();
            if (bkNr == 0) {
                printMsg("What book?");
                return;
            }
        }
    } else {
        bkNr < 40 ? ui->lw_books->setCurrentRow(bkNr - 1) : ui->lw_books->setCurrentRow(bkNr);
        bookSelected();
    }

    QString bookName = ::g_bookNames[bkNr];
    int finalChapter = Utilities::getChapterCount(bkNr);
    QMenu chapMenu(bookName, this);

    QAction *title = chapMenu.addAction(bookName);
    title->setEnabled(false);
    chapMenu.addSeparator();

    for (int i = 1; i <= finalChapter; ++i)
        chapMenu.addAction(QString::number(i));

    QAction *action = chapMenu.exec(QCursor().pos());
    if (action != nullptr) {
        int chapNr = action->text().toInt();
        QHash<QString, int> job = { {"bk", bkNr}, {"c1", chapNr} };
        printQ.enqueue(job);
        ui->lw_chapters->setCurrentRow(chapNr - 1);
        processPrintQueue();
    }
}

void MainWindow::highlightMatches()
{
    // enter pressed in lineEdit find
    QString input = ui->lineEdit_find->text();
    clearHighLights();
    matchFormat.setFontWeight(700);

    if (activeScheme == "system")
        matchFormat.setBackground(QPalette().color(QPalette::AlternateBase));
    else
        matchFormat.setBackground(QColor(scheme["bgClr"]));

    int count = 0;
    while (textBrowser->find(input, findInPageflags)) {
        count += 1;
        textBrowser->textCursor().mergeCharFormat(matchFormat);
    }

    if (count == 0)
        ui->label_find_sum->clear();
    else if (count == 1)
        ui->label_find_sum->setText(QString("%1 match").arg(count));
    else
        ui->label_find_sum->setText(QString("%1 matches").arg(count));

    textBrowser->moveCursor(QTextCursor::Start);
    textBrowser->find(input, findInPageflags);
}

void MainWindow::clearHighLights()
{
    // remove old highlight background color
    textBrowser->selectAll();
    textBrowser->setFontWeight(400);

    if (activeScheme == "system")
        textBrowser->setTextBackgroundColor(QPalette().color(QPalette::Base));
    else
        textBrowser->setTextBackgroundColor(scheme["bg2Clr"]);

    textBrowser->moveCursor(QTextCursor::Start);
}

void MainWindow::countMatches()
{
    QString input = ui->lineEdit_find->text();
    textBrowser->moveCursor(QTextCursor::Start);

    int count = 0;
    while (textBrowser->find(input, findInPageflags))
        count += 1;

    if (count == 0)
        ui->label_find_sum->clear();
    else if (count == 1)
        ui->label_find_sum->setText(QString("%1 match").arg(count));
    else
        ui->label_find_sum->setText(QString("%1 matches").arg(count));
}

void MainWindow::setFindInPageLocation()
{
    if (ui->cb_find_loc->currentIndex() == 2)
        textBrowser = ui->strongs_tb;
    else if (ui->cb_find_loc->currentIndex() == 1)
        textBrowser = ui->info_tb;
    else
        textBrowser = ui->tb_scriptures;
}

void MainWindow::startFind()
{
    // executes from lineedit signal: text changed, on every keypress
    QString input = ui->lineEdit_find->text();
    clearHighLights();

    if (input.length() <= 1) {
        ui->label_find_sum->clear();
        return;
    } else {
        countMatches();
        textBrowser->moveCursor(QTextCursor::Start);
        textBrowser->find(input, findInPageflags);
    }
}

void MainWindow::findInPage(const QString &nextOrPrev)
{
    // runs when button next or previous is activated
    QString input = ui->lineEdit_find->text();

    if (nextOrPrev == "prev") {
        findInPageflags.setFlag(QTextDocument::FindBackward, true);
        textBrowser->find(input, findInPageflags);
        findInPageflags.setFlag(QTextDocument::FindBackward, false); // reset
    } else {
        textBrowser->find(input, findInPageflags);
    }
}

void MainWindow::showFindFrame()
{
    if (ui->strongs_tb->hasFocus())
        ui->cb_find_loc->setCurrentText("Strongs");
    else if (ui->info_tb->hasFocus())
        ui->cb_find_loc->setCurrentText("Info");
    else
        ui->cb_find_loc->setCurrentText(tr("Bible"));

    // ui->find_frame->show();
    ui->lineEdit_find->clear();
    ui->label_find_sum->clear();
    ui->lineEdit_find->setFocus();
    textBrowser->moveCursor(QTextCursor::Start);

    ui->find_frame->show();
    QPropertyAnimation *animation = new QPropertyAnimation(ui->find_frame, "maximumHeight");
    animation->setDuration(200);
    animation->setStartValue(0);
    animation->setEndValue(43);
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void MainWindow::modifyFindInPageFlags()
{
    if (ui->chkBox_find_case->isChecked())
        findInPageflags.setFlag(QTextDocument::FindCaseSensitively, true);
    else
        findInPageflags.setFlag(QTextDocument::FindCaseSensitively, false);

    if (ui->chkBox_find_whole->isChecked())
        findInPageflags.setFlag(QTextDocument::FindWholeWords, true);
    else
        findInPageflags.setFlag(QTextDocument::FindWholeWords, false);
}

void MainWindow::setTranslation()
{
    /* runs every time when user switches translation from the content tab combobox
        and when cb index is changed from code */
    ui->tb_scriptures->clear();
    activeTl = ui->cb_select_translation->currentData().toString().toLower();
    QString full = ui->cb_select_translation->currentText();
    ui->search_lbl_tl->setText(full);
    setHasNotes();

    if (not ui->menu_history->actions().isEmpty())
        ui->menu_history->actions().constLast()->activate(QAction::Trigger);
}

void MainWindow::populateSearchCbs()
{
    ui->search_cb_where->addItem( tr("this book"), "this");
    ui->search_cb_where->addItem( tr("all books"), "all");
    ui->search_cb_where->addItem( tr("O.T."), "O.T.");
    ui->search_cb_where->addItem( tr("N.T."), "N.T.");
    ui->search_cb_where->addItem( tr("Law"), "1");
    ui->search_cb_where->addItem( tr("History"), "2");
    ui->search_cb_where->addItem( tr("Wisdom"), "3");
    ui->search_cb_where->addItem( tr("Prophets"), "4");
    ui->search_cb_where->addItem( tr("Gospels"), "5");
    ui->search_cb_where->addItem( tr("Acts"), "6");
    ui->search_cb_where->addItem( tr("Epistles"), "7");
    ui->search_cb_where->addItem( tr("Apocalyptic"), "8");

    ui->search_cb_anyExact->addItem(tr("any order"), "any");
    ui->search_cb_anyExact->addItem(tr("exact match"), "exact");
}

void MainWindow::updateCbTranslations()
{
    ui->cb_select_translation->clear();
    QString sql = "SELECT abbreviation, version "
                  "FROM version_info ORDER BY abbreviation ASC";
    QSqlQuery query(sql, bibleDb );
    QString abbr;
    QString desc;

    while (query.next()) {
        abbr = query.value(0).toString().toUpper();
        desc = query.value(1).toString();
        ui->cb_select_translation->addItem(desc, abbr);
    }
}

void MainWindow::printFromHistory(QHash<QString, int> job)
{
    // called from ui->menu_history action
    printQ.enqueue(job);
    int bk = job["bk"];
    int c1 = job["c1"];
    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(c1 - 1);
    processPrintQueue();
}

void MainWindow::addToHistory(QHash<QString, int> job)
{
    // keep a history of print jobs, 10 items max
    // don't add doubles
    if (!printHistory.isEmpty()) {
        if (printHistory.last() == job)
            return;
    }

    printHistory.enqueue(job);
    if (printHistory.length() > 10)
        printHistory.removeFirst();

    int bk = job["bk"];
    QString bookName = ::g_bookNames[bk];
    int c1 = job["c1"];
    int c2 = 0;
    int v1 = 0;
    int v2 = 0;

    // contains instead of operator[]()to prevent silent inserts of non existing keys
    if (job.contains("c2"))
        c2 = job["c2"];
    if (job.contains("v1"))
        v1 = job["v1"];
    if (job.contains("v2"))
        v2 = job["v2"];

    QString aTxt;

    if (v1 > 0 && v2 > 0)
        aTxt = QString("%1 %2:%3-%4").arg(bookName).arg(c1).arg(v1).arg(v2);
    else if (v1 > 0)
        aTxt = QString("%1 %2:%3").arg(bookName).arg(c1).arg(v1);
    else if (c1 > 0 && c2 > c1)
        aTxt = QString("%1 %2-%3").arg(bookName).arg(c1).arg(c2);
    else
        aTxt = QString("%1 %2").arg(bookName).arg(c1);

    QAction *jobAction = new QAction(this);
    jobAction->setText(aTxt);
    connect(jobAction, &QAction::triggered, this, [this, job] () {
        printFromHistory(job);}
    );
    ui->menu_history->addAction(jobAction);

    if (ui->menu_history->actions().count() > 10)
        ui->menu_history->removeAction(ui->menu_history->actions().at(0));
}

void MainWindow::processPrintQueue()
{
    ui->tb_scriptures->clear();

    while (not printQ.isEmpty())
        printScriptures();

    if(iFilter && (guiLanguage == "en" || guiLanguage == "nl")) filterImmersion();
    if(jFilter && (guiLanguage == "en" || guiLanguage == "nl")) filterJudeans();

}

void MainWindow::printScriptures()
{
    /* call this method for every job in the print queue with processPrintQueue
     does single chapters and ranges of chapters or verses */
    if(!ui->bible_frame->isVisible())
        ui->bible_frame->show();

    QHash<QString, int> job = printQ.dequeue();
    addToHistory(job);

    int bk = job["bk"];
    QString sBk = QString::number(job["bk"]);
    QString bookName = ::g_bookNames[bk];

    // chapter1, 2 - verse1, 2
    QString sC1 = QString::number(job["c1"]);
    int c2 = job["c2"];
    QString sC2 = QString::number(job["c2"]);
    int v1 = job["v1"];
    QString sV1 = QString::number(job["v1"]);
    int v2 = job["v2"];
    QString sV2 = QString::number(job["v2"]);

    if(bk == 0) return;

    // only check for maps if a whole chapter is requested
    if (showMaps and (v1 == 0)) {
        // check for maps
        QString sqlGetImg {QString("select book, chapter, name from img_references where "
                                   "book is %1 and chapter is %2").arg(sBk, sC1)};
        QSqlQuery queryImg(sqlGetImg, varDb);
        QString mapName {};
        QString maps {};

        while (queryImg.next()) {
            mapName = queryImg.value(2).toString();
            maps.append(QString(
                                "<a href='img:%1' style='text-decoration:none;"
                                "color:%2; font-size:small; font-style:italic;'>"
                                "<img width='20' height='20' "
                                "src=':/data/img/map.png'> %1 "
                                "</a> &nbsp; "
                                ).arg(mapName, scheme.value("titleClr")));
        }

        if (!maps.isEmpty())
            ui->tb_scriptures->insertHtml("<br><p>" + maps + "</p>");
    }

    QString chapterSql;
    QString verseSQL;

    if (c2 > 0 )
        chapterSql = QString("BETWEEN %1 AND %2").arg(sC1, sC2);
    else
        chapterSql = "= " + sC1;

    QString sql = QString("SELECT c, v, t from t_%1 where b = %2 and c %3 ")
            .arg(activeTl, sBk, chapterSql);

    if (v1 > 0 and v2 > 0 and c2 == 0) {
        verseSQL = QString("AND v BETWEEN %1 AND %2").arg(sV1, sV2);
        sql.append(verseSQL);
    } else if (v1 > 0 and c2 == 0){
        verseSQL = "AND v = " + sV1;
        sql.append(verseSQL);
    }

    QSqlQuery query(sql, bibleDb);

    // user can choose between book or table layout mode
    QString tableMode = "<table cellspacing='0' cellpadding='3'>";
    QString bookMode {};

    QString verseTableMode; // verse for table mode
    QString verseBookMode; // verse for book mode

    QString subheading;
    QString sCh, sNr, txt;
    QString breakOrNot {};

    QList<QString> endChar { ".", "!", "?" };
    int textL {};
    int iNr {};
    int prevNr {};

    while (query.next()) {
        sCh = query.value(0).toString();
        sNr = query.value(1).toString();
        iNr = query.value(1).toInt();
        txt = query.value(2).toString();
        textL += txt.length();

        /*------------------------------------------------------
            add heading if it's the first verse
            or when there's a verse sequence break
        ------------------------------------------------------*/
        if ( iNr == 1 || (iNr != (prevNr + 1) ) ) {
            textL = 0;

            if (scripLayout == "table") {
                tableMode.append(QString("<tr><td></td><td "
                                    "style='font-size:medium; color:%1;"
                                    "font-weight:400;'>"
                                    "%2 %3</td></tr>")
                                    .arg(scheme.value("nrClr"), bookName, sCh));
            } else {

                bookMode.append(QString(
                            "<br><p style='font-size:medium; color:%1;"
                            "font-weight:400;text-align:left;'>"
                            "%2 %3</p>"
                            ).arg(scheme.value("nrClr"), bookName, sCh));
            }
        }

        /*-----> subheading check <-----*/
        if (activeSubh != "none") {
            QString sqlCheckSH = QString("SELECT subheading from subheadings_%1"
                                        " where book is %2 and chapter is %3 "
                                        "and verse is %4")
                                        .arg(activeSubh, sBk, sCh, sNr);
            QSqlQuery queryCSH(sqlCheckSH, varDb);

            while (queryCSH.next()) {
                textL = 0;
                subheading = queryCSH.value(0).toString();

                if (scripLayout == "table") {
                    tableMode.append(QString(
                                         "<tr><td></td><td style='font-size:small;"
                                         "text-align:left;color:%1;font-weight:600;'>"
                                         "%2</td></tr>"
                                        ).arg(scheme.value("titleClr"), subheading));
                } else {
                    bookMode.append(QString(
                                    "<p style='font-size:small;text-align:left;"
                                    "color:%1;font-weight:600;'>"
                                    "%2</p>"
                                    ).arg(scheme.value("titleClr"), subheading));
                    }
            }

        }

        /*-----> add verse, number + text <-----*/
        if (scripLayout == "table") {
            verseTableMode = QString(
                "<tr>"
                "<td style='color:%3;font-weight:400;font-size:small'>%1</td>"
                "<td style='color:%4;font-weight:400'> %2</td>"
                "</tr>").arg(sNr, txt, scheme.value("nrClr"), scheme.value("txtClr"));
        } else {
            // check for linebreaktag at start of line
            // in order to keep nr and text on the same line
            QRegularExpressionMatch breakMatch = breakRegex->match(txt);
            if (breakMatch.hasMatch()) {
                txt.remove(*breakRegex);
                breakOrNot = "<br>";
            } else {
                breakOrNot = "";
            }

            verseBookMode = QString(
                        "%5<span style='color:%1;font-weight:400;font-size:small;"
                        "text-align:left'>%2 </span>"
                        "<span style='color:%3;font-weight:400;"
                        "text-align:left'>%4 </span>"
                        ).arg(scheme.value("nrClr"), sNr,
                              scheme.value("txtClr"), txt,
                              breakOrNot);

            /* check for end of linechar since we want a break after a verse
             * for better readability if lenght is above certain length */

            // assuming a tl with footnotes will have markup with linebreaks as well
            if (not hasNotes) {
                if (endChar.contains(txt.right(1)) && textL > 280) {
                    verseBookMode.append("<br>");
                    if (textL > 350)
                        verseBookMode.append("<br>");
                    textL = 0;
                }
            }
        }
        // text with footnotes, replace tags to something Qt recognizes
        if (hasNotes) {
            if (scripLayout == "table") {
                verseTableMode.replace(*noteRegex,
                                  QString("<sup><a href='note:%1 %2 %3 \\1'"
                                    "style='text-decoration:none;color:%4;'>"
                                    "[\\1]</a></sup>")
                                    .arg(sBk, sCh, sNr, scheme.value("txtClr")) );

                verseTableMode.remove("<pb/>");
                verseTableMode.replace("<J>", QString("<span style='color:%1;"
                                                "font-weight:400;'>")
                                                .arg(scheme.value("titleClr")));
                verseTableMode.replace("</J>", "</span>");
                verseTableMode.replace("<e>", "<i>");
                verseTableMode.replace("</e>", "</i>");
            } else {
                verseBookMode.replace(*noteRegex,
                                  QString("<sup><a href='note:%1 %2 %3 \\1'"
                                    "style='text-decoration:none;color:%4;'>"
                                    "[\\1]</a></sup>")
                                    .arg(sBk, sCh, sNr, scheme.value("txtClr")) );

                verseBookMode.replace("<pb/>", "<br>");
                verseBookMode.replace("<J>", QString("<span style='color:%1;"
                                                "font-weight:400;'>")
                                                .arg(scheme.value("titleClr")));
                verseBookMode.replace("</J>", "</span>");
                verseBookMode.replace("<e>", "<i>");
                verseBookMode.replace("</e>", "</i>");
                }
        }

        if (scripLayout == "table")
            tableMode.append(verseTableMode);
        else
            bookMode.append(verseBookMode);

        prevNr = iNr;
    } // end query


    if (scripLayout == "table") {
        tableMode.append("</table><br>");
        ui->tb_scriptures->insertHtml(tableMode);
    } else {
        bookMode.append("<br>");
        ui->tb_scriptures->insertHtml(bookMode);
    }
}

void MainWindow::filterImmersion()
{
    // find and replace, baptism to immersion
    QTextDocument::FindFlags imFlag;
    imFlag.setFlag(QTextDocument::FindWholeWords);
    QTextCharFormat charF;
    charF.setFontItalic(true);
    QRegularExpression regE;
    regE.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    QString replacement;

    // |de doper
    const QString regP = "dopen|gedoopt|doopt|doopte|de doop|de dopen|der dopen|"
            "ik doop|een doop|zijn doop|"
            "baptism|baptisms|baptised|baptized|baptize|baptise|baptizer";
    regE.setPattern(regP);

    QTextDocument *doc = ui->tb_scriptures->document();
    QTextCursor cursor(doc);
    QString match;

    while ( !cursor.isNull() && !cursor.atEnd() ) {
        cursor = doc->find(regE, cursor, imFlag);

        if(!cursor.isNull()) {
            match = cursor.selectedText().toLower();

            if(immersionDict.contains(match)) {
                replacement = immersionDict[match];
                charF.setToolTip("Immersion filter (" + match + ")");
                cursor.mergeCharFormat(charF);
                cursor.insertText(replacement);
            }
        }
    }
}

void MainWindow::filterJudeans(){
    /*
    find and replace, Jews to Judeans

    Judean is often a catchall word, but Judahite implies descendancy from Judah.
    So especially in later history, Judean is safer/easier to use.
    Context might illuminate more about who and what.
    Tribe / race, religion.

    UITZONDERING 2: Schrijf de lange klinker /ee/ dubbel in leenwoorden
    die in het Nederlands een vorm op -eeën of -eeër hebben en geen grondwoord op /ee/ hebben.
    Europeeër (zoals in Europees), farizeeën - farizeeër (zoals in farizees), Pyreneeën
    (zoals in Pyrenees)

    4. MAAR: Schrijf de lange klinker /ee/ met een enkel teken in de verbogen vorm op
    -ese(r) en in afleidingen op -isch en -isme van leenwoorden die geen grondwoord op /ee/ hebben.
    in verbogen vormen op -ese(r): Europese, Europeser, farizese, Pyrenese
    in afleidingen op -isch of -isme: farizeïsch, farizeïsme
    */
    QTextDocument::FindFlags jFlag;
    jFlag.setFlag(QTextDocument::FindWholeWords);
    QTextCharFormat charF;
    charF.setFontItalic(true);
    QRegularExpression regE;
    regE.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
    QString replacement;

    const QString regP = "Joodse|Joods|Jodin|Joden|Jood|het Jodendom|"
                    "Jewish|Jewess|Jews|Jew";
    // the longest similar word must come before the shorter word
    regE.setPattern(regP);

    QTextDocument *doc = ui->tb_scriptures->document();
    QTextCursor cursor(doc);
    QString match;

    while ( !cursor.isNull() && !cursor.atEnd() ) {
        cursor = doc->find(regE, cursor, jFlag);

        if(!cursor.isNull()) {
            match = cursor.selectedText().toLower();

            if(judeansDict.contains(match)) {
                replacement = judeansDict[match];
                charF.setToolTip(match + " (Judeans filter)");
                cursor.mergeCharFormat(charF);
                cursor.insertText(replacement);
            }
        }
    }
}

void MainWindow::setFilters()
{
    if(ui->action_immersion_filter->isChecked())
        iFilter = true;
    else
        iFilter = false;

    if(ui->action_judeans_filter->isChecked())
        jFilter = true;
    else
        jFilter = false;

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("filters/immersionFilter", iFilter);
    settings.setValue("filters/judeansFilter", jFilter);
}

void MainWindow::aboutFilters()
{
    QString msg = QString(tr(
                "The filters replace two important words that have become confused, "
                "with clear biblical alternatives.\n\n"
                "The new words will be in italic. And by hovering over "
                "them a tooltip will show the old word. \n\n"
                "These are only display filters that do not change the bible database. "
                "(relevant when using the search tab) \n\n"
                "The filters are not active in the parallel window since the performance hit "
                "was substantial while using many translations."
                ));
    QMessageBox msgBox(QMessageBox::Information, tr("about filters"), msg, QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::chapterSelected()
{
    if (!ui->lw_books->currentItem()) {
        printMsg("no book selected");
        return;
    }

    QList<QListWidgetItem *> lwItems = ui->lw_chapters->selectedItems();
    QHash<QString, int> job;
    int bookNumber = ui->lw_books->currentItem()->data(0x0100).toInt();
    int chapterNumber;

    for (QListWidgetItem* item : lwItems ) {
        chapterNumber = item->data(0x0100).toInt();
        job["bk"] = bookNumber;
        job["c1"] = chapterNumber;
        printQ.append(job);
    }

    processPrintQueue();
}

void MainWindow::bookSelected()
{
    if (ui->lw_books->currentRow() == 39) {
        ui->lbl_book_title->setText("");
        ui->tb_scriptures->clear();
        ui->lw_chapters->clear();
    } else {
        setBookTitle();
        updateChapterWidget();

        int bookNumber = ui->lw_books->currentItem()->data(0x0100).toInt();
        ui->lw_chapters->setCurrentRow(0);

        QHash<QString, int> job;
        job["bk"] = bookNumber;
        job["c1"] = 1;
        printQ.enqueue(job);
        processPrintQueue();
    }
}

QString MainWindow::getLongTitle(const int bk)
{
    QString title;
    QString bookNumber = QString::number(bk);
    QString sqlGetTitle = QString("SELECT title from lang_%1 "
                                "WHERE book_nr = %2").arg(bknLanguage, bookNumber);
    QSqlQuery getTitle(sqlGetTitle, booksDb);
    while (getTitle.next())
        title = getTitle.value(0).toString();
    return title;
}

void MainWindow::setBookTitle(QString title)
{
    // set text of bookname button
    if (title.isEmpty()) {
        if (ui->lw_books->currentItem()) {
            int bk = ui->lw_books->currentItem()->data(0x0100).toInt();
            title = getLongTitle(bk);
        } else {
            title = "";
        }
    }

    ui->lbl_book_title->setText(title);
}

void MainWindow::updateChapterWidget()
{
    if(!ui->bible_frame->isVisible())
        ui->bible_frame->show();

    ui->lw_chapters->clear();
    int bookNumber = ui->lw_books->currentItem()->data(0x0100).toInt();

    int finalChapter = Utilities::getChapterCount(bookNumber);

    for (int i = 1; i <= finalChapter; ++i) {
        QListWidgetItem *newChapterItem = new QListWidgetItem;
        QString chapterNr = QString::number(i);
        newChapterItem->setText(chapterNr);
        newChapterItem->setData(Qt::UserRole, i);
        ui->lw_chapters->addItem(newChapterItem);
    }
}

void MainWindow::updateBooksWidget(const QString &lang)
{
    ui->lw_books->clear();
    ui->lw_chapters->clear();
    ui->lbl_book_title->setText("");
    ::g_bookNames.clear();
    ::g_bookNames.append("zero");
    QString getBooks;

    if (lang.isEmpty()) {
        getBooks = "SELECT book_nr, name, abbreviation from lang_" + bknLanguage;
    } else {
        bknLanguage = lang;
        getBooks = "SELECT book_nr, name, abbreviation from lang_" + lang;
    }

    QSqlQuery query(getBooks, booksDb);
    QString bookName;
    QString tooltip;
    int bookNr;

    while (query.next()) {
        bookNr = query.value(0).toInt();
        tooltip = query.value(0).toString() + " " + query.value(2).toString();
        bookName = query.value(1).toString();

        QListWidgetItem *newBookItem = new QListWidgetItem;
        newBookItem->setText(bookName);
        newBookItem->setToolTip(tooltip);
        newBookItem->setData(Qt::UserRole, bookNr);

        if (bookNr == 40) {
            ui->lw_books->addItem("");
            ui->lw_books->addItem(newBookItem);
        } else {
            ui->lw_books->addItem(newBookItem);
        }

        ::g_bookNames.append(bookName);
    }
}

void MainWindow::updateTLandSubh(const QString &translation, const QString &subheadings)
{
    // signal from settings window
    activeSubh = subheadings;
    activeTl = translation;
    int currentIndex = ui->cb_select_translation->currentIndex();
    int newIndex = ui->cb_select_translation->findData(activeTl.toUpper());

    if (currentIndex == newIndex) {
        ui->tb_scriptures->clear();
        if (not ui->menu_history->actions().isEmpty())
            ui->menu_history->actions().constLast()->activate(QAction::Trigger);
    } else {
        if (newIndex == -1)
            newIndex = 0;
        ui->cb_select_translation->setCurrentIndex(newIndex);
    }
}

void MainWindow::setHasNotes()
{
    // tl has footnotes?
    QString sql = QString("SELECT abbreviation, notes "
                  "FROM version_info where abbreviation is '%1'").arg(activeTl.toLower());
    QSqlQuery query(sql, bibleDb );
    while (query.next())
        hasNotes = query.value(1).toBool();
}

void MainWindow::getBooksAbbr()
{
    ::g_booksAbbr.clear();
    ::g_booksAbbr.append("zero");
    QString abbr;

    QSqlQuery query("SELECT abbreviation from lang_" + bknLanguage, booksDb);

    while (query.next())
        ::g_booksAbbr.append(abbr);
}

void MainWindow::toggleBible()
{
    ui->bible_frame->isVisible() ? ui->bible_frame->hide() : ui->bible_frame->show();
}

void MainWindow::toggleTabW()
{
    ui->tabwidget->isVisible() ? ui->tabwidget->hide() : ui->tabwidget->show();
}

void MainWindow::toggleMenu()
{
    ui->menu_bar->isHidden() ? ui->menu_bar->show() : ui->menu_bar->hide();
}

void MainWindow::toggleInfo()
{
    ui->info_frame->isVisible() ? ui->info_frame->hide() : ui->info_frame->show();
}

void MainWindow::applyScheme(const QString &aScheme)
{
    // set active scheme and color values
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);

    activeScheme = aScheme;
    QString currentScheme = "schemes/" + aScheme;
    QStringList schemeValues = settings.value(currentScheme, "none").toStringList();

    if (activeScheme == "system") {
        qApp->setPalette(QApplication::style()->standardPalette());
        scheme["nrClr"] = QPalette().color(QPalette::WindowText).name();
        scheme["txtClr"] = QPalette().color(QPalette::WindowText).name();
        scheme["titleClr"] = QPalette().color(QPalette::WindowText).name();
        scheme["bgClr"] = QPalette().color(QPalette::Base).name();
        scheme["bg2Clr"] = QPalette().color(QPalette::AlternateBase).name();
        scheme["clashClr"] = QPalette().color(QPalette::Highlight).name();
    } else {
        scheme["nrClr"] = schemeValues.value(0, "none");
        scheme["txtClr"] = schemeValues.value(1, "none");
        scheme["titleClr"] = schemeValues.value(2, "none");
        scheme["bgClr"] = schemeValues.value(3, "none");
        scheme["bg2Clr"] = schemeValues.value(4, "none");
        scheme["clashClr"] = schemeValues.value(5, "none");
    }

    emit setParwStyle(scheme);

    matchFormat.setBackground(QColor(scheme["bg2Clr"]));
    setStyleSheets();
    changeEncTxt();
}

void MainWindow::setStyleSheets()
{
    QFile saFile;

    if (activeScheme == "system") {
        saFile.setFileName(":/data/css/system.css");
    } else {
        saFile.setFileName(":/data/css/soulanchor.css");
        QPalette palette;
        palette.setColor(QPalette::Window, scheme["bgClr"]);
        palette.setColor(QPalette::WindowText, scheme["txtClr"]);
        palette.setColor(QPalette::Base, scheme["bg2Clr"]);
        palette.setColor(QPalette::AlternateBase, scheme["bgClr"]);
        palette.setColor(QPalette::ToolTipBase, scheme["bg2Clr"]);
        palette.setColor(QPalette::ToolTipText, scheme["txtClr"]);
        palette.setColor(QPalette::Text, scheme["txtClr"]);
        palette.setColor(QPalette::PlaceholderText, scheme["txtClr"]);
        palette.setColor(QPalette::Button, scheme["bg2Clr"]);
        palette.setColor(QPalette::ButtonText, scheme["txtClr"]);
        palette.setColor(QPalette::BrightText, scheme["titleClr"]);
        palette.setColor(QPalette::Link, scheme["titleClr"]);
        palette.setColor(QPalette::Highlight, scheme["clashClr"]);
        palette.setColor(QPalette::HighlightedText, scheme["txtClr"]);

        QColor buttonDisabled = scheme["bg2Clr"];
        QColor textDisabled = scheme["txtClr"];
        buttonDisabled.setAlpha(100);
        textDisabled.setAlpha(100);
        palette.setColor(QPalette::Disabled, QPalette::Button, buttonDisabled);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, textDisabled);
        palette.setColor(QPalette::Disabled, QPalette::Text, textDisabled);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, textDisabled);
        qApp->setPalette(palette);
    }

    saFile.open(QIODevice::ReadOnly | QIODevice::Text);
    saStyle = saFile.readAll();
    saFile.close();

    saStyle.replace("txtClr", scheme["txtClr"]);
    saStyle.replace("bgClr", scheme["bgClr"]);
    saStyle.replace("bg2Clr", scheme["bg2Clr"]);
    saStyle.replace("nrClr", scheme["nrClr"]);
    saStyle.replace("titleClr", scheme["titleClr"]);
    saStyle.replace("clashClr", scheme["clashClr"]);
    QColor darkerBg = QColor(scheme["bgClr"]).darker(200);
    saStyle.replace("darkerBg", darkerBg.name());

    qApp->setStyleSheet(saStyle);
}

void MainWindow::on_btn_select_today_clicked()
{
    ui->calendar->setSelectedDate(QDate::currentDate());
}

void MainWindow::on_btn_find_close_clicked()
{
    QPropertyAnimation *animation = new QPropertyAnimation(ui->find_frame, "maximumHeight");
    animation->setDuration(200);
    animation->setStartValue(43);
    animation->setEndValue(0);
    connect(animation, &QPropertyAnimation::finished, this, [this] () {
        ui->find_frame->hide();
        ui->find_frame->setMaximumHeight(43);
    });
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}


void MainWindow::closeInfo()
{
    ui->info_tb->clear(); ui->strongs_tb->clear();
    QPropertyAnimation *animation = new QPropertyAnimation(ui->info_frame, "maximumWidth");
    animation->setDuration(200);
    animation->setStartValue(ui->info_frame->width());
    animation->setEndValue(0);
    connect(animation, &QPropertyAnimation::finished, this, [this] () {
        ui->info_frame->hide();
        ui->info_frame->setMaximumWidth(16777215);
    });
    animation->start(QPropertyAnimation::DeleteWhenStopped);
}

void MainWindow::closeEvent(QCloseEvent*)
{
    QApplication::closeAllWindows();
}

void MainWindow::exitApp()
{
    QApplication::quit();
}
