package com.baobomb.handsdetection;

import android.util.Log;

import org.opencv.core.Mat;

/**
 * Created by LEAPSY on 2016/12/29.
 */

public class MotionDetecter {
    static MotionDetecterCallBack motionDetecterCallBack;

    static {
        System.loadLibrary("motiondetecter");
    }

    public MotionDetecter(MotionDetecterCallBack motionDetecterCallBack) {
        this.motionDetecterCallBack = motionDetecterCallBack;
    }


    public void detectFromCam(Mat firstMat, Mat secondMat, Mat outputMat) {
        motiondetect(firstMat.getNativeObjAddr(), secondMat.getNativeObjAddr(), outputMat.getNativeObjAddr());
    }


    public static void handsMove(String msg) {
        Log.d("Bao", msg);
        motionDetecterCallBack.onMove(msg);
    }

    //動態檢測-設置背景
    public void setBackground(Mat backgroundMat) {
        setBackground(backgroundMat.getNativeObjAddr());
    }

    //相同區域檢測-設置取得的手部區域
    public void setHandsRectMat(Mat holeMat, Mat skinMat) {
        setHandsRectMat(holeMat.getNativeObjAddr(), skinMat.getNativeObjAddr());
    }

    //相同區域檢測-設置手部檢測區域
    public void setHandsDetectRect(Mat holeMat) {
        setHandsDetectRect(holeMat.getNativeObjAddr());
    }

    public static void motionEvent(String msg) {
        Log.d("Bao", msg);
        motionDetecterCallBack.onMove(msg);
    }

    public native void motiondetect(long firstFrame, long secondMat, long outputMat);

    public native void setHandsDetectRect(long holeFrame);

    public native void setHandsRectMat(long holeFrame, long skinFrame);

    public native void setBackground(long background);

}
