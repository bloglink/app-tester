/*******************************************************************************
 * Copyright [2018] <青岛艾普智能仪器有限公司>
 * All rights reserved.
 *
 * version:     0.1
 * author:      zhaonanlin
 * brief:       CAN总线设置
*******************************************************************************/
#include "devsetcan.h"

DevSetCan::DevSetCan(QObject *parent) : QObject(parent)
{
    initDevPort();
    initDevStat();
}

void DevSetCan::initDevPort()
{
    fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);  //打开套接字
    if (fd < 0)
        perror("can0 open error");
    if (system("ifconfig can0 down") < 0)
        perror("can0 down error");
    if (system("ip link set can0 type can bitrate 500000 restart-ms 100") < 0)
        perror("can0 set error");
    if (system("ifconfig can0 up") < 0)
        perror("can0 up error");

    struct ifreq ifr;
    strcpy(ifr.ifr_name, "can0");
    if (ioctl(fd, SIOCGIFINDEX, &ifr) < 0)
        perror("can0 ioctl error");

    struct sockaddr_can addr;
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;
    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        perror("can0 bind error");

    struct can_filter rfilter[4];         //  rfilter 记录总数
    rfilter[0].can_id = 0x0041;           // 电阻板ID
    rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[1].can_id = 0x0061;           // 绝缘板ID
    rfilter[1].can_mask = CAN_SFF_MASK;
    rfilter[2].can_id = 0x0081;           // 匝间板ID
    rfilter[2].can_mask = CAN_SFF_MASK;
    rfilter[3].can_id = 0x0481;           // 匝间板ID
    rfilter[3].can_mask = CAN_SFF_MASK;
    if (setsockopt(fd, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter)) < 0)
        perror("can0 filter error");
}

void DevSetCan::initDevStat()
{
    timeOut = 0;
    currItem = 0;
    sendDevData(CAN_ID_DCR, QByteArray::fromHex("00"));
    sendDevData(CAN_ID_ACW, QByteArray::fromHex("00"));
    sendDevData(CAN_ID_IMP, QByteArray::fromHex("00"));
    sendDevData(CAN_ID_DCR, QByteArray::fromHex("05"));
    sendDevData(CAN_ID_DCR, QByteArray::fromHex("08"));
    sendDevData(CAN_ID_ACW, QByteArray::fromHex("08"));
    sendDevData(CAN_ID_IMP, QByteArray::fromHex("08"));
}

void DevSetCan::sendDevData(int id, QByteArray msg)
{
    tmpMsg.insert(AddrEnum, id);
    tmpMsg.insert(AddrText, msg);
    sender.append(tmpMsg);
    tmpMsg.clear();
}

void DevSetCan::readDevData(int id, QByteArray msg)
{
    tmpMsg.insert(AddrEnum, id);
    tmpMsg.insert(AddrText, msg);
    recver.append(tmpMsg);
    tmpMsg.clear();
    //    if (id != 0x481)
    //        qDebug() << "can recv:"
    //                 << QString("%1").arg(id, 4, 16, QChar('0'))
    //                 << msg.toHex().toUpper();
}

bool DevSetCan::readDevPort()
{
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(fd, &rdfds);
    if (select(fd+1, &rdfds, NULL, NULL, &tv) >= 0) {  // 监听是否可读
        if (FD_ISSET(fd, &rdfds)) {
            if (read(fd, &frame, sizeof(struct can_frame)) > 0)   // 读取缓冲区
                return true;
        }
    }
    return false;
}

bool DevSetCan::sendDevPort(can_frame can)
{
    struct timeval tv;
    tv.tv_sec  = 0;
    tv.tv_usec = 0;
    fd_set rdfds;
    FD_ZERO(&rdfds);
    FD_SET(fd, &rdfds);
    if (select(fd+1, NULL, &rdfds, NULL, &tv) >= 0) {  // 监听是否可写
        if (FD_ISSET(fd, &rdfds)) {
            if (write(fd, &can, sizeof(struct can_frame)) == sizeof(struct can_frame))
                return true;
        }
    }
    return false;
}

