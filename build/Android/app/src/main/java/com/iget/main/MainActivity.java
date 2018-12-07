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
    private int mNativeReporter = 0;

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

                final NetPost netPost = new NetPost();
                mNativeReporter = DataReporter.makeReporter("test", MainActivity.this.getFilesDir().getPath(), netPost);
                netPost.setNativeReporter(mNativeReporter);
                DataReporter.setReportCount(mNativeReporter, 10);
                DataReporter.setFileMaxSize(mNativeReporter, 2 * 1024);
                DataReporter.setExpiredTime(mNativeReporter, 0 * 1000);
                DataReporter.setReportingInterval(mNativeReporter, 0 * 1000);
                DataReporter.start(mNativeReporter);

                for (int i = 0; i < 5000; i++) {
                    DataReporter.push(mNativeReporter, "{\"bid\":1919,\"bname\":\"《领导力21法则：如何培养领袖气质》\",\"progress\":\"97.674416\",\"time\":\"1544164416\",\"progress_title\":\"词汇表\"}" + i);
                }
            }
        });


        mButtonTestRelease.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                int testCount = 100;
                final List<Integer> reporters = new ArrayList<>(testCount);
                final List<TestNetPost> netPosts = new ArrayList<>(testCount);
                for (int i = 0; i < testCount; i++) {
                    final TestNetPost netPost = new TestNetPost();
                    int nativeReporter = DataReporter.makeReporter("testRelease", MainActivity.this.getFilesDir().getPath() + "/test_release" + i, netPost);
                    reporters.add(nativeReporter);
                    netPosts.add(netPost);
                    netPost.setNativeReporter(nativeReporter);
                    DataReporter.setReportCount(nativeReporter, 10);
                    DataReporter.setFileMaxSize(nativeReporter, 2 * 1024);
                    DataReporter.setExpiredTime(nativeReporter, 0 * 1000);
                    DataReporter.start(nativeReporter);
                    for (int j = 0; j < 1000; j++) {
                        DataReporter.push(nativeReporter, " business id:" + i + " test_data: 10040106100401061004010610040106100401061004010610040106100401061004010610040106100401061004010610040106 data id: " + j);
                    }
                }

                Handler handler = new Handler(Looper.getMainLooper());
                handler.postDelayed(new Runnable() {
                    @Override
                    public void run() {
                        for (TestNetPost netPost1 : netPosts) {
                            netPost1.setNativeReporter(0);
                        }

                        for (Integer one : reporters) {
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
