#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>

class TcpSocket: public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(qintptr socketdesc, QTcpSocket* parent = nullptr);
    ~TcpSocket();

private slots:
    void ReadAndParseData();

private:
    QString m_recvDataStr;
};

class TcpServer: public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(const std::string &ip, quint16 port, QTcpServer* parent = nullptr);
    ~TcpServer();

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QList<TcpSocket*> m_socketList;
};

#endif // TCPSERVER_H
