package com.ags.window;

import androidx.appcompat.widget.AppCompatTextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

public class MyTextView extends AppCompatTextView
{
	public MyTextView(Context context)
	{
		super(context);
		
		setTextColor(Color.rgb(0, 255, 0));
		setTextSize(50);
		setGravity(Gravity.CENTER);
		setText("Hello World !!!");
	}
}
