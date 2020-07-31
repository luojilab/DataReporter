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
import android.widget.Toast;

import com.iget.data_reporter.R;
import com.iget.datareporter.DataReporter;
import com.iget.datareporter.IReport;

import java.util.ArrayList;
import java.util.List;
import java.util.Random;

import static android.net.ConnectivityManager.TYPE_WIFI;
import static android.provider.ContactsContract.CommonDataKinds.Email.TYPE_MOBILE;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {


    private IntentFilter mIntentFilter;
    private NetworkChangeReceiver mNetworkChangeReceiver;
    private DataReporter mDataReporter;
    private int mCount = 0;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        findViewById(R.id.button_start).setOnClickListener(this);
        findViewById(R.id.button_push).setOnClickListener(this);
        findViewById(R.id.button_test_release).setOnClickListener(this);
        findViewById(R.id.button_test_reweaken).setOnClickListener(this);

        //监听网络状态
        mIntentFilter = new IntentFilter();
        mIntentFilter.addAction("android.net.conn.CONNECTIVITY_CHANGE");
        mNetworkChangeReceiver = new NetworkChangeReceiver();
        registerReceiver(mNetworkChangeReceiver, mIntentFilter);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.button_start:
                if (mDataReporter == null) {
                    final NetPost netPost = new NetPost();
                    mDataReporter = DataReporter.makeDataReporter("test", MainActivity.this.getFilesDir().getPath() + "/data", "", netPost);
                    netPost.setDataReporter(mDataReporter);
                    //设置单次上报最大条数
                    mDataReporter.setReportCount(10);
                    //设置最大缓存大小 20k
                    mDataReporter.setFileMaxSize(20 * 1024);
                    //设置过期时间，0为不过期，多久的数据都上报，单位为秒
                    mDataReporter.setExpiredTime(0 * 1000);
                    //设置上报间隔，1000表示 1秒报一次，单位毫秒
                    mDataReporter.setReportingInterval(1 * 1000);
                    //设置上报出错重试间隔，5代表上报出错后5秒后重试，如果再次上报失败，重试时间加5秒，也就是10秒。以此类推。单位秒。
                    //如果设置为0，表示出错后立即上报，容易导致上报风暴，服务器打垮
                    mDataReporter.setRetryInterval(5);
                    mDataReporter.start();
                }
                break;
            case R.id.button_push:
                if (mDataReporter != null) {
                    long t = System.currentTimeMillis() / 1000;
                    for (int i = 0; i < 1000; i++) {
                        String data = "ev=s_paid_paid_impression&uid=12005419&scr=1080*2214&t=1547627349367082203&seid=dd86a82b76722c24427b9db1fb462a4d&net=wifi&mac=c6abbef9f4bea0a0&sid=dd86a82b76722c24427b9db1fb462a4d" + " time:" + t + "count:" + mCount;

                        mDataReporter.push(data.getBytes());
                        Log.d("DataReporter:push_", "time:" + t + " count:" + mCount);
                        mCount++;
                    }
                }
                break;
            case R.id.button_test_release:
                int testCount = 100;
                final List<DataReporter> reporters = new ArrayList<>(testCount);
                final List<TestNetPost> netPosts = new ArrayList<>(testCount);
                for (int i = 0; i < testCount; i++) {
                    final TestNetPost netPost = new TestNetPost();
                    DataReporter dataReporter = DataReporter.makeDataReporter("testRelease", MainActivity.this.getFilesDir().getPath() + "/test_release" + i, "testKey", netPost);
                    reporters.add(dataReporter);
                    netPosts.add(netPost);
                    netPost.setDataReporter(dataReporter);
                    dataReporter.setReportCount(10);
                    dataReporter.setFileMaxSize(2 * 1024);
                    dataReporter.setExpiredTime(0 * 1000);
                    dataReporter.setReportingInterval(1000 * 10);
                    dataReporter.start();
                    for (int j = 0; j < 1000; j++) {
                        String data = " business id:" + i + " test_data: 10040106100401061004010610040106100401061004010610040106100401061004010610040106100401061004010610040106 data id: " + j;
                        dataReporter.push(data.getBytes());
                    }
                }

                Handler handler = new Handler(Looper.getMainLooper());
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        for (TestNetPost netPost1 : netPosts) {
                            netPost1.setDataReporter(null);
                        }

                        for (DataReporter one : reporters) {
                            DataReporter.releaseDataReporter(one);
                        }
                    }
                }, 100000);
                break;
            case R.id.button_test_reweaken:
                mDataReporter.reaWaken();
                break;
        }
    }

    class NetworkChangeReceiver extends BroadcastReceiver {

        @Override
        public void onReceive(Context context, Intent intent) {
            if (isNetOk(context)) {
                //网络状态好时 重新唤起下DataReporter
                if (mDataReporter == null) {
                    return;
                }
                mDataReporter.reaWaken();
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
        //DataReporter.releaseDataReporter(mDataReporter);
        mDataReporter = null;
    }
}
