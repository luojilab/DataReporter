package com.iget.demo;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Handler;
import android.os.Looper;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;

import com.iget.data_reporter.R;
import com.iget.datareporter.DataReporter;
import com.iget.datareporter.IReport;

import java.util.Random;

public class MainActivity extends AppCompatActivity {

    private Button mButtonStart;
    private IntentFilter mIntentFilter;
    private NetworkChangeReceiver mNetworkChangeReceiver;
    private DataReporter mDataReporter;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mButtonStart = (Button) findViewById(R.id.button_start);

        //监听网络状态
        mIntentFilter = new IntentFilter();
        mIntentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
        mNetworkChangeReceiver = new NetworkChangeReceiver();
        registerReceiver(mNetworkChangeReceiver, mIntentFilter);

        final ReportImp reportImp = new ReportImp();
        mDataReporter = DataReporter.makeDataReporter("test", MainActivity.this.getFilesDir().getPath(), "testKey", reportImp);
        reportImp.setDataReporter(mDataReporter);
        mDataReporter.setReportCount(10);
        //0表示数据永久有效
        mDataReporter.setExpiredTime(0);
        //10秒报一次
        mDataReporter.setReportingInterval(10 * 1000);
        mDataReporter.setFileMaxSize(2 * 1024);
        mDataReporter.start();
        mButtonStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                for (int i = 0; i < 5000; i++) {
                    String data = new String("{\"progress\":\"11.59\",\"action\":\"pause\",\"uid\":\"10040106\",\"time\":\"1542107284.35\",\"alias_id\":\"6soKIdxYbmmHov0yYs8z\"}");
                    mDataReporter.push(data.getBytes());
                }
            }
        });
    }

    static class ReportImp implements IReport {
        private DataReporter mDataReporter;
        private Handler mUiHandler = new Handler(Looper.getMainLooper());

        public ReportImp() {
        }

        public void setDataReporter(DataReporter dataReporter) {
            mDataReporter = dataReporter;
        }

        @Override
        public void upload(final long key, final byte[][] data) {

            //模拟网络上报
            mUiHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (mDataReporter == null) {
                        return;
                    }

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
                        mDataReporter.uploadSucess(key);
                    } else {
                        mDataReporter.uploadFailed(key);
                        //DataReporter.reaWaken(mNativeReporter);
                    }

                }
            }, 100);

        }
    }

    class NetworkChangeReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (checkNet(context)) {
                //网络状态好时 重新唤起下DataReporter
                if (mDataReporter == null) {
                    return;
                }
                mDataReporter.reaWaken();
            }
        }
    }


    public static boolean checkNet(Context context) {

        try {
            ConnectivityManager connectivity = (ConnectivityManager) context.getSystemService(Context.CONNECTIVITY_SERVICE);
            if (connectivity != null) {

                NetworkInfo info = connectivity.getActiveNetworkInfo();
                if (info != null && info.isConnected()) {

                    if (info.isConnected()) {
                        return true;
                    }
                }
            }
        } catch (Exception e) {
        }
        return false;
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        unregisterReceiver(mNetworkChangeReceiver);
        //取消数据上报，并且把上报对象置空，防止释放之后再次被调用出现crash
        DataReporter.releaseDataReporter(mDataReporter);
        mDataReporter = null;
    }
}
