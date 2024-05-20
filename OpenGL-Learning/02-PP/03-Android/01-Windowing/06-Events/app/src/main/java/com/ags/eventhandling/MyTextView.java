package com.ags.eventhandling;

import androidx.appcompat.widget.AppCompatTextView;
import android.content.Context;
import android.graphics.Color;
import android.view.Gravity;

//Event related packages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

public class MyTextView extends AppCompatTextView implements OnGestureListener, OnDoubleTapListener
{
	private GestureDetector gestureDetector = null;

	public MyTextView(Context context)
	{
		super(context);
		
		setTextColor(Color.rgb(0, 255, 0));
		setTextSize(40);
		setGravity(Gravity.CENTER);
		setText("AGS - Hello World !!!");

		//Create and set Gesture Detector object
		this.gestureDetector = new GestureDetector(context, this, null, false);
		this.gestureDetector.setOnDoubleTapListener(this);
	}

	//Implementation of onTouvh Event of view class
	@Override
	public boolean onTouchEvent(MotionEvent e)
	{
		if(!gestureDetector.onTouchEvent(e))
		{
			super.onTouchEvent(e);
		}
		return true;
	}

	//Implementation of 3 methods onDoubleTapListener interface
	@Override
	public boolean onDoubleTap(MotionEvent e)
	{
		setText("!! AGS - Double Tap !!");
		return true;
	}

	@Override
	public boolean onDoubleTapEvent(MotionEvent e)
	{
		//Do nothing, as we have onDoubleTap method
		return true;
	}

	@Override
	public boolean onSingleTapConfirmed(MotionEvent e)
	{
		setText("! AGS - Single Tap !");
		return true;
	}

	//Impleemntation of 6 methods onGestureListener interface	
	@Override
	public boolean onDown(MotionEvent e)
	{
		//Do nothing, as we have onSingleTapConfirmed method
		return true;	
	}

	@Override
	public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY)
	{
		setText("AGS - Fling !!!");
		return true;
	}

	@Override
	public void onLongPress(MotionEvent motionEvent)
	{
		setText("AGS - Long Press !!!");
	}

	@Override
	public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY)
	{
		setText("AGS - Scrolling / Swiping !!!");
		return true;
	}

	@Override
	public void onShowPress(MotionEvent e)
	{

	}
	
	@Override
	public boolean onSingleTapUp(MotionEvent e)
	{
		return true;
	}

}
