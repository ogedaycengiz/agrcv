#ifndef SERIPRT_H
#define SERIPRT_H


#include <QString>
#include <qextserialport.h>
#include <QByteArray>
#include <QThread>
#include <QTimer>

class SeriPort : public QObject
{
    Q_OBJECT
public:
    SeriPort(QObject *parent = 0);
    void seriPortuAc(QString port_adi);
    void seriPortuKapat();
    void seriPortaYaz(QString deger);
    void run();
    bool islemKontrol();
    void kontrolDegeriniBelirle(int k);
    int kontrolDegeriniDondur();
    bool seriPortDurumDondur();
private:
    PortSettings port_ayarlari;
    void portAyarlariniYap();
   // QString girdi;
    QextSerialPort *seriPort;
    bool gelen_bilgi_okundu;
    QTimer *timer;
    bool kontrol;
    int kk;
signals:
    void seriPorttanBilgiOkundu(QByteArray &bilgi);
private slots:
    void seriporttanOku();
    void gelenBilgiyiOku(QByteArray &b);
};

#endif // SERIPRT_H
