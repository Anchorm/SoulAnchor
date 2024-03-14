/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/
#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    connect(ui->ok_btn, &QToolButton::clicked, this, &SettingsWindow::writeSettings);
    connect(ui->can_btn, &QToolButton::clicked, this, &SettingsWindow::cancelSettings);
    connect(ui->apply_btn, &QToolButton::clicked, this, &SettingsWindow::applySettings);

    getBooknameLanguages();
    setCbGuiLang();
    getSchemes();
    setCbFontSize();
    setCbTranslations();
    setCbSubheadings();
    cancelSettings(); //set the right states from config
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setCbGuiLang()
{
    // set values for the gui language QComboBox based on available qm translation files
    ui->gui_lang_cb->clear();
    QDir translationsDir(dataDir.path() + "/locales/");

    QStringList languages = translationsDir.entryList(
                {"soulanchor_*.qm"}, QDir::Files, QDir::Name);

    if (!languages.isEmpty()) {
        languages.sort();
    } else {
        ui->gui_lang_cb->addItem(QIcon(":/data/flags/en.svg"), "en" );
        return;
    }

    for (QString lang: languages) {
        lang.chop(3);
        QString langcode = lang.mid(11).toLower();
        ui->gui_lang_cb->addItem(QIcon(":/data/flags/" + langcode + ".svg"), langcode );
    }
}

void SettingsWindow::setCbFontSize()
{
    // populate the fontsize combobox
    ui->font_size_cb->clear();
    QList<int> fontSizes = QFontDatabase::standardSizes();

    for (const int &psize: fontSizes)
        ui->font_size_cb->addItem(QString().number(psize));
}

void SettingsWindow::setCbTranslations()
{
    ui->tl_cb->clear();
    QString sql = "SELECT abbreviation "
                  "FROM version_info ORDER BY abbreviation ASC";
    QSqlQuery query(sql, bibleDb );
    QString abbr;

    while (query.next()) {
        abbr = query.value(0).toString().toUpper();
        ui->tl_cb->addItem(abbr, abbr);
    }
}

void SettingsWindow::setCbSubheadings()
{
    ui->subheadings_cb->clear();
    QString sql = "SELECT * from pragma_table_list ";
    QSqlQuery query(sql, varDb );
    QString columnName;
    QString subheadings;
    ui->subheadings_cb->addItem("none");

    while (query.next()) {
        columnName = query.value(1).toString();
        if (columnName.startsWith("subheadings_")) {
            subheadings = columnName.sliced(12).toUpper();
            ui->subheadings_cb->addItem(subheadings);
        }
    }
}

void SettingsWindow::getSchemes()
{
    // get the color schemes and fill the combobox
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.beginGroup("schemes");
    QStringList schemes = settings.allKeys();
    settings.endGroup();

    if (schemes.isEmpty())
        return;
    else
        ui->scheme_cb->addItems(schemes);
}

void SettingsWindow::getBooknameLanguages()
{
    bookNameLanguages.clear();
    QString tableName;
    // get all table names that start with lang_
    QSqlQuery getTables("SELECT * from pragma_table_list", booksDb);

    while (getTables.next()) {
        tableName = getTables.value(1).toString();
        if (tableName.startsWith("lang_"))
            bookNameLanguages.append(tableName.mid(5).toLower());
    }
    bookNameLanguages.sort();

    if (bookNameLanguages.isEmpty()) {
        ui->bkn_lang_cb->addItem("en");
    } else {
        for (const QString &lang: std::as_const(bookNameLanguages))
            ui->bkn_lang_cb->addItem(QIcon(":/data/flags/" + lang + ".svg"), lang);
    }
}

void SettingsWindow::cancelSettings()
{
    // set to conf file values, after cancel btn, and on startup
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    guiLanguage = settings.value("guiLanguage", "en").toString().toLower();
    bknLanguage = settings.value("booknameLanguage", "en").toString().toLower();
    startup = settings.value("startup", "nothing").toString();
    tab = settings.value("tab", "Contents").toString();
    translation = settings.value("translation", "net").toString().toLower();
    subheadings = settings.value("subheadings", "net").toString().toLower();
    showMaps = settings.value("showMaps", "true").toBool();
    showFilters = settings.value("filters/menuVisible", "false").toBool();

    font = settings.value("font/font", "sans").toString();
    fontS =  settings.value("font/fontsize", "12").toString();
    scrCheck =  settings.value("font/scripturesChecked", "false").toBool();
    bkCheck =  settings.value("font/booksChecked", "false").toBool();
    chCheck =  settings.value("font/chaptersChecked", "false").toBool();

    margin = settings.value("margin", "14").toInt();
    layout = settings.value("layout", "table").toString();
    width = settings.value("width", "10").toInt();

    activeScheme = settings.value("activeScheme", "system").toString();

    ui->gui_lang_cb->setCurrentText(guiLanguage);
    ui->bkn_lang_cb->setCurrentText(bknLanguage);
    ui->start_cb->setCurrentText(startup);
    ui->tab_cb->setCurrentText(tab);
    ui->tl_cb->setCurrentText(translation.toUpper());
    ui->subheadings_cb->setCurrentText(subheadings.toUpper());

    if (showMaps)
        ui->showmaps_cb->setCurrentIndex(0);
    else
        ui->showmaps_cb->setCurrentIndex(1);

    if (showFilters)
        ui->filters_cb->setCurrentIndex(0);
    else
        ui->filters_cb->setCurrentIndex(1);

    ui->font_cb->setCurrentText(font);
    ui->font_size_cb->setCurrentText(fontS);

    // scriptures checked, books listwidget, chapters listwidget
    scrCheck ? ui->font_script_chkb->setCheckState(Qt::Checked) :
               ui->font_script_chkb->setCheckState(Qt::Unchecked);
    bkCheck ? ui->font_bk_chkb->setCheckState(Qt::Checked) :
              ui->font_bk_chkb->setCheckState(Qt::Unchecked);
    chCheck ? ui->font_ch_chkb->setCheckState(Qt::Checked) :
              ui->font_ch_chkb->setCheckState(Qt::Unchecked);

    ui->margin_slider->setValue(margin);
    ui->width_slider->setValue(width);
    ui->layout_cb->setCurrentText(layout);
    ui->scheme_cb->setCurrentText(activeScheme);
    hide();

    emitSignals();
}

