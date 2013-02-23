#include <iostream>
#include <GoruntuYoneticisi.h>
#include <GoruntuIsleme.h>
//#include <SeriPort.h>

#include <QApplication>

#include <QSound>
//#include <phonon/phonon>

#include <TcpBaglantisi.h>
#include <QHostAddress>
#include <SeriPrt.h>
#include <pthread.h>
#include <ParalelP.h>
#include <fstream>
#include <cstdlib>

using namespace std;
using namespace cv;

void kameradanIslet(Mat &rr);
void dosyadanIslet(const char *res_yol);
void ornekGoruntuOku(const char *res_yol);
void noktaBelirle(vector<Point> &n);
void refCizgileriniCiz(Mat &res, int vB, Point &p1p, Point &q1p, Point &p2p, Point &q2p);
void refNoktalariBul(Mat &res, Mat &backproj);
static void onMouse(int event, int x, int y, int, void *);
void arkaplanCikarimiMOG(Mat ornekRes);
void bitkiGoster(string b, Mat res);
void yonBilgisiniGonder(bool yon, int uzaklik, int orta_nok_uzakligi);
void goruntuOrtaNoktaHesapla(Point &oo);
void dogruHesaplaN(vector<Point> &nn, Point &n_o, bool b);
void goruntuyuKaydir(int ekran_kaymasi, Point &oo);
QString seriPortCikisiniFormatlandir(QString veri);

bool kaymaKontrol = false;

GoruntuYoneticisi gy;
GoruntuIsleme goruntuIsleme;
Mat ornekHist;
Mat bitki, ornekGoruntu, ornekGHist;
Mat histres;
Mat km;
vector<Point> im;
Mat blobRes;
int pS = 0;
bool cizdirme_aktif = false;
int thres, sinirBoyut;
int sat = 0, val = 0;

Mat arkaplanmog, onplanmog;
Mat mogBlob;

//Phonon::MediaObject *mediaObject;
TcpBaglantisi *tcpBaglantisi;

vector<Rect> rv; //Blob vektörleri.

//Seri Port.
SeriPort *seriPort;
int ks = 0;

/*Threadler(işlem parçacıkları)*/
pthread_t goruntuisleme_thread;
pthread_t seriport_thread;

void *goruntuisleme_t_fonksiyon(void *v);
void *seriport_t_fonksiyon(void *v);

static void paralelbgr(const Mat &res, Mat &sonuc);
static void paralelgolge_bul(const Mat &res, Mat &sonuc);
static void paralel_golgeSil(const Mat &res, Mat &sonuc);

int main(int argc, char *argv[])
{

   QApplication a(argc, argv);
   //mediaObject = Phonon::createPlayer(Phonon::NoCategory, Phonon::MediaSource("/home/eren/GuvenlikVidyo/alarm/alarm.wav"));
  // tcpBaglantisi = new TcpBaglantisi();
   //tcpBaglantisi->listen(QHostAddress::Any, 1234);
   seriPort = new SeriPort();
   seriPort->seriPortuAc(QString("/dev/ttyUSB0"));
   int t_sonuc1, t_sonuc2;
   t_sonuc1 = pthread_create(&goruntuisleme_thread, NULL, goruntuisleme_t_fonksiyon, NULL);
    //QSound::play("alarm/alarm.wav");
   // const char *res_yol1 = "/home/eren/bitki.jpg";
   // const char *res_yol2 = "/home/eren/ornek.jpg";
   // dosyadanIslet(res_yol1);
   // dosyadanIslet(res_yol2);
   // namedWindow("Sinirlar", CV_WINDOW_AUTOSIZE);
    //namedWindow("Seg", CV_WINDOW_AUTOSIZE);
   // ornekGoruntuOku(res_yol1);
    /*setMouseCallback("Sinirlar", onMouse, 0);
    createTrackbar("Esik Degeri:", "Sinirlar", &thres, 255, 0);
    createTrackbar("Maksimum Sinir:", "Sinirlar", &sinirBoyut, 3000, 0);
    createTrackbar("Saturation:", "Seg", &sat, 255, 0);
    createTrackbar("Val:", "Seg", &val, 255, 0);*/

  /*  SeriPort *seriPort = new SeriPort();
    char *port_adi = "/dev/ttyS0";
    seriPort->portBelirle(port_adi);
    bool b = seriPort->ac();
    if (b) cout << "Acildi" << endl;
    else cout << "Port acilamadi" << endl;*/
    VideoCapture capture;
    const string dosya_adi = "/home/eren/kayit.avi";
    capture.open(dosya_adi);

    //ofstream paralel;
  //  paralel.open("/home/eren/normal.txt");
   // paralel << "---Paralel İşleme---\n";

    int k = 0;
    while (true)
    {
        double gecen_sure;
        gecen_sure = static_cast<double>(getTickCount());
        //kameradanIslet();

        Mat tmp;
        capture >> tmp;
        kameradanIslet(tmp);
       // imshow("tttm", tmp);
        gecen_sure = (static_cast<double>(getTickCount()) - gecen_sure) * 1000.;
        gecen_sure /= getTickFrequency();
        cout << "Geçen Süre = " << gecen_sure << " ms" << endl;
        //paralel << gecen_sure << ",\n";
        //k++;
        char c = cvWaitKey(20);
        if (c == 27)
        {
            exit(1);
        }
    }
    return 0;
}

