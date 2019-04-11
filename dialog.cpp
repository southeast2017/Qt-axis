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
//    server = new TcpServer("192.168.43.99", 7777);
//    server = new TcpServer("10.21.11.73", 7777);
    server = new TcpServer("169.254.245.177", 7777);

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

/* 名称：正反解
 * 描述：未来将会删除本个函数
 */
void Dialog::getIFResultFromSocket(int dir, QVector<double> vector)
{
    if (dir == Negative) {
        ui->lineEdit_10->setText(QString::number(vector[0]));
        ui->lineEdit_11->setText(QString::number(vector[1]));
        ui->lineEdit_12->setText(QString::number(vector[2]));
        ui->lineEdit_13->setText(QString::number(vector[3]));
        ui->lineEdit_14->setText(QString::number(vector[4]));
        ui->lineEdit_15->setText(QString::number(vector[5]));
    } else if (dir == Positive) {
        ui->lineEdit_7->setText(QString::number(vector[0]));
        ui->lineEdit_8->setText(QString::number(vector[1]));
        ui->lineEdit_9->setText(QString::number(vector[2]));
    }
}

/* 未来将会删除这个函数 */
void Dialog::getRequestFromSocket(int dir)
{
    QVector<double> data;
    if (dir == Positive) {
        data.append(ui->lineEdit_16->text().toDouble());
        data.append(ui->lineEdit_17->text().toDouble());
        data.append(ui->lineEdit_18->text().toDouble());
        data.append(ui->lineEdit_19->text().toDouble());
        data.append(ui->lineEdit_20->text().toDouble());
        data.append(ui->lineEdit_21->text().toDouble());
    } else if (dir == Negative) {
        data.append(ui->lineEdit_4->text().toDouble());
        data.append(ui->lineEdit_5->text().toDouble());
        data.append(ui->lineEdit_6->text().toDouble());
    }

    emit sendDataToSocket(dir, data);
}

void Dialog::newSocketConnectToDialog(TcpSocket* socket)
{
    qRegisterMetaType<QVector<double>>("QVector<double>");
    /* *** 连接：更新ui的信号与槽 *** */
    connect(socket, &TcpSocket::sendIFResultToDialog, this, &Dialog::getIFResultFromSocket);
    /* *** 线程从ui获取数据需要阻塞 请求=>页面 *** */
    connect(socket, &TcpSocket::reqDataFromDialog, this, &Dialog::getRequestFromSocket, Qt::BlockingQueuedConnection);
    /* *** 线程从ui获取数据需要阻塞 页面-->数据=>页面 *** */
    connect(this, &Dialog::sendDataToSocket, socket, &TcpSocket::getDataFromDialog);
    /* *** 线程读取的数据发送到TCP线程中 *** */
    connect(mAds->mOperation, &Operation::setValue, socket, &TcpSocket::sendDataToClient);
}

