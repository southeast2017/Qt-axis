#include "tcpserver.h"
#include <QDebug>

TcpSocket::TcpSocket(qintptr socketdesc, QTcpSocket* parent): QTcpSocket(parent)
{
    this->setSocketDescriptor(socketdesc);
}

TcpSocket::~TcpSocket()
{

}

void TcpSocket::ReadAndParseData()
{
    QString recvStr = this->readAll();

    QJsonDocument jsonDocument;
    jsonDocument = QJsonDocument::fromJson(recvStr.toUtf8());

    QJsonObject jsonObject = jsonDocument.object();

//    qDebug()<< recvStr<< endl;
    qDebug()<< jsonDocument["Name"].toString()<< endl;
    qDebug()<< jsonDocument["age"].toInt()<< endl;
}

TcpServer::TcpServer(const std::string &ip, quint16 port, QTcpServer* parent): QTcpServer(parent)
{
    bool isError = false;
    if (ip.empty()) {
        isError = this->listen(QHostAddress::AnyIPv4, port);
    } else {
        isError = this->listen(QHostAddress(ip.c_str()), port);
    }
    qDebug()<< isError<< endl;
}

TcpServer::~TcpServer()
{
    this->close(); // 关闭服务器的监听
    QList<TcpSocket*>::iterator it = m_socketList.begin();
    for (; it != m_socketList.end(); ) {
        TcpSocket* sock = *it;
        m_socketList.erase(it++);
        sock->deleteLater();
        sock = nullptr;
    }
}

void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    TcpSocket* socket = new TcpSocket(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), socket, SLOT(ReadAndParseData()));

    QThread* thread = new QThread(socket);
    connect(socket, &TcpSocket::disconnected, thread, &QThread::quit);
    socket->moveToThread(thread);
    thread->start();

    emit newConnection();
}
