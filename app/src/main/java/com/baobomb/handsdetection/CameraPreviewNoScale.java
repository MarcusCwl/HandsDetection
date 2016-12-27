package com.baobomb.handsdetection;

import android.content.Context;
import android.graphics.PixelFormat;
import android.graphics.YuvImage;
import android.hardware.Camera;
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.WindowManager;

import org.opencv.android.CameraBridgeViewBase;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.imgproc.Imgproc;

import java.io.IOException;

public class CameraPreviewNoScale extends SurfaceView implements SurfaceHolder.Callback, Camera.PreviewCallback {
    private static final String TAG = "CameraPreview";
    private static final boolean DEBUG = true;

    private final Context mContext;
    private final SurfaceHolder mHolder;
    CameraPreviewMatCallBack cameraPreviewMatCallBack;

    private Camera mCamera;
    private int cameraType = 1;

    public CameraPreviewNoScale(Context context) {
        super(context);
        mContext = context;
        mHolder = getHolder();
        mHolder.addCallback(this);
    }

    public void setCameraPreviewMatCallBack(CameraPreviewMatCallBack cameraPreviewMatCallBack) {
        this.cameraPreviewMatCallBack = cameraPreviewMatCallBack;
    }

    public void setCamera(int cameraType) {
        if (mCamera != null) {
            mCamera.stopPreview();
            mCamera.release();
            mCamera = null;
        }
        try {
            mCamera = Camera.open(cameraType);
            this.cameraType = cameraType;
            Camera.Parameters parameters = mCamera.getParameters();
            parameters.setPictureSize(80, 60);
            mCamera.setParameters(parameters);
            mCamera.setPreviewCallback(this);
            setCameraDisplayOrientation(mContext, 0, mCamera);
            mCamera.setPreviewDisplay(mHolder);
            mCamera.startPreview();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        if (DEBUG) Log.i(TAG, "surfaceCreated(SurfaceHolder");

        // The Surface has been created, now tell the camera where to draw the preview.
        try {
            mCamera.setPreviewCallback(this);
            mCamera.setPreviewDisplay(holder);
            mCamera.startPreview();
        } catch (IOException e) {
            Log.e(TAG, "Error setting camera preview: " + e.getMessage());
        }
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {

        if (mHolder.getSurface() == null) {
            // Preview surface does not exist.
            return;
        }

        // Stop preview before making changes.
        try {
            mCamera.stopPreview();
        } catch (Exception e) {
            // Tried to stop a non-existent preview, so ignore.
        }

        // TODO: don't hardcode cameraId '0' here... figure this out later.
        setCameraDisplayOrientation(mContext, 0, mCamera);

        // Start preview with new settings.
        try {
            mCamera.setPreviewDisplay(mHolder);
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        } catch (Exception e) {
            Log.d(TAG, "Error starting camera preview: " + e.getMessage());
        }
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        if (DEBUG) Log.i(TAG, "surfaceDestroyed(SurfaceHolder");
        // Take care of releasing the Camera preview in the floating window.
    }

    public void setCameraDisplayOrientation(Context context, int cameraId, Camera camera) {
        WindowManager wm = (WindowManager) context.getSystemService(Context.WINDOW_SERVICE);
        int rotation = wm.getDefaultDisplay().getRotation();

        int degrees;
        switch (rotation) {
            case Surface.ROTATION_0:
                degrees = 90;
                break;
            case Surface.ROTATION_90:
                degrees = 0;
                break;
            case Surface.ROTATION_180:
                degrees = 270;
                break;
            case Surface.ROTATION_270:
                degrees = 180;
                break;
            default:
                degrees = 90;
                break;
        }

        mCamera.setDisplayOrientation(degrees);
        Camera.CameraInfo info = new Camera.CameraInfo();
        Camera.getCameraInfo(cameraId, info);

//        int result;
//        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
//            result = (info.orientation + degrees) % 360;
//            // Compensate for the mirror image.
//            result = (360 - result) % 360;
//        } else {
//            // Back-facing camera.
//            result = (info.orientation - degrees + 360) % 360;
//        }
//        camera.setDisplayOrientation(result);
    }

    @Override
    public void onPreviewFrame(byte[] bytes, Camera camera) {
        int height = mCamera.getParameters().getPictureSize().height;
        int width = mCamera.getParameters().getPictureSize().width;
        if (cameraPreviewMatCallBack != null) {
            Mat mYuv = new Mat(60, 80, CvType.CV_8UC1);
            mYuv.put(0, 0, bytes);
        }
    }

    public void stop() {
        if (null == mCamera) {
            return;
        }
        mCamera.stopPreview();
        mCamera.release();
        mCamera = null;
    }


}
