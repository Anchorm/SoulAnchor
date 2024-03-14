/******************************************************
   SoulAnchor - X11 Bible reading tool
   by Anchorman - soulanchor at protonmail dot com

   this hope we have as an anchor of the soul
   a hope both sure and steadfast
   and one which enters within the veil
   (Hebrews 6:19)

*******************************************/

#include "imagewindow.h"

ImageWindow::ImageWindow(QWidget *parent)
    : QWidget{parent, Qt::Window}
{
    setWindowTitle("Image Window - SoulAnchor");
    setObjectName("ImageWindow");
    setWindowIcon(anchorIcon);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    setMinimumSize(200, 200);

    connect(zoomInBtn, &QToolButton::clicked, this, &ImageWindow::zoomIn);
    connect(zoomOutBtn, &QToolButton::clicked, this, &ImageWindow::zoomOut);
    connect(normalSizeBtn, &QToolButton::clicked, this, &ImageWindow::normalSize);
    connect(fitInViewBtn, &QToolButton::clicked, this, &ImageWindow::fitInView);
    connect(closeBtn, &QToolButton::clicked, this, [this] () {hide();});
    connect(fullscreenBtn, &QToolButton::clicked, this, [this] () {
        isFullScreen() ? showNormal() : showFullScreen(); });

    zoomInBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    zoomInBtn->setIcon(zoomInIcon);
    zoomInBtn->setText(tr("in"));

    zoomOutBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    zoomOutBtn->setIcon(zoomOutIcon);
    zoomOutBtn->setText(tr("out"));

    normalSizeBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    normalSizeBtn->setIcon(zoomNormalIcon);
    normalSizeBtn->setText(tr("normal"));

    fitInViewBtn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    fitInViewBtn->setIcon(zoomFitIcon);
    fitInViewBtn->setText(tr("fit"));

    fullscreenBtn->setToolTip(tr("fullscreen"));
    fullscreenBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    fullscreenBtn->setIcon(fullscreenIcon);
    fullscreenBtn->setShortcut(Qt::Key_F11);

    closeBtn->setToolTip(tr("close"));
    fullscreenBtn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    closeBtn->setIcon(closeIcon);

    new QShortcut(Qt::Key_Escape, this, [this]() {
        if ( isFullScreen() ) showNormal(); });

    cbSelectMaps->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    toolbar->addWidget(cbSelectMaps);
    toolbar->addWidget(zoomInBtn);
    toolbar->addWidget(zoomOutBtn);
    toolbar->addWidget(normalSizeBtn);
    toolbar->addWidget(fitInViewBtn);
    toolbar->addWidget(fullscreenBtn);
    toolbar->addWidget(closeBtn);

    view->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setDragMode(QGraphicsView::ScrollHandDrag);
    view->setFrameStyle(0);
    view->setScene(scene);

    layVbox->setContentsMargins(0,0,0,0);
    layVbox->setSpacing(0);
    layVbox->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
    layVbox->addWidget(view);
    layVbox->addWidget(toolbar);
    setLayout(layVbox);

    updateCbMaps();
    connect(cbSelectMaps, &QComboBox::currentIndexChanged,
            this, &ImageWindow::changeMap);
}

void ImageWindow::updateCbMaps()
{
    QString sqlGetMaps {"select name from img_data"};
    QSqlQuery query(sqlGetMaps, varDb );
    QString title;
    while (query.next()) {
        title = query.value(0).toString();
        cbSelectMaps->addItem(title);
    }
}

void ImageWindow::changeMap()
{
    QString imgName {cbSelectMaps->currentText()};
    QPixmap pixmap {};
    QString sqlGetImg {QString("select name, content from img_data where "
                               "name is '%1'").arg(imgName)};
    QSqlQuery queryImg(sqlGetImg, varDb);

    if (queryImg.next()) {
        pixmap.loadFromData(queryImg.value(1).toByteArray());
    }

    setPixmap(pixmap, imgName);
}

void ImageWindow::setPixmap(const QPixmap &pixmap, const QString &imgName)
{
    setWindowTitle(imgName);
    view->resetTransform();
    view->scene()->clear();

    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    scene->setSceneRect(pixmap.rect());
    scene->addItem(item);
    cbSelectMaps->setCurrentText(imgName);

    if (this->isHidden()) {
        this->show();
        centerWindow();
    } else {
        this->raise();
    }

    view->fitInView(pixmap.rect(), Qt::KeepAspectRatio);
    updateActions();
}

void ImageWindow::normalSize()
{
    view->resetTransform();
    updateActions();
}

void ImageWindow::fitInView()
{
    QRectF rect = scene->sceneRect();
    view->fitInView(rect, Qt::KeepAspectRatio);
    updateActions();
}

void ImageWindow::zoomIn()
{
    hsf = view->transform().m11();
    if (hsf < 2) {
        view->scale(1.5, 1.5);
        updateActions();
    }
}

void ImageWindow::zoomOut()
{
    hsf = view->transform().m11();
    if (hsf > 0.5) {
        view->scale(0.5, 0.5);
        updateActions();
    }
}

void ImageWindow::updateActions()
{
    hsf = view->transform().m11();
    hsf > 0.5 ? zoomOutBtn->setEnabled(true) : zoomOutBtn->setEnabled(false);
    hsf < 2 ? zoomInBtn->setEnabled(true) : zoomInBtn->setEnabled(false);
}

void ImageWindow::centerWindow()
{
    if (parentWidget())
    {
        QScreen* activeScreen = parentWidget()->screen();
        if (activeScreen != nullptr)
        {
            int width = ((parentWidget()->width() / 100) * 90);
            int height = ((parentWidget()->height() / 100) * 90);
            resize(width, height);

            auto winGeo = frameGeometry();
            auto parentGeoCenter = parentWidget()->geometry().center();
            winGeo.moveCenter(parentGeoCenter);
            move(winGeo.topLeft());
        }
    }
}

bool ImageWindow::event(QEvent *event) {
    if (event->type() == QEvent::LanguageChange) {
        zoomInBtn->setText(tr("in"));
        zoomOutBtn->setText(tr("out"));
        normalSizeBtn->setText(tr("normal"));
        fitInViewBtn->setText(tr("fit"));
    }
    return QWidget::event(event);
}
