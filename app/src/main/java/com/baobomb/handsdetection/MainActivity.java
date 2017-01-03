package com.baobomb.handsdetection;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

import com.baobomb.handsdetection.gpuimage.GPUImageView;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

public class MainActivity extends AppCompatActivity implements MotionDetecterCallBack, CameraBridgeViewBase.CvCameraViewListener2, View.OnTouchListener {
    MotionDetecter motionDetecter = new MotionDetecter(this);
    CameraBridgeViewBase mOpenCvCameraView;
    SensorUtil sensorUtil;

    //動態檢測-固定背景
//    Mat backgroundMat;
//    Mat foregroundMat;
//    Mat outputMat;
//    boolean isBackgroundSet = false;

    //動態檢測-即時更新背景
    Mat firstMat;
    Mat secondMat;
    Mat outputMat;
    int frameCount = 0;

    //相同區域檢測
//    Mat gray = new Mat();
//    Mat skinMat = new Mat();
//    Mat outputGray = new Mat();
//    boolean isHandsRectSet = false;
//    boolean isHandsRectSetting = false;
//    int frameCount = 0;

    // Used to load the 'native-lib' library on application startup.
    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Toast.makeText(MainActivity.this, msg.obj.toString(), Toast.LENGTH_SHORT).show();
        }
    };

    static {
        System.loadLibrary("opencv_java3");
    }

    @Override
    public void onMove(String moveType) {
        Message message = Message.obtain(handler);
        message.obj = moveType;
        message.sendToTarget();
    }

    @Override
    public void onMove(String modeType, int moveX, int moveY, int positionX, int positionY) {
        Log.d("Move", modeType + "MoveX : " + moveX + " MoveY : " + moveY + " PositionX : " + positionX + " PositionY : " + positionY);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        setContentView(R.layout.activity_match);
        sensorUtil = new SensorUtil(this);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.cameraView);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.setOnTouchListener(this);
        mOpenCvCameraView.enableView();
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        //動態檢測
        outputMat = new Mat(height, width, CvType.CV_8UC4);

        //相同區域檢測
//        gray = new Mat(height, width, CvType.CV_8UC4);
//        outputGray = new Mat(height, width, CvType.CV_8UC4);
    }

    @Override
    public void onCameraViewStopped() {
//        mGray.release();
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        //相同區域檢測
//        isHandsRectSetting = true;
        return false;
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        //動態檢測-固定背景
//        if (!isBackgroundSet) {
//            backgroundMat = inputFrame.gray();
//            motionDetecter.setBackground(backgroundMat);
//            isBackgroundSet = true;
//        } else {
//            foregroundMat = inputFrame.gray();
//            motionDetecter.detectFromCam(backgroundMat, foregroundMat, outputMat);
//            return outputMat;
//        }
//        return outputMat;
        //動態檢測-即時更新背景
        if (frameCount == 0) {
            firstMat = inputFrame.gray();
            sensorUtil.lockSensor();
            frameCount++;
        } else if (frameCount == 1) {
            secondMat = inputFrame.gray();
            if (!sensorUtil.isMove()) {
                motionDetecter.detectFromCam(firstMat, secondMat, outputMat);
                return outputMat;
            }
            frameCount = 0;
            sensorUtil.unLockSensor();
        }
        return outputMat;

        //相同區域檢測
//        gray = inputFrame.gray();
//        if (isHandsRectSet) {
//            if (frameCount >= 0) {
//                motionDetecter.detectFromCam(gray, skinMat, outputGray);
//                frameCount = 0;
//            } else {
//                frameCount++;
//            }
//        } else if (isHandsRectSetting) {
//            outputGray = inputFrame.gray();
//            motionDetecter.setHandsRectMat(gray, skinMat);
//            isHandsRectSet = true;
//            isHandsRectSetting = false;
//        } else {
//            outputGray = inputFrame.gray();
//            motionDetecter.setHandsDetectRect(gray);
//        }
//        return outputGray;
    }
}
