package com.baobomb.handsdetection;

import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.ImageView;
import android.widget.TextView;

import com.baobomb.handsdetection.gpuimage.GPUImageView;

import org.opencv.android.Utils;
import org.opencv.core.Mat;

public class MainActivity extends AppCompatActivity {
    Mat nativeMat = new Mat();
    Mat empty = new Mat();
    HandsDetecter handsDetecter = new HandsDetecter();
    ThermalHandler thermalHandler;
    GPUImageView gpuImageView;
    GPUImageView detectImageView;
    // Used to load the 'native-lib' library on application startup.

    static {
        System.loadLibrary("opencv_java3");
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        gpuImageView = (GPUImageView) findViewById(R.id.gpuImageView);
        detectImageView = (GPUImageView) findViewById(R.id.detectImageView);
    }

    @Override
    protected void onStart() {
        super.onStart();
        thermalHandler = new ThermalHandler();
        thermalHandler.init();
        thermalHandler.setListener(new ThermalHandler.OnLeptonListener() {
            @Override
            public void onLeptonFrameReceive(Bitmap bitmap, int centerTem) {
                Utils.bitmapToMat(bitmap, nativeMat);
                handsDetecter.detect(nativeMat, empty);
                Bitmap dstBitmap = Bitmap.createBitmap(empty.cols(), empty.rows(), Bitmap.Config.ARGB_8888);
                Utils.matToBitmap(empty, dstBitmap);
                Bitmap nativeBitmap = Bitmap.createBitmap(nativeMat.cols(), nativeMat.rows(), Bitmap.Config.ARGB_8888);
                Utils.matToBitmap(nativeMat, nativeBitmap);
                detectImageView.setImage(dstBitmap);
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
}
