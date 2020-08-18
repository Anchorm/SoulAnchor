#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainWindow)
{    
    ui->setupUi(this);

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);

    // restore window state
    int width70 = (screen()->geometry().width() / 100) * 70;
    int height70 = (screen()->geometry().height() / 100) * 70;
    QByteArray geo = settings.value("Window/geometry").toByteArray();
    QByteArray state = settings.value("Window/windowState").toByteArray();
    if (!geo.isEmpty() && !state.isEmpty()) {
        restoreGeometry(geo);
        restoreState(state);
    } else {
        resize(width70, height70);
        centerApp();
    }

    tab = settings.value("tab", "0").toString();
    startup = settings.value("startup", "nothing").toString();
    guiLanguage = settings.value("guiLanguage", "english").toString();
    bknLanguage = settings.value("bknLanguage", "english").toString();
    scripMargin = settings.value("margin", "15").toInt();
    scripDisplay = settings.value("display", "table").toString();
    rosterRead = settings.value("Rosters/rosterRead", "false").toBool();

    ui->info_frame->hide();
    ui->frame_find->hide();
    textBrowser = ui->tb_scriptures;

    if (tab == "Contents")
        ui->tabwidget->setCurrentIndex(0);
    else if (tab == "Search")
        ui->tabwidget->setCurrentIndex(1);
    else
        ui->tabwidget->setCurrentIndex(2);

    // bible frame vs info frame
    ui->splitter_background->setStretchFactor(0,2);
    ui->splitter_background->setStretchFactor(1,2);

    ui->splitter_bibleframe->setStretchFactor(0,1);
    ui->splitter_bibleframe->setStretchFactor(1,10);

    // give info frame more space than strongs frame
    ui->splitter_med->setStretchFactor(0,3);
    ui->splitter_med->setStretchFactor(1,2);

    defaultFormat.setBackground(Qt::transparent);
    defaultFormat.setFontWeight(QFont::Normal);
    matchFormat.setBackground(QColor(141, 192, 232));

    ui->tb_scriptures->document()->setDocumentMargin(scripMargin);

    connect(ui->action_quit, &QAction::triggered, this, &MainWindow::exit, Qt::QueuedConnection);
    ui->action_quit->setShortcut(QKeySequence("Ctrl+q"));


    connect(ui->action_toggle_tab, &QAction::triggered, this, &MainWindow::toggleTabW);
    ui->action_toggle_tab->setShortcut(Qt::Key_F8);

    connect(ui->action_toggle_bible, &QAction::triggered, this, &MainWindow::toggleBible);
    ui->action_toggle_bible->setShortcut(Qt::Key_F9);

    connect(ui->action_toggle_info, &QAction::triggered, this, &MainWindow::toggleInfo);
    ui->action_toggle_info->setShortcut(Qt::Key_F10);

    connect(ui->action_fullscreen, &QAction::triggered, this, &MainWindow::toggleFullscreen);
    ui->action_fullscreen->setShortcut(Qt::Key_F11);

    connect(ui->action_toggle_menu, &QAction::triggered, this, &MainWindow::toggleMenu);
    ui->action_toggle_menu->setShortcut(Qt::Key_F12);


    connect(ui->action_make_roster, &QAction::triggered, this, [this] () {
        !rosterW->isVisible() ? rosterW->show() : rosterW->raise(); } );

    connect(ui->action_open_parallel, &QAction::triggered, this, &MainWindow::openParW);
    ui->action_open_parallel->setShortcut(QKeySequence("Ctrl+p"));

    connect(ui->action_about, &QAction::triggered, this, [this] () {
        aboutW->centerWindow();
        !aboutW->isVisible() ? aboutW->show() : aboutW->raise(); });

    connect(ui->action_settings, &QAction::triggered, this, [this] () {
        !settingsW->isVisible() ? settingsW->show()  : settingsW->raise();});

    connect(ui->action_lord_prayer, &QAction::triggered, this, &MainWindow::theLordsPrayer);
    connect(ui->action_breaking_bread, &QAction::triggered, this, &MainWindow::breakingBread);
    connect(ui->action_immersion, &QAction::triggered, this, &MainWindow::immersion);
    connect(ui->action_salvation, &QAction::triggered, this, &MainWindow::salvation);
    connect(ui->action_worries, &QAction::triggered, this, &MainWindow::worries);

    connect(ui->action_overview, &QAction::triggered, this, &MainWindow::showOverview);
    connect(ui->action_shortcuts, &QAction::triggered, this, &MainWindow::showShortcuts);
    connect(ui->action_emergency, &QAction::triggered, this, [this] () {
        showEncPic(":/img/emergency.jpg"); } );

    connect(ui->action_create_html_bible, &QAction::triggered, this, &MainWindow::openHtmlW);
    connect(ui->action_export_Bible, &QAction::triggered, this, &MainWindow::openExportBibleW);

    new QShortcut(QKeySequence(Qt::Key_Plus), this, SLOT(nextChapter()));
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(nextChapter()));
    new QShortcut(QKeySequence(Qt::Key_Minus), this, SLOT(prevChapter()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(prevChapter()));

//    new QShortcut(QKeySequence(Qt::Key_Print), this, SLOT(printQtextD()));

    // find in page - current textdocument
    connect(ui->cb_find_loc, &QComboBox::currentTextChanged,
            this, &MainWindow::setFindInPageLocation);
    new QShortcut(QKeySequence("Ctrl+f"), this, SLOT(showFindFrame()));
    connect(ui->btn_find_next, &QToolButton::clicked, this, [this]() {
        findInPage("next"); });
    connect(ui->btn_find_prev, &QToolButton::clicked, this, [this]() {
        findInPage("prev"); });
    connect(ui->lineEdit_find, &QLineEdit::textEdited, this, &MainWindow::startFind);
    connect(ui->lineEdit_find, &QLineEdit::returnPressed, this, &MainWindow::highlightMatches);
    ui->btn_find_prev->setShortcut(Qt::Key_F2);
    ui->btn_find_next->setShortcut(Qt::Key_F3);
    connect(ui->chkBox_find_case, &QCheckBox::stateChanged,
            this, &MainWindow::modifyFindInPageFlags);
    connect(ui->chkBox_find_whole, &QCheckBox::stateChanged,
            this, &MainWindow::modifyFindInPageFlags);

    new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(escapeKey()));

    connect(new QShortcut(QKeySequence("Alt+1"),  this), &QShortcut::activated, [=]()
        {ui->tabwidget->setCurrentIndex(0);});
    connect(new QShortcut(QKeySequence("Alt+2"),  this), &QShortcut::activated, [=]()
        {ui->tabwidget->setCurrentIndex(1);});
    connect(new QShortcut(QKeySequence("Alt+3"),  this), &QShortcut::activated, [=]()
        {ui->tabwidget->setCurrentIndex(2);});

    connect(ui->background_frame, &QTextEdit::customContextMenuRequested,
            this, &MainWindow::ccMenuBackground);
    connect(ui->bible_frame, &QFrame::customContextMenuRequested,
            this, &MainWindow::ccMenuBibleFrame);
    connect(ui->info_tb, &QFrame::customContextMenuRequested,
            this, &MainWindow::ccMenuInfo);
    connect(ui->strongs_tb, &QFrame::customContextMenuRequested,
            this, &MainWindow::ccMenuStrongs);

    connect(ui->btn_book_title , &QPushButton::clicked, this, &MainWindow::showAboutBook);

    connect(ui->lw_books, &QListWidget::itemClicked, this, &MainWindow::bookSelected);
    connect(ui->lw_books, &QListWidget::itemActivated, this, &MainWindow::bookSelected);
    connect(ui->lw_chapters, &QListWidget::itemClicked, this, &MainWindow::chapterSelected);
    connect(ui->lw_chapters, &QListWidget::itemActivated, this, &MainWindow::chapterSelected);
    connect(ui->lineEdit_select, &QLineEdit::returnPressed, this, [this] () {
        printRequest(ui->lineEdit_select->text());    });

    populateSearchCbs();
    connect(ui->search_le_what, &QLineEdit::returnPressed, this, &MainWindow::searchScriptures);
    connect(ui->search_btn_find, &QToolButton::clicked, this, &MainWindow::searchScriptures);
    connect(ui->search_le_what, &QLineEdit::textChanged, this, [this](){
        if(ui->search_le_what->text().isEmpty()) ui->search_tb->clear();    });

    connect(ui->action_daily_proverb, &QAction::triggered, this, &MainWindow::todaysProverb);
    connect(ui->btn_daily_proverb, &QPushButton::clicked, this, &MainWindow::todaysProverb);
    connect(ui->btn_daily_psalm, &QPushButton::clicked, this, &MainWindow::todaysPsalm);
    connect(ui->action_daily_psalm, &QAction::triggered, this, &MainWindow::todaysPsalm);
    connect(ui->btn_daily_letter, &QPushButton::clicked, this, &MainWindow::todaysLetter);
    connect(ui->action_letter, &QAction::triggered, this, &MainWindow::todaysLetter);
    connect(ui->action_morning, &QAction::triggered, this, &MainWindow::morning);
    connect(ui->action_evening, &QAction::triggered, this, &MainWindow::evening);

    connect(ui->btn_roster, &QPushButton::clicked, this, &MainWindow::readingPlan);
    connect(ui->action_show_roster, &QAction::triggered, this, &MainWindow::loadRoster);
    ui->frame_roster_btns->hide();

    setStyleSheets();
    updateBooksWidget();
    updateCbTranslations();

    tlAbbr = settings.value("translation").toString();
    int tlIndex = ui->cb_select_translation->findData(tlAbbr);
    ui->cb_select_translation->setCurrentIndex(tlIndex);
    QString full = ui->cb_select_translation->currentText();
    ui->search_lbl_tl->setText(QString("Search in:\n %1").arg(full));
    connect(ui->cb_select_translation, &QComboBox::currentTextChanged,
            this, &MainWindow::setTranslation);

    getBooksAbbr();
    createOtNtMenus();
    setEncTxt();
    changeEncTxt();
    setEncPic();
    changeEncPic();

    makeMusicMenu();
    connect(ui->action_stop_media, &QAction::triggered,
            this, [this] () { mplayer->stop(); });
    connect(mplayer, &QMediaPlayer::stateChanged, this, &MainWindow::stopPlayer);
    connect(ui->action_play_random, &QAction::triggered, this, &MainWindow::playRandom);

    makeTextMenuItems();
    applyFont();

    // signals
    QObject::connect(settingsW, &SettingsWindow::fontChanged,
                     this, &MainWindow::applyFont );
    QObject::connect(this, &MainWindow::parOpened,
                     parW, &ParWindow::setTlandJob );
    QObject::connect(rosterW, &Roster::rosterCreated,
                     this, &MainWindow::addRostersToMenu );

    addRostersToMenu();
    setActiveRoster();
    buildBookmarkMenu();

    iFilter = settings.value("Filters/iFilter").toBool();
    jFilter = settings.value("Filters/jFilter").toBool();
    ui->action_immersion_filter->setChecked(iFilter);
    ui->action_judeans_filter->setChecked(jFilter);

    connect(ui->action_immersion_filter, &QAction::triggered, this, &MainWindow::setFilters );
    connect(ui->action_judeans_filter, &QAction::triggered, this, &MainWindow::setFilters );
    connect(ui->action_about_filters, &QAction::triggered, this, &MainWindow::aboutFilters );

    connect(ui->strongs_le, &QLineEdit::returnPressed, this, [this](){
        getStrongs(ui->strongs_le->text()); });
    connect(ui->strongs_pb, &QPushButton::clicked, this, [this](){
        getStrongs(ui->strongs_le->text()); });
    connect(ui->strongs_le, &QLineEdit::textChanged, this, [this](){
        if(ui->strongs_le->text().isEmpty()) ui->strongs_tb->clear();    });

    connect(ui->dict_le, &QLineEdit::returnPressed, this, [this](){
        getDictSug(""); });
    connect(ui->dict_le, &QLineEdit::textChanged, this, [this](){
        if(ui->dict_le->text().isEmpty()) ui->info_tb->clear();    });

    rosterRead ? ui->cb_roster_read->setChecked(true) : ui->cb_roster_read->setChecked(false);

    strongTl = "t_akjv_s";

    if (startup == "Psalm") todaysPsalm();
    else if (startup == "Proverb") todaysProverb();
    else if (startup == "Letter") todaysLetter();
    else if (startup == "Reading Plan") readingPlan();
    else ui->bible_frame->hide();

    ui->tb_scriptures->moveCursor(QTextCursor::Start);

    settingsW->cancel();
}

