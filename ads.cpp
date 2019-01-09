#include "ads.h"

Operation::Operation(QObject* parent): QObject (parent)
{
    pAddr = &Addr;
    nPort = AdsPortOpen();
    nErr = AdsGetLocalAddress(pAddr);
    pAddr->port = 851;

    mValueStruct = new vStruct[6];
}

Operation::~Operation()
{
    delete [] mValueStruct;
}

void Operation::setStatus(QString command)
{
    unsigned long lHdlVar;
    char szVar[] = { "MAIN.CurrentJob" };
//    char szVar[] = { "GVL.ModeOfOperation[0]" };
    int mCommand = command.toInt();

    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, sizeof(szVar), szVar);

    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(mCommand), &mCommand);
//    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_RELEASEHND, 0, sizeof(lHdlVar), &lHdlVar);
}

/* setSpeed => 设置轴的转动速度
 * axisNumber: 轴编号 (-1: 表示全部轴)
 * value: 设定值
*/
void Operation::setSpeed(QString axisNumber, double value)
{
    unsigned long lHdlVar;
    QString tmp = "GVL.AxisMoveSpeed[" + axisNumber + "]";
    char* targetRegister = tmp.toLocal8Bit().data();

    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, unsigned(tmp.length()+1), targetRegister);
    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar,
        sizeof(value), &value);
}

void Operation::readSpeed(int* value)
{
    unsigned long lHdlVar;
    char targetRegister[] = "GVL.AxisMoveSpeed";
    double tmp[6];

    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, sizeof(targetRegister), targetRegister);

    nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar,
                          sizeof(tmp[0])*6, tmp);
    for (int i = 0; i < 6; ++i) {
        value[i] = int(tmp[i]);
    }
}

void Operation::readStatus()
{
    unsigned long lHdlVar;
//    char szVar[] = { "MAIN.CurrentJob" };
//    char szVar[] = "GVL.ModeOfOperation[0]";

    // 读取第轴的位移
    for (int i = 0; i < 6; i++) {

        QString tmp = "GVL.Axis[" + QString::number(i) +"].NcToPlc.ActPos";
        char* szVar = tmp.toUtf8().data();
        unsigned long tSize = unsigned(tmp.length()+1);

        nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
            &lHdlVar, tSize, szVar);

        nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(double), &mValueStruct[i].position);
    }

    // 读取第轴的速度
    // 缺少一个错误机制
    for (int i = 0; i < 6; i++) {
        QString tmp = "GVL.Axis[" + QString::number(i) +"].NcToPlc.ActVelo";

        char* szVar = tmp.toUtf8().data();
        unsigned long tSize = unsigned(tmp.length()+1);

        nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
            &lHdlVar, tSize, szVar);

        nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(double), &mValueStruct[i].speed);
    }
    emit setValue(mValueStruct);
}

Ads::Ads(QObject* parent): QObject (parent)
{
    mOperation = new Operation();
    QThread *mThread = new QThread();
    mOperation->moveToThread(mThread);
    mThread->start();

    QObject::connect(this, &Ads::setStatus, mOperation, &Operation::setStatus);
    QObject::connect(this, &Ads::readStatus, mOperation, &Operation::readStatus);
    QObject::connect(this, &Ads::setSpeed, mOperation, &Operation::setSpeed);
    QObject::connect(this, &Ads::readSpeed, mOperation, &Operation::readSpeed, Qt::DirectConnection);

    // 可以制作一个定时器，来通过定时器来触发读取数据的函数
    mTimer = new QTimer();
    QObject::connect(mTimer, &QTimer::timeout, this, &Ads::getData);
    mTimer->start(100);
}

Ads::~Ads()
{
    delete mTimer;
    delete mOperation;
}

void Ads::getData()
{
    emit readStatus();
    mTimer->start(100);
}
