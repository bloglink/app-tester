/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       后台管理
*******************************************************************************/
#include "appbackup.h"

AppBackup::AppBackup(QWidget *parent) : QWidget(parent)
{
    initUI();

}

void AppBackup::initUI()
{
    initLayout();
    initBoxText();
    initMacText();
}

void AppBackup::initLayout()
{
    boxGroup = new QGroupBox(this);
    boxGroup->setTitle(tr("后台配置"));

    macGroup = new QGroupBox(this);
    macGroup->setTitle(tr("本机信息"));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(boxGroup);
    layout->addWidget(macGroup);
}

void AppBackup::initBoxText()
{
    QVBoxLayout *layout = new QVBoxLayout;
    boxGroup->setLayout(layout);

    QStringList names;
    names << tr("产品型号") << tr("产品编号") << tr("评审编号") << tr("出厂日期")
          << tr("语言支持") << tr("工位设置") << tr("最高电压") << tr("自动测试")
          << tr("测试延时") << tr("目标网络") << tr("网络端口");

    for (int i=0; i < names.size(); i++) {
        QHBoxLayout *box = new QHBoxLayout;
        layout->addLayout(box);
        QString tmp = QString("%1").arg(i+1, 2, 10, QChar('0'));
        box->addWidget(new QLabel(tmp + " " + names.at(i) + ":", this));
        QLineEdit *input = new QLineEdit(this);
        input->setFixedHeight(30);
        box->addWidget(input);
        txts.append(input);
    }
    layout->addStretch();

    QHBoxLayout *btn = new QHBoxLayout;
    layout->addLayout(btn);
    btn->addStretch();
}

void AppBackup::initMacText()
{
    QVBoxLayout *layout = new QVBoxLayout;
    macGroup->setLayout(layout);

    text = new QTextBrowser(this);
    layout->addWidget(text);
    layout->addStretch();

    QHBoxLayout *btn = new QHBoxLayout;
    layout->addLayout(btn);

    isBack = new QCheckBox(this);
    isBack->setText(tr("同时清除后台配置数据"));
    btn->addWidget(isBack);

    btn->addStretch();

    QPushButton *recy = new QPushButton(this);
    recy->setText(tr("恢复出厂"));
    recy->setFixedSize(97, 44);
    btn->addWidget(recy);
    connect(recy, SIGNAL(clicked(bool)), this, SLOT(recovery()));

    QHBoxLayout *s = new QHBoxLayout;
    layout->addLayout(s);
    s->addStretch();

    QPushButton *test = new QPushButton(this);
    test->setText(tr("定子背景"));
    test->setFixedSize(97, 44);
    s->addWidget(test);
    connect(test, SIGNAL(clicked(bool)), this, SLOT(flashopt()));

    QPushButton *back = new QPushButton(this);
    back->setText(tr("电枢背景"));
    back->setFixedSize(97, 44);
    s->addWidget(back);
    connect(back, SIGNAL(clicked(bool)), this, SLOT(flashopt()));

    QPushButton *save = new QPushButton(this);
    save->setText(tr("保存"));
    save->setFixedSize(97, 44);
    s->addWidget(save);
    connect(save, SIGNAL(clicked(bool)), this, SLOT(saveSettings()));
}

void AppBackup::initSettings()
{
    int r = tmpSet[(1000 + Qt::Key_0)].toInt();
    for (int i=0; i < txts.size(); i++) {
        txts.at(i)->setText(tmpSet[r + i].toString());
    }
}

void AppBackup::saveSettings()
{
    int r = tmpSet[(1000 + Qt::Key_0)].toInt();
    for (int i=0; i < txts.size(); i++) {
        tmpSet[r + i] = txts.at(i)->text();
    }
    tmpSet.insert(Qt::Key_0, Qt::Key_Save);
    tmpSet.insert(Qt::Key_1, "aip_backup");
    emit sendAppMsg(tmpSet);
}

