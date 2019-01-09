#ifndef ADS_H
#define ADS_H

#include <QObject>
#include <QDebug>
#include <QThread>
#include <QTimer>
#include "TcAdsDef.h"
#include "TcAdsAPI.h"

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
