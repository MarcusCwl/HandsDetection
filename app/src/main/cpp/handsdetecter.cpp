#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

extern "C"
void
Java_com_baobomb_handsdetection_HandsDetecter_detect(
        JNIEnv *env,
        jobject, jlong nativeMat, jlong emptyMat) {
    Mat *cvMat = (Mat *) nativeMat;
    Mat *empty = (Mat *) emptyMat;
    Mat hsv;
    Mat dst;
    Mat r1;
    Mat r2;

    cvtColor(*cvMat, hsv, CV_RGB2HSV);
    inRange(hsv,Scalar(0,100,120),Scalar(10,255,255),r1);
    inRange(hsv,Scalar(170,100,120),Scalar(180,255,255),r2);
    dst = r1+r2;
//    inRange(hsv, Scalar(0, 58, 20), Scalar(50, 173, 230), dst);
    dst.copyTo(*empty);
}
