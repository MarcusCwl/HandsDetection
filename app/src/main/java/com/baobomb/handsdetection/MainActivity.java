package com.baobomb.handsdetection;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import com.baobomb.handsdetection.gpuimage.GPUImageView;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.Utils;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

public class MainActivity extends AppCompatActivity implements CameraBridgeViewBase.CvCameraViewListener2, HandsDetecterCallBack {
    Mat nativeMat = new Mat();
    HandsDetecter handsDetecter = new HandsDetecter(this);
    ThermalHandler thermalHandler;
    GPUImageView gpuImageView;
    GPUImageView detectImageView;
    CameraBridgeViewBase mOpenCvCameraView;
    // Used to load the 'native-lib' library on application startup.
    Handler handler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            Toast.makeText(MainActivity.this, "偵測到手部移動", Toast.LENGTH_SHORT).show();
        }
    };

    static {
        System.loadLibrary("opencv_java3");
    }

    @Override
    public void onMove() {
        Message message = Message.obtain(handler);
        message.arg1 = 0;
        message.sendToTarget();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        gpuImageView = (GPUImageView) findViewById(R.id.gpuImageView);
        detectImageView = (GPUImageView) findViewById(R.id.detectImageView);
        mOpenCvCameraView = (CameraBridgeViewBase) findViewById(R.id.cameraView);
        mOpenCvCameraView.setCvCameraViewListener(this);
        mOpenCvCameraView.enableView();
    }

    @Override
    protected void onStart() {
        super.onStart();
        thermalHandler = new ThermalHandler();
        thermalHandler.init();
        thermalHandler.setListener(new ThermalHandler.OnLeptonListener() {
            @Override
            public void onLeptonFrameReceive(Bitmap bitmap, int centerTem) {
                Mat empty = new Mat();
                Utils.bitmapToMat(bitmap, nativeMat);
                handsDetecter.detectFromThermal(nativeMat, empty);
                Bitmap dstBitmap = Bitmap.createBitmap(empty.cols(), empty.rows(), Bitmap.Config.ARGB_8888);
                Utils.matToBitmap(empty, dstBitmap);
                Bitmap nativeBitmap = Bitmap.createBitmap(nativeMat.cols(), nativeMat.rows(), Bitmap.Config.ARGB_8888);
                Utils.matToBitmap(nativeMat, nativeBitmap);
//                detectImageView.setImage(dstBitmap);
                gpuImageView.setImage(nativeBitmap);
            }
        });
//        handsDetecter.detect(nativeMat, empty);
//        Bitmap dstBitmap = Bitmap.createBitmap(empty.cols(), empty.rows(), Bitmap.Config.ARGB_8888);
//        Utils.matToBitmap(empty,dstBitmap);
//        Bitmap dstBitmap = Bitmap.createBitmap(nativeMat.cols(),nativeMat.rows(), Bitmap.Config.ARGB_8888);
//        Utils.matToBitmap(nativeMat,dstBitmap);
//        imageView.setImageBitmap(dstBitmap);
    }

    @Override
    public void onCameraViewStarted(int width, int height) {

    }

    @Override
    public void onCameraViewStopped() {
//        mGray.release();
    }


    @Override
    public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame inputFrame) {
        Mat mRgba = inputFrame.rgba();
        Mat empty = new Mat();
        handsDetecter.detectFromCamera(mRgba);
        return mRgba;
    }
}
