#ifndef VIDEOSTAB_H
#define VIDEOSTAB_H

#include <opencv2/opencv.hpp>

class OzellikTakibi
{
public:
    OzellikTakibi(int max_ozellik, double qlevel, double min_mesafe);
private:
    cv::Mat gri;
    cv::Mat onceki_gri;

    std::vector<cv::Point2f> noktalar[2];
    std::vector<cv::Point2f> ilk_noktalar;
    std::vector<cv::Point2f> ozellikler;

    int m_max_ozellik;
    double m_qlevel;
    double m_min_mesafe;

    std::vector<uchar> durum;
    std::vector<float> hata;
};

class VideoStab
{
public:
    VideoStab();
private:

};

#endif // VIDEOSTAB_H
