#include "clickablevideowidget.h"

ClickableVideoWidget::ClickableVideoWidget(QWidget *parent, Qt::WindowFlags f)
    : QGst::Ui::VideoWidget(parent, f){};

void ClickableVideoWidget::mousePressEvent(QMouseEvent *ev) {
  QPoint p = ev->pos();
  emit mousePressed(&p);
}

void ClickableVideoWidget::mouseMoveEvent(QMouseEvent *ev) {
  QPoint p = ev->pos();
  emit mouseMoved(&p);
}
void ClickableVideoWidget::mouseReleaseEvent(QMouseEvent *ev) {
  QPoint p = ev->pos();
  emit mouseReleased(&p);
}