void DevSetCan::getAllDat()
{
    while (readDevPort()) {
        QByteArray msg;
        for (int i=0; i < frame.can_dlc; i++) {
            msg.append(frame.data[i]);
        }
        readDevData(frame.can_id, msg);
    }
}

void DevSetCan::putAllDat()
{
    while (!sender.isEmpty()) {
        tmpMsg = sender.dequeue();
        frame.can_id = tmpMsg[AddrEnum].toInt();
        QByteArray msg = tmpMsg[AddrText].toByteArray();
        frame.can_dlc = msg.size();
        for (int i=0; i < frame.can_dlc; i++) {
            frame.data[i] = msg.at(i);
        }
        sendDevPort(frame);
        tmpMsg.clear();
        //        qDebug() << "can send:"
        //                 << QString("%1").arg(frame.can_id, 4, 16, QChar('0'))
        //                 << msg.toHex().toUpper();
    }
}

void DevSetCan::updateAll()
{
    for (int i=0; i < 100; i++) {
        if (recver.isEmpty())
            break;
        tmpMsg = recver.dequeue();
        int id = tmpMsg[AddrEnum].toInt();
        QByteArray msg = tmpMsg[AddrText].toByteArray();
        tmpMsg.clear();
        if (msg.size() < 2)
            continue;
        switch (id) {
        case 0x0041:
            updateDCR(msg);
            break;
        case 0x0061:
            updateAcw(msg);
            break;
        case 0x0081:
        case 0x0481:
            updateImp(id, msg);
            break;
        default:
            break;
        }
    }
}

void DevSetCan::updateDCR(QByteArray msg)
{
    quint8 cmd = quint8(msg.at(0));
    quint8 num = quint8(msg.at(1));
    if (cmd == 0x00) {
        qDebug() << "dcr recv:" << msg.toHex();
        tmpDat[DataDCRS] = num;  // 修改电阻板状态
        updateDat();
    }
    if (cmd == 0x01) {
        tmpDat[DataDCRS] = 0x01;  // 修改电阻板状态
        if (num == 1 && currItem == AddrDCRS1) {
            int s = tmpSet[AddrWeld].toInt();
            int n = quint8(msg.at(2));
            int g = quint8(msg.at(3));
            quint32 r = quint32(msg.at(4)) + quint32(msg.at(5)) * 0x0100
                    + quint32(msg.at(6))*0x10000  + quint32(msg.at(7))*0x1000000;
            r = g > 3 ? r / 1000 : r;  // 换算为mΩ
            tmpDat[s + n] = r;
            int w = tmpSet.value(AddrDCRSW).toInt();
            int t = tmpSet[AddrDCRS1].toInt();
            int h = tmpSet[t + 1].toInt();
            double std = tmpSet[w + n].toInt();
            qDebug() << "dcr calc:" << n << g << r << std << msg.toHex();
            if (abs(r-std)/std >= h) {
                tmpDat[DataDCRJ] = DataNG;
                tmpDat[DataOKNG] = DataNG;
            }
        }
        if (num == 2 && currItem == AddrDCRS2) {
            int s = tmpSet[AddrChip].toInt();
            int n = quint8(msg.at(2));
            int g = quint8(msg.at(3));
            quint32 r = quint32(msg.at(4)) + quint32(msg.at(5)) * 0x0100
                    + quint32(msg.at(6))*0x10000  + quint32(msg.at(7))*0x1000000;
            tmpDat[s + n] = r;
            qDebug() << "dcr calc:" << n << g << r << msg.toHex();
        }
        if (num == 1 && currItem == AddrDCRS3) {
            int s = tmpSet[AddrDiag].toInt();
            int n = quint8(msg.at(2));
            quint32 r = quint32(msg.at(4)) + quint32(msg.at(5)) * 0x0010 + quint32(msg.at(6))*0x1000;
            tmpDat[s + n] = r;
        }
    }
    if (cmd == 0x05) {
        tmpMsg.insert(AddrEnum, Qt::Key_Shop);
        tmpMsg.insert(DataTemp, quint8(msg.at(1)) + quint8(msg.at(2))*256);
        emit sendAppMsg(tmpMsg);
        tmpMsg.clear();
    }
}

