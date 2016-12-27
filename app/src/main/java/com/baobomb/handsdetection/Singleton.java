package com.baobomb.handsdetection;

/**
 * Created by LEAPSY on 2016/12/27.
 */

public class Singleton {
    private static Singleton m_Instance;
    public ThermalHandler thermalHandler;

    public Singleton() {
        super();
        m_Instance = this;
    }

    public static Singleton getInstance() {
        if (m_Instance == null) {
            synchronized (Singleton.class) {
                if (m_Instance == null) new Singleton();
            }
        }
        return m_Instance;
    }
}
