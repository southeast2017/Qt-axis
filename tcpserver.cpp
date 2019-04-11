#include "tcpserver.h"

TcpSocket::TcpSocket(qintptr socketdesc, QTcpSocket* parent): QTcpSocket(parent)
{
    this->setSocketDescriptor(socketdesc);

    AxisPos.fill(0.0, 6);
    PosXYZ.fill(0.0, 3);
    GlobalPos.fill({0, 0, 0, 0}, 4);
//    lastState.fill(0.0, 6);
}

TcpSocket::~TcpSocket()
{

}

unsigned char TcpSocket::dataCRC8(QByteArray recBuffer)
{
    unsigned char crc = 0;
    int pBuffer = 0;

    // 确保数据正确
//    assert(recBuffer.size() <= 0);

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

//    if (crc == *(recBuffer.end()-2))
//        return true;
//    else
//        return false;
    return crc;
}

void TcpSocket::dataAnalysis(QByteArray recBuffer)
{
    // [1]确保数据正确
    if (recBuffer.isEmpty())
        return ;

    // [2]对数据进行CRC8校验检查
    if (dataCRC8(recBuffer) == *(recBuffer.end()-2)) {

        // 解析数据
        qDebug()<< "data_parse_analysis"<< endl;
    } else
        return ;
}