static void onMouse(int event, int x, int y, int, void*)
{

}


/*rr parametresi video'dan gelen görüntü karesidir. Daha sonra bu değiştirilip kameradan oku
 *metodu aktifleştirilecektir.
*/
void kameradanIslet(Mat &rr)
{
    //gy.vidyoDanOku();
   // Mat r = gy.kameradanOku();
    Mat r = rr;
    r.copyTo(ornekGoruntu);
    blur(ornekGoruntu, ornekGoruntu, Size(9,9));
   // goruntuIsleme.kmeansCluster(ornekGoruntu);
   // medianBlur(ornekGoruntu, ornekGoruntu, 9);
 //   cvSmooth(ornekGoruntu, ornekGoruntu, CV_MEDIAN);
   // GaussianBlur(ornekGoruntu, ornekGoruntu, Size(9,9), 0.1, 0.2);
   // blur(r, r, Size(3,3));
    //r.copyTo(km);
    /*Blob Çıkarımı.*/
    /*Mat tr;
    cvtColor(km, tr, CV_BGR2GRAY);
    blur(tr, tr, Size(5,5));
    threshold(tr, tr, thres, 255, CV_THRESH_BINARY);
    blobRes = goruntuIsleme.blobCikarimi(tr, 255, sinirBoyut);*/
    //Örnek histograma bağlı olarak arkaplan çıkarımı yap.
    //goruntuIsleme.arkaplanCikarimi(r, ornekHist, backproj);
    /*Mat h;
    ornekGHist = goruntuIsleme.histogramHesapla(km, h);*/

    //goruntuIsleme.histResDegisimi(ornekHist, h);

   // Mat tampon;
  //  km.copyTo(tampon);
    //goruntuIsleme.arkaplanCikarimi(tampon, ornekHist, backproj);


    //Histogram Eşitleme uygula.
   // Mat ss;
    //goruntuIsleme.histogramEsitleme(km, ss);

   /* char c = cvWaitKey(10);
    if (c == 27)
    {
        exit(1);
    }*/
    Mat h = Mat(ornekGoruntu.size(), CV_8UC1);
    paralelgolge_bul(ornekGoruntu, h);
   // goruntuIsleme.golgeBul(ornekGoruntu, h);
    Mat grayhist = Mat(50, 256, CV_8U, Scalar(255));
    int ix = goruntuIsleme.grayHistogram(h, grayhist);
    //imshow("gray hist", grayhist);
    //int th = goruntuIsleme.histogramlaEsikDegeriBul(h);
    cout << "Eşik Değeri: " << ix << endl;
    imshow("BGRR", h);
    threshold(h, h, ix / 2, 255, CV_THRESH_BINARY_INV);

 //   erode(h, h, Mat(), Point(-1,-1), 4);
    erode(h, h, Mat());
    dilate(h, h, Mat());
    dilate(h, h, Mat());
    imshow("BGR", h);
   Mat glg;
   r.copyTo(glg);
   paralel_golgeSil(h, glg);
 //  goruntuIsleme.golgeleriSil(h, glg);
   imshow("golge tamamen silindi", glg);
   Mat glgcikarim = Mat(ornekGoruntu.size(), CV_8UC1);
   paralelbgr(glg,glgcikarim);
  // goruntuIsleme.rgbUzerindenCikarim(glg, glgcikarim);
   erode(glgcikarim, glgcikarim, Mat(), Point(-1,-1), 2);
   dilate(glgcikarim, glgcikarim, Mat());
   blur(glgcikarim, glgcikarim, Size(3,3));
   imshow("Golge", glgcikarim);
   Mat grayhist_glg = Mat(50, 256, CV_8U, Scalar(255));
   int ix_glg = goruntuIsleme.grayHistogram(glgcikarim, grayhist_glg);

   Mat adaptive_cikarim = Mat(glgcikarim.size(), CV_8UC1);
   adaptiveThreshold(glgcikarim ,adaptive_cikarim, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 5, 3);
   bitwise_not(adaptive_cikarim, adaptive_cikarim);
   GaussianBlur(adaptive_cikarim, adaptive_cikarim, Size(3,3), 0);
  // dilate(glgcikarim, glgcikarim, Mat());
   Mat kernel = (Mat_<uchar>(3,3) <<  0, 1, 0, 1, 1, 1, 0, 1, 0);
   //erode(adaptive_cikarim, adaptive_cikarim, kernel, Point(-1, -1), 1);
//   dilate(adaptive_cikarim, adaptive_cikarim, kernel, Point(-1,-1), 1);

  /* Point maxPt;
   Point minPt;
   int max = -1;
   int min = -1;
   for (int i = 0; i < adaptive_cikarim.rows; i++)
   {
       uchar *p = adaptive_cikarim.ptr(i);
       for (int j = 0; j < adaptive_cikarim.cols; j++)
       {
           if (p[j] > 128)
           {
               int bolge = floodFill(adaptive_cikarim, Point(j, i), CV_RGB(0, 0, 64));
               if (bolge > max)
               {
                   maxPt = Point(j, i);
                   max = bolge;
               }
               if (bolge < 20)
               {
                   minPt = Point(j, i);
                   min = bolge;

               }

           }
       }
   }
   floodFill(adaptive_cikarim, minPt, CV_RGB(0,0,0));
   floodFill(adaptive_cikarim, maxPt, CV_RGB(0,0,255));

   cout << "Max ff: " << max << endl;*/


   //imshow("Adaptive Threshold", adaptive_cikarim);


   dilate(glgcikarim, glgcikarim, kernel);
   erode(glgcikarim, glgcikarim, kernel, Point(-1, -1), 1);
   threshold(glgcikarim, glgcikarim, 10, 255, CV_THRESH_BINARY);
   dilate(glgcikarim, glgcikarim, kernel);
   erode(glgcikarim, glgcikarim, kernel);
  // blur(glgcikarim, glgcikarim, Size(9,9));
   //GaussianBlur(glgcikarim, glgcikarim, Size(5,5), 1.5);
   //erode(glgcikarim, glgcikarim, Mat(), Point(-1,-1), 2);
   //dilate(glgcikarim, glgcikarim, Mat(), Point(-1,-1), 4);
   //threshold(glgcikarim, glgcikarim, 200, 255, CV_THRESH_BINARY);
 /*  blur(glgcikarim, glgcikarim, Size(5,5));
   erode(glgcikarim, glgcikarim, Mat(), Point(-1, -1), 3);
   imshow("Blur golge", glgcikarim);

   erode(glgcikarim, glgcikarim, Mat(), Point(-1, -1), 3);
   dilate(glgcikarim, glgcikarim, Mat());
   dilate(glgcikarim, glgcikarim, Mat(), Point(-1,-1), 2);

   threshold(glgcikarim, glgcikarim, 200, 255, CV_THRESH_BINARY);*/


   // bool b = goruntuIsleme.mogUzerindenBlobCikarimi(ornekGoruntu, h, 1000 , 50000, rv);

    //Bitkileri resimden çıkar.
//    Mat bitkiler = Mat(ornekGoruntu.size(), CV_8UC3, Scalar(255,255,255));
    /*if (rv.size() != 0)
    {
        for (int i = 0; i < rv.size(); i++)
        {
            Mat rvm = ornekGoruntu(rv.at(i));
            stringstream ss;
            ss << "Bitki: " << i+1;
            string s = ss.str();
            bitkiGoster(s, rvm);
            //addWeighted(rvm, 0.7, rvm, 0.1, 1.5, bitkiler);
            //add(rvm, rvm, bitkiler);
        }
        rv.clear();
    }*/
   // imshow("Bitkiler", bitkiler);

    refNoktalariBul(ornekGoruntu, glgcikarim);
    circle(ornekGoruntu, Point(320, 240), 10, Scalar(255,255,255), 3);
    imshow("BGR", h);
   // gy.goruntuEkle(ornekGoruntu);
     imshow("goruntu", ornekGoruntu);
     imshow("glg cikarimi", glgcikarim);


}

