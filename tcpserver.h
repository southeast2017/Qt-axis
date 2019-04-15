#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>
#include <QJsonDocument>
#include <QJsonObject>
#include <QtMath>
#include <QDebug>
#include <QDialog>
#include "ads.h"

#define Positive 1  // 正解
#define Negative 0  // 反解

class TcpSocket: public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(qintptr socketdesc, QTcpSocket* parent = nullptr);
    ~TcpSocket();

    void dataAnalysis(QByteArray recBuffer); // 解析处理拼接后的数据
    unsigned char dataCRC8(QByteArray recBuffer); // 对数据进行CRC校验

signals:
    void ctrlPotAction(int, int); // 控制机械臂点动
    void ctrlSaveOrClearPosInfo(int); // 控制记录或者清除点信息

public slots:
    void ReadAndParseData();                        // 从socket中获取数据

    void sendDataToClient(vStruct*); // 将数据发送给客户端

private:
    bool isReturnData; // 控制服务器向client回复pos信息
    QVector<double> ratio = {49.99, 64.56, 99.69, 101.81, 160.68, 121}; // 角度比
};


class TcpServer: public QTcpServer
{
    Q_OBJECT
public:
    TcpServer(const std::string &ip, quint16 port, QTcpServer* parent = nullptr);
    ~TcpServer();

signals:
    void newSocket(TcpSocket*); // 创建了一个新的socket

protected:
    void incomingConnection(qintptr socketDescriptor);

private:
    QList<TcpSocket*> m_socketList;
};

#endif // TCPSERVER_H
