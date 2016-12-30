#include <jni.h>
#include <string>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
Point startPoint = Point(0, 0);
int ifInit = 1;
jclass cls;
jmethodID mid;
Scalar mLowerBound = Scalar(0);
Scalar mUpperBound = Scalar(0);
Scalar mBlobHsv;
int positionX;
int positionY;
int moveX;
int moveY;
Mat mPyrDownMat;
Mat mHsvMat;
Mat mMask;
Mat mDilatedMask;
Mat mHierarchy;

//手部移動Call
void handsMove(JNIEnv *env, int moveType) {
    cls = env->FindClass("com/baobomb/handsdetection/HandsDetecter");
    //再找类中的方法
    if (moveType != 8) {
        mid = env->GetStaticMethodID(cls, "handsMove", "(Ljava/lang/String;)V");
    } else {
        mid = env->GetStaticMethodID(cls, "handsMove", "(Ljava/lang/String;IIII)V");
    }
    if (mid == NULL) {
        return;
    }
    //回调java中的方法
    if (moveType == 4) {
        env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("right"));
    } else if (moveType == 5) {
        env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("left"));
    } else if (moveType == 6) {
        env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("down"));
    } else if (moveType == 7) {
        env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("up"));
    } else if (moveType == 8) {
        env->CallStaticVoidMethod(cls, mid, env->NewStringUTF("move"), moveX, moveY, positionX,
                                  positionY);
    }
    ifInit = 1;
}

