#pragma once

#include <QWidget>
#include <QPixmap>

class Icon : public QWidget {
public:
    Icon(QWidget *parent) : QWidget(parent) {};
    void setPixmap(const QPixmap &pixmap);
    const QPixmap pixmap();

protected:
    void paintEvent(QPaintEvent *event);
    QPixmap mPixmap;
};