void TcpSocket::ReadAndParseData()
{
    /* [1]从TCP中读取数据 */
    QByteArray recStr = this->readAll();

    static QByteArray recBuffer;

    /* [2]组装数据 */
    for (auto itor = recStr.begin(); itor != recStr.end(); itor++) {

        // 1.帧尾放进缓存空间
        recBuffer.append(*itor);

        // 2.判断数据是否达到指定长度
        if (recBuffer.length() >= 11) {

            // 判断0x0D是否是帧尾
            if (*itor == 0x0D)
                // 将数据进行解析处理
                dataAnalysis(recBuffer);

            // 清空缓存空间
            recBuffer.clear();
        }
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

//    this->write("hello, world");
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

    emit newSocket(socket);
    emit newConnection();
}

/* *** 运动学正解函数 *** */
void TcpSocket::fTest()
{
    QVector<double> theta;
    theta.fill(0, 6);

    for (int i = 0; i < 6; i++)
        theta[i] = AxisPos[i] * M_PI / 180;

    // 运动学正解
    fkineStep(theta);
}

/* *** 运动学正解 *** */
void TcpSocket::fkineStep(QVector<double>& Theta)
{
    QVector<QVector<double>> T;
    T.fill({0, 0, 0, 0}, 4);

    // 方向纠正
    Theta[1] = -Theta[1];
    Theta[2] = -Theta[2];
    Theta[5] = -Theta[5];

    double d1 = 0.28;
    double d4 = 0.35014205;
    double d6 = 0.0745;
    double a2 = 0.34966093;

    double s1 = qSin(Theta[0]);
    double c1 = qCos(Theta[0]);

    double s2 = qSin(Theta[1]);
    double c2 = qCos(Theta[1]);

    double s23 = qSin(Theta[1] + Theta[2]);
    double c23 = qCos(Theta[1] + Theta[2]);

    double s4 = qSin(Theta[3]);
    double c4 = qCos(Theta[3]);

    double s5 = qSin(Theta[4]);
    double c5 = qCos(Theta[4]);

    double s6 = qSin(Theta[5]);
    double c6 = qCos(Theta[5]);

    double a = s1 * c4 + c1 * s4 * s23;
    double b = s1 * s4 - c1 * c4 * s23;
    double d = c1 * c4 - s1 * s4 * s23;
    double c = c1 * s4 + s1 * c4 * s23;
    double e = c5 * s23 + c4 * s5 * c23;
    double f = s5 * s23 - c4 * c5 * c23;

    T[0][0] = s6 * a + c6 * (c5 * b - s5 * c1 * c23);
    T[0][1] = c6 * a - s6 * (c5 * b - s5 * c1 * c23);
    T[0][2] = s5 * b + c5 * c1 * c23;
    T[0][3] = d6 * (s5 * b + c5 * c1 * c23) + d4 * c1 * c23 - a2 * c1 * s2;

    T[1][0] = -s6 * d - c6 * (c5 * c + s5 * s1 * c23);
    T[1][1] = s6 * (c5 * c + s5 * s1 * c23) - c6 * d;
    T[1][2] = -s5 * c + c5 * s1 * c23;
    T[1][3] = d4 * s1 * c23 - d6 * (s5 * c - c5 * s1 * c23) - a2 * s1 * s2;

    T[2][0] = -c6 * f - s4 * s6 * c23;
    T[2][1] = s6 * f - c6 * s4 * c23;
    T[2][2] = e;
    T[2][3] = d1 + d4 * s23 + a2 * c2 + d6 * e;

    T[3][0] = 0;
    T[3][1] = 0;
    T[3][2] = 0;
    T[3][3] = 1;

    GlobalPos = T; // 保存姿态矩阵
    QVector<double> pos = {T[0][3], T[1][3], T[2][3]};

    qDebug()<< GlobalPos;

    emit sendIFResultToDialog(Positive, pos);
}

/* *** 运动学反解函数 *** */
void TcpSocket::iTest()
{
    QVector<QVector<double>> T;
    QVector<double> Angle_Last;
    QVector<double> Angle;
    QVector<double> Q_Ikine;
    T.fill({0, 0, 0, 0}, 4);
    Angle_Last.fill(0, 6);
    Angle.fill(0, 6);
    Q_Ikine.fill(0, 6);

    //统一转弧度
    for (int i = 0; i < 6; i++) {
        Angle[i] *= M_PI / 180;
        Angle_Last[i] *= M_PI / 180;
    }

    GlobalPos[0][3] = PosXYZ[0];  // PosX
    GlobalPos[1][3] = PosXYZ[1];  // PosY
    GlobalPos[2][3] = PosXYZ[2];  // PosZ
    T = GlobalPos;

    //反解
    Q_Ikine = ikineStep(T,Angle_Last);

    //弧度转角度
    for (int i = 0; i < 6; i++) {
//        Q_Ikine[i] *= ;   // 0-底座  5-抓手   // error -south
        Angle[i] = Q_Ikine[5 - i] * 180 / M_PI;
    }

    emit sendIFResultToDialog(Negative, Angle);
}

/* *** 运动学反解 *** */
QVector<double> TcpSocket::ikineStep(QVector<QVector<double>>& T, QVector<double> angleLast)
{
    QVector<double> angleBest;
    angleBest.fill(0, 6);

    double d1 = 0.28;
    double d4 = 0.35014205;
    double d6 = 0.0745;
    double a2 = 0.34966093;

    double nx = T[0][0];
    double ny = T[1][0];
    double nz = T[2][0];
    double ox = T[0][1];
    double oy = T[1][1];
    double oz = T[2][1];
    double ax = T[0][2];
    double ay = T[1][2];
    double az = T[2][2];
    double px = T[0][3];
    double py = T[1][3];
    double pz = T[2][3];

    int n = 0;
    double Angle[8][6];

    double S1 = 0, S3 = 0, S4 = 0, S5 = 0, S6 = 0;
    double C1 = 0, C3 = 0, C5 = 0, C6 = 0;
    double angle1 = 0, angle2 = 0, angle3 = 0, angle4 = 0, angle5 = 0, angle6 = 0;

    double px1 = 0;
    double ax1 = 0;
    double ox1 = 0;
    double nx1 = 0;

    double py1 = 0;
    double az1 = 0;
    double ak2 = 0;
    double ck2 = 0;

    double K1 = 0;
    double K2 = 0;
    double C23 = 0;
    double S23 = 0;
    double angle23 = 0;

    double error = 0, error_min = 100;

    for (int r1 = 1; r1 < 3; r1++)
    {
        if (r1 == 1) { angle1 = qAtan2((-py + d6 * ay), (ax * d6 - px)); }
        else
        {
            if (angle1 > 0) { angle1 = angle1 - M_PI; }
            else { angle1 = angle1 + M_PI; }
        }

        S1 = qSin(angle1);
        C1 = qCos(angle1);

        px1 = px * C1 + py * S1;
        ax1 = ax * C1 + ay * S1;
        ox1 = ox * C1 + oy * S1;
        nx1 = nx * C1 + ny * S1;

        py1 = pz - d1;
        az1 = ax * S1 - ay * C1;
        //
        for (int r2 = 1; r2 < 3; r2++)
        {
            ak2 = 2 * a2 * d4;
            ck2 = ((px1 - d6 * ax1) * (px1 - d6 * ax1) + (py1 - d6 * az) * (py1 - d6 * az) - a2 * a2 - d4 * d4);
            S3 = ck2 / ak2;// S3第一次计算结果
            C3 = qSqrt(1 - S3 * S3);// C3第一次计算结果
            if (r2 == 1)
            {
                if (qAbs(qAbs(ak2) - qAbs(ck2)) < 1e-13)
                {
                    if (ak2 * ck2 < 0) angle3 = -M_PI/2;
                    else angle3 = M_PI/2;
                }
                else angle3 = qAtan2(S3, C3);
            }
            else
            {
                if (angle3 > 0) { angle3 = - angle3 - M_PI; }
                else { angle3 = -angle3 + M_PI; }
            }

            //
            if (angle3 >= (2.0 / 9.0) * M_PI)
                angle3 = angle3 - M_PI*2;

            if (angle3 <= -(11.0 / 9.0) * M_PI)
                angle3 = angle3 + M_PI*2;

            S3 = qSin(angle3);
            C3 = qCos(angle3);

            K1 = d6 * ax1 - px1;
            K2 = py1 - d6 * az;
            C23 = (K2 * a2 * C3 / K1 - a2 * S3 - d4) * K1 / (K1 * K1 + K2 * K2);
            S23 = (-K2 * C23 + a2 * C3) / K1;
            angle23 = qAtan2(S23, C23);
            angle2 = angle23 - angle3;
            if (angle2 > 1.9198621771937625)
                angle2 = angle2 - M_PI*2;
            if (angle2 < -1.9198621771937625)
                angle2 = angle2 + M_PI*2;
            //
            for (int r3 = 1; r3 < 3; r3++)
            {
                if (r3 == 1) { angle4 = qAtan2(az1, (-ax1 * S23 + az * C23)); }
                else
                {
                    if (angle4 > 1.5e-16)//Matlab计算是绝对0
                        angle4 = angle4 - M_PI;
                    else
                        angle4 = angle4 + M_PI;
                }

                S4 = qSin(angle4);
                if ((S4 < 0.001) && (S4 > -0.001))
                    S5 = -ax1 * S23 + az * C23;
                else
                    S5 = az1 / S4;

                C5 = C23 * ax1 + az * S23;
                angle5 = qAtan2(S5, C5);
                S5 = qSin(angle5);

                if ((S5 < 0.001) && (S5 > -0.001))
                {
                    S6 = -oz;
                    C6 = nz;
                }
                else
                {
                    S6 = (C23 * ox1 + oz * S23) / S5;
                    C6 = -(C23 * nx1 + nz * S23) / S5;
                }
                angle6 = qAtan2(S6, C6);

                Angle[n][0] = angle1;// * 180 / Math.PI;
                Angle[n][1] = angle2;// * 180 / Math.PI;
                Angle[n][2] = angle3;//* 180 / Math.PI;
                Angle[n][3] = angle4;//* 180 / Math.PI;
                Angle[n][4] = angle5;//* 180 / Math.PI;
                Angle[n][5] = angle6;//* 180 / Math.PI;

                n = n + 1;
            }
        }
    }

    int j = 0;
    if (n == 0)
    {
        angleBest[0] = 0;
        angleBest[1] = 0;
        angleBest[2] = 0;
        angleBest[3] = 0;
        angleBest[4] = 0;
        angleBest[5] = 0;
    }
    else
    {
        for (int i = 0; i < n; i++)
        {
            error = qAbs(Angle[i][0] - angleLast[0]) / 5.934119 + qAbs(Angle[i][1] - angleLast[1]) / 3.83972 +
                    qAbs(Angle[i][2] - angleLast[2]) / 4.537856 + qAbs(Angle[i][3] - angleLast[3]) / 6.2831853 +
                    qAbs(Angle[i][4] - angleLast[4]) / 4.363323 + qAbs(Angle[i][5] - angleLast[5]) / 6.2831853;
            if (i == 0)
            {
                error_min = error;
                j = i;
            }
            else if (error < error_min)
            {
                error_min = error;
                j = i;
            }
        }
        angleBest[0] = Angle[j][0];
        angleBest[1] = -Angle[j][1];
        angleBest[2] = -Angle[j][2];
        angleBest[3] = Angle[j][3];
        angleBest[4] = Angle[j][4];
        angleBest[5] = -Angle[j][5];
    }

    return angleBest;
}

void TcpSocket::getDataFromDialog(int dir, QVector<double> data)
{
    if (dir == Positive) {
        AxisPos = data;
        this->fTest();
    } else if (dir == Negative) {
        PosXYZ = data;
        this->iTest();
    }
}

/* 名称：发送数据给客户端
 * 描述：将六个轴的坐标数据发送给client端
 */
void TcpSocket::sendDataToClient(vStruct* data)
{
    QByteArray msg;
    QVector<int> angle;

    for (int i = 0; i < 6; i++) {
       int res = int(data[i].position/ratio[i]*2)%360;
       angle.append(res);
       qDebug()<< "res:"<< res;
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


//        QByteArray tmp = QByteArray::number(angle[i], 16);
//        if (tmp.size() >= 3) {
//            QByteArray h, l;
//            h.push_back(tmp[0]);
//            l.push_back(tmp[1]);
//            l.push_back(tmp[2]);
//            msg.append(char(h.toInt(nullptr, 16)));
//            msg.append(char(l.toInt(nullptr, 16)));
//        } else {
//            msg.append(char(0x00));
//            msg.append(char(tmp.toInt(nullptr, 16)));
//        }
    }
    msg.append(char(dataCRC8(msg)));
    msg.append(0x0D); // 帧尾

    this->write(msg);
}
