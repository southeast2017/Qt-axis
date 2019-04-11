#ifndef ADS_H
#define ADS_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

/* 说明：ads文件主要的服务对象是本程序的界面部分
 *      所有的对应的操作函数，都是具体针对于界面上的所对应按钮
 *      本函数虽然可以应用于别的地方，但是不推荐应用于别的地方
 */

typedef struct {
    double position;
    double speed;
}vStruct;

class Operation: public QObject
{
    Q_OBJECT
public:
    Operation(QObject* parent = nullptr);
    ~Operation();

public slots:
    void setStatus(short, short);
    void setSpeed(QString axisNumber, double value);
    void readStatus();
    void readSpeed(int*);
    void setSwitch(bool);
    void setAPosition(double*);

signals:
    void setValue(vStruct*);
    void setUiStatus(QString);

private:
    long nErr;
    long nPort;
    AmsAddr Addr;
    PAmsAddr pAddr;

    vStruct* mValueStruct;
};

class Ads: public QObject
{
    Q_OBJECT
public:
    Ads(QObject* parent = nullptr);
    ~Ads();

    Operation *mOperation;

signals:
    void setStatus(short, short);
    void setSpeed(QString, double);
    void readStatus();
    void readSpeed(int*);
    void setSwitch(bool);
    void setAPosition(double*);
    void goToAPosition();

private slots:
    void getData();

private:
    QTimer *mTimer;
};

#endif // ADS_H
