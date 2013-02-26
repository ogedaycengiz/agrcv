#ifndef PARALELP_H
#define PARALELP_H

#include <opencv2/opencv.hpp>

//using namespace cv;

class ParallelComputing : public cv::ParallelLoopBody
{
public:
    ParallelComputing(const cv::Mat &img, cv::Mat &result);
    ParallelComputing(const cv::Mat &img, cv::Scalar &scalar);
    virtual void operator() (const cv::Range &range);
    virtual void operator= (const ParallelComputing &parallelComputing);
    virtual cv::Mat getResult() const;
protected:
    void compute(int nstripes = -1);
private:
    int getCpuCoreCount() const;
    cv::Mat inputImage, outputImage;
};

template<typename T>
class Compute : public ParallelComputing
{
public:
    Compute();
    virtual cv::Mat getResult() const {return cv::Mat();}
protected:
    void compute(int nstripes);
private:
    cv::Mat result;
};

class bgrHesapla_ParalelP : public cv::ParallelLoopBody
{
public:
    bgrHesapla_ParalelP(const cv::Mat &_girdi, cv::Mat &_sonuc)
    {
        girdi = _girdi;
        sonuc = _sonuc;
    }

    virtual void operator() (const cv::Range &aralik) const
    {
        uchar *hedef = sonuc.data + aralik.start;
        uchar *kaynak = girdi.data + aralik.start * 3;

        int pikselSayisi = aralik.size();
        //cout << "Piksel Sayisi ::" << pikselSayisi << endl;
        for (int i = 0; i < pikselSayisi; i++, kaynak+=3)
        {
            uchar b = kaynak[0];
            uchar g = kaynak[1];
            uchar r = kaynak[2];

            uchar gl = saturate_cast<uchar>(2 * g - r - b);
            *hedef = gl;
            hedef++;
        }
    }

private:
    cv::Mat girdi;
    cv::Mat sonuc;
};

struct GolgeBul_ParalelP : public cv::ParallelLoopBody
{
    GolgeBul_ParalelP(const cv::Mat &_girdi, cv::Mat &_sonuc) :
        girdi(_girdi), sonuc(_sonuc)
    {
    }

    virtual void operator() (const cv::Range &aralik) const
    {
        uchar *hedef = sonuc.data + aralik.start;
        uchar *kaynak = girdi.data + aralik.start * 3;

        int pikselSayisi = aralik.size();
        for (int i = 0; i < pikselSayisi; i++, kaynak += 3)
        {
            uchar b = kaynak[0];
            uchar g = kaynak[1];
            uchar r = kaynak[2];

            uchar gl = saturate_cast<uchar>(3 * g - r - b);
            uchar nl = saturate_cast<uchar>(2 * g - r - b);
            *hedef = gl - nl;
            hedef++;
        }
    }
private:
    cv::Mat girdi;
    cv::Mat sonuc;
};

class GolgeSil_ParalelP : public cv::ParallelLoopBody
{
public:
    GolgeSil_ParalelP(const cv::Mat &_girdi, cv::Mat &_sonuc) :
        girdi(_girdi), sonuc(_sonuc)
    {

    }
    virtual void operator() (const cv::Range &aralik) const
    {
        int ort_golge_disi_r = 0;
        int ort_golge_disi_g = 0;
        int ort_golge_disi_b = 0;

        int ort_golge_ici_r = 0;
        int ort_golge_ici_g = 0;
        int ort_golge_ici_b = 0;

        int ort_golge_ici_sayac = 0;
        int ort_golge_disi_sayac = 0;
        uchar *hedef = sonuc.data + aralik.start * 3;
        uchar *kaynak = girdi.data + aralik.start;

        int pikselSayisi = aralik.size();
        for (int i = 0; i < pikselSayisi; i++, hedef += 3)
        {
            if (*kaynak < 200)
            {
                int p1 = (int)hedef[0];
                int p2 = (int)hedef[1];
                int p3 = (int)hedef[2];

                ort_golge_ici_b += p1;
                ort_golge_ici_g += p2;
                ort_golge_ici_r += p3;

                ort_golge_ici_sayac++;
            }
            else if (*kaynak > 200)
            {
                int p1 = (int)hedef[0];
                int p2 = (int)hedef[1];
                int p3 = (int)hedef[2];

                ort_golge_disi_b += p1;
                ort_golge_disi_g += p2;
                ort_golge_disi_r += p3;

                ort_golge_disi_sayac++;
            }
            kaynak++;
        }
        ort_golge_ici_b = (ort_golge_ici_b / ort_golge_ici_sayac);
        ort_golge_ici_g = (ort_golge_ici_g / ort_golge_ici_sayac);
        ort_golge_ici_r = (ort_golge_ici_r / ort_golge_ici_sayac);



        ort_golge_disi_b = (ort_golge_disi_b / ort_golge_disi_sayac);
        ort_golge_disi_g = (ort_golge_disi_g / ort_golge_disi_sayac);
        ort_golge_disi_r = (ort_golge_disi_r / ort_golge_disi_sayac);


        int c_b = ort_golge_disi_b - ort_golge_ici_b;
        int c_g = ort_golge_disi_g - ort_golge_ici_g;
        int c_r = ort_golge_disi_r - ort_golge_ici_r;

        int c = ((ort_golge_disi_b * ort_golge_ici_b) + (ort_golge_disi_g * ort_golge_ici_g) + (ort_golge_disi_r * ort_golge_ici_r))
                /
                ((ort_golge_ici_b * ort_golge_ici_b) + (ort_golge_ici_g * ort_golge_ici_g) + (ort_golge_ici_r * ort_golge_ici_r));

        /*Kaynak ve hedef göstericilerini başlangıç konumuna çek.
         */
        kaynak = girdi.data + aralik.start;
        hedef = sonuc.data + aralik.start * 3;

        for (int i = 0; i < pikselSayisi; i++, hedef += 3)
        {
            if (*kaynak < 200)
            {
                hedef[0] += (uchar) c_b/2 +c;
                hedef[1] += (uchar) c_g/2 +c;
                hedef[2] += (uchar) c_r/2 +c;
            }
            kaynak++;
        }

    }
private:
    cv::Mat girdi;
    cv::Mat sonuc;

    cv::Scalar ort_golge_disi;
    cv::Scalar ort_golge_ici;

    void topla(cv::Scalar &n, int b, int g, int r)
    {
        n.val[0] += b;
        n.val[1] += g;
        n.val[2] += r;
    }
    void bol(cv::Scalar &n, int sayac_degeri)
    {
        n.val[0] /= sayac_degeri;
    }
};

#endif // PARALELP_H
