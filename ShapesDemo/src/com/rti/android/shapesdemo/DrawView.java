/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
package com.rti.android.shapesdemo;

import com.rti.dds.infrastructure.RETCODE_NO_DATA;
import com.rti.dds.infrastructure.ResourceLimitsQosPolicy;
import com.rti.dds.subscription.*;
import com.rti.android.shapesdemo.idl.*;
import com.rti.android.shapesdemo.ShapesDDS;
import com.rti.android.shapesdemo.util.Vector2d;
import com.rti.android.shapesdemo.ShapesObject.ShapeKind;

import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.preference.PreferenceManager;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;


import java.util.*;

/* Class that manages shapes that have been published and subscribed to as well as 
 * the canvas on which the shapes are drawn.
 */
public class DrawView extends View
{
	/* Handle to functions to send/receive data via DDS */
	private ShapesDDS shapesDDS = null;

	private Paint paintCanvasBorder;

	/* Used to manage the drag/drop algorithm */
	private ShapesObject dragObject = null;
	private float dragStartX;
	private float dragStartY;
	private long dragStartTime;
	private Vector2d dragVector = new Vector2d(0, 0);
	private final float SPEED_DAMPING_FACTOR = 50;
	private final float SPEED_MAX = 25;
	private final float WIDTH = 250;
	private final float HEIGHT = 250;
	
	private boolean isPaused = false;
	private boolean isAccel = false;
	
	private float accX = 0;
	private float accY = 0;

	/* store all of the objects that are being published and subscribed */
	private Map<ShapeKind, Map<String, ShapesObject>> pubObjects;
	private Map<ShapeKind, Map<String, ShapesObject>> subObjects;
	private Map<ShapeKind, DataReader> shapeReaders;
	
	/* Used to access the preferences set via the "Settings" menu */
	private SharedPreferences settings;

	/* Constructor */
	public DrawView(Context context, AttributeSet attrs)
	{
		super(context, attrs);

		paintCanvasBorder = new Paint();

		paintCanvasBorder.setColor(Color.BLACK);
		paintCanvasBorder.setStrokeWidth(5);
		paintCanvasBorder.setStyle(Paint.Style.STROKE);

		pubObjects = new HashMap<ShapeKind, Map<String, ShapesObject>>();

		pubObjects.put(ShapeKind.SQUARE, new HashMap<String, ShapesObject>());
		pubObjects.put(ShapeKind.CIRCLE, new HashMap<String, ShapesObject>());
		pubObjects.put(ShapeKind.TRIANGLE, new HashMap<String, ShapesObject>());

		subObjects = new HashMap<ShapeKind, Map<String, ShapesObject>>();
		subObjects.put(ShapeKind.SQUARE, new HashMap<String, ShapesObject>());
		subObjects.put(ShapeKind.CIRCLE, new HashMap<String, ShapesObject>());
		subObjects.put(ShapeKind.TRIANGLE, new HashMap<String, ShapesObject>());

		shapeReaders = new HashMap<ShapeKind, DataReader>();
		
		/* So that the Eclipse editor can render the canvas */
		if (!isInEditMode()) {
			startDDS();
		}

	}
	
	/* Called to start DDS */
	public boolean startDDS()
	{
		if ( shapesDDS != null ) 
		{
			return true;
		}
		
		/* Get some settings from preference manager */
		settings = PreferenceManager.getDefaultSharedPreferences(getContext());
        int domainID = Integer.parseInt(settings.getString("domainID", "0"));
        String peerList = settings.getString("peerList", null);
        
		shapesDDS = new ShapesDDS();
		if (!shapesDDS.initialize(domainID, peerList))
		{
			shapesDDS = null;
			return false;
		}
		
		return true;
	}
	
	/* Called to stop DDS */
	public boolean stopDDS()
	{	
		/* Clear all objects that have been created previously */
		for (Map.Entry<ShapeKind, Map<String, ShapesObject>> entry : pubObjects.entrySet()) {
			pubObjects.get(entry.getKey()).clear();
		}
		for (Map.Entry<ShapeKind, Map<String, ShapesObject>> entry : subObjects.entrySet()) {
			subObjects.get(entry.getKey()).clear();
		}

		shapeReaders.clear();
		
		shapesDDS.stop();
		
		shapesDDS = null;
		
		return true;
	}