void dosyadanIslet(const char *res_yol)
{
   // namedWindow("histogram", CV_WINDOW_AUTOSIZE);
    Mat r = gy.dosyadanResimOku(res_yol);
    if (!r.empty())
        histres = goruntuIsleme.histogramHesapla(r, ornekHist);
   // imshow("histogram", histres);
    char c = cvWaitKey(10);
    if (c == 27)
    {
        exit(1);
    }

}

void ornekGoruntuOku(const char *res_yol)
{
    ornekGoruntu = gy.dosyadanResimOku(res_yol);
    Mat h = Mat(ornekGoruntu.size(), CV_8UC1);
    //cvtColor(ornekGoruntu, h, CV_BGR2GRAY);
   /* ornekGHist = goruntuIsleme.histogramHesapla(ornekGoruntu, h);
    Mat r;
    ornekGoruntu.copyTo(r);
    goruntuIsleme.arkaplanCikarimi(r, ornekHist, backproj);*/
    goruntuIsleme.rgbUzerindenCikarim(ornekGoruntu, h);
   // int th = goruntuIsleme.histogramlaEsikDegeriBul(h);
    //cout << "Eşik Değeri: " << th << endl;
    threshold(h, h, 100, 255, CV_THRESH_BINARY);
    erode(h, h, Mat());
    erode(h, h, Mat());
    dilate(h, h, Mat());
    dilate(h, h, Mat());
 //   bool b = goruntuIsleme.mogUzerindenBlobCikarimi(ornekGoruntu, h, 1000 , 50000, rv);

    //Bitkileri resimden çıkar.
//    Mat bitkiler = Mat(ornekGoruntu.size(), CV_8UC3, Scalar(255,255,255));
    for (int i = 0; i < rv.size(); i++)
    {
        Mat rvm = ornekGoruntu(rv.at(i));
        stringstream ss;
        ss << "Bitki: " << i+1;
        string s = ss.str();
        bitkiGoster(s, rvm);
        //addWeighted(rvm, 0.7, rvm, 0.1, 1.5, bitkiler);
        //add(rvm, rvm, bitkiler);
    }
    rv.clear();
   // imshow("Bitkiler", bitkiler);

    refNoktalariBul(ornekGoruntu, h);
    imshow("BGR", h);
    imshow("goruntu", ornekGoruntu);
    //imshow("ns", ns);
}

