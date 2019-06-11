#include "applenotify.h"

AppleNotify::AppleNotify(QObject *parent) : QObject(parent)
{
    supports.clear();

    m_pUsbManager = new QUsbManager(this);
    connect(m_pUsbManager, SIGNAL(deviceInserted(QtUsb::FilterList)),
            SLOT(onDeviceInserted(QtUsb::FilterList)));
}

void AppleNotify::onDeviceInserted(QtUsb::FilterList list)
{
    if(supports.isEmpty()) return;

    foreach (QtUsb::DeviceFilter f, list)
    {
        if(f.vid == 1452) //0x05ac = 1452
        {
            const QString pid = QString::number(f.pid, 16).toLower();

            //iphone ipad  去掉ipod
            if(supports.indexOf(pid) != -1)
            {
                qDebug() << "发现iPhone设备";
                emit appleDeviceInserted();
            }
            break;
        }
    }
}

void AppleNotify::supportIPad()
{
    static int i=0;
    if(i!=0) return;
    ++i;
    supports << "129a"  //iPad
             << "129f"  //iPad 2
             << "12a2"  //iPad 2 (3G; 64GB)
             << "12a3"  //iPad 2 (CDMA)
             << "12a4"  //iPad 3 (wifi)
             << "12a5"  //iPad 3 (CDMA)
             << "12a6"  //iPad 3 (3G, 16 GB)
             << "12a9"  //iPad 2
             << "12ab"; //iPad 4/Mini1
}

void AppleNotify::supportIPhone()
{
    static int i=0;
    if(i!=0) return;
    ++i;

    supports << "1290"  //iPhone
             << "1292"  //iPhone 3G
             << "1294"  //iPhone 3GS
             << "1297"  //iPhone 4
             << "129c"  //iPhone 4(CDMA)
             << "12a0"  //iPhone 4S
             << "12a8"; //iPhone5/5C/5S/6
}

void AppleNotify::supportIPod()
{
    static int i=0;
    if(i!=0) return;
    ++i;
    supports << "1201" //iPod 3G
             << "1202" //iPod 2G
             << "1203" //iPod 4.Gen Grayscale 40G
             << "1204" //iPod [Photo]
             << "1205" //iPod Mini 1.Gen/2.Gen
             << "1206" //iPod '06'
             << "1207" //iPod '07'
             << "1208" //iPod '08'
             << "1209" //iPod Video
             << "120a" //iPod Nano
             << "1223" //iPod Classic/Nano 3.Gen (DFU mode)
             << "1224" //iPod Nano 3.Gen (DFU mode)
             << "1225" //iPod Nano 4.Gen (DFU mode)
             << "1231" //iPod Nano 5.Gen (DFU mode)
             << "1240" //iPod Nano 2.Gen (DFU mode)
             << "1242" //iPod Nano 3.Gen (WTF mode)
             << "1243" //iPod Nano 4.Gen (WTF mode)
             << "1245" //iPod Classic 3.Gen (WTF mode)
             << "1246" //iPod Nano 5.Gen (WTF mode)
             << "1255" //iPod Nano 4.Gen (DFU mode)
             << "1260" //iPod Nano 2.Gen
             << "1261" //iPod Classic
             << "1262" //iPod Nano 3.Gen
             << "1263" //iPod Nano 4.Gen
             << "1265" //iPod Nano 5.Gen
             << "1266" //iPod Nano 6.Gen
             << "1267" //iPod Nano 7.Gen
             << "1291" //iPod Touch 1.Gen
             << "1293" //iPod Touch 2.Gen
             << "1296" //iPod Touch 3.Gen (8GB)
             << "1299" //iPod Touch 3.Gen
             << "129e" //iPod Touch 4.Gen
             << "12aa" //iPod Touch 5.Gen [A1421]
             << "1300" //iPod Shuffle
             << "1301" //iPod Shuffle 2.Gen
             << "1302" //iPod Shuffle 3.Gen
             << "1303";//iPod Shuffle 4.Gen
}
