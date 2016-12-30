package com.baobomb.handsdetection;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;

/**
 * Created by LEAPSY on 2016/11/16.
 */

public class SensorUtil implements SensorEventListener {

    private SensorManager sensorManager;
    private Sensor acc_sensor;
    private Context context;
    private float accValues[] = new float[3];
    private float lockAccValues[] = new float[3];

    public SensorUtil(Context context) {
        this.context = context;
        initSensor();
    }


    public void initSensor() {
        try {
            sensorManager = (SensorManager) context.getSystemService(Context.SENSOR_SERVICE);
            acc_sensor = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER);
            sensorManager.registerListener(this, acc_sensor, SensorManager.SENSOR_DELAY_GAME);
        } catch (Exception e) {
            Log.d("MRLauncher", "Lightsensor error : " + e.toString());
        }
    }

    public void unRegisterListener() {
        sensorManager.unregisterListener(this);
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int i) {

    }

    @Override
    public void onSensorChanged(SensorEvent sensorEvent) {
        if (sensorEvent.sensor.getType() == Sensor.TYPE_ACCELEROMETER) {
            accValues = sensorEvent.values;
        }
        Log.d("BAO", "ACC : " + accValues[0] + "," + accValues[1] + "," + accValues[2]);
    }

    public void lockSensor() {
        lockAccValues[0] = accValues[0];
        lockAccValues[1] = accValues[1];
        lockAccValues[2] = accValues[2];
    }

    public boolean isMove() {
        if (Math.abs(accValues[0] - lockAccValues[0]) > 0.2) {
            return true;
        }
        if (Math.abs(accValues[1] - lockAccValues[1]) > 0.2) {
            return true;
        }
        if (Math.abs(accValues[2] - lockAccValues[2]) > 0.2) {
            return true;
        }
        return false;
    }

    public void unLockSensor() {
        lockAccValues[0] = 0;
        lockAccValues[1] = 0;
        lockAccValues[2] = 0;
    }
}