//設定中心點平均色
void setSkinColor(Mat *skinRect, int screenWidth, int screenHeight, int screenCenterX,
                  int screenCenterY) {
    Scalar mColorRadius = Scalar(25, 50, 50, 0);
    Mat mRgba = Mat();
    skinRect->copyTo(mRgba);
    //相機寬
    int cols = skinRect->cols;
    //相機高
    int rows = skinRect->rows;
    //X軸偏移量
    int xOffset = (screenWidth - cols) / 2;
    //Y軸偏移量
    int yOffset = (screenHeight - rows) / 2;
    int x = screenCenterX - xOffset;
    int y = screenCenterY - yOffset;
    //觸摸區域
    Rect touchedRect = Rect();
    //設定觸摸區域左上角座標 X & Y 若是小於5 則設為0
    touchedRect.x = (x > 5) ? x - 5 : 0;
    touchedRect.y = (y > 5) ? y - 5 : 0;
    //設定觸摸區域寬高 若小於圖像矩陣寬高 則各設為5 若大於圖像矩陣寬高 則設為觸摸區域左上角到圖像右下角
    touchedRect.width = (x + 5 < cols) ? x + 5 - touchedRect.x : cols - touchedRect.x;
    touchedRect.height = (y + 5 < rows) ? y + 5 - touchedRect.y : rows - touchedRect.y;
    //取出觸摸區域的圖像矩陣
    Mat touchedRegionRgba = mRgba.operator()(touchedRect);
    //宣告 HSV顏色型態圖像矩陣
    Mat touchedRegionHsv = Mat();
    cvtColor(touchedRegionRgba, touchedRegionHsv, COLOR_RGB2HSV_FULL);
    mBlobHsv = sum(touchedRegionHsv);
    int pointCount = touchedRect.width * touchedRect.height;
    int length = sizeof(mBlobHsv.val) / sizeof(mBlobHsv.val[0]);
    for (int i = 0; i < length; i++) {
        mBlobHsv.val[i] /= pointCount;
    }
    double minH = (mBlobHsv.val[0] >= mColorRadius.val[0]) ? mBlobHsv.val[0] - mColorRadius.val[0]
                                                           : 0;
    double maxH = (mBlobHsv.val[0] + mColorRadius.val[0] <= 255) ? mBlobHsv.val[0] +
                                                                   mColorRadius.val[0] : 255;
    mLowerBound.val[0] = minH;
    mUpperBound.val[0] = maxH;
    mLowerBound.val[1] = mBlobHsv.val[1] - mColorRadius.val[1];
    mUpperBound.val[1] = mBlobHsv.val[1] + mColorRadius.val[1];
    mLowerBound.val[2] = mBlobHsv.val[2] - mColorRadius.val[2];
    mUpperBound.val[2] = mBlobHsv.val[2] + mColorRadius.val[2];
    mLowerBound.val[3] = 0;
    mUpperBound.val[3] = 255;
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

//使用輪廓檢測
void findHandsContoursByCamera(Mat *cvMat, JNIEnv *env) {
    Mat detectMat;
    cvMat->copyTo(detectMat);
    GaussianBlur(detectMat, detectMat, Size(3, 3), 1.1);
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
//    pyrDown(detectMat, mPyrDownMat);
//    pyrDown(mPyrDownMat, mPyrDownMat);
    cvtColor(detectMat, mHsvMat, COLOR_RGB2HSV_FULL);
    inRange(mHsvMat, mLowerBound, mUpperBound, mMask);
    dilate(mMask, mDilatedMask, Mat());
    findContours(mDilatedMask, contours, mHierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    Rect rect;
    int maxWidth = 0;
    int maxHeight = 0;
    for (int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        //計算可以包含輪廓的最小長方形區域
        Rect tempRect = boundingRect(Mat(contours_poly[i]));
        if (tempRect.width * tempRect.height > maxWidth * maxHeight) {
            rect = tempRect;
        }
    }
    int detectWidth = detectMat.rows / 2;
    int detectHeight = detectMat.cols / 2;
    if (rect.area() > 1000) {
        Scalar color = Scalar(255, 0, 0);
        drawContours(*cvMat, contours_poly, 1, color, 1, 8, vector<Vec4i>(), 0, Point());
        rectangle(*cvMat, rect.tl(), rect.br(), color, 2, 8, 0);
    }
}

//使用CC API
void ccByCamera(Mat *cvMat, JNIEnv *env) {
    Mat hsv;
    Mat dst;
    Mat r1;
    cvtColor(*cvMat, hsv, CV_RGB2HSV);
    inRange(hsv, mLowerBound, mUpperBound, r1);
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
                    ifInit = 0;
                } else {
                    double dx = outputCenter.at<double>(label, 0);
                    double dy = outputCenter.at<double>(label, 1);
                    int moveXDistance = (int) (dx - startPoint.x);
                    int moveYDistance = (int) (dy - startPoint.y);
                    if (moveXDistance > camWidth / 2 & moveYDistance < camHeight / 2) {
                        //right
                        handsMove(env, 4);
                    } else if (moveXDistance < -camWidth / 2 & moveYDistance < camHeight / 2) {
                        //left
                        handsMove(env, 5);
                    } else if (moveXDistance < camWidth / 2 & moveYDistance > camHeight / 2) {
                        //down
                        handsMove(env, 6);
                    } else if (moveXDistance < camWidth / 2 & moveYDistance < -camHeight / 2) {
                        //up
                        handsMove(env, 7);
                    } else {
                        positionX = (int) dx;
                        positionY = (int) dy;
                        moveX = moveXDistance;
                        moveY = moveYDistance;
                        handsMove(env, 8);
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

void Java_com_baobomb_handsdetection_HandsDetecter_setSkinColor(
        JNIEnv *env,
        jobject, jlong nativeMat, jint screenWidth, jint screenHeight, jint screenCenterX,
        jint screenCenterY) {
    Mat *cvMat = (Mat *) nativeMat;
    setSkinColor(cvMat, screenWidth, screenHeight, screenCenterX, screenCenterY);
//    connectComponent(cvMat, empty);
}

void Java_com_baobomb_handsdetection_HandsDetecter_detectFromCamera(
        JNIEnv *env,
        jobject, jlong nativeMat) {
    Mat *cvMat = (Mat *) nativeMat;
    findHandsContoursByCamera(cvMat, env);
//    ccByCamera(cvMat, env);
//    connectComponent(cvMat, empty);
}
}