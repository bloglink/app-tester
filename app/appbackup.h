/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       后台管理
*******************************************************************************/
#ifndef APPBACKUP_H
#define APPBACKUP_H

#include <QLabel>
#include <QDebug>
#include <QLayout>
#include <QWidget>
#include <QVariant>
#include <QProcess>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QShowEvent>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include <QTableWidget>
#include <QHostAddress>
#include <QTextBrowser>
#include <QNetworkInterface>

#include "main.h"

class AppBackup : public QWidget
{
    Q_OBJECT
public:
    explicit AppBackup(QWidget *parent = 0);
signals:
    void sendAppMsg(QTmpMap msg);
private slots:
    void initUI();
    void initLayout();
    void initBoxText();
    void initMacText();
    void initSettings();
    void saveSettings();
    void initNetworks();
    void flashopt();
    void recovery();
    void recvAppMsg(QTmpMap msg);
    virtual void showEvent(QShowEvent *e);
private:
    QGroupBox *boxGroup;
    QGroupBox *macGroup;
    QList<QLineEdit*> txts;
    QTmpMap tmpSet;
    QTextBrowser *text;
    QCheckBox *isBack;
};

#endif // APPBACKUP_H
