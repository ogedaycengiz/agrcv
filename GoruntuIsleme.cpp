#include <GoruntuIsleme.h>

GoruntuIsleme::GoruntuIsleme()
{

    histogramBoyutu = 180;
    h_araligi[0] = 0;
    h_araligi[1] = 180;
    h_araligiGtr = h_araligi;
    vmin = 100; vmax = 256; smin = 100;
    mog = new BackgroundSubtractorMOG2();
    ort_golge_ici_sayac = 0;
    ort_golge_disi_sayac = 0;

    /*Kmeans clustering algoritması parametreleri*/
    ornekSayisi = 400;
    kumeSayisi = 5;
    kumeSayisi = MIN(ornekSayisi, kumeSayisi);
}


Mat GoruntuIsleme::histogramHesapla(Mat &res,  Mat &histRes )
{
    cvtColor(res, hsv, CV_BGR2HSV);
    int kanal[] = {0,0};
    hue.create(hsv.size(), hsv.depth());
    mixChannels(&hsv, 1, &hue, 1, kanal, 1);
   // Mat roi(hue, roiBolgesi), roiMaske(maske, roiBolgesi);
    minSaturationUygula(hsv, maske, 128);
    calcHist(&hue, 1, 0, maske, histRes, 1, &histogramBoyutu, &h_araligiGtr);
    minMaxLoc(histRes, &histMinDeger, &histMaksDeger, 0,0);
    normalize(histRes, histRes, histMinDeger, histMaksDeger, CV_MINMAX);
    Mat histogramRes = Mat::zeros(200,320, CV_8UC3);
    histogramRes = Scalar::all(0);
    int binGenisligi = histogramRes.cols / histogramBoyutu;
    Mat tampon(1, histogramBoyutu, CV_8UC3);
    for (int i = 0; i < histogramBoyutu; i++)
    {
        tampon.at<Vec3b>(i) = Vec3b(saturate_cast<uchar>(i * 180. / histogramBoyutu), 255, 255);
    }
    cvtColor(tampon, tampon, CV_HSV2BGR);
    for (int i = 0; i < histogramBoyutu; i++)
    {
        int val = saturate_cast<int>(histRes.at<float>(i) * histogramRes.rows / 255);
        //cout << val << endl;
        rectangle(histogramRes, Point(i * binGenisligi, histogramRes.rows)
                  , Point((i+1) * binGenisligi, histogramRes.rows - val),
                  Scalar(tampon.at<Vec3b>(i)), -1);
        int d = saturate_cast<int>(histRes.at<float>(i));
        stringstream ss;
        ss << d;
        string s = ss.str();
        putText(histogramRes, s, Point(i * binGenisligi, histogramRes.rows), 1, 0.8, Scalar(255,255,255));
    }
    return histogramRes;
}


void GoruntuIsleme::arkaplanCikarimi(Mat &res, Mat &ornekHist, Mat &arkRes)
{
    /*Alçak geçiren filtre uygula.*/
//Kod buraya gelecek. Saturation ve Value kanalları üzerinde uygulanacak.
    cvtColor(res, res, CV_BGR2HSV);
    inRange(hsv, Scalar(0, smin, MIN(vmin, vmax))
            , Scalar(180, 256, MAX(vmin, vmax)), maske);
    int kanal[] = {0,0};
    hue.create(res.size(), res.depth());
    mixChannels(&res, 1, &hue, 1, kanal, 1);
    calcBackProject(&hue, 1, 0, ornekHist, arkRes, &h_araligiGtr, 180.0);
    arkRes &= maske;
    //dilate(arkRes, arkRes, Mat());
   // satValFiltreUygula(arkRes,arkRes, 20, 30);
    esiklemeYap(arkRes, 128, 255);
    satValFiltreUygula(arkRes,res, 128, 255);
    erode(arkRes, arkRes, Mat(), Point(1,1), 3);

}


