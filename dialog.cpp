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
    delete server;
    mAds = nullptr;
    server = nullptr;
    delete ui;
}

void Dialog::MyUiInit()
{
    ui->btn_setModeMan->setChecked(true);
    ui->content->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    ui->radioButton->setChecked(true);

    mAds = new Ads();
    server = new TcpServer("192.168.43.99", 7777);
//    server = new TcpServer("10.66.73.73", 7777);
//    server = new TcpServer("169.254.245.177", 7777);

    connect(server, &TcpServer::newSocket, this, &Dialog::newSocketConnectToDialog); // 每个socket都与Dialog进行连接

    connect(mAds->mOperation, &Operation::setValue, this, &Dialog::setUiValue);
    connect(mAds->mOperation, &Operation::setUiStatus, this, &Dialog::setUiStatus);


    // 示教模式，设置速度的轴索引
    mCurrentIndexSpeed = 0;
    mAxisNumber = 0;

    // 要先读取寄存器内的数据，然后更新界面，进行界面的初始化
    emit mAds->readSpeed(mSpeedMaxValue);
    ui->horizontalSlider->setValue(mSpeedMaxValue[0]);
    ui->horizontalSlider_2->setValue(mSpeedMaxValue[1]);
    ui->horizontalSlider_3->setValue(mSpeedMaxValue[2]);
    ui->horizontalSlider_4->setValue(mSpeedMaxValue[3]);
    ui->horizontalSlider_9->setValue(mSpeedMaxValue[4]);
    ui->horizontalSlider_10->setValue(mSpeedMaxValue[5]);

    // 失能"移动到A点"按钮
    ui->pushButton_11->setDisabled(true);
    // 失能"反复"按钮
    ui->btn_position->setDisabled(true);
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

/* 名称：回到标记零点
 * 描述：机械臂回到标记零点
 */
void Dialog::on_btn_reset_clicked()
{
    emit mAds->setStatus(-1, 11);
}

/* 名称：更新Ui界面数据
 * 描述：从底层获取数据，然后更新到Ui界面
 */
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

void Dialog::setUiStatus(QString value)
{
    ui->lineEdit->setText(value);
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

void Dialog::on_btn_switch_clicked()
{
    emit mAds->setSwitch(ui->btn_switch->isChecked());
}

// 设置选定轴 => Begin
void Dialog::on_radioButton_clicked()
{
    mAxisNumber = 0;
}

void Dialog::on_radioButton_2_clicked()
{
    mAxisNumber = 1;
}

void Dialog::on_radioButton_3_clicked()
{
    mAxisNumber = 2;
}

void Dialog::on_radioButton_4_clicked()
{
    mAxisNumber = 3;
}

void Dialog::on_radioButton_5_clicked()
{
    mAxisNumber = 4;
}

void Dialog::on_radioButton_6_clicked()
{
    mAxisNumber = 5;
}
// 设置选定轴 => End

/* 名称：Move to home
 * 描述：意义未知
 */
void Dialog::on_pushButton_21_clicked()
{
    emit mAds->setStatus(-1, 6);
}

/* 名称：复位
 * 描述：将机械臂复位（软件复位，此时机械臂不会进行动作）
 */
void Dialog::on_pushButton_19_clicked()
{
    emit mAds->setStatus(-1, 9);
}

/* 名称：标记零点
 * 描述：将当前位置设定为零点位置
 */
void Dialog::on_pushButton_29_clicked()
{
    emit mAds->setStatus(-1, 1);
}

/* 名称：正转
 * 描述：按钮按下不松开，执行此函数
 *      设置伺服状态机为3，控制正向旋转
 */
void Dialog::on_btn_zDo_pressed()
{
    emit mAds->setStatus(mAxisNumber, 3);
}

/* 名称：正转
 * 描述：按钮松开，还原为原来的状态机
 *      原来的状态机为0，停止状态
 */
void Dialog::on_btn_zDo_released()
{
    emit mAds->setStatus(-1, 0);
}

/* 名称：反转
 * 描述：按钮按下不松开，执行此函数
 *      设置伺服状态机为4，控制反向旋转
 */
void Dialog::on_btn_fDo_pressed()
{
    emit mAds->setStatus(mAxisNumber, 4);
}

/* 名称：反转
 * 描述：松开按钮，还原为原来的状态机
 * 原来的状态机为0，停止状态
 */
void Dialog::on_btn_fDo_released()
{
    emit mAds->setStatus(-1, 0);
}

/* 名称：标记A点
 * 描述：先获取当前机械臂的坐标位置，
 *      然后将坐标点设置到伺服驱动内
 */
void Dialog::on_pushButton_10_clicked()
{
    positionStore[0] = ui->label_24->text().toDouble();
    positionStore[1] = ui->label_26->text().toDouble();
    positionStore[2] = ui->label_28->text().toDouble();
    positionStore[3] = ui->label_30->text().toDouble();
    positionStore[4] = ui->label_32->text().toDouble();
    positionStore[5] = ui->label_34->text().toDouble();
    QString tmp;
    for (int i = 0; i < 5; ++i) {
        tmp += QString::number(positionStore[i]) + ",";
    }
    tmp += QString::number(positionStore[5]);
    ui->textEdit->setText(tmp);

    emit mAds->setAPosition(positionStore);

    // 标记A点，将"移动A点"按钮设定为使能状态
    ui->pushButton_11->setEnabled(true);
    // 标记A点，将"反复"按钮设定为使能状态
    ui->btn_position->setEnabled(true);
}

/* 名称：移动到A点
 * 描述：已经设置过A点坐标之后，执行此函数，则会控制机械臂
 *      单次到达A点坐标位置
 */
void Dialog::on_pushButton_11_clicked()
{
    if (ui->btn_setModeShow->isCheckable()&&ui->textEdit->toPlainText() != "") {
        emit mAds->setStatus(-1, 7);
    }
}

/* 名称：清除A点
 * 描述：清除A点标记，用0来替代原来的坐标位置
 */
void Dialog::on_btn_chooseGCode_clicked()
{
    for (int i = 0; i < 6; i++)
        positionStore[i] = 0;
    ui->textEdit->clear();

    emit mAds->setAPosition(positionStore);

    // 未标记A点，将"移动到A点"按钮设为失能状态
    ui->pushButton_11->setDisabled(true);
    // 未标记A点，将"反复"按钮设定为失能状态
    ui->btn_position->setDisabled(true);
}

/* 名称：反复
 * 描述：在标记A点动作以后，可以执行反复动作
 */
void Dialog::on_btn_position_clicked()
{
    emit mAds->setStatus(-1, 8);
}

/* 名称：停止
 * 描述：让机械臂处于停止状态
 */
void Dialog::on_btn_goOn_clicked()
{
    emit mAds->setStatus(-1, 5);
}

/* 名称：新的socket与ui线程通信
 * 描述：新生成的socket与ui线程进行通信，里面是socket线程与ui线程之间信号与槽的连接
 */
void Dialog::newSocketConnectToDialog(TcpSocket* socket)
{
    // 控制回复位置信息
    connect(mAds->mOperation, &Operation::setValue, socket, &TcpSocket::sendDataToClient);
    // 每个轴的点动控制
    connect(socket, &TcpSocket::ctrlPotAction, mAds->mOperation, &Operation::setStatus);
    // 控制状态机
    connect(socket, &TcpSocket::ctrlSaveOrClearPosInfo, this, &Dialog::ctrlSaveOrClearPosInfo);
}

/* 名称：控制保存或者是清除位置信息
 * 描述：用来控制机械臂清除或者标记位置信息
 */
void Dialog::ctrlSaveOrClearPosInfo(int act)
{
    switch (act) {
        case 1: // 标记0点
            on_pushButton_29_clicked();
            break;
        case 5: // 设置为停止状态
            on_btn_goOn_clicked();
            break;
        case 6: // Move To Home
            on_pushButton_21_clicked();
            break;
        case 8: // 反复
            on_btn_position_clicked();
            break;
        case 11: // 回到0点
            on_btn_reset_clicked();
            break;
        case 12: // 标记A点
            on_pushButton_10_clicked();
            break;
        case 13: // 移到A点
            on_pushButton_11_clicked();
            break;
        case 14: // 清除A点
            on_btn_chooseGCode_clicked();
            break;
        default:
            break;
    }
}

