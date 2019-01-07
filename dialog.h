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

private:
    Ui::Dialog *ui;

    Ads *mAds;
};

#endif // DIALOG_H