MainWindow::~MainWindow() {
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("Window/geometry", saveGeometry());
    settings.setValue("Window/windowState", saveState());
    delete ui;
}

void MainWindow::showIntro() {
    QString intro = (
        "Hi, welcome to SoulAnchor.\t\n\n"

        "This project is still a work in progress. There might be problems/bugs. "
        "Note that some options will not work "
        "until you add your own resources (e.g. audio bible). "
        "See the MOD file for details. \n\n"
    );

    QWidget *appWidget = nullptr;
    const QWidgetList topWidgets = QApplication::topLevelWidgets();
         for (QWidget *widget: topWidgets) {
             if (widget->objectName() == "MainWindow") {
                appWidget = widget;
                break;
             }
         }

    QMessageBox msgBox(QMessageBox::Information, "introduction", intro, QMessageBox::Ok);
    if (appWidget != nullptr){
        auto myrect = msgBox.frameGeometry(); // create a rectangle
        auto appGeoCenter = appWidget->geometry().center(); // get app center
        myrect.moveCenter(appGeoCenter); // move the rect
        msgBox.move(myrect.topLeft()); // move the msgbox
    }
    msgBox.exec();

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("showIntro", "false");
}

void MainWindow::strongify()
{
    // make it strong
    QString nrStyle = "font-weight:normal;color:grey;padding:1px";
    QString txtStyle = "font-weight:normal;padding:1px;color:#454545;";
    QString christStyle = "font-weight:normal;color:indianred;";
    QString strongStyle = "font-weight:bold;color:mediumblue;"
                          "text-decoration:none;"; //color:#deb1b1 #B1B1DE #1E314C #9E0E0D

    QHash<QString, int> job;
    if (!printHistory.isEmpty()){
        job = printHistory.last();
    } else {
        return;
    }

    int bk = job["bk"];
    int c1 = job["c1"];

    const QString bkStr = QString::number(bk);
    const QString cStr = QString::number(c1);

    QString sql = QString("SELECT b, c, v, t from %1 where b = %2 and c = %3 ")
            .arg(strongTl, bkStr, cStr);
    QSqlQuery query(sql, dbH.bibleDb);

    QString v;
    QString t;
    QString bookName = ::g_bookNames[bk];
    QString header = QString(tr("showing %1 %2 with Strong's numbers using the AKJV"))
            .arg(bookName, cStr);

    QString strongified = "<table>"
            "<th style='color:grey'>" + header + "</th><br>";

    while (query.next()) {
        v = query.value(2).toString();
        t = query.value(3).toString();
        strongified += "<tr><td style='" + nrStyle + "'><small>" + v +
                "</td></small>" + " <td style='" + txtStyle + "'>" + t +"</td></tr>";
    }

    strongified += "</table>";

    QString strongNr;
    QString hebrewsOrGreek;

    if (bk < 40)
        hebrewsOrGreek = "H";
    else
        hebrewsOrGreek = "G";

    // *************************************************************
    // look for the words of the Lord tags and replace and change style
    // *************************************************************

    strongified.replace("<J>","<span style='" + christStyle + "'>");
    strongified.replace("</J>","</span>");

    // *************************************************************
    // use regex to change strong tags into anchors
    // *************************************************************

    QString strongPattern =
            "(?<strongStartTag><S>)"
            "\\s*(?<strongNr>\\d+)"
            "\\s*(?<strongEndTag></S>)";

    QRegularExpression re(strongPattern);
    QRegularExpressionMatchIterator gmatch = re.globalMatch(strongified);

    QString startAnchor;

    QRegularExpressionMatch match;

    while (gmatch.hasNext())
    {
        match = gmatch.next();
        strongNr = hebrewsOrGreek;
        strongNr += match.captured("strongNr");
        startAnchor = "<a style='" + strongStyle + "' href='strongs:";
        startAnchor += strongNr + "'>";

        strongified.replace(match.captured(), startAnchor + " <small>" + strongNr + "</small></a>");
    }

    ui->tb_scriptures->setHtml(strongified);
}

void MainWindow::getTWOT(QString twot) {
    // i think there's still copyright on the TWOT so for those
    // who own the TWOT, they can add a table: name: twot: columns: topic, definition

    QSqlQuery chkTWOT("SELECT name FROM sqlite_master WHERE type = 'table' "
                           "and name = 'twot' ", dbH.dictDb);

    if (not chkTWOT.next()) {
        ui->strongs_tb->moveCursor(QTextCursor().Start);
        ui->strongs_tb->insertHtml("<p style='color:brown;'>This database is not included.</p>");
        return;
    }

    QString pattern =
            "\\s*"
            "\\d{1,4}"
            "\\s*";

    QRegularExpression re(pattern);
    QRegularExpressionMatch match = re.match(twot);

    // twot topic number
    QString twotNr;
    if (match.hasMatch()) {
        twotNr = match.captured();
    } else {
        ui->strongs_tb->setHtml("TWOT entry not found");
        return;
    }

    if (twotNr.length() == 1) {
        twotNr.prepend("000");
    } else if (twotNr.length() == 2) {
        twotNr.prepend("00");
    } else if (twotNr.length() == 3) {
        twotNr.prepend("0");
    }

    QString sql_twot = QString("SELECT topic, definition "
                          "FROM twot WHERE topic LIKE '%1%' ").arg(twotNr);

    QSqlQuery query_twot(sql_twot, dbH.dictDb);
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
    ui->strongs_tb->setHtml(result);
}

