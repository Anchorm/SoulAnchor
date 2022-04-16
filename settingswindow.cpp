#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    connect(ui->ok_btn, &QPushButton::clicked, this, &SettingsWindow::writeSettings);
    connect(ui->can_btn, &QPushButton::clicked, this, &SettingsWindow::cancelSettings);
    connect(ui->apply_btn, &QPushButton::clicked, this, &SettingsWindow::applySettings);

    getBooknameLanguages();
    setCbGuiLang();
    getSchemes();
    setCbFontSize();
    setCbTranslations();
    cancelSettings(); //set the right states from config
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::setCbGuiLang() {
    // set values for the gui language QComboBox based on available qm translation files
    ui->gui_lang_cb->clear();
    QDir translationsDir(":/data/lang");

    QStringList languages = translationsDir.entryList(
                {"*.qm"}, QDir::Files, QDir::Name);

    for (QString lang: languages) {
        lang.chop(3);
        ui->gui_lang_cb->addItem(lang.mid(11));
    }
}

void SettingsWindow::setCbFontSize() {
    // populate the fontsize combobox
    ui->font_size_cb->clear();
    QList<int> fontSizes = QFontDatabase::standardSizes();

    for (const int &psize: fontSizes) {
        ui->font_size_cb->addItem(QString().number(psize));
    }
}

void SettingsWindow::setCbTranslations() {
    ui->tl_cb->clear();
    QString sql = "SELECT abbreviation "
                  "FROM bible_version_key ORDER BY abbreviation ASC";
    QSqlQuery query(sql, dbH.bibleDb );
    QString abbr;

    while (query.next())
    {
        abbr = query.value(0).toString();
        ui->tl_cb->addItem(abbr, abbr);
    }
}

void SettingsWindow::getSchemes() {
    // get the color schemes and fill the combobox
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    settings.beginGroup("Schemes");
    QStringList schemes = settings.allKeys();
    settings.endGroup();

    if (schemes.isEmpty()) {
        return;
    } else {
        ui->scheme_cb->addItems(schemes);
    }
}

void SettingsWindow::getBooknameLanguages() {
    QString columnName;
    // get all column names
    QSqlQuery getColumns("PRAGMA table_info(number_name);", ::dbH.bibleDb);
    while (getColumns.next()) {
        columnName = getColumns.value(1).toString();
        if (columnName.startsWith("name_")){
            bookNamesLanguages.append(columnName.mid(5));
        }
    }

    if (bookNamesLanguages.isEmpty()) {
        ui->bkn_lang_cb->addItem("english");
    } else {
        for (const QString &lang: qAsConst(bookNamesLanguages)) {
            ui->bkn_lang_cb->addItem(lang);
        }
    }
}

void SettingsWindow::cancelSettings() {
    // set to conf file values, after cancel btn click or on startup
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    guiLanguage = settings.value("guiLanguage", "english").toString();
    bknLanguage = settings.value("bknLanguage", "english").toString();
    startup = settings.value("startup", "nothing").toString();
    tab = settings.value("tab", "Contents").toString();
    translation = settings.value("translation", "AKJV").toString();

    font = settings.value("Font/font", "sans").toString();
    fontS =  settings.value("Font/fontsize", "12").toString();
    scrCheck =  settings.value("Font/scrCheck", "false").toBool();
    bkchCheck =  settings.value("Font/bkchCheck", "false").toBool();

    margin = settings.value("margin", "14").toInt();
    display = settings.value("display", "table").toString();
    width = settings.value("width", "10").toInt();

    activeScheme = settings.value("activeScheme", "none").toString();

    ui->gui_lang_cb->setCurrentText(guiLanguage);
    ui->bkn_lang_cb->setCurrentText(bknLanguage);
    ui->start_cb->setCurrentText(startup);
    ui->tab_cb->setCurrentText(tab);
    ui->tl_cb->setCurrentText(translation);

    ui->font_cb->setCurrentText(font);
    ui->font_size_cb->setCurrentText(fontS);
    if (scrCheck) {
        ui->font_script_chkb->setCheckState(Qt::Checked);
    } else {
        ui->font_script_chkb->setCheckState(Qt::Unchecked);
    }
    if (bkchCheck) {
        ui->font_bkch_chkb->setCheckState(Qt::Checked);
    } else {
        ui->font_bkch_chkb->setCheckState(Qt::Unchecked);
    }

    ui->margin_slider->setValue(margin);
    ui->width_slider->setValue(width);
    ui->display_cb->setCurrentText(display);
    ui->scheme_cb->setCurrentText(activeScheme);
    hide();

    emit booknameLangChanged(bknLanguage);
    emit schemeChanged(activeScheme);
    emit fontChanged(font, fontS, margin, width);
}

void SettingsWindow::writeSettings() {
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    guiLanguage = ui->gui_lang_cb->currentText();
    bknLanguage = ui->bkn_lang_cb->currentText();
    startup = ui->start_cb->currentText();
    tab = ui->tab_cb->currentText();
    translation = ui->tl_cb->currentText();

    font = ui->font_cb->currentText();
    fontS = ui->font_size_cb->currentText();
    scrCheck =  ui->font_script_chkb->isChecked();
    bkchCheck = ui->font_bkch_chkb->isChecked();

    margin = ui->margin_slider->value();
    display = ui->display_cb->currentText();
    width = ui->width_slider->value();
    activeScheme = ui->scheme_cb->currentText();

    settings.setValue("guiLanguage", guiLanguage);
    settings.setValue("bknLanguage", bknLanguage);
    settings.setValue("startup", startup);
    settings.setValue("tab", tab);
    settings.setValue("translation", translation);

    settings.setValue("Font/font", font);
    settings.setValue("Font/fontsize", fontS);
    settings.setValue("Font/scrCheck", scrCheck);
    settings.setValue("Font/bkchCheck",bkchCheck);

    settings.setValue("margin", margin);
    settings.setValue("display", display);
    settings.setValue("width", width);
    settings.setValue("activeScheme", activeScheme);

    settings.sync();
    this->hide();
    applySettings();
}

void SettingsWindow::applySettings() {
    bknLanguage = ui->bkn_lang_cb->currentText();
    font = ui->font_cb->currentText();
    fontS = ui->font_size_cb->currentText();
    activeScheme = ui->scheme_cb->currentText();
    margin = ui->margin_slider->value();
    width = ui->width_slider->value();

    emit booknameLangChanged(bknLanguage);
    emit schemeChanged(activeScheme);
    emit fontChanged(font, fontS, margin, width);
}
