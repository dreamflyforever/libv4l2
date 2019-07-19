#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QLabel>
#include <QImage>

#include "libv4l2.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    int cameraInit();
    int cameraEXit();

private slots:
    void repaintImage();
    void btnCtrlClicked();

private:
    Ui::Widget *ui;
    QTimer *timer;
    QLabel *label;
    QImage *image;

    int width;
    int height;

    bool camStatus;
    int fd;
    struct v4l2_buf* v4l2_buf;
};

#endif // WIDGET_H