void AppBackup::initNetworks()
{
    text->clear();
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    for (int i=0; i < list.size(); i++) {
        QString str = QString("%1").arg(i+1, 2, 10, QChar('0'));
        text->insertPlainText(tr("[%1] %2\n").arg(str).arg(list.at(i).hardwareAddress()));
        QList<QNetworkAddressEntry> addr = list.at(i).addressEntries();
        if (addr.size() > 0)
            text->insertPlainText(tr("[%1] %2\n").arg(str).arg(addr.at(0).ip().toString()));
    }
    QProcess sys;
    sys.start("df -h");
    sys.waitForReadyRead();
    if (1) {
        QByteArray msg = sys.readAll();
        QStringList s = QString(msg).split("\n");
        for (int i=0; i < s.size(); i++) {
            if (QString(s.at(i)).endsWith("/")) {
                QStringList t = QString(s.at(i)).split(" ", QString::SkipEmptyParts);
                if (t.size() >= 6)
                    text->insertPlainText(tr("[DiskA]\tUSED:%1\tALL:%2\n").arg(t.at(4)).arg(t.at(1)));
            }
            if (QString(s.at(i)).endsWith("/mnt/sdcard")) {
                QStringList t = QString(s.at(i)).split(" ", QString::SkipEmptyParts);
                if (t.size() >= 6)
                    text->insertPlainText(tr("[DiskB]\tUSED:%1\tALL:%2\n").arg(t.at(4)).arg(t.at(1)));
            }
            if (QString(s.at(i)).endsWith("/mnt/nandflash")) {
                QStringList t = QString(s.at(i)).split(" ", QString::SkipEmptyParts);
                if (t.size() >= 6)
                    text->insertPlainText(tr("[DiskC]\tUSED:%1\tALL:%2\n").arg(t.at(4)).arg(t.at(1)));
            }
            if (QString(s.at(i)).endsWith("/mnt/usb1")) {
                QStringList t = QString(s.at(i)).split(" ", QString::SkipEmptyParts);
                if (t.size() >= 6)
                    text->insertPlainText(tr("[DiskD]\tUSED:%1\tALL:%2\n").arg(t.at(4)).arg(t.at(1)));
            }
            if (QString(s.at(i)).endsWith("/mnt/usb2")) {
                QStringList t = QString(s.at(i)).split(" ", QString::SkipEmptyParts);
                if (t.size() >= 6)
                    text->insertPlainText(tr("[DiskE]\tUSED:%1\tALL:%2\n").arg(t.at(4)).arg(t.at(1)));
            }
        }
    }
    sys.close();
    sys.start("ps");
    sys.waitForReadyRead();
    if (1) {
        QByteArray msg = sys.readAll();
        QStringList s = QString(msg).split("\n");
        for (int i=0; i < s.size(); i++) {
            if (QString(s.at(i)).contains("aip")) {
                QStringList t = QString(s.at(i)).split(" ", QString::SkipEmptyParts);
                text->insertPlainText(tr("[MSize]\tUSED:%1\n").arg(t.at(2)));
            }
        }
    }
    sys.close();
    sys.deleteLater();
}

void AppBackup::flashopt()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    QPixmap picture = QPixmap(btn->text() == tr("电枢背景") ? ":/logo_arm.png" : ":/logo_aip.bmp");
    picture.save("01.bmp", "bmp");
    QProcess cmd;
    cmd.start("sync");
    cmd.waitForFinished();
    cmd.start("flash_opt bmp 01.bmp");
    cmd.waitForFinished();
    cmd.close();
    cmd.start("rm 01.bmp");
    cmd.waitForFinished();
    cmd.close();
    cmd.deleteLater();
    text->insertPlainText("bmp ok");
}

void AppBackup::recovery()
{
    qWarning() << "start recovery";
    QProcess sys;
    if (isBack->isChecked()) {
        sys.start("rm /mnt/nandflash/backup.db");
        sys.waitForFinished();
    }
    sys.start("rm /mnt/nandflash/system.db");
    sys.waitForFinished();
    sys.start("rm /mnt/nandflash/config.db");
    sys.waitForFinished();
    sys.start("rm /mnt/nandflash/record.db");
    sys.waitForFinished();
    sys.start("reboot");
    sys.waitForFinished();
}

void AppBackup::recvAppMsg(QTmpMap msg)
{
    switch (msg.value(Qt::Key_0).toInt()) {
    case Qt::Key_Copy:
        tmpSet = msg;
        break;
    case Qt::Key_News:
        break;
    default:
        break;
    }
}

void AppBackup::showEvent(QShowEvent *e)
{
    this->setFocus();
    initSettings();
    initNetworks();
    e->accept();
}