void noktaBelirle(vector<Point> &n)
{
    if (pS == 10)
    {
        im.clear();
        pS = 0;
    }

    pS++;
    int xO = 0, yO = 0;
    for (int i = 0; i < n.size(); i++)
    {
        Point p = n.at(i);
        xO += p.x;
        yO += p.y;
    }
    xO = xO / n.size();
    yO = yO / n.size();
    cout << "px: " << xO << "py: " << yO << endl;
    stringstream ss;
    ss << " " << xO << "," <<  yO;
    string s = ss.str();
    //putText(km, s, Point(xO, yO), 1, 0.8, Scalar(255,255,255));
    im.push_back(Point(xO, yO));
   // circle(km, Point(xO, yO), 5, Scalar(255,0,0), 5);
}

void refNoktalariBul(Mat &res, Mat &backproj)
{
    if (!backproj.empty())
    {
        vector<Point> n1, n2, n3, n4, n5;
        n1 = goruntuIsleme.ayristir(backproj, 0, 320, 0, 96);
        n2 = goruntuIsleme.ayristir(backproj, 0, 320, 96, 192);
        n3 = goruntuIsleme.ayristir(backproj, 0, 320, 192, 288);
        n4 = goruntuIsleme.ayristir(backproj, 0, 320, 288, 384);
        n5 = goruntuIsleme.ayristir(backproj, 0, 320, 384, 480);
        if (!n1.empty() && n1.size() > 10)
            noktaBelirle(n1);
        if (!n2.empty() && n2.size() > 10)
            noktaBelirle(n2);
        if (!n3.empty() && n3.size() > 10)
            noktaBelirle(n3);
        if (!n4.empty() && n4.size() > 10)
            noktaBelirle(n4);
        if (!n5.empty() && n5.size() > 10)
            noktaBelirle(n5);

        vector<Point> m1, m2, m3, m4, m5;
        m1 = goruntuIsleme.ayristir(backproj, 320, 640, 0, 96);
        m2 = goruntuIsleme.ayristir(backproj, 320, 640, 96, 192);
        m3 = goruntuIsleme.ayristir(backproj, 320, 640, 192, 288);
        m4 = goruntuIsleme.ayristir(backproj, 320, 640, 288, 384);
        m5 = goruntuIsleme.ayristir(backproj, 320, 640, 384, 480);
        if (!m1.empty() && m1.size() > 10)
            noktaBelirle(m1);
        if (!m2.empty() && m2.size() > 10)
            noktaBelirle(m2);
        if (!m3.empty() && m3.size() > 10)
            noktaBelirle(m3);
        if (!m4.empty() && m4.size() > 10)
            noktaBelirle(m4);
        if (!m5.empty() && m5.size() > 10)
            noktaBelirle(m5);
        int imBoyut = im.size();
        if (imBoyut == 10)
        {
            refCizgileriniCiz(res,10, im.at(4), im.at(9), im.at(0), im.at(5));
        }
        else if (imBoyut == 8)
        {
            refCizgileriniCiz(res, 8, im.at(4), im.at(7), im.at(0), im.at(3));
        }
        else if (imBoyut == 6)
        {
            refCizgileriniCiz(res, 6, im.at(0), im.at(2), im.at(3), im.at(5));
        }
        else if (imBoyut == 4)
        {
            refCizgileriniCiz(res, 4, im.at(0), im.at(1), im.at(2), im.at(3));
        }
    }
}

