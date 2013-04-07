#include <GoruntuYoneticisi.h>

GoruntuYoneticisi::GoruntuYoneticisi()
{
    kamerayiAc(-1);

}

Mat GoruntuYoneticisi::dosyadanResimOku(const char *resim_yolu)
{
    Mat res;
    if (resim_yolu)
    {
        cout << resim_yolu << endl;
        res = imread(resim_yolu, CV_LOAD_IMAGE_COLOR);
    }
    return res;
}


Mat GoruntuYoneticisi::kameradanOku()
{
    Mat res;
    if (kameraDurumu)
    {
        Mat r;
        capture >> r;
        r.copyTo(res);
    }
   // imshow("ress", res);
    return res;
}


void GoruntuYoneticisi::kamerayiAc(int id)
{
    capture.open(id);
    kameraAyarlariniYap();
    if (capture.isOpened())
    {
        cout << "Kamera acildi\n";
        kameraDurumu = true;
    }
    else
    {
        kameraDurumu = false;
    }
}


void GoruntuYoneticisi::kamerayiKapat()
{
    capture.release();
    writer.release();
}


void GoruntuYoneticisi::kameraAyarlariniYap()
{
    capture.set(CV_CAP_PROP_FPS, 30.0);
    capture.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

    capture.set(CV_CAP_PROP_EXPOSURE, 250);
    capture.set(CV_CAP_PROP_GAIN, 100);
    capture.set(CV_CAP_PROP_SATURATION, 180);

    Size S = Size((int) 640,
                  (int) 480);
    int ex = static_cast<int>(CV_FOURCC('D','I','V','X'));

    char EXT[] = {ex & 0XFF , (ex & 0XFF00) >> 8,(ex & 0XFF0000) >> 16,(ex & 0XFF000000) >> 24, 0};

    cout << "Codec : " << EXT;
    string dosya_yolu = "/home/linaro/kayitvv.avi";
    writer.open(dosya_yolu, ex,15, S, true);

    if (writer.isOpened())
    {
        cout << "Vidyo : Yazmaya basliyor...";
    }
    else
    {
        cout << "Vidyo: Yazma yapılamiyor...";
    }
}


void GoruntuYoneticisi::goruntuEkle(Mat res)
{
    if (writer.isOpened())
        writer << res;
}

void GoruntuYoneticisi::vidyoDanOku()
{
    const string dosya_adi = "/home/linaro/kayit.avi";
    capture.open(dosya_adi);
    if (capture.isOpened())
    {
        cout << "Kamera acildi\n";
        kameraDurumu = true;
    }
    else
    {
        kameraDurumu = false;
    }
}