vector<Point> GoruntuIsleme::ayristir(Mat &res, int sX1, int sX2, int sY1, int sY2)
{
    //Hiç beyaz piksel yoksa vektör'ün out of range olmasını engeller(bias).
    rectangle(res, Rect(sX1, sY1, sX1 + sX2, sY1 + sY2), Scalar(255,255, 255), 1, 4);
    vector<Point> noktalar;
    uchar *p = NULL;
    int i = 0;
    for (int y = sY1; y < sY2; y++)
    {
        p = res.ptr<uchar>(y);
        for (int x = sX1; x < sX2; x++)
        {
            if (p[x] == 255)
            {
                i++;
                noktalar.push_back(Point(x,y));
                //cout << p[x] << endl;
            }

        }
    }
    return noktalar;
}


void GoruntuIsleme::esiklemeYap(Mat &res, int esik_degeri, int deger)
{
    threshold(res, res, esik_degeri, deger, CV_THRESH_BINARY);
}


Mat GoruntuIsleme::siyahBeyazYap(Mat &res)
{
    Mat sb;
    cvtColor(res, sb, CV_BGR2GRAY);
    return sb;
}


void GoruntuIsleme::satValFiltreUygula(Mat &res, Mat &hsv, int saturasyon, int val)
{
    vector<Mat> v;
    split(hsv, v);
    esiklemeYap(v[1], saturasyon, val);
    bitwise_and(res, v[1], res);
}


void GoruntuIsleme::minSaturationUygula(Mat &res, Mat &maske, int minSat)
{
    if (minSat > 0)
    {

        vector<Mat> v;
        split(res, v);
        threshold(v[1], maske, minSat, 255, CV_THRESH_BINARY);
        //En az değerli kesim değerlerini süzgeçten geçir(filtrele).

    }
}


Mat GoruntuIsleme::histogramGuncelle(Mat &res, Rect &roi)
{
    Mat yeniHist;
    Mat hist;
    Mat histR = res(roi);
    yeniHist = histogramHesapla(histR, hist);
    return yeniHist;
}


void GoruntuIsleme::histResDegisimi(Mat &res, Mat &hist)
{
 //   cout << res.rows << endl;
    if (res.cols == hist.cols && res.rows == hist.rows)
    {
        uchar *p, *q;
        for (int y = 0; y < res.cols; y++)
        {
            p = res.ptr<uchar>(y);
            q = hist.ptr<uchar>(y);
            for (int x = 0; x < res.rows; x++)
            {
               // cout << "hist = " << x << " deger = " << saturate_cast<int>((saturate_cast<float>(q[x]) * 255) / histMaksDeger) << endl;
                p[x] = saturate_cast<int>((saturate_cast<float>(q[x]) * 180) / histMaksDeger);
            }
        }
    }
}


Mat GoruntuIsleme::blobCikarimi(Mat &res, int param1, int param2)
{
    //Mat'ten IplImage'e dönüşüm yap.
    IplImage blobRes = res;
    IplImage *blobSon = 0;
    blobSon = cvCreateImage(cvGetSize(&blobRes), IPL_DEPTH_8U, 3);
    blobs = CBlobResult(&blobRes, NULL, param1);
    blobs.Filter(blobs, B_INCLUDE, CBlobGetArea(), B_GREATER, param2);
    cvMerge(&blobRes, &blobRes, &blobRes, NULL, blobSon);
    for (int i = 0; i < blobs.GetNumBlobs(); i++)
    {
        guncelBlob = blobs.GetBlob(i);
        guncelBlob->FillBlob(blobSon, CV_RGB(255,0,0));
    }
    //IplImage'den Mat'e dönüşüm yap.
    Mat bMat = cvarrToMat(blobSon).clone();
    cvReleaseImage(&blobSon);
    return bMat;
}


void GoruntuIsleme::arkplanCikarimimog(Mat &res, Mat &arkaplan, Mat &onplan)
{
    Mat cekirdek(7, 3, CV_8U);
    mog->operator ()(res, onplan);
    mog->getBackgroundImage(arkaplan);

    threshold(onplan, onplan, 128, 255, CV_THRESH_BINARY);

    erode(onplan, onplan, Mat());
    dilate(onplan, onplan, cekirdek);

}


