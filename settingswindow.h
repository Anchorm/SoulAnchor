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
#include "setup.h"
#include "databasehandler.h"
#include <QWidget>
#include <QSqlQuery>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

    DatabaseHandler& dbH = DatabaseHandler::getInstance();
    QSqlDatabase& bibleDb = dbH.getDatabase("bibles");
    QSqlDatabase& varDb = dbH.getDatabase("various");
    QSqlDatabase& booksDb = dbH.getDatabase("books");

    QStringList bookNameLanguages;
    QString guiLanguage;
    QString bknLanguage;
    QString startup;
    QString tab;
    QString translation;
    QString subheadings;
    QString font;
    QString fontS;
    // use font for scriptures, bookslist, chapterslist
    bool scrCheck;
    bool bkCheck;
    bool chCheck;
    bool showMaps; // show map links
    bool showFilters; // show filters menu
    int margin;
    QString layout;
    QString activeScheme;
    int width;

    void setCbGuiLang();
    void getBooknameLanguages();
    void getSchemes();
    void setCbFontSize();
    void setCbTranslations();
    void setCbSubheadings();
    void emitSignals();

    Ui::SettingsWindow *ui;

private slots:
    void writeSettings();
    void applySettings();

public:
    void centerWindow();
    void cancelSettings();
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow() override;

signals:
    // connect to mainwindow slot applyFont
    void fontChanged(const QString &font, const QString &fontS,
                     const int &margin, const int &width,
                     const bool scrCheck, const bool bkCheck, const bool chCheck);
    void schemeChanged(const QString &aScheme); // connect to mainwindow applyScheme
    void booknameLangChanged(const QString &lang); // to mainwindow updateBooksWidget
    // mainwindow updateTLandSubh
    void subheadingsChanged(const QString &translation, const QString &subheadings);
    void showMapsChanged(const bool &showM);
    void showFiltersChanged(const bool &showF);
};

#endif // SETTINGSWINDOW_H
