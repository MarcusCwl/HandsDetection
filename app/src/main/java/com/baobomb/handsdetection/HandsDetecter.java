package com.baobomb.handsdetection;

import android.content.Context;
import android.util.Log;
import android.widget.Toast;

import org.opencv.core.Mat;

/**
 * Created by chenweilun on 2016/12/25.
 */

public class HandsDetecter {
    static HandsDetecterCallBack handsDetecterCallBack;

    public HandsDetecter(HandsDetecterCallBack handsDetecterCallBack) {
        this.handsDetecterCallBack = handsDetecterCallBack;
    }

    static {
        System.loadLibrary("handsdetecter");
    }

    public void detectFromThermal(Mat nativeMat, Mat empty) {
        detectFromThermal(nativeMat.getNativeObjAddr(), empty.getNativeObjAddr());
    }

    public void detectFromCamera(Mat nativeMat) {
        detectFromCamera(nativeMat.getNativeObjAddr());
    }

    public static void handsMove(String msg) {
        Log.d("Bao", msg);
        handsDetecterCallBack.onMove();
    }

    public native void detectFromThermal(long nativeMat, long empty);

    public native void detectFromCamera(long nativeMat);
}
