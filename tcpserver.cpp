#include "tcpserver.h"

TcpSocket::TcpSocket(qintptr socketdesc, QTcpSocket* parent): QTcpSocket(parent)
{
    this->setSocketDescriptor(socketdesc);
    isReturnData = false; // 初始状态，服务器不回复位置信息
}

TcpSocket::~TcpSocket()
{

}

unsigned char TcpSocket::dataCRC8(QByteArray recBuffer)
{
    unsigned char crc = 0;
    int pBuffer = 0;

    // 确保数据正确
    if (recBuffer.size() <= 0)
        qDebug()<< "crc error!";

    // 计算CRC8校验码
    for (int i = 0; i < recBuffer.size()-2; i++) {
        for (unsigned char i = 0x80; i != 0; i /= 2) {
            if ((crc & 0x80) != 0) {
                crc *= 2;
                crc ^= 0x07;
            } else
                crc *= 2;

            if ((recBuffer.at(pBuffer)&i) != 0)
                crc ^= 0x07;
        }
        pBuffer++;
    }

    return crc;
}

void TcpSocket::dataAnalysis(QByteArray recBuffer)
{
    // [1]确保数据正确
    if (recBuffer.isEmpty())
        return ;
//    qDebug()<< "do?";
    // [2]对数据进行CRC8校验检查
//    if (dataCRC8(recBuffer) == *(recBuffer.end()-2)) {

        // 解析数据
        int cmd = int(recBuffer[1]);
        switch (cmd) {
            case 0: { // 控制服务器端是否给客户端回复pos信息
                isReturnData = int(recBuffer[2]) == 0? false: true;
                break;
            }
            case 5: { // 控制机械臂点动
                int axis = int(recBuffer[2]);
                int dir = int(recBuffer[3]);
                emit ctrlPotAction(axis, dir);
                break;
            }
            case 6: { // 控制机械臂标记或者清除记录点
                emit ctrlSaveOrClearPosInfo(int(recBuffer[2]));
                break;
            }
            default:
                break;
        }


//    } else
//        return ;
}

void TcpSocket::ReadAndParseData()
{
    /* [1]从TCP中读取数据 */
    QByteArray recStr = this->readAll();
    qDebug()<< recStr;

    static QByteArray recBuffer;

    /* [2]组装数据 */
    for (auto itor = recStr.begin(); itor != recStr.end(); itor++) {

        // 1.帧尾放进缓存空间
        recBuffer.append(*itor);

        // 2.判断是否接收到0x0D
        if (*itor == 0x0D) {

            // 判断是哪个类型的数据
            int cmd = int(recBuffer[1]);
            switch (cmd) {
                case 0:
                case 6:
                    if (recBuffer.size() >= 5) {
                        dataAnalysis(recBuffer);
                        recBuffer.clear();
                    }
                    break;
                case 5:
                    if (recBuffer.size() >= 6) {
                        dataAnalysis(recBuffer);
                        recBuffer.clear();
                    }
                    break;
                default:
                    break;
            }
        }
//        // 2.判断数据是否达到指定长度
//        if (recBuffer.length() >= 11) {

//            // 判断0x0D是否是帧尾
//            if (*itor == 0x0D)
//                // 将数据进行解析处理
//                dataAnalysis(recBuffer);

//            // 清空缓存空间
//            recBuffer.clear();
//        }
    }

//    QString recvStr = this->readAll();

//    QJsonDocument jsonDocument;
//    jsonDocument = QJsonDocument::fromJson(recvStr.toUtf8());

//    QJsonObject jsonObject = jsonDocument.object();

//    int cmd = jsonDocument["age"].toInt();

//    switch (cmd) {
//        case Negative:
//            emit reqDataFromDialog(Negative);
//            break;
//        case Positive:
//            emit reqDataFromDialog(Positive);
//            break;
//    }
}

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
        QByteArray msg;
        QVector<int> angle;

        for (int i = 0; i < 6; i++) {
           int res = int(data[i].position/ratio[i]*2)%360;
           angle.append(res);
//           qDebug()<< "res:"<< res;
        }

        msg.append(0x0B); // 帧头
        msg.append(0x01); // 命令码
        for (int i = 0; i < 6; i++) {
            QByteArray h, l;
            QByteArray tmp;
            int tmpAngle = angle[i];

            if (tmpAngle < 0)
                tmpAngle = -tmpAngle;

            tmp = QByteArray::number(tmpAngle, 16);
            if (tmp.size() >= 3) {
                h.push_back(tmp[0]);
                l.push_back(tmp[1]);
                l.push_back(tmp[2]);
                char h_val = char(h.toInt(nullptr, 16));
                if (angle[i] < 0)
                    h_val |= 0x10;
                msg.append(h_val);
                msg.append(char(l.toInt(nullptr, 16)));
            } else {
                if (angle[i] < 0)
                    msg.append(char(0x10));
                else
                    msg.append(char(0x00));
                msg.append(char(tmp.toInt(nullptr, 16)));
            }
        }
        msg.append(char(dataCRC8(msg)));
        msg.append(0x0D); // 帧尾

        qDebug()<< msg;
        this->write(msg);
    }
}
