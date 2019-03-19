package com.example.administrator.meteorology.hotspot;

/**
 * Created by Administrator on 2018/4/4 0004.
 */

import java.io.BufferedReader;
import java.io.FileReader;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.Timer;
import java.util.TimerTask;
import android.content.Context;
import android.net.wifi.WifiConfiguration;
import android.net.wifi.WifiManager;
import android.util.Log;

/**
 * 热点管理
 */
public class WifiApAdmin {
    public static final String TAG = "WifiApAdmin";

    //关闭热点
    public static void closeWifiAp(Context context) {
        WifiManager wifiManager = (WifiManager) context.getSystemService(Context.WIFI_SERVICE);//实例化
        closeWifiAp(wifiManager);
    }

    private WifiManager mWifiManager = null;
    private Context mContext = null;

    public WifiApAdmin(Context context) {
        //构造时要先关闭原有热点
        mContext = context;
        mWifiManager = (WifiManager) mContext.getSystemService(Context.WIFI_SERVICE);
        closeWifiAp(mWifiManager);
    }

    private String mSSID = "";
    private String mPasswd = "";
    public void startWifiAp(String ssid, String passwd) {
        mSSID = ssid;
        mPasswd = passwd;

        //关闭WiFi
        if (mWifiManager.isWifiEnabled()) {
            mWifiManager.setWifiEnabled(false);
        }

        //开启热点
        stratWifiAp();

        //设置检查任务
        MyTimerCheck timerCheck = new MyTimerCheck() {
            @Override
            public void doTimerCheckWork() {
                if (isWifiApEnabled(mWifiManager)) {
                    Log.v(TAG, "Wifi enabled success!");
                    //成功之后不再检查，退出任务
                    this.exit();
                } else {
                    Log.v(TAG, "Wifi enabled failed!");
                }
            }

            @Override
            public void doTimeOutWork() {
                this.exit();
            }
        };

        //检查15次WiFi热点是否成功
        timerCheck.start(15, 1000);

    }

    public void stratWifiAp() {
        Method method = null;
        try {
            method = mWifiManager.getClass().getMethod("setWifiApEnabled", WifiConfiguration.class, boolean.class);
            WifiConfiguration netConfig = new WifiConfiguration();

            netConfig.SSID = mSSID;
            netConfig.preSharedKey = mPasswd;

            netConfig.allowedAuthAlgorithms.set(WifiConfiguration.AuthAlgorithm.OPEN);
            netConfig.allowedProtocols.set(WifiConfiguration.Protocol.RSN);
            netConfig.allowedProtocols.set(WifiConfiguration.Protocol.WPA);
            netConfig.allowedKeyManagement.set(WifiConfiguration.KeyMgmt.WPA_PSK);
            netConfig.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.CCMP);
            netConfig.allowedPairwiseCiphers.set(WifiConfiguration.PairwiseCipher.TKIP);
            netConfig.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.CCMP);
            netConfig.allowedGroupCiphers.set(WifiConfiguration.GroupCipher.TKIP);

            method.invoke(mWifiManager, netConfig, true);

        } catch (IllegalArgumentException e) {
            e.printStackTrace();
        } catch (IllegalAccessException e) {
            e.printStackTrace();
        } catch (InvocationTargetException e) {
            e.printStackTrace();
        } catch (SecurityException e) {
            e.printStackTrace();
        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        }
    }

    private static void closeWifiAp(WifiManager wifiManager) {
        if (isWifiApEnabled(wifiManager)) {
            try {
                Method method = wifiManager.getClass().getMethod("getWifiApConfiguration");
                method.setAccessible(true);

                WifiConfiguration config = (WifiConfiguration) method.invoke(wifiManager);

                Method method2 = wifiManager.getClass().getMethod("setWifiApEnabled", WifiConfiguration.class, boolean.class);
                method2.invoke(wifiManager, config, false);
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (IllegalArgumentException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            }
        }
    }

    private static boolean isWifiApEnabled(WifiManager wifiManager) {
        try {
            Method method = wifiManager.getClass().getMethod("isWifiApEnabled");
            method.setAccessible(true);
            return (Boolean) method.invoke(wifiManager);

        } catch (NoSuchMethodException e) {
            e.printStackTrace();
        } catch (Exception e) {
            e.printStackTrace();
        }

        return false;
    }

    public String getConnectedHotIP() {
        String connectedIP = "";
        try {
            BufferedReader br = new BufferedReader(new FileReader(
                    "/proc/net/arp"));
            String line;
            while ((line = br.readLine()) != null) {
                String[] splitted = line.split(" +");
                if (splitted != null && splitted.length >= 4) {
                    String ip = splitted[0];
                    connectedIP = ip;
                }
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        return connectedIP;
    }

}