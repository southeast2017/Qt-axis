#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QFile>
#include "ads.h"
#include "tcpserver.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = nullptr);
    ~Dialog();

    void MyUiInit();
    void AllButtonReset();

public slots:
    void setUiValue(vStruct*);
    void setUiStatus(QString);

    void newSocketConnectToDialog(TcpSocket*);

private slots:
    void on_btn_setModeMan_clicked();

    void on_btn_setModeAuto_clicked();

    void on_btn_paramSet_clicked();

    void on_btn_setModeShow_clicked();

    void on_btn_reset_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_horizontalSlider_9_valueChanged(int value);

    void on_horizontalSlider_10_valueChanged(int value);

    void on_comboBox_activated(int index);

    void on_horizontalSlider_5_valueChanged(int value);

    void on_btn_switch_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

    void on_radioButton_3_clicked();

    void on_radioButton_4_clicked();

    void on_radioButton_5_clicked();

    void on_radioButton_6_clicked();

    void on_btn_zDo_pressed();

    void on_btn_zDo_released();

    void on_btn_fDo_pressed();

    void on_btn_fDo_released();

    void on_pushButton_10_clicked();

    void on_btn_goOn_clicked();

    void on_pushButton_29_clicked();

    void on_pushButton_11_clicked();

    void on_btn_chooseGCode_clicked();

    void on_btn_position_clicked();

    void on_pushButton_19_clicked();

    void on_pushButton_21_clicked();

    void ctrlSaveOrClearPosInfo(int act);

private:
    int mCurrentIndexSpeed; // 示教模式下拉框的索引
    int mSpeedMaxValue[6]; // 每个轴的最大速度
    short mAxisNumber; // 运动轴选择

    double positionStore[6];

private:
    Ui::Dialog *ui;

    Ads* mAds;
    TcpServer* server;
};

#endif // DIALOG_H
