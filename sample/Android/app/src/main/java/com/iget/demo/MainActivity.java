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
        //设置单次上报最大条数
        mDataReporter.setReportCount(10);
        //设置过期时间，0为不过期，多久的数据都上报，单位为秒
        mDataReporter.setExpiredTime(0);
        //设置上报间隔，1000表示 1秒报一次，单位毫秒
        mDataReporter.setReportingInterval(10 * 1000);
        //设置最大缓存大小 20k
        mDataReporter.setFileMaxSize(20 * 1024);
        //设置上报出错重试间隔，5代表上报出错后5秒后重试，如果再次上报失败，重试时间加5秒，也就是10秒。以此类推。单位秒。
        //如果设置为0，表示出错后立即上报，容易导致上报风暴，服务器打垮
        mDataReporter.setRetryInterval(5);
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
