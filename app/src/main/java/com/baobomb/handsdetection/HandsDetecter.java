package com.baobomb.handsdetection;

import android.util.Log;

import org.opencv.core.Mat;

/**
 * Created by chenweilun on 2016/12/25.
 */

public class HandsDetecter {
    static {
        System.loadLibrary("handsdetecter");
    }

    public void detect(Mat nativeMat, Mat empty){
        detect(nativeMat.getNativeObjAddr(),empty.getNativeObjAddr());
    }

    public static void matCallBack(long mat) {

    }

    public native void detect(long nativeMat,long empty);
}