void MainWindow::getStrongs(QString strongs){
    if (strongs.isEmpty()) {
        ui->strongs_tb->clear();
        return;
    }

    QString numberRequest, numberResult;
    QString desc, sql_strong, sql_bdbt;

    QString pattern =
            "\\s*"
            "[gGhH]"
            "\\d{1,4}"
            "\\s*";

    QRegularExpression re(pattern);
    QRegularExpressionMatch match = re.match(strongs);

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

    QSqlQuery query_strong(sql_strong, dbH.dictDb);
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

    result.replace("<p/>", "<br>");
    result.replace("<p />", "<br>");
    result.replace("<font color='1'>", "<font color='gold'>");
    result.replace("<font color='2'>", "<font color='gold'>");
    result.replace("<font color='3'>", "<font color='gold'>");
    result.replace("<font color='4'>", "<font color='gold'>");
    result.replace("<font color='5'>", "<font color='gold'>");

    result += "<br><br>BDB-T:<br><br>";
    QSqlQuery query_bdbt(sql_bdbt, dbH.dictDb);
    while (query_bdbt.next()) {
        result += query_bdbt.value(1).toString();
    }

    ui->info_frame->show();
    ui->strongs_tb->document()->setDefaultStyleSheet("a {color:white;font-weight:bold;}"
                                                     "hr {color:white;}");
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
    ui->info_lbl_title->setText("dictionaries index");
    QString dictsLinks = "Showing results for: ";

    // only the first time it's empty
    if (!dictIndex.isEmpty()) {
        ui->info_tb->setHtml("<b>working...</b>");
        ui->info_tb->repaint();
        ui->info_tb->setHtml(dictIndex);
        return;
    }

    ui->info_tb->setHtml("<b>this will take a moment...</b>");
    ui->info_tb->repaint();

    QStringList dictTables;
    QSqlQuery query_get_tables("SELECT name FROM info WHERE strong = 'false' "
                               "ORDER BY name ASC", dbH.dictDb);

    while (query_get_tables.next()) {
        dictTables.append(query_get_tables.value(0).toString());
    }

    QString aStyle = "text-decoration:none;font-weight:normal;color:cornflowerblue";
    QString topicHref;
    QString topicTitle;
    QString sql_get_topics;
    QSqlQuery query_get_topics(dbH.dictDb);

    for (const QString &table : dictTables) {
        dictsLinks += "<a href='dict:" + table + "'>" + table + "</a> ";
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

void MainWindow::getDictWord(QString word) {
    // retrieve the word from ALL dictionaries after a link has been clicked
    ui->info_lbl_title->setText(word);
    QString result;
    QString desc;
    QString activeDicts = "Showing results for: ";

    QStringList dictTables;
    QSqlQuery query_get_tables("SELECT name FROM info WHERE strong = 'false' "
                               "ORDER BY name ASC", dbH.dictDb);
    while (query_get_tables.next()) {
        dictTables.append(query_get_tables.value(0).toString());
    }

    QString sql_dict;
    QSqlQuery dict_query(dbH.dictDb);

    word.replace("'", "_");

    for (const QString &table : dictTables) {
        desc.clear();
        sql_dict = QString("SELECT topic, definition "
                           "FROM %1 WHERE topic LIKE '%2' ").arg(table.toLower(), word);

        dict_query.exec(sql_dict);
        while (dict_query.next()) {
            desc = dict_query.value(1).toString();
        }

        if (not desc.isEmpty()) {
            activeDicts += "<a href='dict:" + table + "'>" + table + "</a> ";
            result.append("  <h2 id='" + table
                          + "' style='color:royalblue;font-family:serif;'>"
                          + table + ":</h2><br>");
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
                               "FROM %1 WHERE topic LIKE '%2%' ").arg(table.toLower(), word);

            dict_query.exec(sql_dict);
            while (dict_query.next()) {
                desc = dict_query.value(1).toString();
            }

            if (not desc.isEmpty()) {
                activeDicts += "<a href='dict:" + table + "'>" + table + "</a> ";
                result.append("  <h2 id='" + table
                              + "' style='color:royalblue;font-family:serif;'>"
                              + table + ":</h2><br>");
                result += desc + "<br><br>";
            }
        }
    }

    if (!result.isEmpty()) {
        result.replace("<p/>", "<br><br>");
        result.replace("<p />", "<br><br>");
        result.replace("href='S:", "href='dictword:");
        result.prepend(activeDicts + "<br>");

        ui->info_tb->setHtml(result);

        if (!dictwordHistory.isEmpty()) {
            if (word != dictwordHistory[0]) {
                dictwordHistory.prepend(word);
            }
        } else {
            dictwordHistory.prepend(word);
        }
        if (dictwordHistory.size() > 10) dictwordHistory.removeLast();
    } else {
        ui->info_tb->setHtml( word + " not found in dictionaries");
    }
}

void MainWindow::getDictSug(QString word) {
    // find word suggestions in dictionaries
    if (word.isEmpty()) {
        word = ui->dict_le->text();
    }
    if (word.isEmpty() || word.length() < 1) {
        ui->info_lbl_title->clear();
        ui->info_tb->clear();
        return;
    }

    QString result;
    QString links;
    QString topic;
    QString aStyle = "text-decoration:none;font-weight:normal;color:mediumblue";
    ui->info_lbl_title->setText("index for: " + word);

    QStringList dictTables;
    QSqlQuery query_get_tables("SELECT name FROM info WHERE strong = 'false' "
                               "ORDER BY name ASC", dbH.dictDb);
    while (query_get_tables.next()) {
        dictTables.append(query_get_tables.value(0).toString());
    }

    QString sql_dict;
    QSqlQuery dict_query(dbH.dictDb);
    for (const QString &table : dictTables) {
        sql_dict = QString("SELECT topic, definition "
                          "FROM '%1' WHERE topic like '%2%'").arg(table.toLower(), word);
        links.clear();

        dict_query.exec(sql_dict);
        while (dict_query.next()) {
            topic = dict_query.value(0).toString();
            links += QString("<a style=\"%1'\" href=\"dictword:%2\">%2</a><br>")
                    .arg(aStyle, topic);
        }

        if (not links.isEmpty()) {
            result.append("<big>" + table + ":</big><br><br>");
            result.append(links);
            result.append("<br><br>");
        }
    }

    if (!result.isEmpty()) {
        ui->info_tb->setHtml(result);
    } else {
        ui->info_tb->setHtml( word + " not found in dictionaries");
    }
}

void MainWindow::addBookmark(){
    QHash<QString, int> job;
    if (!printHistory.isEmpty()){
        job = printHistory.last();
    } else {
        return;
    }

    int bk = job["bk"];
    int c1 = job["c1"];
    int c2 = job["c2"];
    int v1 = job["v1"];
    int v2 = job["v2"];

    QString bookName = ::g_bookNames[bk];
    QString bookmarkName = QString("%1 %2").arg(bookName).arg(c1);

    if (c2 > c1)
    {
        bookmarkName += QString("-%1").arg(c2);
    }
    else if (v1 > 0)
    {
        bookmarkName += QString(":%1").arg(v1);
        if (v2 > v1)
        {
            bookmarkName += QString("-%1").arg(v2);
        }
    }

    QString sqlAdd =
            QString("INSERT INTO 'bookmarks' "
            "(name, book, chapter1, chapter2, verse1, verse2) "
            "VALUES ('%1', %2, %3, %4, %5, %6)")
            .arg(bookmarkName).arg(bk).arg(c1).arg(c2).arg(v1).arg(v2);
    QSqlQuery queryAdd(sqlAdd, dbH.bookmarksDb);
    queryAdd.exec();

    buildBookmarkMenu();
}

void MainWindow::renameBookmark(){
    if (ui->menu_bookmarks->actions().size() < 4)
        return;

    QStringList bookmarks;
    QString sqlGet = QString("SELECT name FROM 'bookmarks' ");
    QSqlQuery queryGet(sqlGet, dbH.bookmarksDb);
    while (queryGet.next()) {
        QString bkmName = queryGet.value(0).toString();
        bookmarks.append(bkmName);
    }

    bool ok = false;
    QString oldName = QInputDialog::getItem(this, tr("rename bookmark"),
                                       tr("Choose bookmark to rename:"), bookmarks, 0, false, &ok);

    if (ok && !oldName.isEmpty() ){
        ok = false;
        QString newName = QInputDialog::getText(this, tr("rename bookmark"),
                                       tr("And the new name is:"), QLineEdit::Normal,"", &ok);

        if (ok && !newName.isEmpty()) {
            QString sqlUpdate = QString("UPDATE 'bookmarks' SET name = '%1' "
                                     "WHERE name = '%2' ").arg(newName, oldName);
            QSqlQuery updateQuery(sqlUpdate, dbH.bookmarksDb);
            updateQuery.exec();

            buildBookmarkMenu();
        }
    }
}

void MainWindow::deleteBookmark(){
    if (ui->menu_bookmarks->actions().size() < 4)
        return;

    QStringList bookmarks;
    QString sqlGet = QString("SELECT name FROM 'bookmarks' ");
    QSqlQuery queryGet(sqlGet, dbH.bookmarksDb);
    while (queryGet.next()) {
        QString bkmName = queryGet.value(0).toString();
        bookmarks.append(bkmName);
    }

    bool ok = false;
    QString bookmarkName = QInputDialog::getItem(this, tr("delete bookmark"),
                                       tr("bookmark to remove:"), bookmarks, 0, false, &ok);

    if (ok && !bookmarkName.isEmpty() ) {
        QString sqlDelete = QString("DELETE FROM 'bookmarks' "
                                 "WHERE name = '%1' ").arg(bookmarkName);
        QSqlQuery deleteQuery(sqlDelete, dbH.bookmarksDb);
        deleteQuery.exec();

        buildBookmarkMenu();
    }
}

void MainWindow::buildBookmarkMenu()
{
    QList bkmAcs = ui->menu_bookmarks->actions();
    for (QAction *ac : bkmAcs ){
        delete ac;
    }

    QAction *addBbmAction = new QAction(bookAddIcon, tr("Add Bookmark"), this);
    connect(addBbmAction, &QAction::triggered, this, &MainWindow::addBookmark );
    ui->menu_bookmarks->addAction(addBbmAction);

    QAction *renameBbmAction = new QAction(tr("Rename"), this);
    connect(renameBbmAction, &QAction::triggered, this, &MainWindow::renameBookmark );
    ui->menu_bookmarks->addAction(renameBbmAction);

    QAction *deleteBbmAction = new QAction(bookRemoveIcon, tr("Delete"), this);
    connect(deleteBbmAction, &QAction::triggered, this, &MainWindow::deleteBookmark );
    ui->menu_bookmarks->addAction(deleteBbmAction);

    ui->menu_bookmarks->addSeparator();

    QString sqlGet = QString("SELECT name FROM 'bookmarks' ");
    QSqlQuery queryGet(sqlGet, dbH.bookmarksDb);
    while (queryGet.next()) {
        QString bkmName = queryGet.value(0).toString();
        QAction *bookmarkAction = new QAction(bkmName, this);
        bookmarkAction->setIcon(bookmarkIcon);
        connect(bookmarkAction, &QAction::triggered, this, [this, bkmName](){
            printBookmark(bkmName); });
        ui->menu_bookmarks->addAction(bookmarkAction);
    }
}

void MainWindow::printBookmark(const QString &bookmarkName){
    int bk = 0; int c1 = 0; int c2 = 0; int v1 = 0; int v2 = 0;

    QString sqlGet = QString("SELECT * FROM 'bookmarks' "
                             "WHERE name = '%1' ").arg(bookmarkName);
    QSqlQuery queryGet(sqlGet, dbH.bookmarksDb);

    while (queryGet.next() ){
        bk = queryGet.value(1).toInt();
        c1 = queryGet.value(2).toInt();
        c2 = queryGet.value(3).toInt();
        v1 = queryGet.value(4).toInt();
        v2 = queryGet.value(5).toInt();
    }

    if (bk == 0 || c1 == 0) {
        return;
    }

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
        ui->btn_book_title->setText("");
        rosterRead = true;
        settings.setValue("Rosters/rosterRead", "true");
    } else {
        rosterRead = false;
        settings.setValue("Rosters/rosterRead", "false");
        loadRoster();
    }
}

void MainWindow::on_btn_next_session_clicked()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    ui->cb_roster_read->setChecked(false);
    rosterRead = false;
    settings.setValue("Rosters/rosterRead", "false");

    QString rosterName = settings.value("Rosters/activeRoster").toString();
    if ( rosterName.isEmpty() ) {
        return;
    }

    int finalSession = 0;
    QString sqlGetFinal = QString("SELECT COUNT(DISTINCT SESSION) FROM '%1' ").arg(rosterName);
    QSqlQuery getFin(sqlGetFinal, dbH.rosterDb);
    while (getFin.next() ) {
        finalSession = getFin.value(0).toInt();
    }

    int currentSession = 0;
    QString sqlGetCur = QString("SELECT SESSION FROM '%1' WHERE CURRENT = 'true'"
                                "ORDER BY SESSION DESC LIMIT 1 ").arg(rosterName);
    QSqlQuery getCur(sqlGetCur, dbH.rosterDb);
    while (getCur.next() ) {
        currentSession = getCur.value(0).toInt();
    }

    int nextSession = currentSession + 1;
    if (nextSession > finalSession) {
        ui->tb_scriptures->setHtml("<br><center>You have reached the "
                                   "end of the reading plan.</center><br>");
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
    settings.setValue("Rosters/rosterRead", "false");

    QString rosterName = settings.value("Rosters/activeRoster").toString();
    if ( rosterName.isEmpty() ) {
        return;
    }

    int currentSession = 0;
    QString sqlGetCur = QString("SELECT SESSION FROM '%1' WHERE CURRENT = 'true'"
                                "ORDER BY SESSION DESC LIMIT 1 ").arg(rosterName);
    QSqlQuery getCur(sqlGetCur, dbH.rosterDb);
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

void MainWindow::updateSession(const QString &rosterName, int session){
    // update current session in roster.db
    QString sqlClear = QString("UPDATE '%1' SET CURRENT = NULL "
                             "WHERE CURRENT = 'true' ").arg(rosterName);
    QSqlQuery clearCurrent(sqlClear, dbH.rosterDb);
    clearCurrent.exec();

    QString sqlSet = QString("UPDATE '%1' SET CURRENT = 'true' "
                             "WHERE SESSION = %2;").arg(rosterName).arg(session);
    QSqlQuery setCurrent(sqlSet, dbH.rosterDb);
    setCurrent.exec();
}

void MainWindow::printSession(const QString &rosterName) {
    // print current reading plan session
    QString sql = QString("SELECT SESSION, BOOK, CHAPTER "
                          "from '%1' where CURRENT = 'true' ").arg(rosterName);

    QSqlQuery getSession(sql, dbH.rosterDb);
    int bk = 0, ch = 0;
    QString session;
    QStringList fullRosterName = rosterName.split(" ");
    QString shortRosterName = fullRosterName[0];

    QString finalSession;
    QString sqlGetFinal = QString("SELECT COUNT(DISTINCT SESSION) FROM '%1' ").arg(rosterName);
    QSqlQuery getFin(sqlGetFinal, dbH.rosterDb);
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

    if (rosterRead) {
        printQ.clear();
    } else {
        processPrintQueue();
    }
}

void MainWindow::on_action_delete_roster_triggered(){
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
        QSqlQuery queryDrop(sqlDrop, dbH.rosterDb);
        queryDrop.exec();

        QSettings settings(settingsFile.fileName(), QSettings::IniFormat);

        if (rosterName == settings.value("Rosters/activeRoster")) {
            settings.setValue("Rosters/activeRoster", "");
            settings.setValue("Rosters/rosterRead", "false");
            rosterRead = false;
            if (ui->frame_roster_btns->isVisible()) ui->frame_roster_btns->hide();
        }
        addRostersToMenu();

        QString msg = QString("<br><center>%1 has been removed</center><br>").arg(rosterName);
        ui->tb_scriptures->setHtml(msg);
    }
}

void MainWindow::on_action_reset_roster_triggered(){
    if (rosterGroup->actions().isEmpty())
        return;

    QString rosterName;
    QStringList rosters;
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
        settings.setValue("Rosters/rosterRead", "false");
        ui->cb_roster_read->setCheckState(Qt::Unchecked);
    }
}

