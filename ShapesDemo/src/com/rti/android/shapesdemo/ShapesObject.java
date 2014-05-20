/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

package com.rti.android.shapesdemo;

import com.rti.dds.infrastructure.*;
import com.rti.android.shapesdemo.idl.ShapeType;
import com.rti.android.shapesdemo.idl.ShapeTypeDataWriter;
import com.rti.android.shapesdemo.util.Vector2d;

import java.util.Locale;
import java.util.Random;

import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Path;
import android.util.Log;

/* Class that implements the behavior of an shape */
class ShapesObject
{
	public enum ShapeKind {
		SQUARE, CIRCLE, TRIANGLE
	}

	private ShapeType shape = new ShapeType();
	private ShapeTypeDataWriter writer = null;
	private ShapeKind type;
	private int drawColor;
	private float speed;
	
	/* a normalized vector indicating direction of travel */
	private Vector2d direction;
	
	/* bounding box */
	private float left;
	private float top;
	private float right;
	private float bottom;

	/* used for drawing */
	private Paint paintObject;
	private Paint paintBorder;
	private Path triangle;

	/* These colors were coordinated with the WxWidgets version of ShapesDemo */
	private final int PURPLE = Color.rgb(176, 0, 255);
	private final int ORANGE = Color.rgb(255, 130, 0);

	private final static Random rand = new Random();

	/* Constructor */
	public ShapesObject(ShapeKind type, String color, int size, ShapeTypeDataWriter writer)
	{
		this(type, color, rand.nextInt(235), rand.nextInt(235), size, rand.nextInt(20),
		        new Vector2d(rand.nextInt(100), rand.nextInt(100)).normalize());
		this.writer = writer;
	}

	/* Constructor */
	public ShapesObject(ShapeKind type, String color, int x, int y,
	        int size, float speed, Vector2d direction)
	{
		this.type = type;
		this.shape.color = color.trim().toUpperCase(Locale.US);
		this.shape.x = x;
		this.shape.y = y;
		this.shape.shapesize = size;
		this.speed = speed;
		this.direction = direction;

		left = x - (size / 2);
		top = y - (size / 2);
		right = x + (size / 2);
		bottom = y + (size / 2);

		/* Brush for interior */
		paintObject = new Paint();
		paintObject.setStrokeWidth(0);
		paintObject.setStyle(Paint.Style.FILL);

		/* Brush for borders */
		paintBorder = new Paint();
		paintBorder.setStrokeWidth(2);
		paintBorder.setStyle(Paint.Style.STROKE);

		/* Need to create triangular shape manually */
		triangle = new Path();
		
		// Must use .equal() to compare string values
		if (this.shape.color.equals("PURPLE")) {
			drawColor = PURPLE;
		} else if (this.shape.color.equals("BLUE")) {
			drawColor = Color.BLUE;
		} else if (this.shape.color.equals("RED")) {
			drawColor = Color.RED;
		} else if (this.shape.color.equals("GREEN")) {
			drawColor = Color.GREEN;
		} else if (this.shape.color.equals("YELLOW")) {
			drawColor = Color.YELLOW;
		} else if (this.shape.color.equals("CYAN")) {
			drawColor = Color.CYAN;
		} else if (this.shape.color.equals("MAGENTA")) {
			drawColor = Color.MAGENTA;
		} else if (this.shape.color.equals("ORANGE")) {
			drawColor = ORANGE;
		} else {
			Log.d("ShapesObject", "Color '" + this.shape.color + " unknown, setting to BLACK");
			drawColor = Color.BLACK;
		}
	}

	/* Call this to have a shape draw itself */
	public void draw(Canvas canvas, boolean drawBorder)
	{

		paintObject.setColor(drawColor);

		if (drawBorder) {
			if (drawColor == Color.BLUE) {
				paintBorder.setColor(Color.RED);
			} else {
				paintBorder.setColor(Color.BLUE);
			}
		}

		switch (type) {
			case SQUARE:
				canvas.drawRect(left, top, right, bottom, paintObject);

				if (drawBorder) {
					canvas.drawRect(left, top, right, bottom, paintBorder);
				}
				break;

			case CIRCLE:
				canvas.drawCircle(shape.x, shape.y, shape.shapesize / 2, paintObject);

				if (drawBorder) {
					canvas.drawCircle(shape.x, shape.y, shape.shapesize / 2, paintBorder);
				}
				break;

			case TRIANGLE:

				triangle.reset();
				triangle.moveTo(shape.x, top);
				triangle.lineTo(right, bottom);
				triangle.lineTo(left, bottom);
				triangle.close();

				canvas.drawPath(triangle, paintObject);

				if (drawBorder) {
					canvas.drawPath(triangle, paintBorder);
				}
				break;
		}
	}
	
	/*
	 * Send data via DDS
	 */
	public boolean write()
	{
		if (writer == null)
		{
			return false;
		}
		
		try {
            writer.write(shape, InstanceHandle_t.HANDLE_NIL);
        } catch(RETCODE_ERROR e) {
            Log.e("ShapesObject", "Write error " + 
                    e.getClass().toString() + ": " + e.getMessage());
           return false;
        }
		return true;
	}
	
	/* Check if X,Y coordinate is within the shape's bounding box 
	 * at current location.
	 */
	public boolean contains(float x, float y)
	{
		if (left < x && x < right && top < y && y < bottom) {
			return true;
		}
		return false;
	}
	
	public void setPosition(int x, int y)
	{
		this.shape.x = x;
		this.shape.y = y;
		/* Recalculate bounding box */
		left = x - (shape.shapesize / 2);
		top = y - (shape.shapesize / 2);
		right = x + (shape.shapesize / 2);
		bottom = y + (shape.shapesize / 2);
	}

	public int getX()
	{
		return shape.x;
	}

	public int getY()
	{
		return shape.y;
	}

	public float getSpeed()
	{
		return speed;
	}

	public void setSpeed(float speed)
	{
		this.speed = speed;
	}

	public int getSize()
	{
		return shape.shapesize;
	}

	public void setSize(int size)
	{
		shape.shapesize = size;
	}

	public String getColor()
	{
		return shape.color;
	}

	public Vector2d getDirection()
	{
		return direction;
	}

	public void setDirection(Vector2d direction)
	{
		this.direction.copy(direction);
	}
}