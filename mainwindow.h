#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPoint>
#include <headunitworker.h>
#include <iostream>
#include "headunit/hu_gst.h"
#include "clickablevideowidget.h"

#define MOUSE_DOWN 0
#define MOUSE_UP 1
#define MOUSE_MOVE 2

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
private slots:
  bool huButtonClick();
  bool huVideoClick(QPoint *point);
  bool huVideoMove(QPoint *point);
  bool huVideoRelease(QPoint *point);
signals:
  void stopHU();

private:
  Ui::MainWindow *ui;
  int mouseStatus = MOUSE_UP;
  // headunitworker *headunitThread;
};
#endif // MAINWINDOW_H
