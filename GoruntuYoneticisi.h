#ifndef GORUNTUYONETICISI_H
#define GORUNTUYONETICISI_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <sstream>

using namespace cv;
using namespace std;


//Bu sınıf thread olacak.
class GoruntuYoneticisi
{
public:
    GoruntuYoneticisi();
    Mat dosyadanResimOku(const char *resim_yolu);
    Mat kameradanOku();
    void kamerayiKapat();
    void goruntuEkle(Mat res);
    void vidyoDanOku();
    void kamerayiAc(int id);
private:
    VideoCapture capture;
    VideoWriter writer;
    bool kameraDurumu;
    void kameraAyarlariniYap();
};

#endif // GORUNTUYONETICISI_H
