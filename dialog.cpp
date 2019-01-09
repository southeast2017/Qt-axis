#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    MyUiInit();
}

Dialog::~Dialog()
{
    delete mAds;
    delete ui;
}

void Dialog::MyUiInit()
{
    ui->btn_setModeMan->setChecked(true);
    ui->content->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);

    mAds = new Ads();

    QObject::connect(mAds->mOperation, &Operation::setValue, this, &Dialog::setUiValue);

    // 示教模式，设置速度的轴索引
    mCurrentIndexSpeed = 0;

    // 要先读取寄存器内的数据，然后更新界面，进行界面的初始化
    emit mAds->readSpeed(mSpeedMaxValue);
    ui->horizontalSlider->setValue(mSpeedMaxValue[0]);
    ui->horizontalSlider_2->setValue(mSpeedMaxValue[1]);
    ui->horizontalSlider_3->setValue(mSpeedMaxValue[2]);
    ui->horizontalSlider_4->setValue(mSpeedMaxValue[3]);
    ui->horizontalSlider_9->setValue(mSpeedMaxValue[4]);
    ui->horizontalSlider_10->setValue(mSpeedMaxValue[5]);
}

void Dialog::AllButtonReset()
{
    ui->btn_setModeMan->setChecked(false);
    ui->btn_setModeAuto->setChecked(false);
    ui->btn_paramSet->setChecked(false);
}

// 切换到手动模式界面
void Dialog::on_btn_setModeMan_clicked()
{
    AllButtonReset();
    ui->btn_setModeMan->setChecked(true);
    ui->content->setCurrentIndex(0);
}

// 切换到自动模式界面
void Dialog::on_btn_setModeAuto_clicked()
{
    AllButtonReset();
    ui->btn_setModeAuto->setChecked(true);
    ui->content->setCurrentIndex(1);
}

// 切换到参数设置界面
void Dialog::on_btn_paramSet_clicked()
{
    AllButtonReset();
    ui->btn_paramSet->setChecked(true);
    ui->content->setCurrentIndex(2);
}

// 切换到示教模式
void Dialog::on_btn_setModeShow_clicked()
{
    if (ui->btn_setModeShow->isChecked())
        ui->isToShowPage->setCurrentIndex(1);
    else
        ui->isToShowPage->setCurrentIndex(0);
}

void Dialog::on_btn_reset_clicked()
{
    emit mAds->setStatus("0");
}

void Dialog::on_btn_zDo_clicked()
{
    emit mAds->setStatus("24");
}

void Dialog::on_btn_fDo_clicked()
{
//    emit mAds->readStatus();
}

void Dialog::setUiValue(vStruct* value)
{
    ui->label_24->setText(QString::number(value[0].position));
    ui->label_26->setText(QString::number(value[1].position));
    ui->label_28->setText(QString::number(value[2].position));
    ui->label_30->setText(QString::number(value[3].position));
    ui->label_32->setText(QString::number(value[4].position));
    ui->label_34->setText(QString::number(value[5].position));

    ui->label_4->setText(QString::number(value[0].speed));
    ui->label_5->setText(QString::number(value[1].speed));
    ui->label_6->setText(QString::number(value[2].speed));
    ui->label_10->setText(QString::number(value[3].speed));
    ui->label_11->setText(QString::number(value[4].speed));
    ui->label_12->setText(QString::number(value[5].speed));
}

// 设置速度的槽函数 => Begin
void Dialog::on_horizontalSlider_valueChanged(int value)
{
    mSpeedMaxValue[0] = value;
    if (mCurrentIndexSpeed == 0)
        ui->horizontalSlider_5->setValue(value);
    emit mAds->setSpeed("0", double(value));
}

void Dialog::on_horizontalSlider_2_valueChanged(int value)
{
    mSpeedMaxValue[1] = value;
    if (mCurrentIndexSpeed == 1)
        ui->horizontalSlider_5->setValue(value);
    emit mAds->setSpeed("1", double(value));
}

void Dialog::on_horizontalSlider_3_valueChanged(int value)
{
    mSpeedMaxValue[2] = value;
    if (mCurrentIndexSpeed == 2)
        ui->horizontalSlider_5->setValue(value);
    emit mAds->setSpeed("2", double(value));
}

void Dialog::on_horizontalSlider_4_valueChanged(int value)
{
    mSpeedMaxValue[3] = value;
    if (mCurrentIndexSpeed == 3)
        ui->horizontalSlider_5->setValue(value);
    emit mAds->setSpeed("3", double(value));
}

void Dialog::on_horizontalSlider_9_valueChanged(int value)
{
    mSpeedMaxValue[4] = value;
    if (mCurrentIndexSpeed == 4)
        ui->horizontalSlider_5->setValue(value);
    emit mAds->setSpeed("4", double(value));
}

void Dialog::on_horizontalSlider_10_valueChanged(int value)
{
    mSpeedMaxValue[5] = value;
    if (mCurrentIndexSpeed == 5)
        ui->horizontalSlider_5->setValue(value);
    emit mAds->setSpeed("5", double(value));
}
// 设置速度的槽函数 => End

// 更新选定轴索引
void Dialog::on_comboBox_activated(int index)
{
    mCurrentIndexSpeed = index;
    ui->horizontalSlider_5->setValue(mSpeedMaxValue[index]);
}

// 设定对应轴的速度
void Dialog::on_horizontalSlider_5_valueChanged(int value)
{
    switch (mCurrentIndexSpeed) {
        case 0:
            ui->horizontalSlider->setValue(value);
            break;
        case 1:
            ui->horizontalSlider_2->setValue(value);
            break;
        case 2:
            ui->horizontalSlider_3->setValue(value);
            break;
        case 3:
            ui->horizontalSlider_4->setValue(value);
            break;
        case 4:
            ui->horizontalSlider_9->setValue(value);
            break;
        case 5:
            ui->horizontalSlider_10->setValue(value);
            break;
        default:
            break;
    }
}
