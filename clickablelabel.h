// https://wiki.qt.io/Clickable_QLabel

#ifndef CLICKABLELABEL_H
#define CLICKABLELABEL_H

#include <QLabel>
#include <QWidget>
#include <Qt>

class ClickableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableLabel(QWidget* parent = Q_NULLPTR, [[maybe_unused]]Qt::WindowFlags f = Qt::WindowFlags());
    ~ClickableLabel() override;

signals:
    void clicked();

protected:
    void mousePressEvent([[maybe_unused]]QMouseEvent* event) override;

};

#endif // CLICKABLELABEL_H
