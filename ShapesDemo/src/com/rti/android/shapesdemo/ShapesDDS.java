/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/

package com.rti.android.shapesdemo;

import java.util.*;

import com.rti.dds.domain.*;
import com.rti.dds.publication.*;
import com.rti.dds.subscription.*;
import com.rti.dds.infrastructure.StatusKind;
import com.rti.dds.topic.Topic;

import com.rti.android.shapesdemo.idl.*;
import com.rti.android.shapesdemo.ShapesObject.ShapeKind;

import android.util.Log;

/* Class that manages how RTI Connext DDS is used for the ShapesDemo */
public class ShapesDDS
{
	private DomainParticipant participant = null;
	private Publisher  publisher  = null;
	private Subscriber subscriber = null;

	private Topic topic_square   = null;
	private Topic topic_circle   = null;
	private Topic topic_triangle = null;

	/* Must call once after creation */
	public boolean initialize(int domainID, String peerList)
	{
		DomainParticipantQos partQos = new DomainParticipantQos();
		DomainParticipantFactory.get_instance().get_default_participant_qos(partQos);
		
		// Replace peer list if string is not null
		if ( peerList != null ) {
			StringTokenizer tokens = new StringTokenizer(peerList, ",");
			
			partQos.discovery.initial_peers.setMaximum(tokens.countTokens());

			while (tokens.hasMoreTokens()) {		
			    partQos.discovery.initial_peers.add(tokens.nextToken());
			}
		}
		Log.d("ShapeDDS", "creating participant");
		participant = 
                    DomainParticipantFactory.get_instance().create_participant(
                        domainID, partQos, null, StatusKind.STATUS_MASK_NONE);

		if (participant == null) {
			Log.e("ShapeDDS", "Failed to create participant.");
			return false;
		}
		
		ShapeTypeTypeSupport.register_type(participant, 
                  ShapeTypeTypeSupport.get_type_name());

		publisher = participant.create_publisher(DomainParticipant.PUBLISHER_QOS_DEFAULT, 
				null, StatusKind.STATUS_MASK_NONE);

		if (publisher == null) {
			Log.e("ShapeDDS", "Failed to create publisher.");
			return false;
		}

		subscriber = participant.create_subscriber(DomainParticipant.SUBSCRIBER_QOS_DEFAULT, 
				null, StatusKind.STATUS_MASK_NONE);

		if (subscriber == null) {
			Log.e("ShapeDDS", "Failed to create subscriber.");
			return false;
		}

		/* pre-create all topics that are used */
		topic_square = participant.create_topic(
                "Square",
                ShapeTypeTypeSupport.get_type_name(), 
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, StatusKind.STATUS_MASK_NONE);
		
		if (topic_square == null) {
			Log.e("ShapeDDS", "Failed to create Square topic.");
			return false;
		}

		topic_circle = participant.create_topic(
                "Circle",
                ShapeTypeTypeSupport.get_type_name(), 
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, StatusKind.STATUS_MASK_NONE);
		
		if (topic_circle == null) {
			Log.e("ShapeDDS", "Failed to create Circle topic.");
			return false;
		}

		topic_triangle = participant.create_topic(
                "Triangle",
                ShapeTypeTypeSupport.get_type_name(), 
                DomainParticipant.TOPIC_QOS_DEFAULT,
                null, StatusKind.STATUS_MASK_NONE);
		
		if (topic_triangle == null) {
			Log.e("ShapeDDS", "Failed to create Triangle topic.");
			return false;
		}

		return true;
	}

	public void stop()
	{
		participant.delete_contained_entities();
        DomainParticipantFactory.get_instance().delete_participant(participant);
	}

	public ShapeTypeDataWriter create_writer(ShapeKind kind)
	{
		Topic topic;

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
		
		return (ShapeTypeDataWriter) publisher.create_datawriter(
				topic,Publisher.DATAWRITER_QOS_DEFAULT, 
				null, StatusKind.STATUS_MASK_NONE);
	}

	public DataReader create_reader(ShapeKind kind, DataReaderListener listener)
	{
		Topic topic;

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
		return subscriber.create_datareader(topic, Subscriber.DATAREADER_QOS_DEFAULT, 
				listener, StatusKind.DATA_AVAILABLE_STATUS);
	}

}
