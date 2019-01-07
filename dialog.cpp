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

    mAds = new Ads();

    QObject::connect(mAds->mOperation, &Operation::setValue, this, &Dialog::setUiValue);
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
