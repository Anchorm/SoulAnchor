/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#ifndef IMAGEWINDOW_H
#define IMAGEWINDOW_H

#include "databasehandler.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QToolButton>
#include <QScreen>
#include <QMenu>
#include <QShortcut>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QToolBar>
#include <QComboBox>

class ImageWindow : public QWidget
{
    Q_OBJECT

    DatabaseHandler& dbH = DatabaseHandler::getInstance();
    QSqlDatabase& varDb = dbH.getDatabase("various");

    const QIcon anchorIcon = QIcon(":/data/img/anchor.png");
    QVBoxLayout *layVbox = new QVBoxLayout();
    qreal hsf; // horizontal scaling factor

    QGraphicsScene *scene = new QGraphicsScene(this);
    QGraphicsView *view = new QGraphicsView(this);
    QToolBar *toolbar = new QToolBar(this);

    QToolButton *zoomInBtn = new QToolButton(this);
    QToolButton *zoomOutBtn = new QToolButton(this);
    QToolButton *normalSizeBtn = new QToolButton(this);
    QToolButton *fitInViewBtn = new QToolButton(this);
    QToolButton *closeBtn = new QToolButton(this);
    QToolButton *fullscreenBtn = new QToolButton(this);

    QComboBox *cbSelectMaps = new QComboBox(this);

    const QIcon closeIcon = QIcon(":/data/img/standardbutton-close-32.png");
    const QIcon zoomInIcon = QIcon(":/data/img/zoom_in.png");
    const QIcon zoomOutIcon = QIcon(":/data/img/zoom_out.png");
    const QIcon zoomNormalIcon = QIcon(":/data/img/zoom_actual.png");
    const QIcon zoomFitIcon = QIcon(":/data/img/zoom_fit.png");
    const QIcon fullscreenIcon = QIcon(":/data/img/view-fullscreen.png");

    void centerWindow();
    void updateActions();
    void updateCbMaps();
    bool event(QEvent *event);

private slots:
    void zoomIn();
    void zoomOut();
    void fitInView();
    void normalSize();

public slots:
    void changeMap();
    void setPixmap(const QPixmap &pixmap, const QString &imgName);

public:
    explicit ImageWindow(QWidget *parent = nullptr);

signals:

};

#endif // IMAGEWINDOW_H
