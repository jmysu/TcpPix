/*
 *
 *  A Simple TCP client APP send QPixma on demand
 *
 */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QFileDialog>
#include <QBuffer>
#include <QTimer>
#include <QTime>
#include <QMovie>

#include "tcpsendpix.h"
#include "tcprecvpix.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QTimer sendTimerN;
    QTime  timeElapsed;
    QMovie *myGif;
    QString m_imagePath;

public slots:

private slots:
    void on_pushButtonSend_clicked();
    void on_pushButtonLoad_clicked();
    void newImageReceived(QPixmap pm);

    void on_pushButtonSendTimer_clicked();
    void slotError();

private:
    Ui::MainWindow *ui;

    QTcpSocket tcpClient;
    int iRecvCnt=0;

    TcpSendPix *myTcpSendPix;
    TcpRecvPix *server;
};

#endif // MAINWINDOW_H
