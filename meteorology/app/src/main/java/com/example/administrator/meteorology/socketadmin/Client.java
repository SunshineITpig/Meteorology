package com.example.administrator.meteorology.socketadmin;

import com.example.administrator.meteorology.Constant;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.net.Socket;
import java.net.UnknownHostException;

/**
 * 客户端
 * Created by Administrator on 2018/4/18 0018.
 */
public abstract class Client extends Thread {

    public String serverIp;
    public DisplayMesage console;
    public String editcontent;

    public DataOutputStream out;
    public DataInputStream in;

    public Client(String serverAddress, String editcontent,DisplayMesage console) {
        super();
        serverIp = serverAddress;
        this.console = console;
        this.editcontent = editcontent;
    }

    public Client(String serverAddress,DisplayMesage console) {
        super();
        serverIp = serverAddress;
        this.console = console;
    }

    public  abstract void logic();

    public void run() {
        try{
            Socket clientSocket = new Socket(serverIp, 8004);
            console.displayMesage("创建客户端成功");
            in = new DataInputStream(clientSocket.getInputStream());
            out = new DataOutputStream(clientSocket.getOutputStream());

            logic();

            in.close();
            out.close();
            clientSocket.close();
        } catch (UnknownHostException e) {
            e.printStackTrace();
            console.displayMesage("异常");
        } catch (IOException e) {
            e.printStackTrace();
            console.displayMesage("异常");
        }
    }

    public static void writeTime(int timedata, DataOutputStream out) throws IOException {
        out.writeUTF(Constant.HEAD_REQUEST);// 写包头
        out.writeInt(Constant.CMD_TIME);// 写命令号
        out.writeInt(4);// 写长度
        out.writeInt(timedata);// 写数据
        out.flush();
    }

    public static float[] getData( DataOutputStream out, DataInputStream in) throws IOException {

        out.writeUTF(Constant.HEAD_REQUEST);// 写包头
        out.writeInt(Constant.CMD_REQUEST);// 写命令号
        out.flush();

        String head = in.readUTF();// 读包头
        int Cmd = in.readInt();// 读命令号
        int Len = in.readInt();// 读长度

        float[] result= new float[Len/4];
        if(head.equals(Constant.HEAD_RESPONSES) && Cmd ==Constant.CMD_RESPONSES && Len == 16){
            for(int j =0;j<4; j++){
                result[j] = in.readFloat();// 读数据
            }
        }
        return result;
    }

}
