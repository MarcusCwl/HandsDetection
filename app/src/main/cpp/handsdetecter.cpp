#include <jni.h>
#include <string>
#include <iostream>
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
    Mat threshold_img;
    Mat r1;
    Mat r2;

    cvtColor(*cvMat, hsv, CV_RGB2HSV);
    //紅色
//    inRange(hsv, Scalar(0, 100, 120), Scalar(10, 255, 255), r1);
//    inRange(hsv, Scalar(170, 100, 120), Scalar(180, 255, 255), r2);
//    dst = r1 + r2;

    //膚色
    inRange(hsv, Scalar(0, 58, 20), Scalar(50, 173, 230), r1);
    dst = r1;
    cv::dilate(dst, dst, Mat(), Point(-1, -1), 3);
    cv::erode(dst, dst, Mat(), Point(-1, -1), 3);
    threshold(dst, threshold_img, 125, 255, THRESH_BINARY);
    threshold_img.copyTo(*empty);

    RNG rng(12345);
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
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        //隨機給一個顏色
        if (boundRect[i].area() > 90000) {//長方形區域面積超過900，則畫在影像上
            drawContours(*empty, contours_poly, i, color, 1, 8, vector<Vec4i>(), 0, Point());
            rectangle(*empty, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
        }
    }
}

void connectComponent(Mat thresholdImg, Mat *empty) {
    Mat connectedImage;
    Mat outputState;
    Mat outputCenter;
    int nLabels = connectedComponentsWithStats(thresholdImg, connectedImage, outputState,
                                               outputCenter, 8,
                                               CV_32S);
//    inRange(hsv, Scalar(0, 58, 20), Scalar(50, 173, 230), dst);
    vector<Vec3b> colors(nLabels);
    colors[0] = Vec3b(0, 0, 0);
    cout << "Number of connected components = " << nLabels << endl << endl;
    for (int label = 1; label < nLabels; ++label) {
        int a = rand() & 255;
        colors[label] = Vec3b(255, 255, 255);
        cout << "Component " << label << std::endl;
        cout << "CC_STAT_LEFT   = " << outputState.at<int>(label, cv::CC_STAT_LEFT) << std::endl;
        cout << "CC_STAT_TOP    = " << outputState.at<int>(label, cv::CC_STAT_TOP) << std::endl;
        cout << "CC_STAT_WIDTH  = " << outputState.at<int>(label, cv::CC_STAT_WIDTH) << std::endl;
        cout << "CC_STAT_HEIGHT = " << outputState.at<int>(label, cv::CC_STAT_HEIGHT) << std::endl;
        cout << "CC_STAT_AREA   = " << outputState.at<int>(label, cv::CC_STAT_AREA) << std::endl;
        cout << "CENTER   = (" << outputState.at<double>(label, 0) << "," <<
        outputState.at<double>(label, 1) << ")" << std::endl << std::endl;
    }
    Mat connectedDst(thresholdImg.size(), CV_8UC3);

    for (int r = 0; r < thresholdImg.rows; ++r) {
        for (int c = 0; c < thresholdImg.cols; ++c) {
            int label = connectedImage.at<int>(r, c);
            Vec3b &pixel = connectedDst.at<cv::Vec3b>(r, c);
            pixel = colors[label];
        }
    }
    connectedDst.copyTo(*empty);
}
