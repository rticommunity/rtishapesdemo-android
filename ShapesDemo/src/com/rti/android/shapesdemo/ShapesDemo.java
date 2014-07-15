/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
package com.rti.android.shapesdemo;

import com.rti.android.shapesdemo.ShapesObject.*;

import android.app.Activity;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Handler;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.CompoundButton;
import android.widget.CheckBox;
import android.widget.RadioButton;

/**
 * The main Activity started for ShapesDemo
 * 
 * Handles all of the button actions and has a periodic event to
 * redraw the canvas.
 */
public class ShapesDemo extends Activity implements SensorEventListener
{
	
	private DrawView drawView;

	private Button publishButton;
	private Button subscribeButton;
	private CheckBox pauseCheckBox;
	private CheckBox accelCheckBox;
	
	private SensorManager sensorManager;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		
		// Use the activity_shapes_demo to layout the graphics
		setContentView(R.layout.activity_shapes_demo);

		// This is the drawing canvas for the shapes
		drawView = (DrawView) findViewById(R.id.DrawView);

		// All of the action buttons
		publishButton = (Button) findViewById(R.id.buttonPublish);
		subscribeButton = (Button) findViewById(R.id.buttonSubscribe);
		pauseCheckBox = (CheckBox) findViewById(R.id.checkboxPause);
		accelCheckBox = (CheckBox) findViewById(R.id.checkboxAccel);

		// What to do when the "Publish" button is clicked
		publishButton.setOnClickListener(new View.OnClickListener()
		{
			public void onClick(View v)
			{
				ShapeKind shapeType;
				CheckBox checkbox;

				// First determine which shape is selected
				if (((RadioButton) findViewById(R.id.radioSquare)).isChecked()) {
					shapeType = ShapeKind.SQUARE;
				} else if (((RadioButton) findViewById(R.id.radioCircle))
				        .isChecked()) {
					shapeType = ShapeKind.CIRCLE;
				} else {
					shapeType = ShapeKind.TRIANGLE;
				}

				/* Then for all colors that have been checked create a publication
				 * Reset state of color checkbox when done.
				 */
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxPurple)).isChecked()) {
					drawView.addPublication(shapeType, "PURPLE");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxBlue)).isChecked()) {
					drawView.addPublication(shapeType, "BLUE");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxRed)).isChecked()) {
					drawView.addPublication(shapeType, "RED");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxGreen)).isChecked()) {
					drawView.addPublication(shapeType, "GREEN");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxYellow)).isChecked()) {
					drawView.addPublication(shapeType, "YELLOW");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxCyan)).isChecked()) {
					drawView.addPublication(shapeType, "CYAN");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxMagenta)).isChecked()) {
					drawView.addPublication(shapeType, "MAGENTA");
					checkbox.setChecked(false);
				}
				if ((checkbox = (CheckBox) findViewById(R.id.checkboxOrange)).isChecked()) {
					drawView.addPublication(shapeType, "ORANGE");
					checkbox.setChecked(false);
				}
			}
		});

		// What to do when the "subscribe" button is clicked
		subscribeButton.setOnClickListener(new View.OnClickListener()
		{
			public void onClick(View v)
			{
				ShapeKind shapeType;

				// First determine which shape is selected
				if (((RadioButton) findViewById(R.id.radioSquare)).isChecked()) {
					shapeType = ShapeKind.SQUARE;
				} else if (((RadioButton) findViewById(R.id.radioCircle)).isChecked()) {
					shapeType = ShapeKind.CIRCLE;
				} else {
					shapeType = ShapeKind.TRIANGLE;
				}

				// Now add a subscription for that shape
				drawView.addSubscription(shapeType);
			}
		});
		
		// What to do when the "Pause" button is clicked
		pauseCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
		{
			public void onCheckedChanged(CompoundButton b, boolean isChecked)
			{
				drawView.isPaused(isChecked);
			}
		});
		
		// What to do when the "Accel" button is clicked
		accelCheckBox.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener()
		{
			public void onCheckedChanged(CompoundButton b, boolean isChecked)
			{
				drawView.isAccel(isChecked);
			}
		});
		
		// Register a listener with the accelerometer sensor
		sensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);
		sensorManager.registerListener(this, 
				sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER),
				SensorManager.SENSOR_DELAY_NORMAL);		
	}

	// Sensor callback
	public void onAccuracyChanged(Sensor sensor, int accuracy)
	{
		// nothing to do
	}
	
	// Sensor callback
	public void onSensorChanged(SensorEvent event)
	{
		// If is accelerometer
		if ( event.sensor.getType() == Sensor.TYPE_ACCELEROMETER) 
		{	
			// Only if "Accel" is selected
			if (accelCheckBox.isChecked())
			{
				drawView.setAccX(event.values[0]);
				drawView.setAccY(event.values[1]);	
			}
		}
	}
	
	@Override
	protected void onPostCreate(Bundle savedInstanceState)
	{
		super.onPostCreate(savedInstanceState);

		// Kickoff a periodically executed function, 33 ms or about 30 Hz
		periodicHandler.postDelayed(periodicRunnable, 33);
	}

	// Code to setup a periodically executed function
	Handler periodicHandler = new Handler();
	Runnable periodicRunnable = new Runnable()
	{
		@Override
		public void run()
		{
			// should cause the canvas to be redrawn
			drawView.invalidate();
			// call self again in 33 ms
			periodicHandler.postDelayed(periodicRunnable, 33);
		}
	};

	/* code below is to handle the Start and Stop DDS menu options */
	private MenuItem startDDSMenuItem;
	private MenuItem stopDDSMenuItem;
	
	@Override
	public boolean onCreateOptionsMenu(Menu menu)
	{
		getMenuInflater().inflate(R.menu.menu, menu);
		
		startDDSMenuItem = menu.findItem(R.id.menu_start);
		stopDDSMenuItem = menu.findItem(R.id.menu_stop);
		
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item)
	{
		switch (item.getItemId()) {
			case R.id.menu_settings:
				startActivity(new Intent(this, ShapesSettings.class));
				return true;
				
			/* Start DDS is selected */	
			case R.id.menu_start:

				/* start DDS */
				if (drawView.startDDS())
				{
					/* if started, disable the "Start DDS" menu item,
					 * enable the "Stop DDS" menu item and "Publish"
					 * and "Subscribe" buttons
					 */
					startDDSMenuItem.setEnabled(false);
					stopDDSMenuItem.setEnabled(true);
					publishButton.setEnabled(true);
					subscribeButton.setEnabled(true);
				}
				return true;
				
	        /* Stop DDS is selected */	
			case R.id.menu_stop:

				/* stop DDS */
				if (drawView.stopDDS())
				{
					/* if stopped, enable the "Start DDS" menu item,
					 * disable the "Stop DDS" menu item and "Publish"
					 * and "Subscribe" buttons
					 */
					startDDSMenuItem.setEnabled(true);
					stopDDSMenuItem.setEnabled(false);
					publishButton.setEnabled(false);
					subscribeButton.setEnabled(false);
				}
				return true;
				
			default:
				return false;
		}
	}
	
}
