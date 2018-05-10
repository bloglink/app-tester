/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       电枢测试仪主界面
*******************************************************************************/
#ifndef APPWINDOW_H
#define APPWINDOW_H

#include <QLayout>
#include <QDateTime>
#include <QMainWindow>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

#include "devsetcan.h"
#include "devserial.h"
#include "devsetrtc.h"
#include "devbuzzer.h"
#include "devscreen.h"
#include "tcpsocket.h"
#include "udpsocket.h"

#include "appauthor.h"
#include "appsignin.h"
#include "appsystem.h"
#include "appbackup.h"
#include "applogger.h"
#include "appmaster.h"
#include "appaction.h"
#include "typconfig.h"
#include "typsetdcr.h"
#include "typsetacw.h"
#include "typsetimp.h"
#include "typoffdcr.h"
#include "typoffacw.h"
#include "typoffimp.h"
#include "sqlcreate.h"
#include "sqlrecord.h"
#include "sqlupload.h"
#include "sqlsdcard.h"
#include "sqlunqual.h"
#include "apptester.h"

#include "boxdialog.h"

#include "appdefine.h"

#define WIDTH 150

class AppWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AppWindow(QWidget *parent = 0);
signals:
    void sendAppMap(QVariantMap msg);
    void sendUdpMap(QVariantMap msg);
private slots:
    void initUI();
    int initTitle();
    int initLayout();
    int initAuthor();
    int initDevice();
    int initScreen();
    int initSqlDir();
    int initTester();
    int initSignin();
    int initSystem();
    int initMaster();
    int initAction();
    int initBackup();
    int initLogger();
    int initConfig();
    int initSetDcr();
    int initSetAcw();
    int initSetImp();
    int initOffDcr();
    int initOffAcw();
    int initOffImp();
    int initRecord();
    int initUpload();
    int initSdcard();
    int initUnqual();
    int readSqlite();
    int readModels();
    int sendSqlite();
    int sendSignin();
    int initSocket();
    int initThread();
    int getNextItem();
    void initButton(QString title, QString name);
    void showBoxPop(QString text, int t);
    void saveSqlite();
    void saveModels();
    void clickButtons();
    void showTester();
    bool checkAction(QString msg);
    void screensShow(QString msg);
    void recvAppShow(QString msg);
    virtual void cloudAntimation();
    int taskThread();
    int taskClearData();
    int taskCheckPlay();
    int taskCheckSafe();
    int taskCheckCode();
    int taskStartView();
    int taskStartTest();
    int taskStartSave();
    int taskStartBeep();
    int taskClearBeep();
    int testThread();
    int testClearData();
    int testStartSend();
    int testStartTest();
    void testUpdate(QVariantMap msg);
    void recvUdpMap(QVariantMap msg);
    void recvAppMap(QVariantMap msg);
    void wait(int ms);
private:
    QStackedWidget *stack;
    QVBoxLayout *btnLayout;
    QFrame *btnFrame;
    QList<QPushButton*> buttons;
    QVariantMap config;
    QVariantMap tmpMap;

    QStringList sources;
    QStringList masters;
    QList<int> sgroups;
    QList<int> saction;
    QList<int> maction;

    QString verNumb;
    BoxDialog *boxbar;
    bool isSignin;

    int preindex;

    QList<int> addrList;
    QStringList taskList;
    QStringList testList;
    int currTask;
    int currTest;
    int taskShift;
    int testShift;
    int testIndex;
    int timeOut;
    int timeTst;
    QElapsedTimer t;
};

#endif // APPWINDOW_H