void refCizgileriniCiz(Mat &res, int vB, Point &p1p, Point &q1p, Point &p2p, Point &q2p)
{
   /* for (int i = 0; i < (vB-1) / 2; i++)
    {
        line(km, im.at(i), im.at(i + 1), Scalar(0,0,255), 2);
    }

    for (int i = (vB-1) / 2; i < vB -1; i++)
    {
        line(km, im.at(i), im.at(i + 1), Scalar(0,0,255), 2);
    }*/

    for (size_t i = 0; i < im.size(); i++)
    {
        circle(res, im.at(i), 5, Scalar(255,0,0), 5);

        stringstream ss;
        ss << " " << "V" <<  i;
        string s = ss.str();
        putText(res, s, im.at(i), 1, 0.8, Scalar(0,0,0));
    }
   // int ortx, orty;
    for (int i = 0; i < im.size() / 2 -1; i++)
    {
        line(res, im.at(i), im.at(i+1), Scalar(255,255,255));
    }

    for (int i = im.size() / 2; i < im.size() - 1; i++)
    {
        line(res, im.at(i), im.at(i+1), Scalar(255,255,255));
    }

    /*n1 ve m1 doğrularını bul.*/
    Point n1p;
    dogruHesaplaN(im, n1p, true);
    line(ornekGoruntu, im.at(4), n1p, Scalar(0, 0, 255));

    Point m1p;
    dogruHesaplaN(im, m1p, false);
    line(ornekGoruntu, im.at(9), m1p, Scalar(0, 0, 255));


    //line(res, p1p, p2p, Scalar(0,0,255));
   // line(res, q1p  ,q2p, Scalar(0,0,255));

    Point p1 = im.at(4), q1 = im.at(9);
    p1 = Point(abs(p1.x - q1.x) / 2 + p1.x, abs(p1.y - q1.y) / 2 + p1.y);
   // circle(ornekGoruntu, p1, 3, Scalar(255,255,255), 2);

    Point p2 = n1p, q2 = m1p;
    p2 = Point(abs(p2.x - q2.x) / 2 + p2.x, abs(p2.y - q2.y) / 2 + p2.y);
    //line(res, p1, q1, Scalar(0,0,255), 3);
    //line(res, p2, q2, Scalar(0,0,255),3);
    //line(res, p1, Point(res.cols / 2, res.rows / 2), Scalar(255,0,0));
    Point orta;
    Point o1 = im.at(4);
    Point o2 = im.at(9);
    orta.x = abs(o2.x - o1.x) / 2 + o1.x;
    orta.y = abs(o2.y - o2.y) / 2 + o1.y - 50;
  //  orta.x =
    circle(res, orta, 5, Scalar(0, 0, 255), 5);

    //line(ornekGoruntu, p1, p2, Scalar(255,255,255));
    double aci = atan2((double) p1.y - p2.y, (double) p1.x - p2.x);
    double hip = sqrt((p1.y - p2.y) * (p1.y - p2.y) + (p1.x - p2.x) * (p1.x - p2.x));

    stringstream ss;
    ss << " " << "tan(alfa): " <<  tan(aci);
    string s = ss.str();
    putText(res, s, Point(10,10), 1, 0.8, Scalar(0,0,0));
    stringstream sss;
    sss << " " << "hip: " << hip;
    s = sss.str();
    putText(res, s, Point(10,20), 1, 0.8, Scalar(0,0,0));

    p2.x = (int) (p1.x  - 1 * hip * cos(aci));
    p2.y = (int) (p1.y - 1 * hip * sin(aci));
    line(res, p1, p2, Scalar(255,255,255), 2);
    Point o_orta;
    o_orta.x = abs(p1.x - p2.x) / 2 + p1.x;
    o_orta.y = abs(p1.y - p2.y) / 2 + p2.y;
    circle(res, o_orta, 10, Scalar(255, 0, 0));

    /*320-240 ve o_orta arasındaki fark*/
    Point orta_nokta_farki = o_orta;
    goruntuOrtaNoktaHesapla(orta_nokta_farki);


    //Yön doğrusuna uzaklığı çiz.
    Point fark;
    fark.x = abs(o_orta.x - orta.x);
    fark.y = abs(o_orta.y - orta.y);
  //  line(res, Point(orta.x, orta.y), fark, Scalar(255,255,0), 2);
    //line(res, fark, p1, Scalar(0,0,255), 2);
    cout << "Fark = " << fark.x << endl;
    if (tan(aci) > 0) //sola dön.
    {
        //yonBilgisiniGonder(true, fark.x, abs(orta.x - o1.x));
    }
    else if (tan(aci) < 0) //sağa dön.
    {
        //yonBilgisiniGonder(false, fark.x, abs(orta.x - o2.x));
    }
    p1.x = (int) (p2.x  + 9 * cos(aci + CV_PI / 4));
    p1.y = (int) (p2.y + 9 * sin(aci + CV_PI / 4));
    line(res, p1, p2, Scalar(0,0,255), 2);

    p1.x = (int) (p2.x  + 9 * cos(aci - CV_PI / 4));
    p1.y = (int) (p2.y + 9 * sin(aci - CV_PI / 4));
    line(res, p1, p2, Scalar(0,0,255), 2);
}

