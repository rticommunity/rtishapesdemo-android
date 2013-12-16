/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

package com.rti.android.shapesdemo;

import com.rti.android.shapesdemo.ShapesObject.ShapeKind;

import android.util.Log;

/* Class that manages how RTI Connext DDS is used for the ShapesDemo */
public class ShapesDDS
{
	private int participant = 0;
	private int publisher = 0;
	private int subscriber = 0;

	private static final String SHAPETYPE = "ShapeType";

	private int topic_square = 0;
	private int topic_circle = 0;
	private int topic_triangle = 0;

	public interface ShapesDDSDataReaderListener
	{
		void on_data_available(String color, int x, int y, int size);
	}

	/* Must call once after creation */
	public boolean initialize(int domainID, String peerList)
	{
		participant = RTIDDSWrapper_create_participant_with_peers(domainID,
		        peerList);

		if (participant == 0) {
			Log.e("ShapeDDS", "Failed to create participant.");
			return false;
		}

		publisher = RTIDDSWrapper_create_publisher(participant);

		if (publisher == 0) {
			Log.e("ShapeDDS", "Failed to create publisher.");
			return false;
		}

		subscriber = RTIDDSWrapper_create_subscriber(participant);

		if (subscriber == 0) {
			Log.e("ShapeDDS", "Failed to create subscriber.");
			return false;
		}

		/* pre-create all topics that are used */
		topic_square = RTIDDSWrapper_create_topic(participant, "Square",
		        SHAPETYPE);
		
		if (topic_square == 0) {
			Log.e("ShapeDDS", "Failed to create Square topic.");
			return false;
		}

		topic_circle = RTIDDSWrapper_create_topic(participant, "Circle",
		        SHAPETYPE);
		
		if (topic_circle == 0) {
			Log.e("ShapeDDS", "Failed to create Circle topic.");
			return false;
		}

		topic_triangle = RTIDDSWrapper_create_topic(participant, "Triangle",
		        SHAPETYPE);
		
		if (topic_triangle == 0) {
			Log.e("ShapeDDS", "Failed to create Triangle topic.");
			return false;
		}

		return true;
	}

	public boolean stop()
	{
		return RTIDDSWrapper_delete_participant(participant);
	}

	public int create_writer(ShapeKind kind)
	{
		int topic;

		switch (kind) {
			case SQUARE:
				topic = topic_square;
				break;
			case CIRCLE:
				topic = topic_circle;
				break;
			default:
				topic = topic_triangle;
				break;
		}
		return RTIDDSWrapper_create_datawriter(publisher, topic);
	}

	public int create_reader(ShapeKind kind,
	        ShapesDDSDataReaderListener listener)
	{
		int topic;

		switch (kind) {
			case SQUARE:
				topic = topic_square;
				break;
			case CIRCLE:
				topic = topic_circle;
				break;
			default:
				topic = topic_triangle;
				break;
		}
		return RTIDDSWrapper_create_datareader(subscriber, topic, listener);
	}

	public int write(int writer, ShapesObject obj)
	{
		return RTIDDSWrapper_SHAPETYPE_datawriter_write(writer, obj.getColor(),
		        (int) obj.getX(), (int) obj.getY(), (int) obj.getSize());
	}
	
	
	/* Declarations needed to use JNI to call native functions */
	public native static boolean RTIDDSWrapper_initialize();
	
	public native static int RTIDDSWrapper_create_participant_with_peers(int domainID,
	        String peerList);

	public native static boolean RTIDDSWrapper_delete_participant(int participant);

	public native static int RTIDDSWrapper_create_publisher(int participant);

	public native static int RTIDDSWrapper_create_subscriber(int participant);

	public native static int RTIDDSWrapper_create_topic(int participant,
	        String topicName, String typeName);

	public native static int RTIDDSWrapper_create_datawriter(int publisher, int topic);

	public native static int RTIDDSWrapper_SHAPETYPE_datawriter_write(int writer,
	        String color, int x, int y, int size);

	public native static int RTIDDSWrapper_create_datareader(int subscriber,
	        int topic, ShapesDDSDataReaderListener listener);

	static {
		/* Loads librti_android_shapesdemo.so, native library implementing
		 * functions declared above.
		 */
		System.loadLibrary("rti_android_shapesdemo");
		
		/* Here we initialize the wrapper, which needs to be done only once */
		if (!RTIDDSWrapper_initialize())
		{
			Log.e("ShapeDDS", "Could not initialize RTIDDSWrapper");
		}
	}
}
