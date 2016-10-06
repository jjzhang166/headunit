#ifndef CLICKABLEVIDEOWIDGET_H
#define CLICKABLEVIDEOWIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include "QGst/Ui/VideoWidget"

class ClickableVideoWidget : public QGst::Ui::VideoWidget {
  Q_OBJECT
signals:
  void mousePressed(QPoint *);
  void mouseMoved(QPoint *);
  void mouseReleased(QPoint *);

public:
  ClickableVideoWidget(QWidget *parent = 0, Qt::WindowFlags f = 0);
  void mousePressEvent(QMouseEvent *ev);
  void mouseMoveEvent(QMouseEvent *ev);
  void mouseReleaseEvent(QMouseEvent *ev);
};

#endif // CLICKABLEVIDEOWIDGET_H