/*void arkaplanCikarimiMOG(Mat ornekRes)
{

    bool alarmDurumu = tcpBaglantisi->alarmDurumunuDondur();
    if (!ornekRes.empty())
    {
        goruntuIsleme.arkplanCikarimimog(ornekRes, arkaplanmog, onplanmog);
        ornekRes.copyTo(mogBlob);
        bool d =  goruntuIsleme.mogUzerindenBlobCikarimi(mogBlob, onplanmog, 100, 50000, rv);
        if (d)
        {
            //mediaObject->play();
        }
        else if (!alarmDurumu)
        {
            //mediaObject->pause();
        }
    }
}*/

void bitkiGoster(string b, Mat res)
{
    imshow(b, res);
}

#define PIKSEL_ESIK 20
#define YUZDE_ESIK 3

void yonBilgisiniGonder(bool yon, int uzaklik, int orta_nok_uzakligi)
{
    //bool b = false;
    cout << "Orta Nokta Uzakligi  = " << orta_nok_uzakligi << endl;
    string s;
    Scalar scalar;
    double olcek = .9;
    float yuzde = 0.0;

    if (uzaklik > PIKSEL_ESIK)  //piksel olarak eşik değeri.
    {
        if (yon)    //sol.
        {
            scalar = Scalar(255, 255, 255);
            olcek = 1;
            s = "SOLA KAYDIR!";
            cout << "Sola Kay.." << endl;
            yuzde = (100 * uzaklik) / orta_nok_uzakligi;
        }
        else if (!yon) //sağ.
        {
            scalar = Scalar(0, 0, 255);
            s = "SAGA KAYDIR!";
            cout << "Sağa Kay.." << endl;
            olcek = .9;
            yuzde = (100 * uzaklik) / orta_nok_uzakligi;
        }
    }
    if (!kaymaKontrol || yuzde < YUZDE_ESIK)
    {
        scalar = Scalar(0, 255, 255);
        s = "HAREKET YOK!";
        cout << "Hareket yok." << endl;
    }
    putText(ornekGoruntu, s, Point(250, 470), FONT_HERSHEY_COMPLEX, olcek, scalar);

    cout << "Yüzde = " << yuzde << endl;
    QString gonderilecek_bilgi;


    if (seriPort->kontrolDegeriniDondur() == 0)
    {
        if (yon)
            gonderilecek_bilgi.append(QString("b"));
        else if (!yon)
            gonderilecek_bilgi.append(QString("a"));
        gonderilecek_bilgi.append(QString::number(yuzde));
    }
    else if (seriPort->kontrolDegeriniDondur() == 1) //a
    {
        gonderilecek_bilgi.append(QString("b"));
        yuzde = 50.0;
        gonderilecek_bilgi.append(QString::number(yuzde));
    }
    else if (seriPort->kontrolDegeriniDondur() == 2) //b
    {
        gonderilecek_bilgi.append(QString("a"));
        yuzde = 50.0;
        gonderilecek_bilgi.append(QString::number(yuzde));
    }

    if (yuzde > YUZDE_ESIK && kaymaKontrol) //yüzde olarak eşik değeri.
    {
        gonderilecek_bilgi = seriPortCikisiniFormatlandir(gonderilecek_bilgi);
        gonderilecek_bilgi.append(QString("\n\r"));
        qDebug() << "Gonderilen bilgi: " << gonderilecek_bilgi;
        seriPort->seriPortaYaz(gonderilecek_bilgi);
    }
    else if (yuzde <= YUZDE_ESIK || !kaymaKontrol)
    {
        seriPort->seriPortaYaz(QString("xc"));
    }

    seriPort->kontrolDegeriniBelirle(0);

    /*if (ks > 1)
    {
        if ((gonderilecek_bilgi.indexOf("a") && !seriPort->islemKontrol()) ||
                (gonderilecek_bilgi.indexOf("b") && seriPort->islemKontrol()))
        {


        }
    }
    else if (ks == 0)
    {
        if (yuzde > 5)
        {
            qDebug() << "Gonderilen bilgi: " << gonderilecek_bilgi;
            seriPort->seriPortaYaz(gonderilecek_bilgi);
        }
    }
    ks++;*/
    gonderilecek_bilgi.clear();
}

