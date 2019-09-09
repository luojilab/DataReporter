package com.iget.main;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.iget.datareporter.DataReporter;
import com.iget.datareporter.IReport;

import java.util.Random;

public class NetPost implements IReport {

    private long mNativeReporter;
    private Object lock[] = new Object[0];
    private Handler mUiHandler = new Handler(Looper.getMainLooper());

    public NetPost() {
    }

    public void setNativeReporter(long nativeReporter) {
        synchronized (lock) {
            mNativeReporter = nativeReporter;
        }
    }

    @Override
    public void upload(final long key, final byte[][] data) {

        //模拟网络上报
        mUiHandler.postDelayed(new Runnable() {
            @Override
            public void run() {

                int min = 0;
                int max = 10;
                Random random = new Random();
                int num = random.nextInt(max) % (max - min + 1) + min;

                //随机定义一个数值5 来模拟网络失败的情况
                //if (num != 5) {
                StringBuffer stringBuffer = new StringBuffer();
                for (int i = 0; i < data.length; i++) {
                    String oneData = new String(data[i]);
                    Log.d("DataReporter:data_", oneData);
                    //stringBuffer.append(oneData);
                }

                synchronized (lock) {
                    if (mNativeReporter == 0) {
                        return;
                    }
                    DataReporter.uploadSucess(mNativeReporter, key);
                }
//                } else {
//                    synchronized (lock) {
//                        if (mNativeReporter == 0) {
//                            return;
//                        }
//                        DataReporter.uploadFailed(mNativeReporter, key);
//                        //DataReporter.reaWaken(mNativeReporter);
//                    }
//                }

//                StringBuffer stringBuffer = new StringBuffer();
//                for (int i = 0; i < data.length; i++) {
//                    stringBuffer.append(data[i]);
//                }
//
//                DataReporter.uploadSucess(mNativeReporter, key);
//                long t = System.currentTimeMillis() / 1000;
//                Log.d("DataReporter:data_", stringBuffer.toString() + " time:" + t);
            }
        }, 0);

    }
}
