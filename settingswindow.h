/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

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
    QString subheadings;
    QString font;
    QString fontS;
    bool scrCheck;
    bool bkCheck;
    bool chCheck;
    int margin;
    QString display;
    QString activeScheme;
    int width;

    void setCbGuiLang();
    void getBooknameLanguages();
    void getSchemes();
    void setCbFontSize();
    void setCbTranslations();
    void setCbSubheadings();
    void cancelSettings();
    void writeSettings();
    void applySettings();

    Ui::SettingsWindow *ui;

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;

signals:
    // a signal on btn click - connect to mainwindow slot applyFont
    void fontChanged(const QString &font, const QString &fontS,
                     const int &margin, const int &width);
    void schemeChanged(const QString &aScheme); // connect to mainwindow applyScheme
    void booknameLangChanged(const QString &lang); // to mainwindow updateBooksWidget
    // mainwindow updateTlandSub
    void subheadingsChanged(const QString &translation, const QString &subheadings);
};

#endif // SETTINGSWINDOW_H