void DevSetCan::updateImp(int id, QByteArray msg)
{
    t.restart();
    if (id == 0x0081) {
        quint8 cmd = quint8(msg.at(0));
        quint8 num = quint8(msg.at(1));
        if (cmd == 0x00) {
            tmpDat[DataIMPS] = num;
            tmpDat[AddrData] = 0;
            updateDat();
            qDebug() << "imp time:" << t.elapsed();
        }
        if (cmd == 0x02) {
            tmpDat[DataIMPG] = quint8(msg.at(2));  // 匝间档位
            tmpDat[DataIMPF] = quint8(msg.at(3));  // 匝间频率
            tmpDat[DataIMPU] = quint8(msg.at(4))*256 + (quint8)msg.at(5);  // 匝间电压
        }
        if (cmd == 0x03) {
            if (num == 0xFF) {
                int s = tmpSet[AddrIMPW].toInt();
                for (int i=0; i < wave.size(); i++) {
                    tmpDat[s + i] = wave.at(i);
                }
                calc();
                wave.clear();
                updateDat();
                qDebug() << "imp time:" << t.elapsed();
            } else {
                tmpDat[AddrData] = num;  // 波形编号
            }
        }
    }
    if (id == 0x0481) {
        for (int i=0; i < 4; i++) {
            wave.append(quint8(msg.at(i*2+0))*256 + quint8(msg.at(i*2+1)));
        }
    }
}

void DevSetCan::calc()
{
    int n = tmpDat[AddrData].toInt() - 1;
    int s = tmpSet[AddrIMPA].toInt() + n;  // 匝间结果地址
    int r = tmpSet[AddrIMPS1].toInt() + AddrIMPSH;  // 匝间上限地址
    int t = tmpSet[AddrIMPSW].toInt();  // 匝间波形地址
    double h = tmpSet[r].toDouble();  // 匝间上限
    quint32 Area1=0;
    quint32 Area2=0;
    quint32 Area3=0;
    quint32 D = 0;

    for (int i=1; i<wave.size()-1; i++) {
        int a1 = tmpSet[t + i].toInt();
        int a2 = wave.at(i);
        Area1 += abs(a1-0x200);
        Area2 += abs(a2-0x200);
        int b1 = tmpSet[t + i - 1].toInt();
        int b2 = tmpSet[t + i].toInt();
        int b3 = tmpSet[t + i + 1].toInt();
        int c1 = wave.at(i - 1);
        int c2 = wave.at(i);
        int c3 = wave.at(i + 1);
        Area3 += abs((b1+b2*2+b3)-(c1+c2*2+c3));
    }
    D = qMin(Area2,Area3/4)*100*1000/Area1;
    D = qMin(D, quint32(99990));
    tmpDat[s] = D;

    if (D >= h*1000) {
        tmpDat[DataIMPJ] = DataNG;
        tmpDat[DataOKNG] = DataNG;
    }
}

void DevSetCan::updateAcw(QByteArray msg)
{
    t.restart();
    quint8 cmd = quint8(msg.at(0));
    quint8 num = quint8(msg.at(1));
    if (cmd == 0x00)
        tmpDat[DataACWS] = num;  // 修改高压板状态
    if (cmd == 0x01) {
        tmpDat[DataACWS] = 0x01;  // 修改高压板状态
        int t = (currItem%AddrACWS1);  // 第几个测试组
        int k = tmpSet[AddrACWS1 + t].toInt();  // 测试参数存储地址
        double h = tmpSet[k + AddrACWSH].toDouble();
        double l = tmpSet[k + AddrACWSL].toDouble();
        int s = tmpSet[AddrINRA + t].toInt();  // 测试结果存储地址
        double v = quint8(msg.at(1))*256 + (quint8)msg.at(2);  // 实测电压
        tmpDat[s + AddrACWU] = v;
        double r = quint8(msg.at(3))*256 + (quint8)msg.at(4);  // 实测结果
        tmpDat[s + AddrACWR] = r;
        double p = quint8(msg.at(5));  // 小数位数
        tmpDat[s + AddrACWP] = p;
        r *= qPow(10, -quint8(msg.at(5)));
        h = (currItem == AddrACWS1) ? h : h/100;  // 耐压电流倍数100
        l = (currItem == AddrACWS1) ? l : l/100;  // 耐压电流倍数100
        if (((h == 0) && (r > l)) || ((h != 0) && (r > l && r < h))) {
            tmpDat[s + AddrACWJ] = DataOK;
        } else {
            tmpDat[s + AddrACWJ] = DataNG;
            tmpDat[DataOKNG] = DataNG;
        }
    }
    if (cmd == 0x08) {
        tmpDat[DataACWV] = msg;
    }
    updateDat();
    qDebug() << "acw time:" << t.elapsed();
}