void SettingsWindow::writeSettings()
{
    // Save btn
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    guiLanguage = ui->gui_lang_cb->currentText();
    bknLanguage = ui->bkn_lang_cb->currentText();
    startup = ui->start_cb->currentText();
    tab = ui->tab_cb->currentText();
    // translation lowercase in db and code except in comboboxes
    translation = ui->tl_cb->currentText().toLower();
    subheadings = ui->subheadings_cb->currentText().toLower();

    if (ui->showmaps_cb->currentIndex() == 0)
        showMaps = true;
    else
        showMaps = false;

    if (ui->filters_cb->currentIndex() == 0)
        showFilters = true;
    else
        showFilters = false;

    font = ui->font_cb->currentText();
    fontS = ui->font_size_cb->currentText();
    scrCheck =  ui->font_script_chkb->isChecked();
    bkCheck = ui->font_bk_chkb->isChecked();
    chCheck = ui->font_ch_chkb->isChecked();

    margin = ui->margin_slider->value();
    layout = ui->layout_cb->currentText();
    width = ui->width_slider->value();
    activeScheme = ui->scheme_cb->currentText();

    settings.setValue("guiLanguage", guiLanguage);
    settings.setValue("booknameLanguage", bknLanguage);
    settings.setValue("startup", startup);
    settings.setValue("tab", tab);
    settings.setValue("translation", translation);
    settings.setValue("subheadings", subheadings);
    settings.setValue("showMaps", showMaps);
    settings.setValue("filters/menuVisible", showFilters);

    settings.setValue("font/font", font);
    settings.setValue("font/fontsize", fontS);
    settings.setValue("font/scripturesChecked", scrCheck);
    settings.setValue("font/booksChecked", bkCheck);
    settings.setValue("font/chaptersChecked", chCheck);

    settings.setValue("margin", margin);
    settings.setValue("layout", layout);
    settings.setValue("width", width);
    settings.setValue("activeScheme", activeScheme);

    settings.sync();
    this->hide();
    applySettings();
}

void SettingsWindow::applySettings()
{
    translation = ui->tl_cb->currentText().toLower();
    subheadings = ui->subheadings_cb->currentText().toLower();
    bknLanguage = ui->bkn_lang_cb->currentText();

    if (ui->showmaps_cb->currentIndex() == 0)
        showMaps = true;
    else
        showMaps = false;

    if (ui->filters_cb->currentIndex() == 0)
        showFilters = true;
    else
        showFilters = false;

    font = ui->font_cb->currentText();
    fontS = ui->font_size_cb->currentText();
    activeScheme = ui->scheme_cb->currentText();
    margin = ui->margin_slider->value();
    width = ui->width_slider->value();
    scrCheck =  ui->font_script_chkb->isChecked();
    bkCheck = ui->font_bk_chkb->isChecked();
    chCheck = ui->font_ch_chkb->isChecked();
    guiLanguage = ui->gui_lang_cb->currentText();

    emitSignals();
}

void SettingsWindow::emitSignals()
{
    emit booknameLangChanged(bknLanguage);
    emit showMapsChanged(showMaps);
    emit showFiltersChanged(showFilters);
    emit subheadingsChanged(translation, subheadings);
    emit fontChanged(font, fontS, margin, width, scrCheck, bkCheck, chCheck);
    emit schemeChanged(activeScheme);
    LanguageManager::changeLanguage(guiLanguage);
    ui->retranslateUi(this);
}

void SettingsWindow::centerWindow()
{
    if (parentWidget()) {
        QScreen* activeScreen = parentWidget()->screen();
        if (activeScreen != nullptr) {
            auto winGeo = frameGeometry();
            auto parentGeoCenter = parentWidget()->geometry().center();
            winGeo.moveCenter(parentGeoCenter);
            move(winGeo.topLeft());
        }
    }
}
