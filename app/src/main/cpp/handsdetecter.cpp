#include <jni.h>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

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
        if (boundRect[i].area() > 200) {//長方形區域面積超過900，則畫在影像上
            //畫在二值化影像上
            rectangle(*empty, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
            //畫在原始影像上
            rectangle(*cvMat, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
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
    cout << "Number of connected components = " << nLabels << endl << endl;
    for (int label = 1; label < nLabels; ++label) {
        int a = rand() & 255;
        colors[label] = Vec3b(255, 255, 255);
        cout << "Component " << label << std::endl;
        cout << "CC_STAT_LEFT   = " << outputState.at<int>(label, CC_STAT_LEFT) << endl;
        cout << "CC_STAT_TOP    = " << outputState.at<int>(label, CC_STAT_TOP) << endl;
        cout << "CC_STAT_WIDTH  = " << outputState.at<int>(label, CC_STAT_WIDTH) << endl;
        cout << "CC_STAT_HEIGHT = " << outputState.at<int>(label, CC_STAT_HEIGHT) << endl;
        cout << "CC_STAT_AREA   = " << outputState.at<int>(label, CC_STAT_AREA) << endl;
        cout << "CENTER   = (" << outputState.at<double>(label, 0) << "," <<
        outputState.at<double>(label, 1) << ")" << std::endl << std::endl;
        int width = outputState.at<int>(label, CC_STAT_WIDTH);
        int height = outputState.at<int>(label, CC_STAT_HEIGHT);
        if (width * height > 200) {
            int left = outputState.at<int>(label, CC_STAT_LEFT);
            int top = outputState.at<int>(label, CC_STAT_TOP);
            int right = outputState.at<int>(label, CC_STAT_LEFT) +
                        outputState.at<int>(label, CC_STAT_WIDTH);
            int bottom = outputState.at<int>(label, CC_STAT_TOP) +
                         outputState.at<int>(label, CC_STAT_HEIGHT);
            rectangle(*cvMat, Point(left, top), Point(right, bottom), color, 1, 8, 0);
        }
    }
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

extern "C"

void Java_com_baobomb_handsdetection_HandsDetecter_detect(
        JNIEnv *env,
        jobject, jlong nativeMat, jlong emptyMat) {
    Mat *cvMat = (Mat *) nativeMat;
    Mat *empty = (Mat *) emptyMat;
//    checkHotPoint(cvMat, empty);
    connectComponent(cvMat, empty);
}
