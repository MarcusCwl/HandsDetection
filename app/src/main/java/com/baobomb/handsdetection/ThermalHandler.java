package com.baobomb.handsdetection;

import android.graphics.Bitmap;
import android.util.Log;

import com.baobomb.handsdetection.Singleton;

import org.opencv.android.Utils;
import org.opencv.core.Core;
import org.opencv.core.Mat;

import tw.ade.hamlet.rkthermallib.ThermalLib;

/**
 * Created by LEAPSY on 2016/12/14.
 */

public class ThermalHandler implements ThermalLib.LeptonListener {

    private ThermalLib mLeptonLib = null;
    private OnLeptonListener onLeptonListener;

    static {
        System.loadLibrary("opencv_java3");
    }

    public ThermalHandler() {

    }

    public void init() {
        Singleton.getInstance().thermalHandler = this;
        mLeptonLib = new ThermalLib();
        mLeptonLib.setListener(this);
        mLeptonLib.startLepton();
    }

    public void setListener(OnLeptonListener listener) {
        this.onLeptonListener = listener;
    }

    public void removeListener() {
        this.onLeptonListener = null;
    }

    public void stopLepton() {
        if (mLeptonLib != null) {
            mLeptonLib.stopLepton();
        }
    }

    @Override
    public void onFrameReceived(Bitmap bitmap, int[] temperature) {
            Bitmap bitmap1 = Bitmap.createBitmap(60, 80, Bitmap.Config.ARGB_8888);
            Mat mat = new Mat();
            Mat dst = new Mat();
            Mat dst2 = new Mat();
            Utils.bitmapToMat(bitmap, mat);
            Core.transpose(mat, dst);
            Core.flip(dst, dst2, 1);
            Utils.matToBitmap(dst2, bitmap1);
            //get center temperature
            int iX, iY;
            int iThermalW, iThermalH;
            iThermalW = 80;
            iThermalH = 60;
            iX = iThermalW / 2;
            iY = iThermalH / 2;
            int iCenterTemper = temperature[2440];
            if (onLeptonListener != null) {
                Log.d("BAO", iCenterTemper + "");
                onLeptonListener.onLeptonFrameReceive(bitmap1, iCenterTemper);

        }
    }

    public static abstract class OnLeptonListener {
        public abstract void onLeptonFrameReceive(Bitmap bitmap, int centerTem);
    }
}
