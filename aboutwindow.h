/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef ABOUTWINDOW_H
#define ABOUTWINDOW_H

#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QTextEdit>
#include <QGuiApplication>
#include <QScreen>
#include <QPushButton>
#include <QFile>
#include <QDesktopServices>

class AboutWindow : public QWidget
{
    Q_OBJECT

    const QIcon anchorIcon = QIcon(":/data/img/anchor.png");
    QTabWidget *tabw = new QTabWidget(this);
    QLabel *lblAnchor = new QLabel(this);
    QLabel *lblInfo = new QLabel(this);
    QLabel *lblLic = new QLabel(this);

    QPushButton *amPb = new QPushButton(this);

    QTextEdit *teCredit = new QTextEdit(this);

    QFrame *aboutFrame = new QFrame(this);
    QFrame *creditFrame = new QFrame(this);
    QFrame *licenseFrame = new QFrame(this);

    QVBoxLayout *mainVbox = new QVBoxLayout();
    QVBoxLayout *aboutVbox = new QVBoxLayout();
    QVBoxLayout *creditVbox = new QVBoxLayout();
    QVBoxLayout *licenseVbox = new QVBoxLayout();

public:
    explicit AboutWindow(QWidget *parent = nullptr);
    void centerWindow();

};

#endif // ABOUTWINDOW_H
