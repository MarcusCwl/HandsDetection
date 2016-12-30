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

    public void setHandsRectMat(Mat holeMat, Mat skinMat) {
        setHandsRectMat(holeMat.getNativeObjAddr(), skinMat.getNativeObjAddr());
    }

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

}