bool GoruntuIsleme::mogUzerindenBlobCikarimi(Mat res, Mat &onplanBlob, int min_area, int max_area, vector<Rect> &rv)
{
    bool durum;
    //Mat tmp;
  //  res.copyTo(tmp);
    matIpl = res;
    mogRes = onplanBlob;
    IplImage *labelRes = cvCreateImage(cvGetSize(&matIpl), IPL_DEPTH_LABEL, 1);

    CvBlobs blbs;
    unsigned int sonuc = cvLabel(&mogRes, labelRes, blbs);
    cvFilterByArea(blbs,min_area, max_area);

    cvRenderBlobs(labelRes, blbs, &matIpl, &matIpl, CV_BLOB_RENDER_CENTROID | CV_BLOB_RENDER_COLOR);


    CvBlobs::iterator itk = blbs.begin();
    while (itk != blbs.end())
    {
        CvBlob *blob=(*itk).second;
        Rect r = Rect(Point(blob->minx, blob->miny), Point(blob->maxx -1, blob->maxy - 1));
        rv.push_back(r);
      //  rectangle(res, r , Scalar(255,255,0), 3);
        itk++;
    }

    cvUpdateTracks(blbs, tracks, 20., 15, 5);
   // CvTracks::iterator it = tracks.begin();
    //CvTrack *track = (*it).second;

    cvRenderTracks(tracks, &matIpl, &matIpl, CV_TRACK_RENDER_ID | CV_TRACK_RENDER_BOUNDING_BOX);

    if (!tracks.empty()) //&& track->lifetime > 4)
    {
       // hedefMerkezi.x = track->centroid.x;
       // hedefMerkezi.y = track->centroid.y;



      /*  cvLine(&matIpl, cvPoint(hedefMerkezi.x, 0), cvPoint(hedefMerkezi.x, matIpl.height), CV_RGB(255,0,0), 4);
        cvLine(&matIpl, cvPoint(0, hedefMerkezi.y), cvPoint(matIpl.width, hedefMerkezi.y), CV_RGB(255,0,0), 4);
        cvCircle(&matIpl, cvPoint(hedefMerkezi.x, hedefMerkezi.y), 5, CV_RGB(255,255,255), 4);*/
        durum  = true;
   }
   else
   {
        durum = false;
   }
   res = &matIpl;
   cvReleaseImage(&labelRes);
   //imshow("MOGGG", res);
   return durum;
}


void GoruntuIsleme::histogramEsitleme(Mat &res, Mat &sonuc)
{
    Mat tampon;
    cvtColor(res, tampon, CV_BGR2GRAY);
    blur(tampon,tampon, Size(7,7));
    equalizeHist(tampon, sonuc);
    threshold(sonuc, sonuc, 128, 255, CV_THRESH_BINARY_INV);
    imshow("histogram esitleme", sonuc);
}


void GoruntuIsleme::satBelirle(int sat)
{
    smin = sat;
}


void GoruntuIsleme::valBelirle(int val)
{
    vmin = val;
}

int GoruntuIsleme::grayHistogram(Mat &res, Mat &hist_res)
{
    int bins = 256;
    int hsize[] = {bins};
    int channels[] = {0};
    float xranges[] = {0, 255};
    const float *ranges[] = {xranges};

    Mat histt;
    calcHist(&res, 1, channels, Mat(), histt, 1, hsize, ranges);

    double maxval = 0;
    double minval = 0;
    minMaxLoc(histt, &minval, &maxval);

    //rectangle(hist_res, Point(0, 0), Point(hsize[0], 50), Scalar(255));

    float enFazla = histt.at<float>(0);
    int indis = 0;
    for (int i = 0; i < hsize[0]; i++)
    {
        if (enFazla > histt.at<float>(i))
        {
            indis = i;
            enFazla = histt.at<float>(i);
        }
       // float value = histt.at<float>(i);
        //cout << i << " = " << histt.at<float>(i) << endl;
     //   int normalized = static_cast<int>(value * 50 / maxval);
      //  line(hist_res, Point(i, 50), Point(i, 50 - normalized), Scalar(0));
    }
    //cout << "İndis" << indis << "deger = " << enFazla << endl;
    return indis;
}

