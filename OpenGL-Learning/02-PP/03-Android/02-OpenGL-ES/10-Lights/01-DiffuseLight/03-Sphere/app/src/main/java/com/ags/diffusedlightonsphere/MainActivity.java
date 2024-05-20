package com.ags.diffusedlightonsphere;

//Pacjkages for full screen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;
import android.content.pm.ActivityInfo;

import com.ags.diffusedlightonsphere.GLESView;


import android.os.Bundle;
import androidx.appcompat.app.AppCompatActivity; 
import android.graphics.Color;

public class MainActivity extends AppCompatActivity 
{
	private GLESView glesView = null;

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

		glesView = new GLESView(this);

		setContentView(glesView);
	}

	@Override
	protected void onPause()
	{
		super.onPause();
	}

	@Override
	protected void onResume()
	{
		super.onResume();
	}

}
/*
1. gradlew.bat build ==> Build Code & APK
2. adb -d install -r D:\RTR\android\01-IDE\app\build\outputs\apk\debug\app-debug.apk   ==> Install apk on phone 
3. gradlew.bat  clean  ==> To clean folder (Remove Build folder)

To create seperate Apps follow below steps :- 
Method 1:
1) Go to android\02-Program-Name\app and edit build.gradle, add App name in  namespace and applicationId. 
2) Go to android\02-Program-Name\app\src\main\java\com\Your_Folder_Name\ and rename folder to your desired name 
3) Edit android\02-Program-Name\app\src\main\java\com\Your_Folder_Name\MainActivity.java and provide your desired package name 
4) Go to android\02-Program-Name\app\src\main\res\values\ and edit strings.xml and add your desired App Name

Method 2:

 */