	/* Called to draw all objects on the canvas */
	private void drawObjects(Canvas canvas)
	{
		canvas.scale(getWidth() / WIDTH, getHeight() / HEIGHT);

		/* Iterate through and render and send the current location of published shapes */
		for (Map.Entry<ShapeKind, Map<String, ShapesObject>> entry : pubObjects
		        .entrySet()) {
			for (Map.Entry<String, ShapesObject> obj : entry.getValue()
			        .entrySet()) {
				// Can change size of objects on the go! 
				obj.getValue().setSize(Integer.parseInt(settings.getString("shapeSize", "30")));
				obj.getValue().draw(canvas, false);
				// ignore if in "pause" mode
				if ( !isPaused ) {
					// Send current location using DDS
					obj.getValue().write();
				}
			}
		}
		
		/* Iterate and render the current location of subscribed shapes */
		for (Map.Entry<ShapeKind, Map<String, ShapesObject>> entry : subObjects
		        .entrySet()) {
			for (Map.Entry<String, ShapesObject> obj : entry.getValue()
			        .entrySet()) {
				obj.getValue().draw(canvas, true);
			}
		}
	}

	
	/* Automatically called by UI thread when canvas needs to be redrawn */
	@Override
	public void onDraw(Canvas canvas)
	{
		/* Draw a border around the canvas */
		canvas.drawRect(0, 0, this.getHeight(), this.getWidth(), paintCanvasBorder);

		/* Calculate new location for published objects */
		moveObjects();
		
		drawObjects(canvas);
	}

	/* Implement dragging of shape objects for drawing canvas */
	@Override
	public boolean onTouchEvent(MotionEvent event)
	{
		// need to scale
		float eventX = event.getX() * WIDTH / getWidth();
		float eventY = event.getY() * HEIGHT / getHeight();

		/* User has pressed down on the canvas */
		if (event.getAction() == MotionEvent.ACTION_DOWN) {
			dragStartX = eventX;
			dragStartY = eventY;
			dragStartTime = System.currentTimeMillis();

			// See if touching an object?
			for (Map.Entry<ShapeKind, Map<String, ShapesObject>> entry : pubObjects
			        .entrySet()) {
				for (Map.Entry<String, ShapesObject> obj : entry.getValue()
				        .entrySet()) {
					// Is the object being "touched" by user?
					if (obj.getValue().contains(dragStartX, dragStartY)) {
						// If multiple objects are "touched" by user, the last object 
						// in the map selected as dragged object
						dragObject = obj.getValue();
					}
				}
			}
		}

		/* User is dragging */
		if (event.getAction() == MotionEvent.ACTION_MOVE) {

			/* Check if we are dragging an object */
			if (dragObject != null) {
				
				// Move object to dragged position
				dragObject.setPosition((int)eventX, (int)eventY);

				// if we've been dragging for more than 500 ms, reset our drag
				// info, so the final
				// velocity will more closely match the user's motion
				if (System.currentTimeMillis() - dragStartTime > 500) {
					dragStartX = eventX;
					dragStartY = eventY;
					dragStartTime = System.currentTimeMillis();
				}
			}

		}

		/* User has finished dragging */
		if (event.getAction() == MotionEvent.ACTION_UP) {

			// Check if dragging object
			if (dragObject != null) {
				long deltaTime = System.currentTimeMillis() - dragStartTime;

				// Calculate a drag vector (from start of drag to end of drag)
				dragVector.set((eventX - dragStartX) / deltaTime,
				        (eventY - dragStartY) / deltaTime);

				// Use length of vector to determine speed
				float speed = dragVector.getLength() * SPEED_DAMPING_FACTOR;

				if (speed > SPEED_MAX) {
					speed = SPEED_MAX;
				}

				// Use normalized drag vector as the new direction
				dragObject.setDirection(dragVector.normalize());
				dragObject.setSpeed(speed);
				dragObject.setPosition((int)eventX, (int)eventY);

				dragObject = null;
			}
		}

		return true;
	}

