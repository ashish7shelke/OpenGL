package com.ags.window;

//Pacjkages for full screen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity; 
import androidx.appcompat.widget.AppCompatTextView;
import android.graphics.Color;
import android.view.Gravity;


public class MainActivity extends AppCompatActivity 
{
	@Override 
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		//Full screen
		//Get and hide action bar
		getSupportActionBar().hide();
		//Do full screen
		WindowCompat.setDecorFitsSystemWindows(getWindow(), false);
		//Get window insets controller
		WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow(), getWindow().getDecorView());
		//Tell above object to hide system bars means sstatus bar, naviation bar and caption bar and IME(input method editor)
		windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());

		//Setting background color to black
		getWindow().getDecorView().setBackgroundColor(Color.BLACK);

		AppCompatTextView myTextView = new AppCompatTextView(this);
		myTextView.setTextColor(Color.rgb(0, 255, 0));
		myTextView.setGravity(Gravity.CENTER);
		myTextView.setText("Hello World !!!");
		myTextView.setTextSize(50);
		setContentView(myTextView);
	}

}