void DevSetCan::updateDat()
{
    tmpDat.insert(AddrEnum, Qt::Key_News);
    tmpDat.insert(AddrText, currItem);
    emit sendAppMsg(tmpDat);
}

void DevSetCan::sampleDCR()
{
    tmpDat.clear();
    currItem = AddrDCRS1;
    sendDevData(CAN_ID_DCR, QByteArray::fromHex("010103"));  // 01启动;01片间;03单工位
}

void DevSetCan::sampleIMP(QTmpMap dat)
{
    setupIMP(dat);

    QByteArray msg = QByteArray::fromHex("0103110001");  // 启动绝缘
    msg[4] = tmpSet[tmpSet[AddrIMPS1].toInt() + AddrIMPSA].toInt();
    sendDevData(CAN_ID_IMP, msg);  // 匝间采样
    qDebug() << "imp send:" << msg.toHex();
}

void DevSetCan::setupDCR()
{
    int addr = tmpSet[AddrModel].toInt();  // 综合配置地址
    int quan = tmpSet[addr + 0].toInt();  // 电枢片数
    int tool = tmpSet[addr + 1].toInt();  // 夹具针数
    QByteArray msg;
    // 03配置;01工位数;01板卡数;0C夹具数
    msg = QByteArray::fromHex("0301010C");
    msg[3] = tool;
    sendDevData(CAN_ID_DCR, msg);
    qDebug() << "dcr send:" << msg.toHex();
    // 04配置;0C电枢片数;02间隔;E803采样数;01自动档;05档位
    msg = QByteArray::fromHex("040C0002E8030105");
    msg[1] = quan;  // 电枢片数
    msg[2] = (currItem == AddrDCRS3) ? quan/2 -1 : 0;  // 间隔片数
    sendDevData(CAN_ID_DCR, msg);
    qDebug() << "dcr send:" << msg.toHex();
}

void DevSetCan::setupACW()
{
    QByteArray msg;
    for (int i=0; i < 4; i++) {
        int addr = tmpSet[AddrACWS1 + i].toInt();  // 高压配置地址
        int v = tmpSet[addr + AddrACWSV].toInt();  // 电压
        int t = tmpSet[addr + AddrACWST].toInt();  // 时间
        // 03配置;00序号;04绝缘;0001轴;0004线;00频率
        msg = QByteArray::fromHex("0300040001000400");
        msg[1] = i;
        msg[2] = (i == 0) ? 0x04 : 0x05;
        msg[4] = (i == 3) ? 0x02 : 0x01;
        msg[6] = (i == 1) ? 0x02 : 0x04;
        msg[7] = (i == 0) ? 0x00 : 0x32;
        sendDevData(CAN_ID_ACW, msg);
        // 04配置;00序号;01F4电压;000A时间;0000下限
        msg = QByteArray::fromHex("040001F4000A0000");
        msg[1] = i;
        msg[2] = v/256;
        msg[3] = v%256;
        msg[4] = t/256;
        msg[5] = t%256;
        sendDevData(CAN_ID_ACW, msg);
        // 05配置;00序号;0000上限;00电弧;030A缓升缓降
        msg = QByteArray::fromHex("0500000000030A");
        msg[1] = i;
        msg[2] = (i == 0) ? 0x00 : 0x09;
        msg[3] = (i == 0) ? 0x00 : 0xC4;
        sendDevData(CAN_ID_ACW, msg);
    }
}