void GoruntuIsleme::golgeleriSil(Mat &res,  Mat &sonuc)
{
    ort_golge_ici_b = 0;
    ort_golge_ici_g = 0;
    ort_golge_ici_r = 0;

    ort_golge_disi_b = 0;
    ort_golge_disi_g = 0;
    ort_golge_disi_r = 0;

    for (int j = 0; j < sonuc.rows; j++)
    {
        uchar *q = res.ptr<uchar>(j);
        for (int i = 0; i < sonuc.cols; i++)
        {
            // uchar *Piksel = &sonuc.at<uchar>(i,j);
             if (q[i] == 255)
             {
                int p1 = sonuc.at<Vec3b>(j, i)[0];
                int p2 = sonuc.at<Vec3b>(j, i)[1];
                int p3 = sonuc.at<Vec3b>(j, i)[2];

                ort_golge_ici_b += p1;
                ort_golge_ici_g += p2;
                ort_golge_ici_r += p3;

                ort_golge_ici_sayac++;

             }
             else if (q[j] == 0)
             {
                int p1 = sonuc.at<Vec3b>(j, i)[0];
                int p2 = sonuc.at<Vec3b>(j, i)[1];
                int p3 = sonuc.at<Vec3b>(j, i)[2];

                ort_golge_disi_b += p1;
                ort_golge_disi_g += p2;
                ort_golge_disi_r += p3;

                ort_golge_disi_sayac++;

             }
        }
    }

    ort_golge_ici_b = (ort_golge_ici_b / ort_golge_ici_sayac);
    ort_golge_ici_g = (ort_golge_ici_g / ort_golge_ici_sayac);
    ort_golge_ici_r = (ort_golge_ici_r / ort_golge_ici_sayac);



    ort_golge_disi_b = (ort_golge_disi_b / ort_golge_disi_sayac);
    ort_golge_disi_g = (ort_golge_disi_g / ort_golge_disi_sayac);
    ort_golge_disi_r = (ort_golge_disi_r / ort_golge_disi_sayac);

    cout << "gdb" << ort_golge_disi_b << endl;
    cout << "gbg" << ort_golge_disi_g << endl;
    cout << "gdr" << ort_golge_disi_r << endl;

    ort_golge_ici_sayac = 0;
    ort_golge_disi_sayac = 0;


    int c_b = ort_golge_disi_b - ort_golge_ici_b;
    int c_g = ort_golge_disi_g - ort_golge_ici_g;
    int c_r = ort_golge_disi_r - ort_golge_ici_r;

    int c = ((ort_golge_disi_b * ort_golge_ici_b) + (ort_golge_disi_g * ort_golge_ici_g) + (ort_golge_disi_r * ort_golge_ici_r))
            /
            ((ort_golge_ici_b * ort_golge_ici_b) + (ort_golge_ici_g * ort_golge_ici_g) + (ort_golge_ici_r * ort_golge_ici_r));

    cout << "c::b" << c_b << endl;
    cout << "c::g" << c_g << endl;
    cout << "c::r" << c_r << endl;

    cout << "c:" << c << endl;
    for (int j = 0; j < res.rows; j++)
    {
     //   uchar *p = sonuc.ptr<uchar>(j);
        uchar *q = res.ptr<uchar>(j);
        for (int i = 0; i < res.cols; i++)
        {
            if (q[i] == 255)
            {
                sonuc.at<Vec3b>(j, i)[0] += c_b/2 + c;
                sonuc.at<Vec3b>(j, i)[1] += c_g/2 + c;
                sonuc.at<Vec3b>(j, i)[2] += c_r/2 + c;
            }
        }
    }
}

void GoruntuIsleme::golgeBul(Mat res, Mat &sonuc)
{
    for (int j = 0; j < res.rows; j++)
    {
        uchar *p = sonuc.ptr<uchar>(j);
     //   uchar *q = res.ptr<uchar>(j);
        for (int i = 0; i < res.cols; i++)
        {
            uchar b = res.at<Vec3b>(j,i)[0];
            uchar g = res.at<Vec3b>(j,i)[1];
            uchar r = res.at<Vec3b>(j,i)[2];
        //    uchar n = saturate_cast<uchar>(b + g + r);
           /* b /= n;
            g /= n;
            r /= n;*/
            uchar gl = saturate_cast<uchar>(3 * g - r - b);
            uchar nl = saturate_cast<uchar>(2 * g - r - b);
          //  cout << "gl" << (int) gl << endl;
            p[i] = gl - nl;
            //sonuc.at<Vec3b>(j,i) = (res.at<Vec3b>(j,i)[1] * 2) - res.at<Vec3b>(j,i)[2] - res.at<Vec3b>(j,i)[0];
        }
    }

}

