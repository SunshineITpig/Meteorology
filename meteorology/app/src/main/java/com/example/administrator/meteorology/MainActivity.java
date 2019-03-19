package com.example.administrator.meteorology;

import android.content.Context;
import android.net.DhcpInfo;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import com.example.administrator.meteorology.hotspot.WifiApAdmin;
import com.example.administrator.meteorology.socketadmin.Client;
import com.example.administrator.meteorology.socketadmin.DisplayMesage;
import com.example.administrator.meteorology.socketadmin.Server;


import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.net.ServerSocket;
import java.net.Socket;


public class MainActivity extends AppCompatActivity implements DisplayMesage {
    /**
     * Called when the activity is first created.
     */
    TextView content;
    Button mBtn1, mBtn2,mBtn3,mBtn4;
    Context context;
    EditText edit1;
    WifiApAdmin wifiAp;
//    String serverAddress;


    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        //
        setContentView(R.layout.activity_main);
        content = (TextView) this.findViewById(R.id.text);
        mBtn1 = (Button) findViewById(R.id.button1);
        mBtn2 = (Button) findViewById(R.id.button2);
        mBtn3 = (Button) findViewById(R.id.button3);
        context = this;
        edit1 = (EditText) findViewById(R.id.edit);

        //

    }

    public void onClick(View view) {
        if (view.getId() == R.id.button1) {
            WifiManager wifiManage = (WifiManager) getSystemService(Context.WIFI_SERVICE);
            DhcpInfo info = wifiManage.getDhcpInfo();
            String serverAddress = intToIp(info.serverAddress);
            String editcontent = edit1.getText().toString();
            if (editcontent.equals(null)){
                editcontent = "5";//
            }
            final int timedata = Integer.parseInt(editcontent);
            Client client = new Client(serverAddress,this){
                @Override
                public void logic(){
                    try{
                        Client.writeTime(timedata,out);
                    }catch (IOException e){
                        e.printStackTrace();
                    }
                }
            };
            client.start();
        }else if (view.getId() == R.id.button2) {
            WifiManager wifiManage = (WifiManager) getSystemService(Context.WIFI_SERVICE);
            DhcpInfo info = wifiManage.getDhcpInfo();
            String serverAddress = intToIp(info.serverAddress);

            Client client = new Client(serverAddress, this){
                @Override
                public void logic(){
                    try{
                         float[] res;
                         res = Client.getData(out,in);
                        for (int i =0; i<res.length; i++){
                            console.displayMesage(String.valueOf(res[i]));
                        }
                    }catch (Exception ex){
                        ex.printStackTrace();
                    }
                }
            };
            client.start();
        } else if (view.getId() == R.id.button3) {
            Server service = new Server(this);
            service.start();
        }else if (view.getId() == R.id.button4) {
            wifiAp = new WifiApAdmin(context);
            wifiAp.startWifiAp(Constant.HOST_SPOT_SSID, Constant.HOST_SPOT_PASS_WORD);
        }
    }

    //解决跨线程调用UI的问题
    Handler handler = new Handler();
    StringBuffer strBuffer = new StringBuffer();
    //实现DisplayMesage接口
    @Override
    public void displayMesage( final String msg) {
        Runnable r = new Runnable() {
            public void run() {
                if (strBuffer.length() > 1024) {
                    strBuffer.delete(0, 100);
                }
                strBuffer.append(msg + "\n");
                content.setText(strBuffer.toString());
                content.invalidate();
            }
        };
        handler.post(r);
    }

    //将获取的int转为真正的ip地址
    private String intToIp(int i) {
        return (i & 0xFF) + "." + ((i >> 8) & 0xFF) + "." + ((i >> 16) & 0xFF) + "." + ((i >> 24) & 0xFF);
    }

}




