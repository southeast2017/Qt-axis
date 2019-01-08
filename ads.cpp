#include "ads.h"

Operation::Operation(QObject* parent): QObject (parent)
{
    pAddr = &Addr;
    nPort = AdsPortOpen();
    nErr = AdsGetLocalAddress(pAddr);
    pAddr->port = 851;
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

    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, 1, &mCommand);
//    nErr = AdsSyncWriteReq(pAddr, ADSIGRP_SYM_RELEASEHND, 0, sizeof(lHdlVar), &lHdlVar);
}

void Operation::readStatus()
{
    mValueStruct = new vStruct[6];

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


    // 可以制作一个定时器，来通过定时器来触发读取数据的函数
    mTimer = new QTimer();
    QObject::connect(mTimer, &QTimer::timeout, this, &Ads::getData);
    mTimer->start(100);
}

Ads::~Ads()
{
    delete mOperation;
}

void Ads::getData()
{
    emit readStatus();
    mTimer->start(100);
}
