#ifndef TCPBAGLANTISI_H
#define TCPBAGLANTISI_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>

#include <QtDebug>

class TcpBaglantisi : public QTcpServer
{
    Q_OBJECT
public:
    TcpBaglantisi();
    bool alarmDurumunuDondur();
private:
    void gelenIstekleriCevapla(int handle);
    bool alarm;
protected:
    void incomingConnection(int handle);
signals:
    void alarmKontrol(bool b);
};

#endif // TCPBAGLANTISI_H
