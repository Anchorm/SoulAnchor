#include "settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent, Qt::Window),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    connect(ui->ok_btn, &QPushButton::clicked, this, &SettingsWindow::writeSettings);
    connect(ui->can_btn, &QPushButton::clicked, this, &SettingsWindow::cancel);
    connect(ui->applyFont_btn, &QPushButton::clicked, this ,
            &SettingsWindow::on_applyFont_btn_clicked );

    getBooknameLanguages(); // dynamic from database
    // gui language is dependent on the available .qm translation files that are compiled in
    // and the cb text is filled from the ui form

    getSchemes(); // from soulanchor.conf
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
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

    if (bookNamesLanguages.isEmpty()){
        ui->bkn_lang_cb->addItem("english");
    }
    for (const QString &lang: qAsConst(bookNamesLanguages)) {
        ui->bkn_lang_cb->addItem(lang);
    }
}

void SettingsWindow::addToCb(const QString &abbr) {
    // add to the available translation cb from mainW since there is already a function there
    ui->tl_cb->addItem(abbr, abbr);
}

void SettingsWindow::cancel() {
    // set to conf file values, after cancel btn click or on startup
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    guiLanguage = settings.value("guiLanguage", "english").toString();
    bknLanguage = settings.value("bknLanguage", "english").toString();
    startup = settings.value("startup", "nothing").toString();
    tab = settings.value("tab", "Contents").toString();
    translation = settings.value("translation", "AKJV").toString();
    font = settings.value("font", "sans").toString();
    fontS =  settings.value("fontsize", "12").toString();
    margin = settings.value("margin", "14").toString();
    display = settings.value("display", "table").toString();

    activeScheme = settings.value("activeScheme", "classic").toString();

    ui->gui_lang_cb->setCurrentText(guiLanguage);
    ui->bkn_lang_cb->setCurrentText(bknLanguage);
    ui->start_cb->setCurrentText(startup);
    ui->tab_cb->setCurrentText(tab);
    ui->tl_cb->setCurrentText(translation);

    ui->font_cb->setCurrentText(font);
    ui->pointSize_sb->setValue(fontS.toInt());

    ui->margin_le->setText(margin);
    ui->display_cb->setCurrentText(display);

    ui->scheme_cb->setCurrentText(activeScheme);

    hide();
}

void SettingsWindow::writeSettings() {
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    guiLanguage = ui->gui_lang_cb->currentText();
    bknLanguage = ui->bkn_lang_cb->currentText();
    startup = ui->start_cb->currentText();
    tab = ui->tab_cb->currentText();
    translation = ui->tl_cb->currentText();
    font = ui->font_cb->currentText();
    fontS = ui->pointSize_sb->text();
    margin = ui->margin_le->text();
    display = ui->display_cb->currentText();

    activeScheme = ui->scheme_cb->currentText();

    settings.setValue("guiLanguage", guiLanguage);
    settings.setValue("bknLanguage", bknLanguage);
    settings.setValue("startup", startup);
    settings.setValue("tab", tab);
    settings.setValue("translation", translation);
    settings.setValue("font", font);
    settings.setValue("fontsize", fontS);
    settings.setValue("margin", margin);
    settings.setValue("display", display);

    settings.setValue("activeScheme", activeScheme);

    settings.sync();

    this->hide();
}

void SettingsWindow::on_applyFont_btn_clicked()
{
    QSettings settings(settingsFile.fileName(), QSettings::IniFormat);
    font = ui->font_cb->currentText();
    fontS = ui->pointSize_sb->text();
    margin = ui->margin_le->text();

    activeScheme = ui->scheme_cb->currentText();

    settings.setValue("font", font);
    settings.setValue("fontsize", fontS);
    settings.setValue("margin", margin);

//    settings.setValue("activeScheme", activeScheme);

    emit fontChanged();
    emit schemeChanged(activeScheme);
}
