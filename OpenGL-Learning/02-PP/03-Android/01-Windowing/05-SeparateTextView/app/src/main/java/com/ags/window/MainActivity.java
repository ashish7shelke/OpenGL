package com.ags.window;

//Pacjkages for full screen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;
import android.content.pm.ActivityInfo;
import com.ags.window.MyTextView;
import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity; 

import android.graphics.Color;



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

		//Do Landscape
		setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

		//Setting background color to black
		getWindow().getDecorView().setBackgroundColor(Color.BLACK);

		MyTextView myTextView = new MyTextView(this);

		setContentView(myTextView);
	}

}
/*
1. gradlew.bat build ==> Build Code & APK
2. adb -d install -r D:\RTR\android\01-IDE\app\build\outputs\apk\debug\app-debug.apk   ==> Install apk on phone 
3. gradlew.bat  clean  ==> To clean folder (Remove Build folder)
 */