	/* Called to calculate new position of published objects based on 
	 * their speed and direction 
	 */
	private void moveObjects()
	{
		// ignore if in "pause" mode
		if ( isPaused ) {
			return;
		}

		// Iterate through published objects
		for (Map.Entry<ShapeKind, Map<String, ShapesObject>> entry : pubObjects
		        .entrySet()) {
			for (Map.Entry<String, ShapesObject> element : entry.getValue()
			        .entrySet()) {

				ShapesObject obj = element.getValue();

				// ignore if object is being dragged
				if ( obj == dragObject )
				{
					continue;
				}

				// if using Accelerometer, change speed and direction by acceleration
				// ignore if in not in "Accel" mode
				if ( isAccel ) 
				{
					float vx = obj.getSpeed()*obj.getDirection().getX() + accX;
					float vy = obj.getSpeed()*obj.getDirection().getY() + accY;
					float speed;
					
					obj.getDirection().set(vx, vy);
					speed = obj.getDirection().getLength();
					if (speed > SPEED_MAX) 
					{
						speed = SPEED_MAX;
					}
					obj.setSpeed(speed);
					obj.getDirection().normalize();
				}
				
				// Move by direction*speed
				float x = obj.getX();
				float y = obj.getY();
				float size = obj.getSize();
				float newX = x + obj.getSpeed()*obj.getDirection().getX();
				float newY = y + obj.getSpeed()*obj.getDirection().getY();
				float left = newX - (size / 2);
				float top = newY - (size / 2);
				float right = newX + (size / 2);
				float bottom = newY + (size / 2);

				// now check to see if we've bounced off the sides at all
				if (right > WIDTH) {
					// off the right side
					newX += WIDTH - right;
					obj.getDirection().setX(obj.getDirection().getX() * -1);
				} else if (left < 0) {
					// off the left side
					newX += 0 - left;
					obj.getDirection().setX(obj.getDirection().getX() * -1);
				}
				if (bottom > HEIGHT) {
					// off the bottom
					newY += HEIGHT - bottom;
					obj.getDirection().setY(obj.getDirection().getY() * -1);
				} else if (top < 0) {
					// off the top
					newY += 0 - top;
					obj.getDirection().setY(obj.getDirection().getY() * -1);
				}

				obj.setPosition((int)newX, (int)newY);
			}
		}
	}

	/* Call to add a shape to be published.  Only one object can be created for each unique
	 * shape/color combination.  If already created, then this is a noop.
	 */
	public void addPublication(ShapeKind kind, String color)
	{
		// Create object of shape/color if not created already
		if (pubObjects.get(kind).get(color) == null) {
			// Create writer 
			ShapeTypeDataWriter writer = shapesDDS.create_writer(kind);

			ShapesObject obj = new ShapesObject(kind, color, 30, writer);
			pubObjects.get(kind).put(color, obj);
		}
	}

	/* Internal class that implements a listener object that is called
	 * back for subscribed shapes.
	 */
	private class ShapesListener extends DataReaderAdapter
	{
		private ShapeKind kind;

		public ShapesListener(ShapeKind kind)
		{
			this.kind = kind;
		}

	    private ShapeTypeSeq _dataSeq = new ShapeTypeSeq();
	    private SampleInfoSeq _infoSeq = new SampleInfoSeq();
	    
		@Override
		public void on_data_available(DataReader reader)
		{		
			ShapeTypeDataReader shapeTypeReader = (ShapeTypeDataReader)reader;

	        try {
	            shapeTypeReader.take(
	                        _dataSeq, 
	                        _infoSeq,
	                        ResourceLimitsQosPolicy.LENGTH_UNLIMITED,
	                        SampleStateKind.ANY_SAMPLE_STATE,
	                        ViewStateKind.ANY_VIEW_STATE,
	                        InstanceStateKind.ANY_INSTANCE_STATE);

	            for (int i = 0; i < _dataSeq.size(); ++i) {
	                SampleInfo info = (SampleInfo)_infoSeq.get(i);

	                if (info.valid_data) {
	                	ShapeType data = (ShapeType) _dataSeq.get(i);
	       			    ShapesObject obj;

		    			// Get a handle to the object of shape/color
		    		
						obj = subObjects.get(kind).get(data.color);
	
		    			if (obj == null) {
		    				// Never seen this shape/color combo before, create
		    				// new entry for subscribed shapes collection
		    				obj = new ShapesObject(kind, data.color, data.x, data.y, 
		    						data.shapesize, 0, new Vector2d(1, 0));
		    				subObjects.get(kind).put(obj.getColor(), obj);
		    			}
	
		    			obj.setPosition(data.x, data.y);
		    			obj.setSize(data.shapesize);
	                }
	            }
	        } catch (RETCODE_NO_DATA noData) {
	            // No data to process
	        } finally {
	        	shapeTypeReader.return_loan(_dataSeq, _infoSeq);
	        }
		}
	}

	/* Call to subscribe to a shape.  Only one subscription will be created for 
	 * each shape kind.  If already created, then this is a noop.
	 */
	public void addSubscription(ShapeKind kind)
	{
		// Create reader if not created already
		DataReader reader = shapeReaders.get(kind);
		
		if (reader == null) {
			ShapesListener listener = new ShapesListener(kind);
			reader = shapesDDS.create_reader(kind, listener);
			shapeReaders.put(kind, reader);
		}
	}

	public void isPaused(boolean state)
	{
		isPaused = state;
	}
	
	public void isAccel(boolean state)
	{
		isAccel = state;
	}
	
	public void setAccX(float acc)
	{
		// acc X-axis direction seem to be swapped from display, need negative 
		accX = -acc/10;
	}
	
	public void setAccY(float acc)
	{
		accY = acc/10;
	}
}
