package com.iget.main;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import com.iget.datareporter.DataReporter;
import com.iget.datareporter.IReport;

import java.util.Random;

public class TestNetPost implements IReport {

    private DataReporter mDataReporter;
    private Object lock[] = new Object[0];
    private Handler mUiHandler = new Handler(Looper.getMainLooper());

    public TestNetPost() {
    }

    public void setDataReporter(DataReporter dataReporter) {
        synchronized (lock) {
            mDataReporter = dataReporter;
        }
    }

    @Override
    public void upload(final long key, final byte[][] data) {

        //模拟网络上报
        mUiHandler.postDelayed(new Runnable() {
            @Override
            public void run() {
                synchronized (lock) {
                    if (mDataReporter == null) {
                        return;
                    }
                    //Log.d("DataReporter_java", "uploadFailed" + this);
                    mDataReporter.uploadFailed(key);
                    StringBuffer stringBuffer = new StringBuffer();
                    for (int i = 0; i < data.length; i++) {
                        String oneData = new String(data[i]);
                        //stringBuffer.append(oneData);
                        Log.d("DataReporter:data_", stringBuffer.toString());
                    }
                }
            }
        }, 0);

    }
}
