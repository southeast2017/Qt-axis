#include "tcpserver.h"

TcpSocket::TcpSocket(qintptr socketdesc, QTcpSocket* parent): QTcpSocket(parent)
{
    this->setSocketDescriptor(socketdesc);
    isReturnData = false;           // 初始状态，服务器不回复位置信息
    m_oldRobotPos.fill(0, 6);       // 用来记录上一次发送的数据
}

TcpSocket::~TcpSocket()
{
}

/* 名称：执行动作
 * 描述：该函数作用为根据JSON格式数据，然后来判定具体要执行的动作命令
 */
void TcpSocket::DoWork(QJsonDocument& JsonData)
{
    QString cmd = JsonData["cmd"].toString();                   // 获取数据内的指令码
    if (cmd == "ctrl_switch") {
        isReturnData = JsonData["switch"].toInt() == 1; // 控制服务器端是否给客户端回复pos信息
    }
    else if (cmd == "ctrl_axis") {                              // 控制机械臂点动
        int axis = JsonData["axis"].toInt();
        int dir = JsonData["dir"].toInt();
        emit ctrlPotAction(axis, dir);
    }
    else if (cmd == "ctrl_point") {                             // 控制机械臂标记或者清除记录点
        int active = JsonData["active"].toInt();
        emit ctrlSaveOrClearPosInfo(active);
    }
    else if (cmd == "ctrl_move") {                              // 控制机械臂按距离移动
        QVector<double> tmp;
        tmp.push_back(JsonData["x"].toDouble());
        tmp.push_back(JsonData["y"].toDouble());
        tmp.push_back(JsonData["z"].toDouble());
        emit ctrlMoveByXYZ(tmp);
    }
}

/* 名称:读数据并解析数据
 * 描述：函数从tcp的缓冲区内读取数据，然后将读取到的数据进行拆包解析
 * 问题：在进行拆包的过程中可能会造成因拆包而产生的延迟，可以尝试开辟
 *      一个新的线程，专门来进行拆包和解析
 */
void TcpSocket::ReadAndParseData()
{
    // ![1]从TCP中读取数据
    QByteArray buffer = this->readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(buffer);

    if (!jsonDocument.isObject()) {                                     // 产生了粘包问题
        QList<QByteArray> data = buffer.split('}');                     // 根据'}'来进行拆包处理
        for (auto i = 0; i < data.size() - 1; ++i) {                    // 遍历拆分后的数据
            jsonDocument = QJsonDocument::fromJson(data.at(i) + '}');   // 添加'}'组成完整的json数据
            if (jsonDocument.isObject()) DoWork(jsonDocument);          // 拆分后是合法的数据,进行动作执行
        }
    }
    else
        DoWork(jsonDocument);
}

/* 名称：构造函数
 * 参数：ip地址，端口号，父类对象
 * 描述：该构造函数会让服务器监听一个ip地址对应的端口号，也就是创建服务器
 */
TcpServer::TcpServer(const std::string &ip, quint16 port, QTcpServer* parent): QTcpServer(parent)
{
    bool isError = false;
    if (ip.empty()) {
        isError = this->listen(QHostAddress::AnyIPv4, port);
    } else {
        isError = this->listen(QHostAddress(ip.c_str()), port);
    }
    qDebug()<< "create:"<< isError<< endl;
}

/* 名称：析构函数
 * 描述：关闭服务器的监听以及关闭每个线程以及线程内对象的释放
 */
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

/* 名称：连接函数
 * 描述：当有一个客户端连接上来之后，就会执行该函数，该函数会创建一个Socket，并
 *      会创建一个新的线程，然后会建立通信
 */
void TcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<< "create new socket";
    TcpSocket* socket = new TcpSocket(socketDescriptor);

    connect(socket, SIGNAL(readyRead()), socket, SLOT(ReadAndParseData()));

    QThread* thread = new QThread(socket);
    connect(socket, &TcpSocket::disconnected, thread, &QThread::quit);
    socket->moveToThread(thread);
    thread->start();

    emit newSocket(socket);
    emit newConnection();
}

/* 名称：发送数据给客户端
 * 描述：将六个轴的坐标数据发送给client端
 */
void TcpSocket::sendDataToClient(vStruct* data)
{
    if (isReturnData) {

        bool isSame = true;
        for (int i = 0; i < 6; ++i) {
            if ((data[i].position > m_oldRobotPos[i])||(data[i].position < m_oldRobotPos[i])) {
                isSame = false;
                break;
            }
        }
        if (!isSame) {
            QJsonObject jsonObject;
            jsonObject.insert("one", (QString::number(data[0].position, 'f', 4)).toDouble());
            jsonObject.insert("two", (QString::number(data[1].position, 'f', 4)).toDouble());
            jsonObject.insert("three", (QString::number(data[2].position, 'f', 4)).toDouble());
            jsonObject.insert("four", (QString::number(data[3].position, 'f', 4)).toDouble());
            jsonObject.insert("five", (QString::number(data[4].position, 'f', 4)).toDouble());
            jsonObject.insert("six", (QString::number(data[5].position, 'f', 4)).toDouble());

            for (int i = 0; i < 6; ++i) // 更新旧值
                m_oldRobotPos[i] = data[i].position;

            QJsonDocument jsonDocument;
            jsonDocument.setObject(jsonObject);
            QByteArray dataArray = jsonDocument.toJson(QJsonDocument::Compact);

            this->write(dataArray);
            this->flush();
        }
    }
}
