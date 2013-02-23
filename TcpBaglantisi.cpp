#include <TcpBaglantisi.h>

TcpBaglantisi::TcpBaglantisi() : QTcpServer()
{
    alarm = false;
}

void TcpBaglantisi::gelenIstekleriCevapla(int handle)
{
    //qDebug() << "Baglanti geldi.";
    QTcpSocket *soket = new QTcpSocket(this);
    if (!soket->setSocketDescriptor(handle))
    {
        qDebug() << "Baglanti saglanamadi...";
        return;
    }
    int islemNu;
   // qDebug() << "Verileri : " << soket->readAll();
    QDataStream islem(soket);
    islem.setVersion(QDataStream::Qt_4_6);
    if (soket->bytesAvailable() < (quint64) sizeof(int))
    {
        if (!soket->waitForReadyRead(30))
        {
            qDebug() << "Veriler okunmaya hazir degil.";
            return;
        }
    }
    islem >> islemNu;
    qDebug() << islemNu;
    if (islemNu == 0)
    {
        emit alarmKontrol(true);
        alarm = true;
    }
    else if (islemNu == 1)
    {
        emit alarmKontrol(false);
        alarm = false;
    }
    soket->waitForDisconnected();
    soket->disconnectFromHost();
}



void TcpBaglantisi::incomingConnection(int handle)
{
    gelenIstekleriCevapla(handle);
}




bool TcpBaglantisi::alarmDurumunuDondur()
{
    return alarm;
}
