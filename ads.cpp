#include "ads.h"

Operation::Operation(QObject* parent): QObject (parent)
{
    pAddr = &Addr;
    nPort = AdsPortOpen();
    nErr = AdsGetLocalAddress(pAddr);
    pAddr->port = 851;

    mValueStruct = new vStruct[6];
    for (int i = 0; i <= 0; i++) {
        mValueStruct[i].position = 0;
        mValueStruct[i].speed = 0;
    }
}

Operation::~Operation()
{
    delete [] mValueStruct;
}

void Operation::setStatus(short axis, short status)
{
    unsigned long lHdlVar;
    char szVar1[] = { "GVL.axisNum" };
    char szVar2[] = { "MAIN.CurrentJob" };

    // 写入坐标轴
    if (axis > -1) {
        nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
            &lHdlVar, sizeof(szVar1), szVar1);

        nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(axis), &axis);
    }

    // 写入状态机
    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, sizeof(szVar2), szVar2);

    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, sizeof(status), &status);
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

    // 读取状态机
    char targetRegister[] = "MAIN.CurrentJobState";
    char tmpCommand[80];
    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, sizeof(targetRegister), targetRegister);
    nErr = AdsSyncReadReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar,
                          sizeof(tmpCommand), &tmpCommand);

    QString resCommand = tmpCommand;

    emit setUiStatus(resCommand);
    emit setValue(mValueStruct);
}

// 设置开关状态
void Operation::setSwitch(bool btnSwitch)
{
    unsigned long lHdlVar;
    char targetRegister[] = "MAIN.POWER";

    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, sizeof(targetRegister), targetRegister);

    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar,
            sizeof(btnSwitch), &btnSwitch);
}

void Operation::setAPosition(double* value)
{
    unsigned long lHdlVar;
    char targetRegister[] = "GVL.MoveA";

    nErr = AdsSyncReadWriteReq(pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar),
        &lHdlVar, sizeof(targetRegister), targetRegister);

    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar,
            sizeof(value[0])*6, value);
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
    QObject::connect(this, &Ads::setSwitch, mOperation, &Operation::setSwitch);
    QObject::connect(this, &Ads::setAPosition, mOperation, &Operation::setAPosition);

    // 可以制作一个定时器，来通过定时器来触发读取数据的函数
    mTimer = new QTimer();
    QObject::connect(mTimer, &QTimer::timeout, this, &Ads::getData);
    mTimer->start(150);
}

Ads::~Ads()
{
    delete mTimer;
    delete mOperation;
}

void Ads::getData()
{
    emit readStatus();
}