/*Bölümleme için çok yavaş çalışıyor.
Tercih edilemez!*/
void GoruntuIsleme::kmeansCluster(Mat res)
{
    Mat ornekler;
    res.copyTo(ornekler);
    Mat labels;
    Mat noktalar(res.rows * res.cols, 3, CV_32FC1);

    for (int j = 0; j < res.rows; j++)
    {
        for (int i = 0; i < res.cols; i++)
        {
            for (int z = 0; z < 3; z++)
            {
                noktalar.at<float>(j + i * res.rows, z) = ornekler.at<Vec3b>(j,i)[z];
            }
        }
    }
    Mat merkezler(kumeSayisi, 1, noktalar.type());
    kmeans(noktalar, kumeSayisi, labels, TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 3, KMEANS_PP_CENTERS, merkezler);
}


void GoruntuIsleme::rgbUzerindenCikarim(Mat res, Mat &sonuc)
{

    /**
     *@res: bgr resim.
     *@sonuc: dönüşüm ve iyileştirme yapıldıktan sonra resim bu değişkene atılır.
     */
    //Mat I;
    for (int j = 0; j < res.rows; j++)
    {
        uchar *p = sonuc.ptr<uchar>(j);
     //   uchar *q = res.ptr<uchar>(j);
        for (int i = 0; i < res.cols; i++)
        {
            uchar b = res.at<Vec3b>(j,i)[0];
            uchar g = res.at<Vec3b>(j,i)[1];
            uchar r = res.at<Vec3b>(j,i)[2];
        //    uchar n = saturate_cast<uchar>(b + g + r);
           /* b /= n;
            g /= n;
            r /= n;*/
            uchar gl = saturate_cast<uchar>(2 * g - r - b);
            if (gl > 200)
                gl = 0;
           // uchar nl = saturate_cast<uchar>(2 * g - r - b);
          //  cout << "gl" << (int) gl << endl;
            p[i] = gl;
            //sonuc.at<Vec3b>(j,i) = (res.at<Vec3b>(j,i)[1] * 2) - res.at<Vec3b>(j,i)[2] - res.at<Vec3b>(j,i)[0];
        }
    }

}


void GoruntuIsleme::grabCutUygula(Mat res, Mat &sonuc, Rect r)
{
    Mat bgM, fgM;
    grabCut(res, sonuc, r, bgM, fgM, 5, GC_INIT_WITH_RECT);
    compare(sonuc, GC_PR_FGD, sonuc, CMP_EQ);
    Mat onplan(res.size(), CV_8UC3, Scalar(255,255,255));
    res.copyTo(onplan, sonuc);
    sonuc = sonuc & 1;
}


int GoruntuIsleme::histogramlaEsikDegeriBul(Mat &res)
{
   // int esik_degeri = 100;
    int tp[255];
    int t;
    for (int k = 0; k < 256; k++)
    {
        for (int j = 0; j < res.rows; j++)
        {
            uchar *p = res.ptr<uchar>(j);
            for (int i = 0; i < res.cols; i++)
            {
                if (p[i] == k)
                {
                    t++;
                }
            }
        }
        tp[k] = t;
        t = 0;
    }
    int maks_i = tp[0];
    int th_i = 0;
    for (int i = 1; i < 256; i++)
    {
        if (maks_i < tp[i])
        {
            maks_i = tp[i];
            th_i = i;
        }
    }
    cout << tp[th_i] << endl;
    return th_i;
}

template <typename T> void ortPikselDegerleriniDondur(Mat &mat)
{
    if (mat.empty())
    {
        return;
    }

    if ((mat.elemSize() / mat.channels()) != sizeof(T))
    {
        return;
    }

    int sutun_sayisi = mat.cols;
    int satir_sayisi = mat.rows;
    int kanal_sayisi = mat.channels();

    if (kanal_sayisi > 1)
    {
        for (int i = 0; i < satir_sayisi; i++)
        {
            for (int j = 0; j < sutun_sayisi; j++)
            {
                 T *Piksel = &mat.at<T>(i,j);
                 for (int c = 0; c < kanal_sayisi; c++)
                 {
                     cout << Piksel[c] << "," << endl;
                 }
                 cout << "\n" << endl;
                //degerler.val[0] += Piksel[0];
            }
        }
    }

}
