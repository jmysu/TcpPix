/*
 * TcpRecvPix.cpp
 *
 *  A SINGLE User TCP client/server w/ QPixmap
 *
 */
#include "tcprecvpix.h"

TcpRecvPix::TcpRecvPix(QObject* parent) : QTcpServer(parent)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(addConnection()));
    _listSockets.clear();
}

void TcpRecvPix::addConnection()
{
    myTcpConnection = nextPendingConnection();
    if (!myTcpConnection) return;

    if (_listSockets.size() > 0) { //Limit client numbers
        qDebug() << "Too many clients!";
        return;
        }

    myTcpBuffer = new QBuffer(this);
    myTcpBuffer->open(QIODevice::ReadWrite);   

    connect(myTcpConnection, SIGNAL(readyRead()), this, SLOT(receiveImage()));
    connect(myTcpConnection, SIGNAL(disconnected()), this, SLOT(removeConnection()));
    //connect(myTcpConnection, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onSocketStateChanged(QAbstractSocket::SocketState)));

    //add socket into list
    _listSockets.push_back(myTcpConnection);
    qDebug() << "TcpServer> Clients" << _listSockets.size();
    for (QTcpSocket* socket : _listSockets) {
        qDebug() << "TcpServer>" << QHostAddress(socket->peerAddress().toIPv4Address()).toString();
        }

    connect(&timerRecv, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    timerRecv.start(1000); //timeout if connected more than 1sec
}

void TcpRecvPix::onSocketStateChanged(QAbstractSocket::SocketState socketState)
{
    qDebug() << "TcpServer>" << socketState;
}

void TcpRecvPix::removeConnection()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    if (socket && (_listSockets.size()>0) ) {
        _listSockets.removeOne(socket);
        qDebug() << "TcpServer> Connection removed.\n";
        }
    disconnect(&timerRecv, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    timerRecv.stop();
}

void TcpRecvPix::receiveImage()
{
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    if (!socket) return;

    QBuffer* buffer = myTcpBuffer;   
    qint64 bytes = buffer->write(socket->readAll());
    buffer->seek(buffer->pos() - bytes);

    QPixmap pm;
    pm.loadFromData(buffer->buffer());
    if (pm.isNull())
        qDebug("TcpServer> The image is null. Something failed.\n");
    else {
        qDebug() << "TcpServer> Pixmap received:" << bytes;
        emit sigNewImage(pm);
        }
}

void TcpRecvPix::slotTimeOut()
{
    qDebug() << "TcpServer> TimeOut reset";

    disconnect(&timerRecv, SIGNAL(timeout()), this, SLOT(slotTimeOut()));
    timerRecv.stop();

    _listSockets.clear();
    emit sigTcpRecvError();
}