int ekranKaymasi = 0;
void goruntuOrtaNoktaHesapla(Point &oo)
{
    line(ornekGoruntu, Point(320, 240), oo, Scalar(255,255,255), 2, 3);
    if (oo.x > 320) //sağ tarafta.
    {
        oo.x = oo.x - 320;
        ekranKaymasi = 1;
    }
    else if (oo.x < 320) // sol tarafta.
    {
        oo.x = 320 - oo.x;
        ekranKaymasi = 2;
    }
    else if (oo.x == 320)
    {
        ekranKaymasi = 0;
        oo.x = 0;
    }
    goruntuyuKaydir(ekranKaymasi, oo);
}

void goruntuyuKaydir(int ekran_kaymasi, Point &oo)
{
    Point o1, o2;
    o1 = im.at(4);
    o2 = im.at(9);
    qDebug() << "Kayma degeri : " << oo.x;
    bool yonBilgisi = false;
    Point fark_o;
    fark_o.x = 0;
   // if (oo.x > 20)
   // {
        if (ekran_kaymasi == 0)
        {
            kaymaKontrol = false;

        }
        else if (ekran_kaymasi == 1)
        {
            kaymaKontrol = true;
            yonBilgisi = false;
            yonBilgisiniGonder(yonBilgisi, oo.x, abs(320 - o2.x));
        }
        else if (ekran_kaymasi == 2)
        {
            kaymaKontrol = true;
            yonBilgisi = true;
            yonBilgisiniGonder(yonBilgisi, oo.x, abs(320 - o1.x));
        }
    //}

    else
    {
        kaymaKontrol = false;
    }
}

