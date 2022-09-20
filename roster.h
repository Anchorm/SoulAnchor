/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef ROSTER_H
#define ROSTER_H

#include "ui_roster.h"
#include "globals.h"
#include "databasehandler.h"
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
    ~Roster() override;

private slots:
    void on_pb_close_clicked();
    void on_pb_make_clicked();
    void on_pb_preview_clicked();
    void populateCb();
    void hideEvent(QHideEvent *) override;

    void on_cb_what_currentTextChanged();

private:
    Ui::Roster *ui;

signals:
    void rosterCreated(); // a new roster has been created - connect to mainwindow addRostersToMenu
};

#endif // ROSTER_H
