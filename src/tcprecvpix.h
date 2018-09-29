/*
 * TcpRecvPix.h
 *
 * A simple QT TCPServer with one client serves QPixmap on localhost
 *
 */
#ifndef TCPRECVPIX_H
#define TCPRECVPIX_H

#include <QTcpSocket>
#include <QTcpServer>
#include <QBuffer>
#include <QPixmap>
#include <QTimer>

class QBuffer;
class QTcpSocket;

class TcpRecvPix: public QTcpServer
{
Q_OBJECT

public:
    TcpRecvPix(QObject* parent = nullptr);
    QTimer timerRecv;

signals:
    void sigNewImage(QPixmap pm);
    void sigTcpRecvError();

private slots:
    void addConnection();
    void removeConnection();
    void receiveImage();
    void slotTimeOut();
    void onSocketStateChanged(QAbstractSocket::SocketState socketState);

private:
    QTcpSocket* myTcpConnection;
    QBuffer* myTcpBuffer;
    QList<QTcpSocket*>  _listSockets;
};

#endif // TCPRECVPIX_H
