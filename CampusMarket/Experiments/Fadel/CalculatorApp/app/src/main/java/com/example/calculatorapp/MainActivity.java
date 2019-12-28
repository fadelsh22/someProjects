package com.example.calculatorapp;

import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import androidx.appcompat.app.AppCompatActivity;

/**
 * @author fadelalshammasi
 */
public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        Button mul=(Button)findViewById((R.id.mulBtn));
        mul.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText firstNum=(EditText)findViewById(R.id.firstNumEditText);
                EditText secondNum=(EditText)findViewById(R.id.secondNumEditText);

                TextView res=(TextView)findViewById(R.id.resultTextView);
                int num1=Integer.parseInt(firstNum.getText().toString());
                int num2=Integer.parseInt(secondNum.getText().toString());
                int result=num1 * num2;

                res.setText(result+"");



            }
        });


        Button add=(Button)findViewById(R.id.addBtn);
        add.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText firstNum=(EditText)findViewById(R.id.firstNumEditText);
                EditText secondNum=(EditText)findViewById(R.id.secondNumEditText);

                TextView res=(TextView)findViewById(R.id.resultTextView);

                int num1=Integer.parseInt(firstNum.getText().toString());
                int num2=Integer.parseInt(secondNum.getText().toString());
                int result=num1+num2;

                res.setText(result+"");


            }

        });

        Button sub=(Button)findViewById(R.id.subBtn);
        sub.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText firstNum=(EditText)findViewById(R.id.firstNumEditText);
                EditText secondNum=(EditText)findViewById(R.id.secondNumEditText);

                TextView res=(TextView)findViewById(R.id.resultTextView);
                int num1=Integer.parseInt(firstNum.getText().toString());
                int num2=Integer.parseInt(secondNum.getText().toString());
                int result=num1 - num2;

                res.setText(result+"");
            }
        });

        Button div=(Button)findViewById((R.id.divBtn));
        div.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                EditText firstNum=(EditText)findViewById(R.id.firstNumEditText);
                EditText secondNum=(EditText)findViewById(R.id.secondNumEditText);

                TextView res=(TextView)findViewById(R.id.resultTextView);
                double num1=Double.parseDouble(firstNum.getText().toString());
                double num2=Double.parseDouble(secondNum.getText().toString());
                double result=num1 / num2;

                res.setText(result+"");
            }
        });


    }
}
