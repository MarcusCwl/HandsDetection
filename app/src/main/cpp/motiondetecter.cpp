#include <jni.h>
#include <string>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <android/log.h>

#define LOG_TAG "HandsDetection"
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__))

using namespace std;
using namespace cv;
jclass cls;
jmethodID mid;
Rect detectRect = Rect();
Point minLoc;
double minVal;

Point startPoint = Point(0, 0);
int ifInit = 1;

void handsMove(JNIEnv *env, int moveType) {
    cls = env->FindClass("com/baobomb/handsdetection/MotionDetecter");
    //再找类中的方法
    mid = env->GetStaticMethodID(cls, "handsMove", "(Ljava/lang/String;)V");
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
    }
    ifInit = 1;
}

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

Mat firstFrame;
Mat secondFrame;
Mat foreground;
Mat backgroundMat;
Rect motionRect = Rect();
Scalar color = Scalar(255, 0, 0);

void setBackground(Mat *background) {
    background->copyTo(backgroundMat);
}

void checkScale(JNIEnv *env, Mat foreground) {
    int frameWidth = foreground.cols;
    int frameHeight = foreground.rows;
    int minDetectArea = foreground.rows / 2 * foreground.cols / 2;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(foreground, contours, hierarchy,
                 CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));
    //在threshold_img影像中尋找所有的輪廓
    //多邊形偵測
//    vector<vector<Point> > hull(contours.size());
//    for (size_t i = 0; i < contours.size(); i++) {
//        convexHull(Mat(contours[i]), hull[i], false);
//    }
//    for (size_t i = 0; i < contours.size(); i++) {
//        Scalar color = Scalar(255, 0, 0);
////        drawContours(foreground, contours, (int) i, color, 1, 8, vector<Vec4i>(), 0, Point());
//        if (hull[i].size() > 7 && hull[i].size() < 10) {
//            drawContours(foreground, hull, (int) i, color, 1, 8, vector<Vec4i>(), 0, Point());
//        }
//    }

    //矩形偵測
    vector<vector<Point> > contours_poly(contours.size());
    vector<Rect> boundRect(contours.size());
    motionRect = Rect();
    motionRect.width = 0;
    motionRect.height = 0;

    for (int i = 0; i < contours.size(); i++) {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        //計算可以包含輪廓的最小長方形區域

        boundRect[i] = boundingRect(Mat(contours_poly[i]));

        if (motionRect.height * motionRect.width <
            boundRect[i].height * boundRect[i].width) {
            if (boundRect[i].area() < 280000) {
                motionRect = boundRect[i];
            }
        }
//        if (boundRect[i].height > boundRect[i].width) {
//            if (motionRect.height * motionRect.width <
//                boundRect[i].height * boundRect[i].width) {
//                motionRect = boundRect[i];
//            }
//        }
    }
    if (motionRect.area() > minDetectArea) {
        __android_log_print(ANDROID_LOG_DEBUG, "TAG", "area %d", motionRect.area());
        if (ifInit) {
            startPoint.x = motionRect.tl().x + (motionRect.width / 2);
            startPoint.y = motionRect.tl().y + (motionRect.height / 2);
            ifInit = 0;
        } else {
            int centerPointX = motionRect.tl().x + (motionRect.width / 2);
            int centerPointY = motionRect.tl().y + (motionRect.height / 2);
            int moveXDistance = (centerPointX - startPoint.x);
            int moveYDistance = (centerPointY - startPoint.y);
            if (moveXDistance > frameWidth / 3 & moveYDistance < frameHeight / 3) {
                //right
                handsMove(env, 4);
            } else if (moveXDistance < -frameWidth / 3 & moveYDistance < frameHeight / 3) {
                //left
                handsMove(env, 5);
            } else if (moveXDistance < frameWidth / 3 & moveYDistance > frameHeight / 3) {
                //down
                handsMove(env, 6);
            } else if (moveXDistance < frameWidth / 3 & moveYDistance < -frameHeight / 3) {
                //up
                handsMove(env, 7);
            }
        }
        rectangle(foreground, motionRect.tl(), motionRect.br(), color, 2, 8, 0);
    }
}


void detectMotionEvent(JNIEnv *env, Mat *firstMat, Mat *secondMat, Mat *outputMat) {
    //動態檢測
    firstMat->copyTo(firstFrame);
    secondMat->copyTo(secondFrame);
//    threshold(firstFrame, firstFrame, 150, 200, THRESH_BINARY);
//    threshold(secondFrame, secondFrame, 150, 200, THRESH_BINARY);
    absdiff(firstFrame, secondFrame, foreground);
    threshold(foreground, foreground, 10, 50, THRESH_BINARY);
    dilate(foreground, foreground, Mat());
    checkScale(env, foreground);
    foreground.copyTo(*outputMat);


    //相同區域檢測
//    Mat result;
//    result.create(firstMat->rows - secondMat->rows + 1, firstMat->cols - secondMat->cols + 1,
//                  CV_32FC1);
//    matchTemplate(*firstMat, *secondMat, result, CV_TM_SQDIFF);
//    minMaxLoc(result, &minVal, 0, &minLoc, 0);
//    int a = (int)minVal;
//    __android_log_print(ANDROID_LOG_INFO, "sometag", "test int = %d", a);
//    if (minVal < 100000000) {
//        rectangle(*outputMat, minLoc, Point(minLoc.x + secondMat->cols, minLoc.y + secondMat->rows),
//                  Scalar::all(0), 3);
//    }
}

extern "C" {

void Java_com_baobomb_handsdetection_MotionDetecter_motiondetect(
        JNIEnv *env,
        jobject, jlong firstMat, jlong secondMat, jlong outputMat) {
    Mat *firstFrame = (Mat *) firstMat;
    Mat *secondFrame = (Mat *) secondMat;
    Mat *outputFrame = (Mat *) outputMat;
    detectMotionEvent(env, firstFrame, secondFrame, outputFrame);
//    motionEventCallback(env);
}

void Java_com_baobomb_handsdetection_MotionDetecter_setBackground(
        JNIEnv *env,
        jobject, jlong backgound) {
    Mat *backgroundMat = (Mat *) backgound;
    setBackground(backgroundMat);
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