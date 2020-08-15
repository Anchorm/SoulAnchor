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
    Ui::SettingsWindow *ui;
    void getBooknameLanguages();
    void writeSettings();

public:
    void on_applyFont_btn_clicked();
    void cancelReset();

signals:
    void fontChanged(); // a signal on btn click - connect to mainwindow slot applyfont
};

#endif // SETTINGSWINDOW_H