void MainWindow::addRostersToMenu() {
    QList rgActions = rosterGroup->actions();
    for ( QAction *ac : rgActions ) {
        delete ac;
    }

    QList rmActions = ui->menu_roster_select->actions();
    for ( QAction *ac : rmActions ) {
        delete ac;
    }

    QSqlQuery getRosters("SELECT name FROM sqlite_master WHERE type = 'table' ", dbH.rosterDb);

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

void MainWindow::rosterActionTriggered(QString rosterName){
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("Rosters/activeRoster", rosterName);
    setActiveRoster();
    loadRoster();
}

void MainWindow::setActiveRoster() {
    //set active roster in menu
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString activeRoster = settings.value("Rosters/activeRoster").toString();
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

void MainWindow::loadRoster() {
    // select the active roster
    if (!ui->bible_frame->isVisible()) {
        ui->background_frame->setStyleSheet(bfStyle);
        ui->bible_frame->show();
    }

    if (ui->menu_roster_select->actions().isEmpty()) {
        ui->tb_scriptures->setHtml("<br><center>no reading plan available</center><br>"
            "<center>goto menu <i>Today\'s Reading</i> and create a plan</center>");
        ui->frame_roster_btns->hide();
        return;
    }

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    QString rosterName = settings.value("Rosters/activeRoster").toString();
    if ( rosterName.isEmpty() ) {
        ui->tb_scriptures->setHtml("<br><center>no reading plan active</center><br>"
            "<center>goto menu <i>Today\'s Reading</i> and create / select a plan</center>");
        ui->frame_roster_btns->hide();
        return;
    } else {
        rosterRead ? ui->cb_roster_read->setChecked(true) : ui->cb_roster_read->setChecked(false);
        ui->frame_roster_btns->show();
        ui->tabwidget->setCurrentIndex(2);
        ui->tb_scriptures->clear();
        ui->btn_book_title->setText("");
        ui->lw_chapters->clear();

        printSession(rosterName);
    }
}

void MainWindow::readingPlan(){
    if (ui->frame_roster_btns->isVisible()){
        ui->frame_roster_btns->hide();
        ui->tb_scriptures->clear();
        ui->btn_book_title->setText("");
    } else {
        loadRoster();
    }
}

void MainWindow::openParW() {
    QHash<QString, int> job;
    if(!printHistory.empty()){
        job = printHistory.last();
    } else {
        job = { {"bk", 1}, {"c1", 1} };
    }
    emit parOpened(tlAbbr, job);
    !parW->isVisible() ? parW->show() : parW->raise();
}

void MainWindow::openHtmlW() {
    !makeHtmlW->isVisible() ? makeHtmlW->show() : makeHtmlW->raise();
}

void MainWindow::openExportBibleW() {
    !exportBibleW->isVisible() ? exportBibleW->show() : exportBibleW->raise();
}

void MainWindow::applyFont(){
    // apply changes from settings window, btn apply
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    scripFont = settings.value("font").toString();
    int ps = settings.value("fontsize").toInt();
    scripFont.setPointSize(ps);
    ui->tb_scriptures->setFont(scripFont);
    ui->lw_chapters->setFont(scripFont);

    scripMargin = settings.value("margin").toInt();
    ui->tb_scriptures->document()->setDocumentMargin(scripMargin);
}

void MainWindow::spokenWord() {
    QString sBk = ui->lw_books->currentIndex().data(0x0100).toString();
    QString sCh = ui->lw_chapters->currentIndex().data(0x0100).toString();

    if (sBk.length() == 1) {
        sBk.prepend("0");
    }

    if (sCh.length() == 1) {
        sCh.prepend("00");
    } else if (sCh.length() == 2) {
        sCh.prepend("0");
    }

    QStringList files = QDir(::userDataDir.path()+ "/audio-bible").entryList();

    QString pattern = QString("^%1.*%2.mp3").arg(sBk,sCh);

    QRegularExpression re(pattern);

    QFileInfo audioBibleBook;
    bool hasMatch = false;

    for (const QString &file : files) {
        QRegularExpressionMatch match = re.match(file);
        if (match.hasMatch()){
            audioBibleBook = QString(::userDataDir.path()+ "/audio-bible/" + file);
            hasMatch = true;
            break;
        }
    }

    if (hasMatch) {
        QString absPath = audioBibleBook.absoluteFilePath();
        QString basename = audioBibleBook.baseName();
        playMusic(absPath, basename);
    } else {
        ui->tb_scriptures->setHtml(
            "<br><center><span style='font-family:sans'>could not "
            "find an mp3 file for this chapter"
            ", see the MOD file for more information</span></center>");
    }
}

void MainWindow::playRandom() {
    if(!musicList.isEmpty()) {
        std::shuffle(musicList.begin(), musicList.end(),
                     std::default_random_engine(std::random_device()()));
        QString absPath = musicList[0][0];
        QString basename = musicList[0][1];
        playMusic(absPath, basename);
    }
}

void MainWindow::stopPlayer() {
    ui->action_stop_media->setText("Stop playing");
    ui->action_stop_media->setDisabled(true);
}

bool MainWindow::compareFunctionR(QAction *a, QAction *b) {
    // to sort the psalms by nr for easy display in the music menu
    QString pattern =
            "\\s*(?<ps>[Pp]salm)"
            "\\s*(?<ch>\\d?\\d?\\d?)";
    QString aNr; QString bNr;
    QRegularExpression re(pattern);
    QRegularExpressionMatch matchA = re.match(a->text());
    QRegularExpressionMatch matchB = re.match(b->text());
    aNr = matchA.captured("ch");
    bNr = matchB.captured("ch");

    return aNr.toInt() < bNr.toInt();
}

bool MainWindow::compareFunctionS(QAction *a, QAction *b) {
    return a->text() < b->text();
}

void MainWindow::showText(const QString &filepath, const QString &filename) {
    // show a contemplation text/md
    ui->info_tb->clear(); ui->info_frame->show();
    ui->info_lbl_title->setText(filename);
    QString line, markdownS;

    QFile textFile(filepath);
    if (!textFile.open(QIODevice::ReadOnly | QIODevice::Text))
        return;

    while (!textFile.atEnd())
    {
            line = textFile.readLine();
            markdownS.append(line);
    }

    textFile.close();
    ui->info_tb->setMarkdown(markdownS);
}

void MainWindow::makeMusicMenu(){    
    QDirIterator it(::userDataDir.path()+ "/music", QStringList() << "*.mp3", QDir::NoFilter, QDirIterator::Subdirectories);

    QVector<QAction*> psalmActions;
    QVector<QAction*> hymnActions;
    QVector<QAction*> divActions;

    while (it.hasNext()) {
        QFileInfo f(it.next());
        QAction *musAction = new QAction(musIcon, f.baseName());
        connect(musAction, &QAction::triggered, this, [this, f]() {
            playMusic( f.absoluteFilePath(), f.baseName() );
        });

        if (f.path() == ::userDataDir.path()+ "/music/psalms"){
            psalmActions.append(musAction);
        } else if (f.path() == ::userDataDir.path()+ "/music/hymns") {
            hymnActions.append(musAction);
        } else {
            divActions.append(musAction);
        }

        QVector<QString> info = {f.absoluteFilePath(), f.baseName()};
        musicList.append(info);
    }

    std::sort(psalmActions.begin(), psalmActions.end(), compareFunctionR);
    std::sort(hymnActions.begin(), hymnActions.end(), compareFunctionS );
    std::sort(divActions.begin(), divActions.end(), compareFunctionS);

        for (QAction *ac : psalmActions) {
            ui->menu_psalms->addAction(ac);
        }
        for (QAction *ac : hymnActions) {
            ui->menu_hymns->addAction(ac);
        }
        for (QAction *ac : divActions) {
            ui->menu_music_word->addAction(ac);
        }
}

void MainWindow::makeTextMenuItems() {
    // add text about filters and populate notes menu
    QVector<QAction*> txtActions;

    QAction *immAction = new QAction(docIcon, tr("about Immersion"), this);
    QAction *judAction = new QAction(docIcon, tr("about Judeans"), this);
    connect(immAction, &QAction::triggered, this, [this]() {
                showText(":txt/immersion.md", "immersion"); });
    connect(judAction, &QAction::triggered, this, [this]() {
                showText(":txt/judeans.md", "Judeans"); });

    ui->menu_filters->addSeparator();
    ui->menu_filters->addAction(immAction);
    ui->menu_filters->addAction(judAction);

    QDirIterator it(::userDataDir.path()+ "/notes", QStringList() <<
                    "*.md" << "*.txt", QDir::NoFilter, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        QFileInfo f(it.next());
        QAction *txtAction = new QAction(docIcon, f.baseName());
        connect(txtAction, &QAction::triggered, this, [this, f]()
        {
            showText( f.absoluteFilePath(), f.baseName() );
        });

        txtActions.append(txtAction);
    }

    std::sort(txtActions.begin(), txtActions.end(), compareFunctionS);

    for (QAction *ac: txtActions) {
        ui->menu_text_word->addAction(ac);
    }
}

void MainWindow::playMusic(QString filepath, QString filename){
    mplayer->stop(); mplayer->setVolume(20);

    QUrl mp3Url = QUrl().fromLocalFile(filepath);
    QMediaContent mp3File = QMediaContent(mp3Url);
    mplayer->setMedia(mp3File);
    mplayer->play();

    ui->action_stop_media->setEnabled(true);
    ui->action_stop_media->setText(QString("Stop playing: %1").arg(filename) );

    QString psalmPattern =
            "\\s*(?<ps>[pP][sS][aA][lL][mM])"
            "\\s*(?<ch>\\d{1,3})";

    QString hymnPattern =
            "(?<Hymn>hymns)";

    QString scripPattern =
            "(?<prt>^[1-3]?)"
            "\\s*(?<bk>[a-zA-Zëüï]+)"
            "\\s*(?<ch1>\\d{1,3})"
            "-?(?<ch2>\\d{0,3})"
            ":?(?<vs1>\\d{0,3})"
            "-?(?<vs2>\\d{0,3})";

    QRegularExpression rePsalm(psalmPattern);
    QRegularExpression reHymn(hymnPattern);
    QRegularExpression reScrip(scripPattern);
    QRegularExpressionMatch psalmMatch = rePsalm.match(filename);
    QRegularExpressionMatch hymnMatch = reHymn.match(filepath);
    QRegularExpressionMatch scripMatch = reScrip.match(filename);

    if(psalmMatch.hasMatch()){
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
    } else if(hymnMatch.hasMatch()) {

        if(!ui->bible_frame->isVisible() ){
            ui->background_frame->setStyleSheet(bfStyle);
        }
        ui->info_tb->clear(); ui->info_frame->show();

        QFile hymnFile(QString(::userDataDir.path()+ "/music/hymns/%1.txt").arg(filename) );
        if (!hymnFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return;

        ui->info_lbl_title->setText(filename);

        QString hymnText;
        while (!hymnFile.atEnd()) {
            hymnText += hymnFile.readLine() + "<br>";
        }

        ui->info_tb->setHtml(QString("<br><div style='%1'><br>%2<br></div>").arg(hymnStyle, hymnText) );

    } else if(scripMatch.hasMatch() and !filepath.contains("audio-bible") ){
        printRequestSingle(filename);
    }
}

void MainWindow::theLordsPrayer(){
    printQ.enqueue( { {"bk", 40}, {"c1", 6}, {"v1", 5}, {"v2", 15} });
    printQ.enqueue( { {"bk", 42}, {"c1", 11}, {"v1", 1}, {"v2", 13} });

    setBookTitle(tr("The Lord's Prayer"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::breakingBread(){
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

void MainWindow::immersion(){
    printQ.enqueue( { {"bk", 40}, {"c1", 3} });
    printQ.enqueue( { {"bk", 41}, {"c1", 1}, {"v1", 1}, {"v2", 8} });
    printQ.enqueue( { {"bk", 42}, {"c1", 3}, {"v1", 7} });
    printQ.enqueue( { {"bk", 43}, {"c1", 3}, {"v1", 22}, {"v2", 23} });
    printQ.enqueue( { {"bk", 44}, {"c1", 1}, {"v1", 5} });
    printQ.enqueue( { {"bk", 44}, {"c1", 2}, {"v1", 36}, {"v2", 42} });
    printQ.enqueue( { {"bk", 44}, {"c1", 8}, {"v1", 12} });
    printQ.enqueue( { {"bk", 44}, {"c1", 8}, {"v1", 36} });
    printQ.enqueue( { {"bk", 44}, {"c1", 9}, {"v1", 18} });
    printQ.enqueue( { {"bk", 44}, {"c1", 10}, {"v1", 47}, {"v2", 48} });
    printQ.enqueue( { {"bk", 44}, {"c1", 16 }, {"v1", 14}, {"v2", 15} });
    printQ.enqueue( { {"bk", 44}, {"c1", 16 }, {"v1", 30}, {"v2", 34} });
    printQ.enqueue( { {"bk", 44}, {"c1", 18 }, {"v1", 7}, {"v2", 8} });
    printQ.enqueue( { {"bk", 44}, {"c1", 19 }, {"v1", 1}, {"v2", 6} });
    printQ.enqueue( { {"bk", 44}, {"c1", 2 }, {"v1", 16} });
    printQ.enqueue( { {"bk", 45}, {"c1", 6 } });
    printQ.enqueue( { {"bk", 48}, {"c1", 3 }, {"v1", 27} });

    setBookTitle(tr("Immersion"));
    ui->lw_chapters->clear();
    ui->lw_books->clearSelection();
    processPrintQueue();
}

void MainWindow::salvation(){
    printQ.enqueue({ {"bk", 21}, {"c1", 12}, {"v1", 13}, {"v2", 14} });
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

void MainWindow::worries() {
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

void MainWindow::showShortcuts() {
    ui->background_frame->setStyleSheet(bfStyle);
    ui->info_tb->clear(); ui->info_lbl_title->clear(); ui->info_frame->show();

    QString info;
    info.append("<style>table {margin:20px} td {padding-right:10px} tr {margin-top:3px;}</style>"
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
                "<tr></tr>"
                "<tr><td>quit</td> <td>ctrl + q</td></tr>"
                "</table>"
                );
    ui->info_tb->setHtml(info);
    ui->info_lbl_title->setText(tr("shortcuts"));
}

void MainWindow::showOverview(){
    ui->bible_frame->hide(); ui->info_frame->hide();
    ui->background_frame->setStyleSheet("#background_frame {"
        "border-image: url(:/img/overview.jpg) 0 0 0 0 stretch stretch}");
}

void MainWindow::showEncPic(const QString &fileName){
    ui->bible_frame->hide(); ui->info_frame->hide();
    ui->background_frame->setStyleSheet(
                QString("#background_frame {background-image: url(%1); "
                "background-repeat: no-repeat;background-position: center}").arg(fileName));
}

void MainWindow::changeEncPic() {
    if (!encPics.isEmpty()) {
        std::shuffle(encPics.begin(), encPics.end(),
                     std::default_random_engine(std::random_device()()));
        QString fileName(encPics[0]);
        QPixmap encPic(fileName);
        QPixmap encPicS(encPic.scaledToWidth(230));

        if(encPicS.height() > 200) {
            encPicLbl->setPixmap(encPicS.scaledToHeight(200));
        } else {
            encPicLbl->setPixmap(encPicS);
        }

        ui->pb_enc_img->disconnect(); // remove old signals else weird things happen!
        connect(ui->pb_enc_img, &QPushButton::clicked, this, [this, fileName]() {
            showEncPic(fileName);});
        connect(ui->pb_enc_img, &QPushButton::customContextMenuRequested,
                this, &MainWindow::changeEncPic);
    }
}

void MainWindow::setEncPic(){
    QDir dir(::dataDir.path() + "/img");
    if (!dir.exists()) {
        ::sout << "Cannot find the img directory in app data dir" <<  Qt::endl;
        return;
    }

    QStringList filters;
    filters << "*.png" << "*.jpg";
    dir.setNameFilters(filters);

    if(!dir.isEmpty()){
        QDirIterator it(dir, QDirIterator::NoIteratorFlags);
        while (it.hasNext()) {
            encPics.append(it.next());
        }

        QVBoxLayout *encPicLay = new QVBoxLayout();
        encPicLay->setContentsMargins(1,1,1,1);
        encPicLay->setSizeConstraint(QLayout::SetMinAndMaxSize);

        encPicLay->addWidget(encPicLbl);
        ui->pb_enc_img->setLayout(encPicLay);
    } else {
        ::sout << "no files in the images directory";
    }
}

void MainWindow::printEncTxt(int bk, int ch, const QString &verse){
    QHash<QString, int> job = { {"bk", bk}, {"c1", ch} };
    printQ.enqueue(job);

    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(ch - 1);

    processPrintQueue();

    ui->tb_scriptures->moveCursor(QTextCursor().Start);
    ui->tb_scriptures->find(verse);
}

void MainWindow::changeEncTxt(){
    std::shuffle(encS.begin(), encS.end(), std::default_random_engine(std::random_device()()));
    QVector<int> enc = encS[0];
    int bk = enc[0], ch = enc[1], vs = enc[2];
    tlAbbr = ui->cb_select_translation->currentData().toString();
    QString sql = QString("SELECT t from t_%1 where b = %2 and c = %3 and v = %4 ")
            .arg(tlAbbr).arg(bk).arg(ch).arg(vs);
    QString verse = "none";

    QSqlQuery query(sql, dbH.bibleDb);
    while (query.next()) {
        verse = query.value(0).toString();
    }

    encTxtLbl->setText(verse);

    ui->pb_enc_txt->disconnect(); // remove old signals else weird things happen!
    connect(ui->pb_enc_txt, &QPushButton::clicked, this, [this, bk, ch, verse]() {
        printEncTxt(bk, ch, verse);});
    connect(ui->pb_enc_txt, &QPushButton::customContextMenuRequested,
            this, &MainWindow::changeEncTxt);
}

void MainWindow::setEncTxt(){
    QString css = "background-color:white;color:grey;font-family:sans;font-size:12pt;padding:3px;margin:1px;";
    encTxtLbl->setWordWrap(true);
    encTxtLbl->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    encTxtLbl->setFixedWidth(220);
    encTxtLbl->setStyleSheet(css);

    QVBoxLayout *encTxtLay = new QVBoxLayout();
    encTxtLay->setContentsMargins(1,1,1,1);
    encTxtLay->setSizeConstraint(QLayout::SetMinAndMaxSize);
    encTxtLay->addWidget(encTxtLbl);
    ui->pb_enc_txt->setLayout(encTxtLay);
}

void MainWindow::escapeKey(){
    if (this->isFullScreen()) {
        this->showNormal();
    } else if (ui->menu_bar->maximumHeight() == 1) {
        ui->menu_bar->setMaximumHeight(23);
    } else if (ui->tabwidget->isHidden()) {
        ui->tabwidget->show();
    } else if (ui->frame_find->isVisible()) {
        ui->frame_find->hide();
    }
}

void MainWindow::todaysProverb(){
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

void MainWindow::todaysPsalm(){
    ui->frame_roster_btns->hide();
    int today = ui->calendar->selectedDate().day();

    int bk = 19;
    ui->lw_books->setCurrentRow(bk - 1);
    int lastCh = (today * 5) + 1;
    int firstCh;

    if ( today == 1 ) {
        firstCh = 1;
    } else {
        firstCh = lastCh - 4;
    }

    int ch;
    QVector<int> psalms;

    if ( today == 31 ) {
        ch  = 119;
    } else {

        for ( int i = firstCh; i <= lastCh; ++i ) {

            if ( i != 119 and i != 151 ) {
                psalms.append(i);
            }
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

void MainWindow::morning(){
    ui->background_frame->setStyleSheet(bfStyle);
    int today = ui->calendar->selectedDate().dayOfYear();
    QString morning;
    QString sql = QString("select day, devotion from mbm where day = '%1'").arg(today);
    QSqlQuery query(sql, dbH.devotionsDb);

    while (query.next()) {
        morning.append(query.value(1).toString());
    }

    morning.replace("<p/>", "<br><br>");
    morning.replace("<p />", "<br><br>");
    ui->info_tb->setHtml(QString("<div style='%1'>"
                                 "<br>%2<br></div>").arg(devStyle, morning));
    ui->info_lbl_title->setText("Morning by morning - Spurgeon");
    ui->info_frame->show();
}

void MainWindow::evening(){
    ui->background_frame->setStyleSheet(bfStyle);
    int today = ui->calendar->selectedDate().dayOfYear();
    QString evening;
    QString sql = QString("select day, devotion from ebe where day = '%1'").arg(today);
    QSqlQuery query(sql, dbH.devotionsDb);

    while (query.next()) {
        evening.append(query.value(1).toString());
    }

    evening.replace("<p/>", "<br><br>");
    evening.replace("<p />", "<br><br>");
    ui->info_tb->setHtml(QString("<div style='%1'><br>%2<br></div>").arg(devStyle, evening));
    ui->info_lbl_title->setText("Evening by evening - Spurgeon");
    ui->info_frame->show();
}

void MainWindow::todaysLetter(){
    ui->frame_roster_btns->hide();
    QList<int> letters = {45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65};
    std::shuffle(letters.begin(), letters.end(),
                 std::default_random_engine(std::random_device()() ));
    int bk = letters[0];

    int lastCh = dbH.getFinalChapter(bk);
    QList<int> chapters;
    for (int ch = 1; ch < (lastCh + 1); ++ch) {
        chapters.append(ch);
    }
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

    int bkNr = url_list[0].toInt();
    int chNr = url_list[1].toInt();
    QHash<QString, int> job = { {"bk", bkNr}, {"c1", chNr} };
    printQ.enqueue(job);

    bkNr < 40 ? ui->lw_books->setCurrentRow(bkNr - 1) : ui->lw_books->setCurrentRow(bkNr);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(chNr - 1);

    processPrintQueue();

    QFlags<QTextDocument::FindFlag> flags = QTextDocument().FindWholeWords;
    QString verse = url_list[2];
    ui->tb_scriptures->moveCursor(QTextCursor().Start);
    ui->tb_scriptures->find(verse, flags);
}

void MainWindow::on_info_tb_anchorClicked(const QUrl &url)
{
    QString sUrl = url.toString(QUrl::RemoveScheme);
    QStringList wrongUrl;
    QString bookName;
    QString goodUrl;

    if (url.scheme() == "bible") {
        printRequest(sUrl);

    // code follows database href tags which is a bit awkward perhaps
    } else if (url.scheme() == "strongs" || url.scheme() == "s" ) {
        getStrongs(sUrl);

        QString check = ui->strongs_tb->toPlainText();

        if (check == "no results\n\ntry a valid strongs number or word\n") {
            ui->strongs_tb->clear();
            getDictWord(sUrl);
        }
    } else if (url.scheme() == "b") {
            wrongUrl = sUrl.split(" ");
            // i guess the other booknumbering is for including books outside the canon
            QString sql = "select normal from convert where weird = " + wrongUrl[0];

            QSqlQuery query(sql, dbH.dictDb);
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

        QSqlQuery query(sql, dbH.dictDb);
        while (query.next()) {
            bookName = ::g_bookNames[query.value(0).toInt()];
        }
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
    QString sUrl = url.toString(QUrl::RemoveScheme);
    getStrongs(sUrl);
}

void MainWindow::versesWithStrongNumber(const QString &strongs, const QString &where){
    ui->search_tb->clear();
    int counter = 0;

    QString sql_findStrongNr = QString("select b, c, v, t from %1 "
                               "where t like '%<S>%2</S>%' "
                               "and b %3").arg(strongTl, strongs.mid(1), where);
    QSqlQuery query(sql_findStrongNr, dbH.bibleDb);

    while (query.next()) {
        int bk = query.value(0).toInt();
        QString bkS = query.value(0).toString();

        if (bk > 39 and (strongs[0] != "G")) {
            break;
        } else if (bk < 40 and (strongs[0] != "H")) {
            break;
        }

        counter += 1;
        QString bookname = ::g_bookNames[bk];
        QString c1 = query.value(1).toString();
        QString v1 = query.value(2).toString();
        QString txt = query.value(3).toString();
        txt.replace("<S>" + strongs.mid(1) + "</S>", " <b>" + strongs.mid(1) + "</b> ");
        txt.replace("<S>", " ");
        txt.replace("</S>", " ");

        QString link = QString("%1/%2/%3").arg(bkS, c1, v1) ;
        QString match = QString("<h3><a style=\"color: #555C96\" href=\"%5\">"\
                "%1 %2:%3 </a></h3>%4<br><br>").arg(bookname, c1, v1, txt, link);
        ui->search_tb->insertHtml(match);
    }

    QString intro;
    if (counter == 0){
        intro = QString("<center><h3>\"%1\"<br>was not found</h3></center>")
                .arg(strongs);
    } else if (counter == 1){
        intro = QString("<center><h3>\"%1\"<br>was found in one verse</h3></center>")
                .arg(strongs);
    } else {
        intro = QString("<center><h3>\"%1\"<br>was found in %2 verses</h3></center>")
                .arg(strongs, QString::number(counter));
    }

    ui->search_tb->moveCursor(QTextCursor().Start);
    intro += "<br><br>";
    ui->search_tb->insertHtml(intro);
}

void MainWindow::searchScriptures() {
    QString what = ui->search_le_what->text().trimmed();
    QString where = ui->search_cb_where->currentData(0x0100).toString();

    if (where == "this") {
        where = "= " + ui->lw_books->currentIndex().data(0x0100).toString();
    } else if (where == "all") {
        where = "between 1 and 66";
    } else if (where == "O.T.") {
        where = "between 1 and 39";
    } else if (where == "N.T.") {
        where = "between 40 and 66";
    } else {
        where = "in (select book_nr from number_name where genre_nr = " + where + ")" ;
    }

    const QString strongPattern =
            "\\s*"
            "[gGhH]"
            "\\d{1,4}"
            "\\s*";

    const QRegularExpression re(strongPattern);
    QRegularExpressionMatch match = re.match(what);
    QString strongs;
    if (match.hasMatch()) {
        strongs = match.captured().toUpper();
        versesWithStrongNumber(strongs, where);
        return;
    }

    QString anyExact = ui->search_cb_anyExact->currentText();
    QString tl = "t_" + ui->cb_select_translation->currentData().toString();
    // qDebug() << what << "in" << where << "how" << anyExact << "in" << tl;

    QString globOrLike; // sql glob or like
    QString astOrPer; // asterisk or percentage

    if (ui->search_chkb_case->isChecked()){
        globOrLike = "GLOB";
        astOrPer = "*";
    }
    else {
        globOrLike = "LIKE";
        astOrPer = "%";
    }

    if (what.length() > 1) {
        ui->search_tb->setHtml("<h3 align=\"center\">Patience is a virtue....</h3>");
        ui->search_tb->repaint();
    } else {
        ui->search_tb->setHtml("<h3 align=\"center\">Search input is empty "
                               "or less than 2 characters.</h3>");
        return;
    }

    QString first_word;
    QString whereClause;
    QStringList searchList = what.split(" ");
    QStringList moreWords; // more than one word

    // build the where clause of the SQL query
    if (anyExact == "any order") {
        first_word = searchList[0];

        // whole words
        // use () to separate the words
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

                moreWords = searchList.mid(1, -1);
                for (QString &word : moreWords) {
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
                moreWords = searchList.mid(1, -1);

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
    // exact match query
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

    QString searchQ = QString("SELECT b,c,v,t FROM %1 WHERE %2").arg(tl, whereClause);
    QSqlQuery query(searchQ, dbH.bibleDb);

    int counter = 0; // count the matches per verse
    ui->search_tb->clear();

    while (query.next()) {
        counter += 1;
        QString bookname = ::g_bookNames[query.value(0).toInt()];
        QString bk = query.value(0).toString();
        QString c1 = query.value(1).toString();
        QString v1 = query.value(2).toString();
        QString txt = query.value(3).toString();
        QString link = QString("%1/%2/%3").arg(bk, c1, txt) ;
        QString match = QString("<h3><a style=\"color: #555C96\" href=\"%5\">"\
                "%1 %2:%3 </a></h3>%4<br><br>").arg(bookname, c1, v1, txt, link);
        ui->search_tb->insertHtml(match);
    }

    // lets highlight all matches and count the results
    QTextCharFormat boldF;
    boldF.setFontWeight(QFont::Bold);
    QFlags<QTextDocument::FindFlag> ourFlags;

    if (ui->search_chkb_whole->isChecked()) {
        ourFlags = ourFlags | QTextDocument::FindWholeWords;
    }
    if (ui->search_chkb_case->isChecked()) {
        ourFlags = ourFlags | QTextDocument::FindCaseSensitively;
    }

    if (anyExact == "any order") {
        for (const QString &word : qAsConst(searchList)) {
            ui->search_tb->moveCursor(QTextCursor().Start);

            while (ui->search_tb->find(word, ourFlags)) {
                ui->search_tb->textCursor().mergeCharFormat(boldF);
            }
        }
    } else {
        ui->search_tb->moveCursor(QTextCursor().Start);

        while (ui->search_tb->find(what, ourFlags)) {
            ui->search_tb->textCursor().mergeCharFormat(boldF);
        }
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

void MainWindow::printRequestSingle(const QString &request) {
    //called by playMusic() Do english lookup only. For one chapter.

    QString pattern =
            "\\s*(?<prt>[1-3]?)"
            "\\s*(?<bk>[a-zA-Zëüï]+)"
            "\\s*(?<ch1>\\d?\\d?\\d?)"           
            ":?(?<vs1>\\d?\\d?\\d?)"
            "-?(?<vs2>\\d?\\d?\\d?)";

    int bkNr = 0, chNr1 = 0 , vsNr1 = 0, vsNr2 = 0;
    QRegularExpression re(pattern);
    QRegularExpressionMatch match = re.match(request);

    if (match.hasMatch()) {
        ui->tb_scriptures->clear();
        QString testBook;
        testBook.clear();

        bkNr = 0; chNr1 = 0 ; vsNr1 = 0; vsNr2 = 0;

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

            if (!match.captured("vs1").isEmpty()){
                QString verse1 = match.captured("vs1");
                vsNr1 = verse1.toInt();
            }
            if (!match.captured("vs2").isEmpty()){
                QString verse2 = match.captured("vs2");
                 vsNr2 = verse2.toInt();
            }

        // get the book number
        QSqlQuery query("SELECT book_nr, name_english from number_name", dbH.bibleDb);

        while (query.next()) {
            if (testBook == query.value(1).toString().toUpper()){
                bkNr = query.value(0).toInt();
                break;
            }
        }

        if (bkNr == 0){
            QSqlQuery query("SELECT abbr_en, book_nr FROM abbr", dbH.bibleDb);

            while (query.next()) {
                if (testBook == query.value(0).toString().toUpper()){
                    bkNr = query.value(1).toInt();
                    break;
                }
            }
        }

        assert(bkNr > -1 && bkNr < 67 );

        // get chapter number(s)
        if (bkNr > 0) {
            int fCh = dbH.getFinalChapter(bkNr);
            if (chNr1 <= 0) chNr1 = 1;
            if (chNr1 > fCh) chNr1 = fCh;

                // single chapter, perhaps with verse(s)
            if (vsNr1 > 0){
                if (vsNr2 > 0){
                    QHash<QString, int> job = { {"bk", bkNr}, {"c1", chNr1}, {"v1", vsNr1}, {"v2", vsNr2} };
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

void MainWindow::printRequest(const QString &request) {
    // scan a string and see if scriptures can be found and printed
    if (request.length() < 2)
        return;

    // TODO: improve?
    QString pattern =
            "\\s*(?<prt>[1-3]?)"
            "\\s*(?<bk>[a-zA-Zëüï]+)"
            "\\s*(?<ch1>\\d?\\d?\\d?)"
            "-?(?<ch2>\\d?\\d?\\d?)"
            ":?(?<vs1>\\d?\\d?\\d?)"
            "-?(?<vs2>\\d?\\d?\\d?)";

    QRegularExpression re(pattern);
    QRegularExpressionMatchIterator gmatch = re.globalMatch(request);
    QRegularExpressionMatch match;
    QString testBook;
    int bkNr = 0, chNr1 = 0 , chNr2 = 0, vsNr1 = 0, vsNr2 = 0;

    while (gmatch.hasNext()) {
        testBook.clear();

        match = gmatch.next();
        bkNr = 0; chNr1 = 0 ; chNr2 = 0; vsNr1 = 0; vsNr2 = 0;
        if (match.hasMatch()) {
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

        // get the book number, first try abbr in db, then lookup in booknames
        QSqlQuery query("SELECT abbr_en, book_nr, abbr_nl FROM abbr", dbH.bibleDb);

        while (query.next()) {
            if (testBook == query.value(0).toString().toUpper()) {
                bkNr = query.value(1).toInt();
                break;
            } else if (testBook == query.value(2).toString().toUpper()) {
                bkNr = query.value(1).toInt();
                break;
            }
        }

        if (bkNr == 0) {
            for(const QString &bookname : qAsConst(::g_bookNames)) {
                if (bookname.contains(testBook, Qt::CaseInsensitive)) {
                    bkNr = ::g_bookNames.indexOf(bookname);
                    break;
                }
            }
        }

        // get chapter and verse number(s) or return
        if (bkNr > 0) {
            int fCh = dbH.getFinalChapter(bkNr);
            if (chNr1 <= 0)
                chNr1 = 1;
            if (chNr1 > fCh)
                chNr1 = fCh;

            QHash<QString, int> job = {
                { "bk", bkNr },
                { "c1", chNr1 }, { "c2", chNr2},
                { "v1", vsNr1 }, { "v2", vsNr2}
            };

            printQ.append(job);

        } else {
            ui->lw_books->clearSelection();
            ui->lw_chapters->clear();
            ui->btn_book_title->setText("");
            ui->tb_scriptures->setHtml(tr("<br><center>no book found</center>"));
            return;
        }
    } //  endof while (gmatch.hasNext())

    if(bkNr == 0)
        return;

    bkNr < 40 ? ui->lw_books->setCurrentRow(bkNr - 1) : ui->lw_books->setCurrentRow(bkNr);

    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(chNr1 - 1);
    processPrintQueue();
}

void MainWindow::centerApp(){
    auto appGeo = frameGeometry();
    auto screenGeoCenter = screen()->geometry().center();
    appGeo.moveCenter(screenGeoCenter);
    move(appGeo.topLeft());
}

void MainWindow::nextChapter(){
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

void MainWindow::prevChapter(){
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

void MainWindow::createOtNtMenus() {
    QString sqlQ = "SELECT book_nr, testament, name_dutch FROM number_name";
    QSqlQuery query(sqlQ, dbH.bibleDb );

    while (query.next()) {
        int bkNr = query.value(0).toInt();
        QString bkName = query.value(2).toString();
        QAction *bookAction = new QAction(bookIcon, bkName, this);
        connect(bookAction, &QAction::triggered, this, [bkNr, this]() {
            popupChapters(bkNr); });

        if (query.value(1) == "OT") {
            otMenu->addAction(bookAction);
        } else if (query.value(1) == "NT"){
            ntMenu->addAction(bookAction);
        }
    }
}

void MainWindow::ccMenuBackground(){
    //custom context menu for background frame
    QMenu ccMenu;
    QAction *parAction = ccMenu.addAction(bookOpenIcon, tr("Open Parallel Window"));
    QAction *showBibleAction = ccMenu.addAction(bookOpenIcon, tr("Show Bible Frame"));
    QAction *showInfoAction = ccMenu.addAction(docIcon, tr("Show Info Frame"));

    QAction *action = ccMenu.exec(QCursor().pos());
    if (action == parAction) {
        openParW();
    }
    else if (action == showBibleAction) {
        ui->background_frame->setStyleSheet(bfStyle);
        ui->bible_frame->show();
    }
    else if (action == showInfoAction) {
        ui->background_frame->setStyleSheet(bfStyle);
        ui->info_frame->show();
    }
}

void MainWindow::ccMenuInfo(){
    //custom context menu for info_frame
    QMenu ccMenu;
    QMenu *histMenu = ccMenu.addMenu(tr("history"));

    for (const QString &histItem : qAsConst(dictwordHistory)) {
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

    if (action == closeAction) {
        ui->info_frame->hide();
    } else if (action == findAction){
        showFindFrame();
    } else if (action == selectAction){
        ui->info_tb->selectAll();
    } else if (action == copyAction){
        ui->info_tb->copy();
    }
}

void MainWindow::ccMenuStrongs(){
    //custom context menu for info_frame
    QMenu ccMenu;
    QMenu *histMenu = ccMenu.addMenu(tr("history"));

    for (QString histItem : qAsConst(strongsHistory)) {
        QAction *histAction = new QAction(histItem);
        if (histItem[0] == "T") {
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

    if (action == closeAction) {
        ui->info_frame->hide();
    } else if (action == findAction){
        showFindFrame();
    } else if (action == selectAction){
        ui->strongs_tb->selectAll();
    } else if (action == copyAction){
        ui->strongs_tb->copy();
    }
}

void MainWindow::ccMenuBibleFrame(){
    QMenu ccMenu;

    QAction *parAction = ccMenu.addAction(bookOpenIcon, tr("Open Parallel Window"));
    QAction *strongAction = ccMenu.addAction(strongIcon, tr("Strongify"));
    ccMenu.addSeparator();

    ccMenu.addMenu(otMenu);
    ccMenu.addMenu(ntMenu);
    QAction *chapAction = ccMenu.addAction(tr("chapters"));

    QAction *nextAction = ccMenu.addAction(nextIcon, tr("next"));
    QAction *prevAction = ccMenu.addAction(prevIcon, tr("previous"));

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
    QAction *aboutTlAction = ccMenu.addAction(docIcon, tr("about ") + tlAbbr);
    ccMenu.addSeparator();
    QAction *viewAction = ccMenu.addAction(tr("toggle table/book display"));
    ccMenu.addSeparator();
    QAction *hideAction = ccMenu.addAction(closeIcon, tr("close"));

    QAction *action = ccMenu.exec(QCursor().pos());
    if (action == parAction) {
        openParW();
    } else if (action == chapAction){
        popupChapters();
    } else if (action == nextAction){
        nextChapter();
    } else if (action == prevAction){
        prevChapter();

    } else if (action == selectAction){
        ui->tb_scriptures->selectAll();
    } else if (action == copyAction){
        ui->tb_scriptures->copy();
    } else if (action == findAction){
        showFindFrame();
    } else if (action == aboutBookAction){
        showAboutBook();
    } else if (action == aboutTlAction){
        showAboutTl();

    } else if (action == audioAction){
        spokenWord();
    } else if (action == stopAudioAction){
        mplayer->stop();

    } else if (action == hideAction){
        ui->bible_frame->hide();
    } else if (action == viewAction){
        scripDisplay == "table" ? scripDisplay = "book" : scripDisplay = "table";
        ui->tb_scriptures->clear();
    } else if (action == strongAction) {
        strongify();
    }
}

void MainWindow::showAboutBook() {
    ui->info_tb->clear(); ui->info_lbl_title->clear(); ui->info_frame->show();
    ui->info_tb->setCurrentCharFormat(emptyFormat);

    if (ui->lw_books->selectedItems().count() == 0) {
        ui->info_lbl_title->setText(tr("no book selected?"));
        return;
    }

    int bkNr = ui->lw_books->currentItem()->data(0x0100).toInt();
    QString bkNrS = ui->lw_books->currentItem()->data(0x0100).toString();
    QString title = ::g_bookNames[bkNr];

    QString pat =
            "\\s*"
            "\\d+"
            "\\s*";

    QRegularExpression re(pat);
    title.remove(re);

    getDictSug(title);

    if (ui->info_tb->document()->toPlainText().endsWith(" not found in dictionaries")) {
            QSqlQuery query("SELECT book_nr, name_english FROM number_name "
                            "WHERE book_nr = " + bkNrS, dbH.bibleDb);
            while (query.next()) {
                title  = query.value(1).toString();
            }
            getDictSug(title);
    }
}

void MainWindow::showAboutTl() {
    // show some information about the active translation from the database
    ui->info_tb->clear(); ui->info_lbl_title->clear(); ui->info_frame->show();
    ui->info_tb->setCurrentCharFormat(emptyFormat);

    QString sql = QString("SELECT info FROM bible_version_key "
                          "WHERE \"abbreviation\" = '%1'").arg(tlAbbr);
    QSqlQuery query(sql, dbH.bibleDb );
    QString info;

    while (query.next()) {
        info = query.value(0).toString() ;
    }

    ui->info_lbl_title->setText(tlAbbr);
    if (info.isEmpty()) {
        ui->info_tb->setText("no information available");
    } else {
        ui->info_tb->setText(info);
    }
}

void MainWindow::popupChapters(int bkNr) {
    // called from the customcontextmenu on the bible frame

    if (bkNr == 0){
        if (ui->lw_books->selectedItems().count() == 0) {
            ui->tb_scriptures->setHtml("<br><p><small>bro... <br><br> what book?</small></p>");
            return;
        } else {
            bkNr = ui->lw_books->currentItem()->data(0x0100).toInt();
        }
    } else {
        bkNr < 40 ? ui->lw_books->setCurrentRow(bkNr - 1) : ui->lw_books->setCurrentRow(bkNr);
        bookSelected();
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
        ui->lw_chapters->setCurrentRow(chapNr - 1);
        processPrintQueue();
    }
}

void MainWindow::highlightMatches(){
    // enter/return pressed in lineedit find
    QString input = ui->lineEdit_find->text();
    textBrowser->moveCursor(QTextCursor::Start);

    //remove possible old highlight color
    textBrowser->selectAll();
    textBrowser->textCursor().mergeCharFormat(defaultFormat);
    textBrowser->moveCursor(QTextCursor::Start);

    int count = 0;
    while (textBrowser->find(input, findInPageflags)) {
        count += 1;
        textBrowser->textCursor().mergeCharFormat(matchFormat);
    }

    if (count == 0) {
        ui->label_find_sum->clear();
    } else if (count == 1){
        ui->label_find_sum->setText(QString("%1 match").arg(count));
    } else {
        ui->label_find_sum->setText(QString("%1 matches").arg(count));
    }

    textBrowser->moveCursor(QTextCursor::Start);
    textBrowser->find(input, findInPageflags);
}

void MainWindow::setFindInPageLocation()
{
    if (ui->cb_find_loc->currentText() == "Bible")
        textBrowser = ui->tb_scriptures;
    else if (ui->cb_find_loc->currentText() == "Info")
        textBrowser = ui->info_tb;
    else
        textBrowser = ui->strongs_tb;
}

void MainWindow::startFind(){
    // executes from lineedit signal: text changed, on every keypress
    QString input = ui->lineEdit_find->text();

    if (input.isEmpty()) {
        ui->label_find_sum->clear();
        textBrowser->selectAll();
        textBrowser->textCursor().mergeCharFormat(defaultFormat);
        textBrowser->moveCursor(QTextCursor::Start);
    } else if (input.length() == 1){
        ui->label_find_sum->clear();
        return;
    } else {
        countMatches();
        textBrowser->moveCursor(QTextCursor::Start);
        textBrowser->find(input, findInPageflags);
    }
}

void MainWindow::countMatches(){
    QString input = ui->lineEdit_find->text();
    textBrowser->moveCursor(QTextCursor::Start);

    int count = 0;
    while (textBrowser->find(input, findInPageflags)) {
        count += 1;
    }

    if (count == 0) {
        ui->label_find_sum->clear();
    } else if (count == 1){
        ui->label_find_sum->setText(QString("%1 match").arg(count));
    } else {
        ui->label_find_sum->setText(QString("%1 matches").arg(count));
    }
}

void MainWindow::findInPage(const QString &nextOrPrev){
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

void MainWindow::showFindFrame() {
    if (ui->tb_scriptures->hasFocus()) {
        ui->cb_find_loc->setCurrentText(tr("Bible"));
    } else if (ui->info_tb->hasFocus()) {
        ui->cb_find_loc->setCurrentText("Info");
    } else {
        ui->cb_find_loc->setCurrentText("Strongs");
    }

    ui->frame_find->show();
    ui->lineEdit_find->clear();
    ui->label_find_sum->clear();
    ui->lineEdit_find->setFocus();
    textBrowser->moveCursor(QTextCursor::Start);
}

void MainWindow::modifyFindInPageFlags(){
    if (ui->chkBox_find_case->isChecked()) {
        findInPageflags.setFlag(QTextDocument::FindCaseSensitively, true);
    } else {
        findInPageflags.setFlag(QTextDocument::FindCaseSensitively, false);
    }

    if (ui->chkBox_find_whole->isChecked()) {
        findInPageflags.setFlag(QTextDocument::FindWholeWords, true);
    } else {
        findInPageflags.setFlag(QTextDocument::FindWholeWords, false);
    }
}

void MainWindow::setTranslation() {
    ui->tb_scriptures->clear();
    tlAbbr = ui->cb_select_translation->currentData().toString();
    QString full = ui->cb_select_translation->currentText();
    ui->search_lbl_tl->setText(QString("Search in:\n %1").arg(full));
}

void MainWindow::populateSearchCbs() {
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

    ui->search_cb_anyExact->addItem("any order", "any");
    ui->search_cb_anyExact->addItem("exact match", "exact");
}

void MainWindow::updateCbTranslations() {
    ui->cb_select_translation->clear();
    QString sql = "SELECT abbreviation, version, language "
                  "FROM bible_version_key ORDER BY abbreviation ASC";
    QSqlQuery query(sql, dbH.bibleDb );
    QString abbr;
    QString desc;

    while (query.next())
    {
        abbr = query.value(0).toString();
        desc = query.value(1).toString();
        ui->cb_select_translation->addItem(desc, abbr);
        settingsW->addToCb(abbr);
        makeHtmlW->addToCb(abbr, desc);
        exportBibleW->addToCb(abbr, desc);
    }
}

void MainWindow::printFromHistory(QHash<QString, int> job){
    printQ.enqueue(job);
    int bk = job["bk"];
    int c1 = job["c1"];
    bk < 40 ? ui->lw_books->setCurrentRow(bk - 1) : ui->lw_books->setCurrentRow(bk);
    setBookTitle();
    updateChapterWidget();
    ui->lw_chapters->setCurrentRow(c1 - 1);
//    ui->tb_scriptures->clear();
    processPrintQueue();
}

void MainWindow::addToHistory(QHash<QString, int> job){
    printHistory.enqueue(job);
    if (printHistory.length() > 10){
        printHistory.removeFirst();
    }

    int bk = job["bk"];
    QString bookName = ::g_bookNames[bk];
    int c1 = job["c1"];
    int c2 = job["c2"];
    int v1 = job["v1"];
    int v2 = job["v2"];

    QString aTxt;

    if (v1 > 0 && v2 > 0) {
        aTxt = QString("%1 %2:%3-%4").arg(bookName).arg(c1).arg(v1).arg(v2);
    } else if (v1 > 0) {
        aTxt = QString("%1 %2:%3").arg(bookName).arg(c1).arg(v1);
    } else if (c1 > 0 && c2 > c1){
        aTxt = QString("%1 %2-%3").arg(bookName).arg(c1).arg(c2);
    } else {
        aTxt = QString("%1 %2").arg(bookName).arg(c1);
    }

    QAction *jobAction = new QAction(this);
    jobAction->setText(aTxt);
    connect(jobAction, &QAction::triggered, this, [this, job] () {
        printFromHistory(job);}
    );
    ui->menu_history->addAction(jobAction);

    if (ui->menu_history->actions().count() > 10){
        ui->menu_history->removeAction(ui->menu_history->actions().at(0)); //actions().first());
    }
}

void MainWindow::processPrintQueue(){
    ui->tb_scriptures->clear();

    while (not printQ.isEmpty()) {
        printScriptures();
    }
}

//call this method for every job in the print queue with processPrintQueue
void MainWindow::printScriptures() {
    if(!ui->bible_frame->isVisible()){
        ui->background_frame->setStyleSheet(bfStyle);
        ui->bible_frame->show();
    }

    QHash<QString, int> job = printQ.dequeue();

    addToHistory(job);

    int bk = job["bk"];
    QString bkStr = QString::number(job["bk"]);
    QString bookName = ::g_bookNames[bk];
//    int c1 = job["c1"];
    QString c1Str = QString::number(job["c1"]);
    int c2 = job["c2"];
    QString c2Str = QString::number(job["c2"]);
    int v1 = job["v1"];
    QString v1Str = QString::number(job["v1"]);
    int v2 = job["v2"];
    QString v2Str = QString::number(job["v2"]);

    if(bk == 0) return;
//    if(c1 == 0) c1 = 1;

    QString chapterSql;
    QString verseSQL;

    if (c2 > 0 ) {
        chapterSql = QString("BETWEEN %1 AND %2").arg(c1Str, c2Str);
    } else {
        chapterSql = "= " + c1Str;
    }

    QString sql = QString("SELECT c, v, t from t_%1 where b = %2 and c %3 ")
            .arg(tlAbbr, bkStr, chapterSql);

    if (v1 > 0 and v2 > 0 and c2 == 0){
        verseSQL = QString("AND v BETWEEN %1 AND %2").arg(v1Str, v2Str);
        sql.append(verseSQL);
    } else if (v1 > 0 and c2 == 0){
        verseSQL = "AND v = " + v1Str;
        sql.append(verseSQL);
    }

    QSqlQuery query(sql, dbH.bibleDb);
    bool addHeader = false;

    QString sChapterTable = "<table cellpadding='1'>";
    QString sChapterBasic;
    QString headerTable, headerBasic, verseTable, verseBasic, sCh, sNr, txt;

    QVector<QString> endChar = {".", "!", "?"};
    int iNr;
    int prevNr = 0;
    int textL = 0;

    while (query.next()) {
        sCh = query.value(0).toString();
        sNr = query.value(1).toString();
        iNr = query.value(1).toInt();
        txt = query.value(2).toString();
        textL += txt.length();

        // header check
        // check for verse 1 and check for verse skip because of a partial chapter query
        if ( iNr == 1 || !(iNr == (prevNr + 1)) )
            addHeader = true;

        prevNr = iNr;

        if (addHeader) {
            addHeader = false;
            headerTable = QString("<tr><td></td><td style='color:#9E0E0D'>"
                                  "<small>%1 %2</small></td></tr>").arg(bookName, sCh);
            sChapterTable.append(headerTable);

            headerBasic = QString("<br><p style='text-align:center;color:#9E0E0D;"
                                  "font-family:serif;font-weight:normal;'>"
                                  "%1 %2</p><br>"
                                  ).arg(bookName, sCh);
            sChapterBasic.append(headerBasic);
        }

        // verse
        verseTable = QString(
            "<tr><td style='width:10px;color:#9E0E0D'><small>%1</small></span></td>"
            "<td style='color:#393E42;'> %2</td></tr>").arg(sNr, txt);

        verseBasic = QString("<span style='color:#393E42'>%1 </span>").arg(txt);

        // check for end of line/verse, since we want a break if size/lenght is above 300
        if (endChar.contains(txt.right(1)) && textL > 300) {
            verseBasic.append("<br>");

            if (textL > 450) {
                verseBasic.append("<br>");
            }

            textL = 0;
        }

        sChapterTable.append(verseTable);
        sChapterBasic.append(verseBasic);

    } // end query

    sChapterTable.append("</table><br>");
    sChapterBasic.append("<br>");


    if (scripDisplay == "table") {
        ui->tb_scriptures->insertHtml(sChapterTable);
    } else {
        ui->tb_scriptures->insertHtml(sChapterBasic);
    }

    if(iFilter && (guiLanguage == "english" || guiLanguage == "dutch")) filterImmersion();
    if(jFilter && (guiLanguage == "english" || guiLanguage == "dutch")) filterJudeans();
}

void MainWindow::filterImmersion() {
    // find and replace, baptism to immersion
    QTextDocument::FindFlags imFlag;
    imFlag.setFlag(QTextDocument::FindWholeWords);
    QTextCharFormat charF;
    charF.setFontItalic(true);
    QRegExp regE;
    regE.setCaseSensitivity(Qt::CaseInsensitive);
    QString replacement;

    // |de doper
    const QString regP = "dopen|gedoopt|doopt|doopte|de doop|de dopen|der dopen|"
            "ik doop|een doop|zijn doop|"
            "baptism|baptisms|baptised|baptized|baptize|baptise|baptizer";
    regE.setPattern(regP);

    QTextDocument *doc = ui->tb_scriptures->document();
    QTextCursor cursor(doc);
    QString match;

    while ( !cursor.isNull() && !cursor.atEnd() )
    {
        cursor = doc->find(regE, cursor, imFlag);

        if(!cursor.isNull())
        {
            match = cursor.selectedText().toLower();
            if(immersionDict.contains(match))
            {
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
    QRegExp regE;
    regE.setCaseSensitivity(Qt::CaseInsensitive);
    QString replacement;

    const QString regP = "Joden|Jood|Joods|Joodse|Jodin|het Jodendom|"
                    "Jews|Jew|Jewish|Jewess";
    regE.setPattern(regP);

    QTextDocument *doc = ui->tb_scriptures->document();
    QTextCursor cursor(doc);
    QString match;

    while ( !cursor.isNull() && !cursor.atEnd() )
    {
        cursor = doc->find(regE, cursor, jFlag);

        if(!cursor.isNull())
        {
            match = cursor.selectedText().toLower();
            if(judeansDict.contains(match))
            {
                replacement = judeansDict[match];
                charF.setToolTip(match + " (Judeans filter)");
                cursor.mergeCharFormat(charF);
                cursor.insertText(replacement);
            }
        }
    }
}

void MainWindow::setFilters() {
    if(ui->action_immersion_filter->isChecked()) {
        iFilter = true;
    } else {
        iFilter = false;
    }

    if(ui->action_judeans_filter->isChecked()) {
        jFilter = true;
    } else {
        jFilter = false;
    }

    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.setValue("Filters/iFilter", iFilter);
    settings.setValue("Filters/jFilter", jFilter);
}

void MainWindow::aboutFilters(){
    ui->background_frame->setStyleSheet(bfStyle);
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

void MainWindow::chapterSelected(){
    QHash<QString, int> job;
    int bookNumber = ui->lw_books->currentItem()->data(0x0100).toInt();
    int chapterNumber;

    QList lwItems = ui->lw_chapters->selectedItems();
    for (QListWidgetItem* item : lwItems ) {
        chapterNumber = item->data(0x0100).toInt();
        job["bk"] = bookNumber;
        job["c1"] = chapterNumber;
        printQ.append(job);
    }

    processPrintQueue();
}

void MainWindow::bookSelected(){
    if (ui->lw_books->currentRow() == 39) {
        printRequest("heb 8");
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

void MainWindow::setBookTitle(QString title) {
    if (title.isEmpty()) {
        title = ui->lw_books->currentItem()->text();
    }
    QString style = "color:rgba(57,62,66,230);font-size:30px;font-family:serif;"
                    "font-style:normal;font-weight:normal;margin:3px;padding:0px;";
    QGraphicsDropShadowEffect shadow;
    shadow.setBlurRadius(3);
    shadow.setColor(QColor(255, 255, 238));
    shadow.setOffset(2,2);
    ui->btn_book_title->setGraphicsEffect(&shadow);
    ui->btn_book_title->setStyleSheet(style);
    ui->btn_book_title->setText(title);
}

void MainWindow::updateChapterWidget(){
    if(!ui->bible_frame->isVisible()) {
        ui->background_frame->setStyleSheet(bfStyle);
        ui->bible_frame->show();
    }
    ui->lw_chapters->clear();
    int bookNumber = ui->lw_books->currentItem()->data(0x0100).toInt();
    int finalChapter = dbH.getFinalChapter(bookNumber);

    for (int i = 1; i <= finalChapter; ++i) {
        QListWidgetItem *newChapterItem = new QListWidgetItem;
        QString chapterNr = QString::number(i);
        newChapterItem->setText(chapterNr);
        newChapterItem->setData(Qt::UserRole, i);
        ui->lw_chapters->addItem(newChapterItem);
    }
}

void MainWindow::updateBooksWidget(){
    ui->lw_books->clear();
    ::g_bookNames.clear();
    ::g_bookNames.append("zero");

    QSqlQuery query("SELECT book_nr, name_" + bknLanguage + " from number_name", dbH.bibleDb);
    QString bookName;
    QString tooltip;
    int bookNr;

    while (query.next()) {
        bookNr = query.value(0).toInt();
        tooltip = "Book " + query.value(0).toString();
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

void MainWindow::getBooksAbbr(){
    ::g_booksAbbr.clear();
    ::g_booksAbbr.append("zero");
    QString abbr;

    QSqlQuery query("SELECT abbr_nl, abbr_en FROM abbr WHERE \"primary\" = '1' ", dbH.bibleDb);
    while (query.next()) {
        if(guiLanguage == "dutch") {
            abbr = query.value(0).toString();
        } else {
            abbr = query.value(1).toString();
        }
        ::g_booksAbbr.append(abbr);
    }
}

void MainWindow::toggleFullscreen(){
    if (isFullScreen()){
        showNormal();
        ui->tabwidget->show();
        ui->menu_bar->setMaximumHeight(23);
        ui->splitter_background->setMaximumSize(16777215, 16777215);
    } else {
        showFullScreen();

        // note that on med frame show() the maximum size will be used again
        int halfW = screen()->geometry().width() / 2;
        int height90 = (screen()->geometry().height() / 100) * 90;
        ui->splitter_background->setMaximumSize(halfW, height90);
        ui->background_frame->layout()->setAlignment(Qt::AlignCenter);

        ui->tabwidget->hide();
        ui->menu_bar->setMaximumHeight(1);
    }
}

void MainWindow::toggleBible(){
    ui->background_frame->setStyleSheet(bfStyle);
    ui->bible_frame->isVisible() ? ui->bible_frame->hide() : ui->bible_frame->show();
}

void MainWindow::toggleTabW(){
    ui->tabwidget->isVisible() ? ui->tabwidget->hide() : ui->tabwidget->show();
}

void MainWindow::toggleMenu(){
    ui->menu_bar->maximumHeight() == 23 ? ui->menu_bar->setMaximumHeight(1)
                                        : ui->menu_bar->setMaximumHeight(23);
}

void MainWindow::toggleInfo(){
    ui->background_frame->setStyleSheet(bfStyle);
    ui->info_frame->isVisible() ? ui->info_frame->hide() : ui->info_frame->show();
}

void MainWindow::setStyleSheets(){
    QFile bfFile(":/css/background_frame.css");
    QFile twFile(":/css/tab_widget.css");
    bfFile.open(QIODevice::ReadOnly | QIODevice::Text);
    twFile.open(QIODevice::ReadOnly | QIODevice::Text);

    bfStyle = bfFile.readAll();
    ui->background_frame->setStyleSheet(bfStyle);

    twStyle = twFile.readAll();
    ui->tabwidget->setStyleSheet(twStyle);

    bfFile.close();
    twFile.close();
}

void MainWindow::on_btn_select_today_clicked()
{
    ui->calendar->setSelectedDate(QDate::currentDate());
}

void MainWindow::closeEvent(QCloseEvent*){
    QApplication::closeAllWindows();
}

void MainWindow::exit()
{
    QApplication::quit();
}

