#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
jclass cls;
jmethodID mid;
Rect detectRect = Rect();
Point minLoc;
double minVal;

//void initJNICallbackMethod(JNIEnv *env) {
//    cls = env->FindClass("com/baobomb/handsdetection/MotionDetecter");
//    //再找类中的方法
//    mid = env->GetStaticMethodID(cls, "motionEvent", "(Ljava/lang/String;)V");
//    if (mid == NULL) {
//        return;
//    }
//}
//
//void motionEventCallback(JNIEnv *env) {
//    if (cls == nullptr && mid == nullptr) {
//        initJNICallbackMethod(env);
//    }
//    env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("move"));
//}


void setHandsRectMat(Mat *holeMat, Mat *skinRect) {
    Mat mRgba = Mat();
    holeMat->copyTo(mRgba);
    //相機寬
    int cols = holeMat->cols;
    //相機高
    int rows = holeMat->rows;
    int x = cols / 2;
    int y = rows / 2;
    //檢測區域
//    Rect touchedRect = Rect();
    //設定觸摸區域左上角座標 X & Y 若是小於5 則設為0
//    touchedRect.x = x - 100;
//    touchedRect.y = y - 200;
    //設定觸摸區域寬高 若小於圖像矩陣寬高 則各設為5 若大於圖像矩陣寬高 則設為觸摸區域左上角到圖像右下角
//    touchedRect.width = (x + 5 < cols) ? x + 5 - touchedRect.x : cols - touchedRect.x;
//    touchedRect.height = (y + 5 < rows) ? y + 5 - touchedRect.y : rows - touchedRect.y;
    //取出觸摸區域的圖像矩陣
    Mat touchedRegionRgba = mRgba.operator()(detectRect);
    touchedRegionRgba.copyTo(*skinRect);
}

void setHandsDetectRect(Mat *holeMat) {
    //相機寬
    int cols = holeMat->cols;
    //相機高
    int rows = holeMat->rows;
    int x = cols / 2;
    int y = rows / 2;
    if (detectRect.x == 0) {
        detectRect.x = x - 150;
        detectRect.width = 300;
    }
    if (detectRect.y == 0) {
        detectRect.y = y - 200;
        detectRect.height = 400;
    }
    rectangle(*holeMat, Point(x - 150, y - 200), Point(x + 150, y + 200), Scalar(255, 0, 0), 1, 8,
              0);

}

void detectMotionEvent(Mat *firstMat, Mat *secondMat, Mat *outputMat) {
//    firstMat->copyTo(firstFrame);
//    secondMat->copyTo(secondFrame);
//    threshold(firstFrame, firstFrame, 80, 255, THRESH_BINARY);
//    threshold(secondFrame, secondFrame, 80, 255, THRESH_BINARY);
//    absdiff(firstFrame, secondFrame, foreground);
//    dilate(foreground, foreground, Mat());
//    foreground.copyTo(*empty);
    Mat result;
    result.create(firstMat->rows - secondMat->rows + 1, firstMat->cols - secondMat->cols + 1,
                  CV_32FC1);
    matchTemplate(*firstMat, *secondMat, result, CV_TM_SQDIFF);
//    double minVal;
//    Point minLoc;
    minMaxLoc(result, &minVal, 0, &minLoc, 0);
    if (minVal < 100000000) {
        rectangle(*outputMat, minLoc, Point(minLoc.x + secondMat->cols, minLoc.y + secondMat->rows),
                  Scalar::all(0), 3);
    }
}

extern "C" {

void Java_com_baobomb_handsdetection_MotionDetecter_motiondetect(
        JNIEnv *env,
        jobject, jlong firstMat, jlong secondMat, jlong outputMat) {
    Mat *firstFrame = (Mat *) firstMat;
    Mat *secondFrame = (Mat *) secondMat;
    Mat *outputFrame = (Mat *) outputMat;
    detectMotionEvent(firstFrame, secondFrame, outputFrame);
//    motionEventCallback(env);
}

void Java_com_baobomb_handsdetection_MotionDetecter_setHandsRectMat(
        JNIEnv *env,
        jobject, jlong holeMat, jlong *skinMat) {
    Mat *hole = (Mat *) holeMat;
    Mat *skin = (Mat *) skinMat;
    setHandsRectMat(hole, skin);
//    motionEventCallback(env);
}

void Java_com_baobomb_handsdetection_MotionDetecter_setHandsDetectRect(
        JNIEnv *env,
        jobject, jlong holeMat) {
    Mat *hole = (Mat *) holeMat;
    setHandsDetectRect(hole);
//    motionEventCallback(env);
}


}