package com.baobomb.handsdetection;

import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.WindowManager;
import android.widget.Toast;

import com.baobomb.handsdetection.gpuimage.GPUImageView;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.core.CvType;
import org.opencv.core.Mat;

public class MatchLocActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2, View.OnTouchListener, MotionDetecterCallBack {
    MotionDetecter motionDetecter = new MotionDetecter(this);
    GPUImageView gpuImageView;
    GPUImageView detectImageView;
    CameraBridgeViewBase mOpenCvCameraView;
    boolean isCheckColor = false;
    Mat mrgba = new Mat();

    Mat empty = new Mat();
    Mat firstFrame;
    Mat secondFrame;
    int frameCount = 0;
    SensorUtil sensorUtil;

    int count = 0;
    // Used to load the 'native-lib' library on application startup.
    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Toast.makeText(MatchLocActivity.this, msg.obj.toString(), Toast.LENGTH_SHORT).show();
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
        setContentView(R.layout.activity_main);
        sensorUtil = new SensorUtil(this);
        gpuImageView = (GPUImageView) findViewById(R.id.gpuImageView);
        detectImageView = (GPUImageView) findViewById(R.id.detectImageView);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.cameraView);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.enableView();
//        handler.postDelayed(runnable, 1000);
    }

    @Override
    protected void onStart() {
        super.onStart();
    }

    @Override
    public void onCameraViewStarted(int width, int height) {
        mrgba = new Mat(height, width, CvType.CV_8UC4);
        empty = new Mat(height, width, CvType.CV_8UC4);
    }

    @Override
    public void onCameraViewStopped() {
//        mGray.release();
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        return false;
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
//        mrgba = inputFrame.rgba();
//        if (isCheckColor) {
//            Mat empty = new Mat();
//            handsDetecter.detectFromCamera(mrgba);
//        } else {
//            Point center = new Point(mrgba.cols() / 2, mrgba.rows() / 2);
//            Imgproc.circle(mrgba, center, 150, new Scalar(255, 255, 255));
//        }
//        return mrgba;
        if (frameCount == 0) {
            firstFrame = inputFrame.gray();
            sensorUtil.lockSensor();
            frameCount++;
        } else if (frameCount == 1) {
            secondFrame = inputFrame.gray();
            frameCount++;
        } else if (frameCount == 2) {
//            if (!sensorUtil.isMove()) {
//                motionDetecter.detectFromCam(firstFrame, secondFrame, empty);
//            }
            motionDetecter.detectFromCam(firstFrame, secondFrame, empty);
            frameCount = 0;
            firstFrame.release();
            secondFrame.release();
            sensorUtil.unLockSensor();
        }
        return empty;
    }
}
