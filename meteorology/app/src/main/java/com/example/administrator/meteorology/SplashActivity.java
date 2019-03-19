package com.example.administrator.meteorology;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Window;
import android.view.WindowManager;

import java.util.Timer;
import java.util.TimerTask;

public class SplashActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        //设置为无标题栏
        supportRequestWindowFeature(Window.FEATURE_NO_TITLE);//继承AppCompatActivity
        //设置为全屏模式
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
                WindowManager.LayoutParams.FLAG_FULLSCREEN);
        //加载界面
        setContentView(R.layout.activity_splash);
        //创建Timer对象
        Timer timer = new Timer();
        //创建TimerTask对象
        TimerTask timerTask = new TimerTask() {
            @Override
            public void run() {
                Intent intent = new Intent(SplashActivity.this, MainActivity.class);
                startActivity(intent);
                finish();
            }
        };
        //使用timer.schedule（）方法调用timerTask，定时3秒后执行run
        timer.schedule(timerTask, 1000);
    }
}
