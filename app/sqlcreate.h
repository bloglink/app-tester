/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       数据初始化
*******************************************************************************/
#ifndef SQLCREATE_H
#define SQLCREATE_H

#include <QDebug>
#include <QObject>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlError>
#include <QStringList>
#include <QSqlDatabase>

#define STEP 0x0010

class SqlCreate : public QObject
{
    Q_OBJECT
public:
    explicit SqlCreate(QObject *parent = 0);

public slots:
    void createRecord();
    void createSqlite();
private slots:
    void insertStatInfo(QSqlQuery query);
    void insertBackinfo(QSqlQuery query);
    void insertSystInfo(QSqlQuery query);
    void insertUserInfo(QSqlQuery query);
    void insertSignInfo(QSqlQuery query);
    void insertTypeInfo(QSqlQuery query);

    void insertOffsetDcr(QSqlQuery query);
    void insertOffsetInr(QSqlQuery query);
    void insertOffsetAcw(QSqlQuery query);
    void insertOffsetImp(QSqlQuery query);

    void insertSqlNetwork(QSqlQuery query);

    void insertMasterInfo(QSqlQuery query);
    void insertSourceInfo(QSqlQuery query);
    void insertConfigInfo(QSqlQuery query);
    void insertSetAcwInfo(QSqlQuery query);
};

#endif // SQLCREATE_H