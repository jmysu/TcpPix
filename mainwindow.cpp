/*
 *
 *  A SINGLE User TCP client/server w/ QPixmap
 *
 */
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(qApp->applicationName());


    //Init server, connect sigNewImage
    server = new TcpRecvPix(nullptr);
    server->listen(QHostAddress::Any, 54321);
    connect(server, SIGNAL(sigNewImage(QPixmap)), this, SLOT(newImageReceived(QPixmap)));
    connect(server, SIGNAL(sigTcpRecvError()), this, SLOT(slotError()));

    //Show server status
    QString sHost;
    if (server->isListening())
        sHost = QString("TcpServer> listening @%1:%2").arg(QHostAddress(QHostAddress::Any).toString()).arg(54321);
    else
        sHost = QString("TcpServer> NOT Listening!!!");
    ui->labelStatusRecv->setText(sHost);

    //Connect sendTimer for stress test
    connect(&sendTimerN, SIGNAL(timeout()), this, SLOT(on_pushButtonSend_clicked()));

    //Load GIF
    myGif = new QMovie(":/image/Trump.gif");
    ui->labelPixSend->resize(ui->frame->size().width()/2, ui->frame->size().height());
    myGif->setScaledSize(ui->labelPixSend->size());
    ui->labelPixSend->setMovie(myGif);
    myGif->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonSend_clicked()
{
    QPixmap pm;
    if (m_imagePath.isEmpty() && myGif->isValid())
        pm = myGif->currentPixmap();
    else
        pm = *ui->labelPixSend->pixmap();


    if (!pm) return; //no pixmap

    myTcpSendPix = new TcpSendPix("localhost", 54321);
    QString sHost = QString("Target %1:%2").arg("localhost").arg(54321);
    ui->labelStatusSend->setText(sHost);
    timeElapsed.start();
    myTcpSendPix->send(pm);
}

void MainWindow::on_pushButtonLoad_clicked()
{
    m_imagePath = QFileDialog::getOpenFileName(this, tr("Open File"),"", tr("JPEG/PNG (*.jpg *.jpeg *.png)" ) );
    if (m_imagePath.isEmpty()) return;

    QImage image;
    bool resultOk = image.load(m_imagePath);
    if (resultOk)
        ui->labelPixSend->setPixmap(QPixmap::fromImage(image));
    QString sInfo = QString("%1 (%2x%3)").arg(m_imagePath).arg(image.width()).arg(image.height());
    ui->labelStatusSend->setText(sInfo);

    iRecvCnt=0;
}

void MainWindow::newImageReceived(QPixmap pm)
{
    if (!pm) return;
    float iSize = pm.width()*pm.height()*pm.depth()/(8*1024*1024.0);
    float iTimeElapsed = timeElapsed.elapsed()/1000.0;
    ui->labelStatusRecv->clear();
    ui->labelPixRecv->setPixmap(pm);
    QString sInfo = QString("Got Pixmap(%1x%2)@%3MB/s x%4").arg(pm.width()).arg(pm.height()).arg(iSize/iTimeElapsed, 4,'f',1).arg(++iRecvCnt);
    ui->labelStatusRecv->setText(sInfo);
}

void MainWindow::on_pushButtonSendTimer_clicked()
{
    (sendTimerN.isActive() ? sendTimerN.stop() : sendTimerN.start(ui->spinBoxTimer->value()) );

    qApp->processEvents();//Important
    qDebug() << "sendTimerN active?" <<  sendTimerN.isActive();
    if (sendTimerN.isActive()==false){
        ui->pushButtonSendTimer->setText("Send N");
        }
    else {
        ui->pushButtonSendTimer->setText("Stop");
        }
}

void MainWindow::slotError()
{
    qDebug() << "!!! Error, restart server!";
}
