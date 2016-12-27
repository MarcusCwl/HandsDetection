package com.baobomb.handsdetection;

import org.opencv.core.Mat;

/**
 * Created by LEAPSY on 2016/12/27.
 */

public interface CameraPreviewMatCallBack {
    void onCallBack(Mat yuv);
}
