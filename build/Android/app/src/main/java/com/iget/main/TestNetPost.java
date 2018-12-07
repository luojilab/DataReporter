package com.iget.main;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.iget.datareporter.DataReporter;
import com.iget.datareporter.IReport;

import java.util.Random;

public class TestNetPost implements IReport {

    private int mNativeReporter;
    private Object lock[] = new Object[0];
    private Handler mUiHandler = new Handler(Looper.getMainLooper());

    public TestNetPost() {
    }

    public void setNativeReporter(int nativeReporter) {
        synchronized (lock) {
            mNativeReporter = nativeReporter;
        }
    }

    @Override
    public void upload(final long key, final String[] data) {

        //模拟网络上报
        mUiHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                synchronized (lock) {
                    if (mNativeReporter == 0) {
                        return;
                    }
                    Log.d("DataReporter_java","uploadFailed" + this);
                    DataReporter.uploadFailed(mNativeReporter, key);
                }
            }
        }, 100);

    }
}
