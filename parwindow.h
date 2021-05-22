#ifndef PARWINDOW_H
#define PARWINDOW_H

#include "databasehandler.h"
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QComboBox>
#include <QSizePolicy>
#include <QTextEdit>
#include <QCheckBox>
#include <QVector>
#include <QList>
#include <QQueue>
#include <QMenu>
#include <QGuiApplication>
#include <QScreen>
#include <QSqlQuery>
#include <QVector>
#include <QList>
#include <QCheckBox>
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
    const QIcon closeIcon = QIcon(":/data/img/window-close.png");
    const QIcon scrollIcon = QIcon(":/data/img/script_yellow.png");

    QLineEdit *leInput = new QLineEdit(this);
    QPushButton *btn_ok = new QPushButton(this);
    QPushButton *btn_prev = new QPushButton(this);
    QPushButton *btn_next = new QPushButton(this);

    QHBoxLayout *hbox_2 = new QHBoxLayout;
    QHBoxLayout *hbox_1 = new QHBoxLayout;

    QComboBox *cb_select = new QComboBox(this);
    QSpacerItem *spacer = new QSpacerItem(60, 30, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *spacer2 = new QSpacerItem(30, 30, QSizePolicy::Fixed, QSizePolicy::Fixed);
    QSpacerItem *spacer3 = new QSpacerItem(99999, 30, QSizePolicy::Preferred, QSizePolicy::Fixed);

    QVBoxLayout *vbox = new QVBoxLayout;
    QTextEdit *te = new QTextEdit(this);

    QVector<QCheckBox*> chkBoxes; // checkboxes to select the translations to use

    QString bookName;
    int lastBookNumber = 0;
    int lastChapNumber = 0;
    QQueue< QHash<QString, int> > history;
    QQueue< QHash<QString, int> > printQ ;

    QMenu *otMenu = new QMenu("OT", this);
    QMenu *ntMenu = new QMenu("NT", this);

    QHash<QString, QString> scheme;

private slots:
    void nextChapter();
    void prevChapter();
    void popupChapters(int bkNr);
    void createOtNtMenus();
    void ccMenuParW();
    void checkTls();
    void printScriptures();
    void printRequest();
    void toggleFullscreen();
    void escapeKey();
    void centerWindow();

public slots:
    void setStyle(const QHash<QString, QString> &clrScheme);
    void setTlandJob(const QString &tlAbbr, const QHash<QString, int> &job);

public:
    explicit ParWindow(QWidget *parent = nullptr);

};

#endif // PARWINDOW_H
