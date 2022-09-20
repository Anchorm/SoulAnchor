/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef PARWINDOW_H
#define PARWINDOW_H

#include "flowlayout.h"
#include "globals.h"
#include "databasehandler.h"

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSizePolicy>
#include <QTextEdit>
#include <QCheckBox>
#include <QList>
#include <QQueue>
#include <QMenu>
#include <QGuiApplication>
#include <QScreen>
#include <QSqlQuery>
#include <QString>
#include <QShortcut>

class ParWindow : public QWidget
{
    Q_OBJECT

    const QIcon anchorIcon = QIcon(":/data/img/anchor.png");
    const QIcon bookIcon = QIcon(":/data/img/book.png");
    const QIcon nextIcon = QIcon(":/data/img/bullet_arrow_right.png");
    const QIcon prevIcon = QIcon(":/data/img/bullet_arrow_left.png");
    const QIcon selectIcon = QIcon(":/data/img/edit-select-all");
    const QIcon copyIcon = QIcon(":/data/img/edit-copy");
    const QIcon closeIcon = QIcon(":/data/img/closedock-16.png");
    const QIcon scrollIcon = QIcon(":/data/img/script_yellow.png");

    QComboBox *cb_select = new QComboBox(this);
    QLineEdit *leInput = new QLineEdit(this);

    QPushButton *btn_ok = new QPushButton(this);
    QPushButton *btn_prev = new QPushButton(this);
    QPushButton *btn_next = new QPushButton(this);

    FlowLayout *flowLayout = new FlowLayout(-1,7,-1);
    QHBoxLayout *hbox_2 = new QHBoxLayout();

    QVBoxLayout *vbox = new QVBoxLayout;
    QTextEdit *te = new QTextEdit(this);

    QList<QCheckBox*> chkBoxes; // checkboxes to select the translations to use

    QString bookName;
    int lastBookNumber = 0;
    int lastChapNumber = 0;
    QQueue< QHash<QString, int> > history;
    QQueue< QHash<QString, int> > printQ ;

    QMenu *otMenu = new QMenu("OT", this);
    QMenu *ntMenu = new QMenu("NT", this);

    QHash<QString, QString> scheme;

    QString pattern =
            "\\s*(?<prt>[1-3]?)"
            "\\s*(?<bk>[a-zA-Zëüï]+)"
            "\\s*(?<ch1>\\d?\\d?\\d?)"
            "-?(?<ch2>\\d?\\d?\\d?)"
            ":?(?<vs1>\\d?\\d?\\d?)"
            "-?(?<vs2>\\d?\\d?\\d?)";

    QRegularExpression *re = new QRegularExpression(pattern);


private slots:
    void nextChapter();
    void prevChapter();
    void popupChapters(int bkNr);
    void createOtNtMenus();
    void ccMenuParW();
    void checkTls();
    void printScriptures();
    void printRequest();
    void centerWindow();

public slots:
    void setStyle(const QHash<QString, QString> &clrScheme);
    void setTlandJob(const QString &tlAbbr, const QHash<QString, int> &job);

public:
    explicit ParWindow(QWidget *parent = nullptr);

};

#endif // PARWINDOW_H
