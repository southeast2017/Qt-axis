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

#define Positive 1  // 正解
#define Negative 0  // 反解

class TcpSocket: public QTcpSocket
{
    Q_OBJECT
public:
    TcpSocket(qintptr socketdesc, QTcpSocket* parent = nullptr);
    ~TcpSocket();

    void fTest(); // 正解测试  关节量->坐标(x,y,z)
    void iTest(); // 反解测试  坐标(x,y,z)->关节量
    void fkineStep(QVector<double>&); // 运动学正解
    QVector<double> ikineStep(QVector<QVector<double>>&, QVector<double>); // 运动学反解

    void dataAnalysis(QByteArray recBuffer); // 解析处理拼接后的数据
    bool dataCRC8(QByteArray recBuffer); // 对数据进行CRC校验

signals:
    void sendIFResultToDialog(int, QVector<double>); // 数据更新ui
    void reqDataFromDialog(int);                     // 向ui请求数据

public slots:
    void ReadAndParseData();                        // 从socket中获取数据
    void getDataFromDialog(int, QVector<double>);   // 从ui获取数据

private:
    QVector<double> AxisPos; // 各轴关节量
    QVector<double> PosXYZ; // 坐标点坐标
    QVector<QVector<double>> GlobalPos; // 姿态矩阵
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
