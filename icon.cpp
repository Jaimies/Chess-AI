#include "icon.h"
#include <QWidget>
#include <QPainter>

void Icon::setPixmap(const QPixmap &pixmap) {
    mPixmap = pixmap;
    update();
}

void Icon::paintEvent(QPaintEvent *event) {
    if (mPixmap.isNull()) {
        return;
    }
    double width = this->width();
    double height = this->height();
    double pixmapWidth = mPixmap.width();
    double pixmapHeight = mPixmap.height();
    double scale = qMax(width / pixmapWidth, height / pixmapHeight);
    QTransform transform;
    transform.translate(width / 2, height / 2);
    transform.scale(scale, scale);
    transform.translate(-pixmapWidth / 2, -pixmapHeight / 2);
    QPainter painter(this);
    painter.setTransform(transform);
    painter.drawPixmap(QPoint(0, 0), mPixmap);
}

const QPixmap Icon::pixmap() { return mPixmap; }
