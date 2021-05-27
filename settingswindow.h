#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "ui_settingswindow.h"
#include "globals.h"
#include "databasehandler.h"
#include <QWidget>
#include <QSqlQuery>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

    QStringList bookNamesLanguages;
    QString guiLanguage;
    QString bknLanguage;
    QString startup;
    QString tab;
    QString translation;
    QString font;
    QString fontS;
    bool scrCheck;
    bool bkchCheck;
    QString margin;
    QString display;
    QString activeScheme;

    void setCbGuiLang();
    void getBooknameLanguages();
    void getSchemes();
    void setCbFontSize();
    void setCbTranslations();
    void cancelSettings();
    void writeSettings();
    void applySettings();

    Ui::SettingsWindow *ui;

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;

signals:
    // a signal on btn click - connect to mainwindow slot applyFont
    void fontChanged(const QString &font, const QString &fontS, const QString &margin);
    void schemeChanged(const QString &aScheme); // connect to mainwindow applyScheme
    void booknameLangChanged(const QString &lang); // to mainwindow updateBooksWidget
};

#endif // SETTINGSWINDOW_H
