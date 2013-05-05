#ifndef GORUNTUISLEME_H
#define GORUNTUISLEME_H

#include <opencv2/opencv.hpp>
//#include <BlobResult.h>
//#include <blob.h>
#include <iostream>

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv/cxcore.h>

#include "cvblob/cvBlob/cvblob.h"


using namespace cv;
using namespace std;
using namespace cvb;

class GoruntuIsleme
{
public:
    GoruntuIsleme();
    Mat histogramHesapla(Mat &res, Mat &histRes);
    void arkaplanCikarimi(Mat &res, Mat &ornekHist, Mat &arkRes);
    vector<Point> ayristir(cv::Mat &res, int sX1, int sX2, int sY1, int sY2);
    void satValFiltreUygula(Mat &res, Mat &hsv, int saturasyon, int val);
    Mat histogramGuncelle(Mat &res, Rect &roi);
    void histResDegisimi(Mat &res, Mat &hist);
    Mat blobCikarimi(Mat &res, int param1, int param2);
    void arkplanCikarimimog(Mat &res, Mat &arkaplan, Mat &onplan);
    bool mogUzerindenBlobCikarimi(Mat res, Mat &onplanBlob, int min_area, int max_area, vector<Rect> &rv);
    void histogramEsitleme(Mat &res, Mat &sonuc);
    void rgbUzerindenCikarim(Mat res, Mat &sonuc);
    void grabCutUygula(Mat res, Mat &sonuc, Rect r);
    int histogramlaEsikDegeriBul(Mat &res);
    void satBelirle(int sat);
    void valBelirle(int val);
    int grayHistogram(Mat &res, Mat &hist_res);
    void golgeleriSil(Mat &res, Mat &sonuc);
    void golgeBul(Mat res, Mat &sonuc);
    void kmeansCluster(Mat res);
private:
    void esiklemeYap(cv::Mat &res, int esik_degeri, int deger);
    cv::Mat siyahBeyazYap(cv::Mat &res);
    void minSaturationUygula(Mat &res, Mat &maske,  int minSat);

    Mat hsv, hue, maske, hist, backproj;
    int vmin;
    int vmax;
    int smin;
    int histogramBoyutu;
    float h_araligi[2];
    const float *h_araligiGtr;
    double histMinDeger;
    double histMaksDeger;
   // Point merkez;
    //CBlobResult blobs;
    //CBlob *guncelBlob;

    /*Arkpalan çıkarımı ==> mog.*/
    BackgroundSubtractorMOG2 *mog;
    CvTracks tracks;
    CvPoint hedefMerkezi;
    IplImage matIpl;
    IplImage mogRes;

    int ort_golge_disi_r;
    int ort_golge_disi_g;
    int ort_golge_disi_b;

    int ort_golge_ici_r;
    int ort_golge_ici_g;
    int ort_golge_ici_b;

    int ort_golge_ici_sayac;
    int ort_golge_disi_sayac;

    template <typename T> void ortPikselDegerleriniDondur(Mat &mat);

    int kumeSayisi;
    int ornekSayisi;

};

#endif // GORUNTUISLEME_H
