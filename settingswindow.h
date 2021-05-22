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

public:
    void addToCb(const QString &abbr);
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

private:
    QStringList bookNamesLanguages;
    QString guiLanguage;
    QString bknLanguage;
    QString startup;
    QString tab;
    QString translation;
    QString font;
    QString fontS;
    QString margin;
    QString display;

    QString activeScheme;

    Ui::SettingsWindow *ui;
    void getBooknameLanguages();
    void writeSettings();
    void getSchemes();

public:
    void on_applyFont_btn_clicked();
    void cancel();

signals:
    void fontChanged(); // a signal on btn click - connect to mainwindow slot applyFont
    void schemeChanged(const QString &aScheme); // connect to applyScheme
};

#endif // SETTINGSWINDOW_H
