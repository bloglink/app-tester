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
#include <QThread>
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
#include "tcpserver.h"
#include "tcpsocket.h"

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
#include "sqlimport.h"
#include "sqlrecord.h"
#include "sqlupload.h"
#include "sqlsdcard.h"
#include "sqlexport.h"
#include "apptester.h"

#include "boxdialog.h"

#include "main.h"

#define WIDTH 150

class AppWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit AppWindow(QWidget *parent = 0);
    ~AppWindow();
signals:
    void sendAppMap(QVariantMap msg);
    void sendNetMsg(QTmpMap msg);
    void sendAppMsg(QTmpMap msg);
private slots:
    int initUI();
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
    int initImport();
    int initExport();
    int initRecord();
    int initUpload();
    int initSdcard();
    int readBackup();
    int readSystem();
    int readConfig();
    int sendSqlite();
    int sendSignin();
    int initSocket();
    int initThread();
    void initButton(QString title, QString name);
    void showBoxPop(QString text, int t);
    void saveBackup(QTmpMap msg);
    void saveSqlite(QTmpMap msg);
    void saveConfig(QTmpMap msg);
    void clickButtons();
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
    int taskStartWait();
    int taskStartTest();
    int taskStartSave();
    int taskStartBeep();
    int taskClearBeep();
    int taskResetTest();
    int taskCheckStop();
    int taskQuickConf();
    int taskQuickWait();
    int testThread();
    int testClearData();
    int testStartSend();
    int testStartTest();
    int getNextItem();
    void showBarCode();
    virtual void keyReleaseEvent(QKeyEvent *e);
    void loopBoxbar();
    void recvSqlMsg(QTmpMap msg);
    void recvNewMsg(QTmpMap msg);
    void recvTmpMsg(QTmpMap msg);
    void recvStaMsg(QTmpMap msg);
    void recvAppMsg(QTmpMap msg);
    void wait(int ms);
private:
    QStackedWidget *stack;
    QVBoxLayout *btnLayout;
    QFrame *btnFrame;
    QList<QPushButton*> buttons;
    BoxDialog *boxbar;

    QThread *sql;

    QString verNumb;

    int preindex;
    int currTask;
    int currTest;
    int taskShift;
    int testShift;
    int currItem;
    qint64 timeOut;
    qint64 timeTst;
    QElapsedTimer t;

    QTmpMap tmpSet;
    QTmpMap tmpMsg;
    QStringList shows;
    QStringList users;
    QList<int> roles;
    QList<int> forms;

    QString barcode;
    QTimer *scanner;
    quint8 station;
};

#endif // APPWINDOW_H
