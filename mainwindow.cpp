#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  connect(ui->pushButton, SIGNAL(released()), this, SLOT(huButtonClick()));
  connect(ui->aa_video, SIGNAL(mousePressed(QPoint *)), this,
          SLOT(huVideoClick(QPoint *)));
  connect(ui->aa_video, SIGNAL(mouseMoved(QPoint *)), this,
          SLOT(huVideoMove(QPoint *)));
  connect(ui->aa_video, SIGNAL(mouseReleased(QPoint *)), this,
          SLOT(huVideoRelease(QPoint *)));
  // headunitThread = new headunitworker();
  setenv("GST_DEBUG", "*:2", 1);
}

MainWindow::~MainWindow() { delete ui; }
bool MainWindow::huButtonClick() {
  /*if(headunitThread->threadRun){
      headunitThread->threadRun = false;
      ui->pushButton->setText("Start HU");
  } else {
      headunitThread->threadRun = true;
      headunitThread->start();
      ui->pushButton->setText("Stop HU");
  }*/

  aa_gst(ui->aa_video);

  return true;
}
bool MainWindow::huVideoClick(QPoint *point) {
  mouseStatus = MOUSE_DOWN;
  aa_touch_event(MOUSE_DOWN, point->x(), point->y());
  return true;
}
bool MainWindow::huVideoMove(QPoint *point) {
  if (mouseStatus == MOUSE_DOWN) {
    if (point->x() > ui->aa_video->width()) {
      QPoint falsepoint = QPoint(ui->aa_video->width(), point->y());
      return huVideoRelease(&falsepoint);
    }
    if (point->y() > ui->aa_video->height()) {
      QPoint falsepoint = QPoint(point->x(), ui->aa_video->height());
      huVideoRelease(&falsepoint);
    }
    if (point->x() < 1) {
      QPoint falsepoint = QPoint(1, point->y());
      return huVideoRelease(&falsepoint);
    }
    if (point->y() < 1) {
      QPoint falsepoint = QPoint(point->x(), 1);
      return huVideoRelease(&falsepoint);
    }
    aa_touch_event(MOUSE_MOVE, point->x(), point->y());
  }
  return true;
}
bool MainWindow::huVideoRelease(QPoint *point) {
  if (mouseStatus == MOUSE_DOWN) {
    mouseStatus = MOUSE_UP;
    aa_touch_event(MOUSE_UP, point->x(), point->y());
  }
  return true;
}
