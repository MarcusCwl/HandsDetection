#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
jclass cls;
jmethodID mid;
Mat firstFrame;
Mat secondFrame;
Mat foreground;
Mat morphologyKernel = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

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

void detectMotionEvent(Mat *firstMat, Mat *secondMat, Mat *empty) {
//    cvtColor(*firstMat, firstFrame, COLOR_RGB2GRAY);
//    cvtColor(*secondMat, secondFrame, COLOR_RGB2GRAY);
    firstMat->copyTo(firstFrame);
    secondMat->copyTo(secondFrame);
    absdiff(firstFrame, secondFrame, foreground);
    threshold(foreground, foreground, 80, 255, THRESH_BINARY);
    morphologyEx(foreground, foreground, MORPH_CLOSE, morphologyKernel, Point(-1, -1), 2,
                 BORDER_REPLICATE);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(foreground, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE,
                 Point(0, 0));
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect;
    boundRect.clear();
    for (int index = 0; index < contours.size(); index++) {
        approxPolyDP(Mat(contours[index]), contours_poly[index], 3, true);
        Rect rect = boundingRect(Mat(contours_poly[index]));
        if (rect.area() > 300) {
            rectangle(foreground, rect, Scalar(255, 0, 0), 2);
        }
    }
    foreground.copyTo(*empty);
}

extern "C" {

void Java_com_baobomb_handsdetection_MotionDetecter_motiondetect(
        JNIEnv *env,
        jobject, jlong firstMat, jlong secondMat, jlong emptyMat) {
    Mat *firstFrame = (Mat *) firstMat;
    Mat *secondFrame = (Mat *) secondMat;
    Mat *empty = (Mat *) emptyMat;
    detectMotionEvent(firstFrame, secondFrame, empty);
//    motionEventCallback(env);
}


}