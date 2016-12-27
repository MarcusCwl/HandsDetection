#include <jni.h>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
Point startPoint = Point(0, 0);
int ifInit = 0;
jclass cls;
jmethodID mid;



void handsMove(JNIEnv *env,String handsMove) {
    cls = env->FindClass("com/baobomb/handsdetection/HandsDetecter");
    //再找类中的方法
    mid = env->GetStaticMethodID(cls, "handsMove", "(Ljava/lang/String;)V");
    if (mid == NULL) {
        return;
    }
    //回调java中的方法
    env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("你好haha"));
}


//使用findContours做輪廓檢測
void checkHotPoint(Mat *cvMat, Mat *empty) {
    Mat hsv;
    Mat dst;
    Mat threshold_img;
    Mat r1;
    Mat r2;
    Mat g;
    Mat y;
    cvtColor(*cvMat, hsv, CV_RGB2HSV);
    //紅色
    inRange(hsv, Scalar(0, 100, 120), Scalar(10, 255, 255), r1);
    inRange(hsv, Scalar(170, 100, 120), Scalar(180, 255, 255), r2);
    inRange(hsv, Scalar(50, 100, 120), Scalar(70, 255, 255), g);
    inRange(hsv, Scalar(20, 100, 120), Scalar(30, 255, 255), y);
    dst = r1 + r2 + g + y;

    //膚色
//    inRange(hsv, Scalar(0, 58, 20), Scalar(50, 173, 230), r1);
//    dst = r1;
    dilate(dst, dst, Mat(), Point(-1, -1), 3);
    erode(dst, dst, Mat(), Point(-1, -1), 3);
    threshold(dst, threshold_img, 125, 255, THRESH_BINARY);
    threshold_img.copyTo(*empty);

    Mat cannyImage;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    //紅色
//    Canny(dst, cannyImage, 50, 100, 3);
    //膚色
    Canny(threshold_img, cannyImage, 50, 100, 3);
    findContours(cannyImage, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    vector<vector<Point>> contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    for (int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        //計算可以包含輪廓的最小長方形區域
        boundRect[i] = boundingRect(Mat(contours_poly[i]));
        Scalar color = Scalar(255, 0, 0);
        if (boundRect[i].area() > 200) {//長方形區域面積超過200，則畫在影像上
            //畫在二值化影像上
            rectangle(*empty, boundRect[i].tl(), boundRect[i].br(), color, 1, 8, 0);
            //畫在原始影像上
            rectangle(*cvMat, boundRect[i].tl(), boundRect[i].br(), color, 1, 8, 0);
        }
    }
}

//二值化圖像 顏色取紅 綠 黃 等溫色 並使用CC作連通檢測
void connectComponent(Mat *cvMat, Mat *empty) {
    Mat hsv;
    Mat dst;
    Mat r1;
    Mat r2;
    Mat g;
    Mat y;
    cvtColor(*cvMat, hsv, CV_RGB2HSV);
    //紅色
    inRange(hsv, Scalar(0, 100, 120), Scalar(10, 255, 255), r1);
    inRange(hsv, Scalar(170, 100, 120), Scalar(180, 255, 255), r2);
    inRange(hsv, Scalar(50, 100, 120), Scalar(70, 255, 255), g);
    inRange(hsv, Scalar(20, 100, 120), Scalar(30, 255, 255), y);
    dst = r1 + r2 + g + y;
    Mat connectedImage;
    Mat outputState;
    Mat outputCenter;
    int nLabels = connectedComponentsWithStats(dst, connectedImage, outputState,
                                               outputCenter, 8,
                                               CV_32S);
    vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);
    Scalar color = Scalar(255, 0, 0);
    Point lt = Point(0, 0);
    Point rb = Point(0, 0);
    int lastWidth = 0;
    int lastHeight = 0;
    int detectSize = ((cvMat->cols) / 3) * ((cvMat->rows) / 3);
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b(255, 255, 255);
        int width = outputState.at<int>(label, CC_STAT_WIDTH);
        int height = outputState.at<int>(label, CC_STAT_HEIGHT);
        if (width * height > lastHeight * lastWidth) {
            if (width * height > detectSize) {
                lastHeight = height;
                lastWidth = width;
                int left = outputState.at<int>(label, CC_STAT_LEFT);
                int top = outputState.at<int>(label, CC_STAT_TOP);
                int right = outputState.at<int>(label, CC_STAT_LEFT) +
                            outputState.at<int>(label, CC_STAT_WIDTH);
                int bottom = outputState.at<int>(label, CC_STAT_TOP) +
                             outputState.at<int>(label, CC_STAT_HEIGHT);
                lt.x = left;
                lt.y = top;
                rb.x = right;
                rb.y = bottom;
            }
        }
    }

    rectangle(*cvMat, lt, rb, color, 1, 8, 0);
    //將二值化出來的img繪製到connectedDst上 並複製到empty上
    Mat connectedDst(dst.size(), CV_8UC3);
    for (int r = 0; r < dst.rows; ++r) {
        for (int c = 0; c < dst.cols; ++c) {
            int label = connectedImage.at<int>(r, c);
            Vec3b &pixel = connectedDst.at<Vec3b>(r, c);
            pixel = colors[label];
        }
    }
    connectedDst.copyTo(*empty);
}


