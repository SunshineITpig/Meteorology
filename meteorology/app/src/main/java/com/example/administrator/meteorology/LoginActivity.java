package com.example.administrator.meteorology;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.Toast;

public class LoginActivity extends AppCompatActivity {

    private EditText et_account;
    private EditText et_psw;
    private Button btn_login;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_login);

        et_account = (EditText)findViewById(R.id.main_et_account);
        et_psw = (EditText)findViewById(R.id.main_et_password);
        btn_login = (Button)findViewById(R.id.main_btn_login);
        btn_login.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String username = et_account.getText().toString();
                String password = et_psw.getText().toString();
                if (username.equals("admin") && password.equals("123456")){
                    Intent intent = new Intent(LoginActivity.this, MainActivity.class);
                    startActivity(intent);
                    finish();
                }else {
                    Toast.makeText(LoginActivity.this, "账号或密码错误", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }
}
