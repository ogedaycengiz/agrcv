#include <SeriPrt.h>
#include <QtDebug>

SeriPort::SeriPort(QObject *parent) : QObject(parent)
{
    portAyarlariniYap();
    seriPort = new QextSerialPort(port_ayarlari);
    timer = new QTimer(this);
    timer->setInterval(10);
//    connect(this->seriPort, SIGNAL(readyRead()), this, SLOT(seriporttanOku()));
    connect(timer, SIGNAL(timeout()), this, SLOT(seriporttanOku()));
    connect(this, SIGNAL(seriPorttanBilgiOkundu(QByteArray&)), this, SLOT(gelenBilgiyiOku(QByteArray&)));
    timer->start();
    gelen_bilgi_okundu = false;
    kontrol = false;
    kk = 0;
}

void SeriPort::portAyarlariniYap()
{
    port_ayarlari.BaudRate = BAUD9600;
    port_ayarlari.DataBits = DATA_8;
    port_ayarlari.Parity = PAR_NONE;
    port_ayarlari.FlowControl = FLOW_OFF;
    port_ayarlari.StopBits = STOP_1;
    port_ayarlari.Timeout_Millisec = 10;
}

void SeriPort::seriPortuAc(QString port_adi)
{
    if (!port_adi.isNull())
    {
        seriPort->setPortName(port_adi);
        bool acildi = false;
        acildi = seriPort->open(QextSerialPort::ReadWrite);
        if (acildi)
        {
            qDebug() << port_adi << " acildi.";
        }
        else
        {
            qDebug() << port_adi << " acilamadi.";
        }
    }
    else
    {
        qDebug() << "Port adi bos oldugundan seri port acilamiyor...";
    }
}

void SeriPort::seriPortuKapat()
{
    if (seriPort->isOpen())
        seriPort->close();
    qDebug() << "Seri port kapatildi";
}

void SeriPort::seriPortaYaz(QString deger)
{
    if (!deger.isNull())
    {
        qDebug() << deger;
        seriPort->write(deger.toAscii().constData());
    }
    else
    {
        qDebug() << "Yazilacak bir deger yok";
    }
}

void SeriPort::seriporttanOku()
{
    QByteArray veriBayt;
    if (seriPort->isOpen())
    {
        int veriBuyuklugu = seriPort->bytesAvailable();
        if (veriBuyuklugu > 0)
        {
            veriBayt.resize(veriBuyuklugu);
            seriPort->read(veriBayt.data(), veriBayt.size());
            emit seriPorttanBilgiOkundu(veriBayt);
            gelen_bilgi_okundu = true;
        }
    }

}

void SeriPort::run()
{

}


void SeriPort::gelenBilgiyiOku(QByteArray &b)
{
    if (b.indexOf("a"))
    {
        kontrol = true;
        kk = 1;
    }
    else if (b.indexOf("b"))
    {
        kontrol = false;
        kk = 2;
    }
}


bool SeriPort::islemKontrol()
{
    return kontrol;
}


void SeriPort::kontrolDegeriniBelirle(int k)
{
    kk = k;
}


int SeriPort::kontrolDegeriniDondur()
{
    return kk;
}

bool SeriPort::seriPortDurumDondur()
{
    return seriPort->isOpen();
}

