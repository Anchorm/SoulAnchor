#ifndef ROSTER_H
#define ROSTER_H

#include "databasehandler.h"
#include "ui_roster.h"
#include <QWidget>
#include <cmath>

namespace Ui {
class Roster;
}

class Roster : public QWidget
{
    Q_OBJECT

public:
    explicit Roster(QWidget *parent = nullptr);
    ~Roster();

private slots:
    void on_pb_close_clicked();
    void on_pb_make_clicked();
    void on_pb_preview_clicked();
    void populateCb();
    void hideEvent(QHideEvent *);

    void on_cb_what_currentTextChanged();

private:
    Ui::Roster *ui;

signals:
    void rosterCreated(); // a new roster has been created - connect to mainwindow addRostersToMenu
};

#endif // ROSTER_H
