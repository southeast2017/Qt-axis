#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QFile>
#include "ads.h"

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

private slots:
    void on_btn_setModeMan_clicked();

    void on_btn_setModeAuto_clicked();

    void on_btn_paramSet_clicked();

    void on_btn_setModeShow_clicked();

    void on_btn_reset_clicked();

    void on_btn_zDo_clicked();

    void on_btn_fDo_clicked();

    void on_horizontalSlider_valueChanged(int value);

    void on_horizontalSlider_2_valueChanged(int value);

    void on_horizontalSlider_3_valueChanged(int value);

    void on_horizontalSlider_4_valueChanged(int value);

    void on_horizontalSlider_9_valueChanged(int value);

    void on_horizontalSlider_10_valueChanged(int value);

    void on_comboBox_activated(int index);

    void on_horizontalSlider_5_valueChanged(int value);

private:
    int mCurrentIndexSpeed; // 示教模式下拉框的索引
    int mSpeedMaxValue[6]; // 每个轴的最大速度

private:
    Ui::Dialog *ui;

    Ads *mAds;
};

#endif // DIALOG_H
