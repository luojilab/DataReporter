package com.iget.main;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Environment;
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

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import static android.net.ConnectivityManager.TYPE_WIFI;
import static android.provider.ContactsContract.CommonDataKinds.Email.TYPE_MOBILE;

public class MainActivity extends AppCompatActivity {

    private Button mButtonStart;
    private Button mButtonTestRelease;
    private Button mButtonCheckData;

    private IntentFilter mIntentFilter;
    private NetworkChangeReceiver mNetworkChangeReceiver;
    private long mNativeReporter = 0;
    private int mCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mButtonStart = (Button) findViewById(R.id.button_start);
        mButtonTestRelease = (Button) findViewById(R.id.button_test_release);
        mButtonCheckData = (Button) findViewById(R.id.button_check_push_data);
        //监听网络状态
        mIntentFilter = new IntentFilter();
        mIntentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
        mNetworkChangeReceiver = new NetworkChangeReceiver();
        registerReceiver(mNetworkChangeReceiver, mIntentFilter);
        mButtonStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                if (mNativeReporter == 0) {
                    final NetPost netPost = new NetPost();
                    mNativeReporter = DataReporter.makeReporter("test", MainActivity.this.getFilesDir().getPath(),"testKey", netPost);
                    netPost.setNativeReporter(mNativeReporter);
                    DataReporter.setReportCount(mNativeReporter, 10);
                    DataReporter.setFileMaxSize(mNativeReporter, 2 * 1024);
                    DataReporter.setExpiredTime(mNativeReporter, 0 * 1000);
                    DataReporter.setReportingInterval(mNativeReporter, 1 * 1000);
                    DataReporter.start(mNativeReporter);
                }
                long t = System.currentTimeMillis() / 1000;
                for (int i = 0; i < 50; i++) {
                    String data = "ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d" + " time:" + t + "count:" + mCount;

                    DataReporter.push(mNativeReporter, data.getBytes());
                    Log.d("DataReporter:push_","time:" + t + " count:" + mCount);
                    mCount++;
                }
            }
        });


        mButtonTestRelease.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int testCount = 100;
                final List<Long> reporters = new ArrayList<>(testCount);
                final List<TestNetPost> netPosts = new ArrayList<>(testCount);
                for (int i = 0; i < testCount; i++) {
                    final TestNetPost netPost = new TestNetPost();
                    long nativeReporter = DataReporter.makeReporter("testRelease", MainActivity.this.getFilesDir().getPath() + "/test_release" + i,"testKey", netPost);
                    reporters.add(nativeReporter);
                    netPosts.add(netPost);
                    netPost.setNativeReporter(nativeReporter);
                    DataReporter.setReportCount(nativeReporter, 10);
                    DataReporter.setFileMaxSize(nativeReporter, 2 * 1024);
                    DataReporter.setExpiredTime(nativeReporter, 0 * 1000);
                    DataReporter.setReportingInterval(nativeReporter, 1000 * 10);
                    DataReporter.start(nativeReporter);
                    for (int j = 0; j < 1000; j++) {
                        String data = " business id:" + i + " test_data: 10040106100401061004010610040106100401061004010610040106100401061004010610040106100401061004010610040106 data id: " + j;
                        DataReporter.push(nativeReporter, data.getBytes());
                    }
                }

                Handler handler = new Handler(Looper.getMainLooper());
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        for (TestNetPost netPost1 : netPosts) {
                            netPost1.setNativeReporter(0);
                        }

                        for (Long one : reporters) {
                            DataReporter.releaseReporter(one);
                        }
                    }
                }, 100000);

            }
        });

        mButtonCheckData.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });

    }

    class NetworkChangeReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (isNetOk(context)) {
                //网络状态好时 重新唤起下DataReporter
                if (mNativeReporter == 0) {
                    return;
                }
                DataReporter.reaWaken(mNativeReporter);
            }
        }
    }


    public static boolean isNetOk(Context context) {

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
        DataReporter.releaseReporter(mNativeReporter);
        mNativeReporter = 0;
    }
}