void dogruHesaplaN(vector<Point> &nn, Point &n_o, bool b)
{
    if (b)
    {
        Point en_buyuk = nn.at(0), en_kucuk = nn.at(0);
        for (size_t i = 1; i < nn.size() / 2; i++)
        {
            Point tampon = nn.at(i);
            if (en_buyuk.x < tampon.x)
            {
                en_buyuk.x = tampon.x;
            }
            if (en_kucuk.x > tampon.x)
            {
                en_kucuk.x = tampon.x;
            }
        }
        //circle(ornekGoruntu, en_buyuk, 2, Scalar(255, 255, 255), 4);
       // circle(ornekGoruntu, en_kucuk, 2, Scalar(0, 255, 0), 4);
        Point fark;
        fark.x = abs(en_buyuk.x - en_kucuk.x);
        fark.y = abs(en_buyuk.y - en_kucuk.y);

        n_o.x = fark.x / 2 + en_kucuk.x;
        n_o.y = fark.y / 2 + en_kucuk.y;
        circle(ornekGoruntu, n_o, 2, Scalar(0, 0, 255), 4);
    }
    else
    {
        Point en_buyuk = nn.at(5), en_kucuk = nn.at(5);
        for (size_t i = nn.size() / 2; i < nn.size(); i++)
        {
            Point tampon = nn.at(i);
            if (en_buyuk.x < tampon.x)
            {
                en_buyuk.x = tampon.x;
            }
            if (en_kucuk.x > tampon.x)
            {
                en_kucuk.x = tampon.x;
            }
        }
        //circle(ornekGoruntu, en_buyuk, 2, Scalar(255, 255, 255), 4);
       // circle(ornekGoruntu, en_kucuk, 2, Scalar(0, 255, 0), 4);
        Point fark;
        fark.x = abs(en_buyuk.x - en_kucuk.x);
        fark.y = abs(en_buyuk.y - en_kucuk.y);

        n_o.x = fark.x / 2 + en_kucuk.x;
        n_o.y = fark.y / 2 + en_kucuk.y;
        circle(ornekGoruntu, n_o, 2, Scalar(0, 0, 255), 4);
    }
}

QString seriPortCikisiniFormatlandir(QString veri)
{
    if (veri.size() == 2)
    {
        QString c = veri.at(1);
        QString d = veri.at(0);
        c.insert(0, QString("0"));
        d.append(c);
        d.insert(0, QString("x"));
        return d;
    }
    else if (veri.size() == 3)
    {
        veri.insert(0, QString("x"));
        return veri;
    }
    return veri;
}

void *goruntuisleme_t_fonksiyon(void *v)
{
}

void *seriport_t_fonksiyon(void *v)
{

}

static void paralelbgr(const Mat &res, Mat &sonuc)
{
    bgrHesapla_ParalelP body(res, sonuc);
    Range t = Range(0, res.total());
    parallel_for_(t, body, 4);
}

static void paralelgolge_bul(const Mat &res, Mat &sonuc)
{
    GolgeBul_ParalelP body(res, sonuc);
    Range t = Range(0, res.total());
    parallel_for_(t, body, 4);
}

static void paralel_golgeSil(const Mat &res, Mat &sonuc)
{
    GolgeSil_ParalelP body(res, sonuc);
    Range t = Range(0, res.total());
    parallel_for_(t, body, 4);
}
