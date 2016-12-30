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
    Mat skinMat = new Mat();
    Mat gray = new Mat();
    Mat outputGray = new Mat();
    boolean isHandsRectSet = false;
    boolean isHandsRectSetting = false;
    int frameCount = 0;
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
        gray = new Mat(height, width, CvType.CV_8UC4);
        outputGray = new Mat(height, width, CvType.CV_8UC4);
    }

    @Override
    public void onCameraViewStopped() {
//        mGray.release();
    }

    @Override
    public boolean onTouch(View view, MotionEvent motionEvent) {
        isHandsRectSetting = true;
        return false;
    }

    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        gray = inputFrame.gray();

        if (isHandsRectSet) {
            if (frameCount >= 0) {
                motionDetecter.detectFromCam(gray, skinMat, outputGray);
                frameCount = 0;
            } else {
                frameCount++;
            }
        } else if (isHandsRectSetting) {
            outputGray = inputFrame.gray();
            motionDetecter.setHandsRectMat(gray, skinMat);
            isHandsRectSet = true;
            isHandsRectSetting = false;
        } else {
            outputGray = inputFrame.gray();
            motionDetecter.setHandsDetectRect(gray);
        }
//        if (isCheckColor) {
//            Mat empty = new Mat();
//            handsDetecter.detectFromCamera(mrgba);
//        } else {
//            Point center = new Point(mrgba.cols() / 2, mrgba.rows() / 2);
//            Imgproc.circle(mrgba, center, 150, new Scalar(255, 255, 255));
//        }
//        return mrgba;
        return outputGray;
    }
}
