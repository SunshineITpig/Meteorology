package com.example.administrator.meteorology.socketadmin;

import com.example.administrator.meteorology.Constant;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.ServerSocket;
import java.net.Socket;

/**
 * Created by Administrator on 2018/4/18 0018.
 */
public class Server extends Thread {

    public DisplayMesage console;

    public ServerSocket socketService;
    public Socket sock;

    public DataOutputStream out;
    public DataInputStream in;
    float[]  data;
    public Server(DisplayMesage console) {
        super();
        this.console = console;
        try {
            socketService = new ServerSocket(8004);
        }catch (IOException e){
            e.printStackTrace();
        }
        console.displayMesage("服务器创建成功，等待连接");
        data = new float[4];
        Float q = new Float(12.34);
        for(int i =0; i<4; i++){
            data[i] = q;
        }
    }

    public void run() {
        try {


            String str = null;
            int Cmd = 0;
            int timedata = 0;
            do{
                sock = socketService.accept();
                console.displayMesage("连接服务器成功");
                in = new DataInputStream(sock.getInputStream());
                out = new DataOutputStream(sock.getOutputStream());

                str = in.readUTF();// 读包头
                if(str.equals(Constant.HEAD_REQUEST)){
                    Cmd = in.readInt();
                    switch (Cmd){
                        case Constant.CMD_TIME:
                            timedata = receiveTime(in);
                            console.displayMesage(String.valueOf(timedata));
                        case Constant.CMD_REQUEST:
                            responsedata(out,in,data);
                    }
                }
                console.displayMesage(str);
                console.displayMesage(String.valueOf(Cmd));


            }while(!str.equals(Constant.END));

            in.close();
            out.close();
            sock.close();
            socketService.close();
            console.displayMesage("服务器关闭");

        } catch (Exception e) {
            e.printStackTrace();
            try{
                in.close();
                out.close();
                sock.close();
                socketService.close();
                console.displayMesage("服务器关闭");
            }catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    public static int receiveTime(DataInputStream inputStream) throws IOException {

        int dataLength = inputStream.readInt();// 读长度
        int time = 0;
        if (dataLength != 0)
            time = inputStream.readInt();// 读时间
        return time;

    }

    public static void responsedata(DataOutputStream out, DataInputStream in, float[] res) throws IOException {

        out.writeUTF(Constant.HEAD_RESPONSES);// 写包头
        out.writeInt(Constant.CMD_RESPONSES);// 写命令号
        out.writeInt(16);// 写长度
        for(int k =0; k<4; k++){
            out.writeFloat(res[k]);// 写数据
        }
        out.flush();

    }

}