void ccByCamera(Mat *cvMat, JNIEnv *env) {
    Mat hsv;
    Mat dst;
    Mat r1;
    Mat r2;
    Mat g;
    Mat y;
    cvtColor(*cvMat, hsv, CV_RGB2HSV);
    inRange(hsv, Scalar(0, 58, 20), Scalar(50, 173, 230), r1);
    dst = r1;
    Mat connectedImage;
    Mat outputState;
    Mat outputCenter;
    int nLabels = connectedComponentsWithStats(dst, connectedImage, outputState,
                                               outputCenter, 8,
                                               CV_32S);
    vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);
    Scalar color = Scalar(255, 0, 0);
    Point lt = Point(0, 0);
    Point rb = Point(0, 0);
    int lastWidth = 0;
    int lastHeight = 0;
    int camWidth = cvMat->cols;
    int camHeight = cvMat->rows;
    int detectSize = ((cvMat->cols) / 3) * ((cvMat->rows) / 3);
    for (int label = 1; label < nLabels; ++label) {
        colors[label] = Vec3b(255, 255, 255);
        int width = outputState.at<int>(label, CC_STAT_WIDTH);
        int height = outputState.at<int>(label, CC_STAT_HEIGHT);
        if (width * height > lastHeight * lastWidth) {
            if (width * height > detectSize) {
                lastHeight = height;
                lastWidth = width;
                int left = outputState.at<int>(label, CC_STAT_LEFT);
                int top = outputState.at<int>(label, CC_STAT_TOP);
                int right = outputState.at<int>(label, CC_STAT_LEFT) +
                            outputState.at<int>(label, CC_STAT_WIDTH);
                int bottom = outputState.at<int>(label, CC_STAT_TOP) +
                             outputState.at<int>(label, CC_STAT_HEIGHT);
                lt.x = left;
                lt.y = top;
                rb.x = right;
                rb.y = bottom;
                if (ifInit) {
                    double dx = outputCenter.at<double>(label, 0);
                    double dy = outputCenter.at<double>(label, 1);
                    startPoint.x = (int) dx;
                    startPoint.y = (int) dy;
                    ifInit = 1;
                } else {
                    double dx = outputCenter.at<double>(label, 0);
                    double dy = outputCenter.at<double>(label, 1);
                    int moveXDistance = abs((int)(dx - startPoint.x));
                    int moveYDistance = abs((int)(dy - startPoint.y));
                    if (moveXDistance > camWidth / 2) {
                        handsMove(env,"move");
                        ifInit = 0;
                    }
                }
            }
        }
    }
    rectangle(*cvMat, lt, rb, color, 1, 8, 0);
    //將二值化出來的img繪製到connectedDst上 並複製到empty上
}
extern "C" {

void Java_com_baobomb_handsdetection_HandsDetecter_detectFromThermal(
        JNIEnv *env,
        jobject, jlong nativeMat, jlong emptyMat) {
    Mat *cvMat = (Mat *) nativeMat;
    Mat *empty = (Mat *) emptyMat;
//    checkHotPoint(cvMat, empty);
    connectComponent(cvMat, empty);
}

void Java_com_baobomb_handsdetection_HandsDetecter_detectFromCamera(
        JNIEnv *env,
        jobject, jlong nativeMat) {
    Mat *cvMat = (Mat *) nativeMat;
    ccByCamera(cvMat, env);
//    connectComponent(cvMat, empty);
}
}