void DevSetCan::setupIMP(QTmpMap map)
{
    int addr = tmpSet[AddrModel].toInt();  // 综合配置地址
    int quan = tmpSet[addr + 0].toInt();  // 电枢片数
    //    int tool = tmpSet[addr + 1].toInt();  // 夹具针数
    int conf = tmpSet[AddrIMPS1].toInt();  // 匝间配置地址
    int v = tmpSet[conf + AddrIMPSV].toInt();  // 电压
    int c = tmpSet[conf + AddrIMPST].toInt();  // 次数
    int f = tmpSet[conf + AddrIMPSF].toInt();  // 频率
    f = (map.value(AddrFreq).isNull()) ? f : map.value(AddrFreq).toInt();  // 采样配置
    // 03配置;00序号;0C电枢片数;01间隔;01F4电压;11工位;07频率
    QByteArray msg = QByteArray::fromHex("03000C0101F41107");
    msg[2] = quan;
    msg[4] = v/256;
    msg[5] = v%256;
    msg[6] = ((v/1000+1) << 4) + c;  // 档位+次数
    msg[7] = f;
    sendDevData(CAN_ID_IMP, msg);
}

void DevSetCan::startDCR()
{
    QByteArray msg = QByteArray::fromHex("010103");  // 01启动;01片间;03单工位
    msg[1] = (currItem == AddrDCRS2) ? 0x02 : 0x01;  // 01片间;02焊接
    sendDevData(CAN_ID_DCR, msg);
    tmpDat[DataDCRS] = 1;
    qDebug() << "dcr send:" << msg.toHex();
}

void DevSetCan::startACW()
{
    QByteArray msg = QByteArray::fromHex("0104001100");  // 01启动;04绝缘;00自动档;11工位;00序号
    msg[1] = (currItem == AddrACWS1) ? 0x04 : 0x05;  // 04绝缘/05交耐
    msg[4] = currItem - AddrACWS1;
    sendDevData(CAN_ID_ACW, msg);
    tmpDat[DataACWS] = 1;
    qDebug() << "acw send:" << msg.toHex();
}

void DevSetCan::startIMP()
{
    int addr = tmpSet[AddrIMPS1].toInt();  // 匝间配置地址
    int time = tmpSet[addr + AddrIMPSA].toInt(); // 间隔(>=1)
    QByteArray msg = QByteArray::fromHex("0100110001");  // 01启动;00序号;11工位;0001间隔
    msg[4] = time;
    sendDevData(CAN_ID_IMP, msg);
    tmpDat[DataIMPS] = DataTest;  // 修改状态为Test
    qDebug() << "imp send:" << msg.toHex();
}

void DevSetCan::startTest(QTmpMap map)
{
    tmpDat.clear();
    tmpDat[DataOKNG] = DataOK;
    currItem = map.value(AddrText).toInt() + AddrModel;
    switch (currItem) {
    case AddrDCRS1:
    case AddrDCRS2:
    case AddrDCRS3:
        startDCR();
        break;
    case AddrACWS1:
    case AddrACWS2:
    case AddrACWS3:
    case AddrACWS4:
        startACW();
        break;
    case AddrIMPS1:
        startIMP();
        break;
    default:
        break;
    }
}

void DevSetCan::recvAppMsg(QTmpMap msg)
{
    int c = msg.value(AddrEnum).toInt();
    switch (c) {
    case Qt::Key_Copy:
        tmpSet = msg;  // 保存设置参数
        break;
    case Qt::Key_Plus:
        getAllDat();
        putAllDat();
        updateAll();
        timeOut++;
        if (timeOut % 500 == 0) {
            if (sender.isEmpty() && recver.isEmpty())
                sendDevData(CAN_ID_DCR, QByteArray::fromHex("05"));
        }
        break;
    case Qt::Key_Send:
        if (msg.value(AddrText).toInt() == AddrModel) {
            setupDCR();
            setupACW();
            setupIMP(msg);
        }
        if (msg.value(AddrText).toInt() == AddrDCRS1) {
            sampleDCR();
        }
        if (msg.value(AddrText).toInt() == AddrIMPS1) {
            sampleIMP(msg);
        }
        break;
    case Qt::Key_Play:
        startTest(msg);
        break;
    default:
        break;
    }
}
