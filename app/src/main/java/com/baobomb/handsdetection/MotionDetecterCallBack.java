package com.baobomb.handsdetection;

/**
 * Created by LEAPSY on 2016/12/27.
 */

public interface MotionDetecterCallBack {
    void onMove(String modeType);

    void onMove(String modeType, int moveX, int moveY, int positionX, int positionY);
}
