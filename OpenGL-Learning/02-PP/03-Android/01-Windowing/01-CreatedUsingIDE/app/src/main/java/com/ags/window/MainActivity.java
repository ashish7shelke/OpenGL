package com.ags.window;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity; 
import android.widget.TextView;

public class MainActivity extends AppCompatActivity 
{
	@Override 
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);

		TextView myTextView = new TextView(this);
		myTextView.setText("Hello World !!!");


		setContentView(myTextView);
	}

}
