package com.iget.main;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.iget.datareporter.DataReporter;
import com.iget.datareporter.IReport;

import java.util.Random;

public class NetPost implements IReport {


    private int mNativeReporter;
    private Object lock[] = new Object[0];
    private Handler mUiHandler = new Handler(Looper.getMainLooper());

    public NetPost() {
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

                int min = 0;
                int max = 10;
                Random random = new Random();
                int num = random.nextInt(max) % (max - min + 1) + min;

                //随机定义一个数值5 来模拟网络失败的情况
                if (num != 5) {
                    StringBuffer stringBuffer = new StringBuffer();
                    for (int i = 0; i < data.length; i++) {
                        stringBuffer.append(data[i]);
                    }
                    Log.d("DataReporter:data_", stringBuffer.toString());
                    synchronized (lock) {
                        if (mNativeReporter == 0) {
                            return;
                        }
                        DataReporter.uploadSucess(mNativeReporter, key);
                    }
                } else {
                    synchronized (lock) {
                        if (mNativeReporter == 0) {
                            return;
                        }
                        DataReporter.uploadFailed(mNativeReporter, key);
                        //DataReporter.reaWaken(mNativeReporter);
                    }
                }

            }
        }, 100);

    }
}
