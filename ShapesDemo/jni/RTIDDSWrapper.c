/*****************************************************************************/
/*         (c) Copyright, Real-Time Innovations, All rights reserved.        */
/*                                                                           */
/*         Permission to modify and use for internal purposes granted.       */
/* This software is provided "as is", without warranty, express or implied.  */
/*                                                                           */
/*****************************************************************************/
#include <jni.h>
#include <android/log.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rti_me_c.h"
#include "disc_dpde/disc_dpde_discovery_plugin.h"
#include "wh_sm/wh_sm_history.h"
#include "rh_sm/rh_sm_history.h"
#include "netio/netio_udp.h"

#include "ShapeType.h"
#include "ShapeTypeSupport.h"
#include "ShapeTypePlugin.h"

static JavaVM *gJvm;

const char *SHAPETYPE = "ShapeType";

/*************************************************************************
 */
void Android_Log_d(const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);

    __android_log_vprint(ANDROID_LOG_DEBUG, "RTIDDSWrapper", format,
            arglist);
    va_end(arglist);
}

/*************************************************************************
 */
void Android_Log_e(const char *format, ...)
{
    va_list arglist;
    va_start(arglist, format);

    __android_log_vprint(ANDROID_LOG_ERROR, "RTIDDSWrapper", format,
            arglist);
    va_end(arglist);
}

/*************************************************************************
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *jvm, void *reserved)
{
    // Need to cache a pointer to the VM
    gJvm = jvm;

    return JNI_VERSION_1_6;
}

/*************************************************************************
 *
 * Need to call this function once to initialize.
 */
JNIEXPORT jboolean JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1initialize(
        JNIEnv* env, jobject this)
{
    DDS_DomainParticipantFactory *factory = NULL;
    struct DPDE_DiscoveryPluginProperty discovery_plugin_properties =
            DPDE_DiscoveryPluginProperty_INITIALIZER;
    RT_Registry_T *registry = NULL;
    struct UDP_InterfaceFactoryProperty *udp_property = NULL;

    DDS_Boolean success = DDS_BOOLEAN_FALSE;

    factory = DDS_DomainParticipantFactory_get_instance();

    registry = DDS_DomainParticipantFactory_get_registry(
            DDS_DomainParticipantFactory_get_instance());

    /* Register the writer history plugin */
    if ( !RT_Registry_register(registry, "wh", WHSM_HistoryFactory_get_interface(),
            NULL, NULL ) )
    {
        Android_Log_e("Failed to register writer history.");
        goto done;
    }

    /* Register the reader history plugin */
    if ( !RT_Registry_register(registry, "rh", RHSM_HistoryFactory_get_interface(),
            NULL, NULL ) )
    {
        Android_Log_e("Failed to register writer history.");
        goto done;
    }

    /* Need to make changes to the UDP transport plugin, so first unregister */
    if (!RT_Registry_unregister(registry, "_udp", NULL, NULL ))
    {
        Android_Log_e("Failed to unregister udp transport.");
        goto done;
    }

    udp_property = (struct UDP_InterfaceFactoryProperty *) malloc(
            sizeof(struct UDP_InterfaceFactoryProperty));
    *udp_property = UDP_INTERFACE_FACTORY_PROPERTY_DEFAULT;

    /* Now set the allowed interfaces to the wifi interface used by Android */
    REDA_StringSeq_set_maximum(&udp_property->allow_interface, 1);
    REDA_StringSeq_set_length(&udp_property->allow_interface, 1);
    *REDA_StringSeq_get_reference(&udp_property->allow_interface, 0) = "wlan0";

    /* Now register the UDP transport */
    if (!RT_Registry_register(registry, "_udp",
            UDP_InterfaceFactory_get_interface(),
            (struct RT_ComponentFactoryProperty*) udp_property, NULL ))
    {
        Android_Log_e("Failed to register udp transport.");
        goto done;
    }

    /* Register the dynamic discovery plugin */
    if ( !RT_Registry_register(registry, "dynamic",
            DPDE_DiscoveryFactory_get_interface(),
            &discovery_plugin_properties._parent, NULL ) )
    {
        Android_Log_e("Failed to register dynamic discovery.");
        goto done;
    }

    success = DDS_BOOLEAN_TRUE;

    done:

    DPDE_DiscoveryPluginProperty_finalize(&discovery_plugin_properties);

    if (!success)
    {
        if (udp_property != NULL )
        {
            free(udp_property);
        }

        return JNI_FALSE;
    }

    Android_Log_d("Initialized all DDS plugins");

    return JNI_TRUE;
}

/*************************************************************************
 */
JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1create_1participant_1with_1peers(
        JNIEnv* env, jobject this, jint domainID, jstring jpeers)
{
    DDS_ReturnCode_t retcode;
    DDS_Entity *entity;
    DDS_DomainParticipant *participant = NULL;
    DDS_DomainParticipantFactory *factory = NULL;
    struct DDS_DomainParticipantQos dp_qos =
            DDS_DomainParticipantQos_INITIALIZER;
    char* peerlist;
    DDS_Boolean success = DDS_BOOLEAN_FALSE;

    Android_Log_d("Creating participant");

    factory = DDS_DomainParticipantFactory_get_instance();

    /* Have to get the c-string from Java */
    peerlist = (char *) (*env)->GetStringUTFChars(env, jpeers, NULL );

    /* Set the discovery plugin used by the participant to the one that we registered */
    OSAPI_Stdio_snprintf(dp_qos.discovery.discovery.name, 8, "dynamic");

    /* Set initial peer list to discovery via multicast and loopback by default */
    if ((peerlist == NULL )|| (strlen(peerlist) == 0)){
        DDS_StringSeq_set_maximum(&dp_qos.discovery.initial_peers,2);
        DDS_StringSeq_set_length(&dp_qos.discovery.initial_peers,2);
        *DDS_StringSeq_get_reference(&dp_qos.discovery.initial_peers,0) =
                DDS_String_dup("239.255.0.1");
        *DDS_StringSeq_get_reference(&dp_qos.discovery.initial_peers,1) =
                DDS_String_dup("127.0.0.1");
    }
    else /* Use user specified peer list */
    {
        if (strchr(peerlist, ','))
        {
            int cnt = 1;
            int i = 0;
            char* pch;

            /* Get the number of peers */
            while (i < strlen(peerlist))
            {
                if (peerlist[i] == ',')
                {
                    cnt ++;
                }
                i++;
            }

            DDS_StringSeq_set_maximum(&dp_qos.discovery.initial_peers,cnt);
            DDS_StringSeq_set_length(&dp_qos.discovery.initial_peers,cnt);

            pch = strtok(peerlist, ",");

            i = 0;
            while ((pch != NULL) && (i < cnt))
            {
                *DDS_StringSeq_get_reference(&dp_qos.discovery.initial_peers,i) = DDS_String_dup(pch);
                i++;
                pch = strtok(NULL,",");
            }
        }
        else
        {
            DDS_StringSeq_set_maximum(&dp_qos.discovery.initial_peers,1);
            DDS_StringSeq_set_length(&dp_qos.discovery.initial_peers,1);
            *DDS_StringSeq_get_reference(&dp_qos.discovery.initial_peers,0) =
                    DDS_String_dup(peerlist);
            Android_Log_d("Using custom peer list : %s", peerlist);
        }
    }

    /* if there are more remote or local endpoints, you need to increase these limits */
    dp_qos.resource_limits.max_destination_ports = 64;
    dp_qos.resource_limits.max_receive_ports = 64;
    dp_qos.resource_limits.local_topic_allocation = 64;
    dp_qos.resource_limits.local_type_allocation = 64;
    dp_qos.resource_limits.local_reader_allocation = 64;
    dp_qos.resource_limits.local_writer_allocation = 64;
    dp_qos.resource_limits.local_publisher_allocation = 64;
    dp_qos.resource_limits.local_subscriber_allocation = 64;
    dp_qos.resource_limits.remote_participant_allocation = 32;
    dp_qos.resource_limits.remote_reader_allocation = 256;
    dp_qos.resource_limits.remote_writer_allocation = 256;
    dp_qos.resource_limits.matching_writer_reader_pair_allocation = 256;
    dp_qos.resource_limits.matching_reader_writer_pair_allocation = 256;

    OSAPI_Stdio_snprintf(dp_qos.participant_name.name, 8, "Android");

    participant = DDS_DomainParticipantFactory_create_participant(factory,
            domainID, &dp_qos, NULL, DDS_STATUS_MASK_NONE);

    if (participant == NULL )
    {
        Android_Log_e("Failed to create participant.");
        goto done;
    }

    /* Register the data types that will be used */
    retcode = DDS_DomainParticipant_register_type(participant, SHAPETYPE,
            ShapeTypeTypePlugin_get());

    if (retcode != DDS_RETCODE_OK)
    {
        Android_Log_e("Could not register type %s.", SHAPETYPE);
        goto done;
    }

    success = DDS_BOOLEAN_TRUE;
    Android_Log_d("Participant created!");

    done:

    /* Should release the string, else memory leak */
    (*env)->ReleaseStringUTFChars(env, jpeers, peerlist);

    if (!success)
    {
        if ( participant != NULL )
        {
            DDS_DomainParticipantFactory_delete_participant(
                    factory, participant);
        }
        participant = NULL;
    }

    return (jint) participant;
}

/*************************************************************************
 */
JNIEXPORT jboolean JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1delete_1participant(
        JNIEnv* env, jobject this, jint jparticipant)
{
    DDS_ReturnCode_t retcode;
    DDS_DomainParticipant *participant = (DDS_DomainParticipant *) jparticipant;

    /* first delete all DDS child-entities of the participant */
    retcode = DDS_DomainParticipant_delete_contained_entities(participant);

    if ( retcode != DDS_RETCODE_OK )
    {
        Android_Log_e("Failed to delete contained entities: %d", retcode);
        return JNI_FALSE;
    }

    /* Must unregister all registered type before deleting participant */
    if ( DDS_DomainParticipant_unregister_type(participant, SHAPETYPE) !=
            ShapeTypeTypePlugin_get() )
    {
        Android_Log_e("Failed to unregister type: %s", SHAPETYPE);
        return JNI_FALSE;
    }

    /* Now can delete participant */
    retcode = DDS_DomainParticipantFactory_delete_participant(
            DDS_DomainParticipantFactory_get_instance(), participant);

    if ( retcode != DDS_RETCODE_OK )
    {
        Android_Log_e("Failed to delete DomainParticipant: %d", retcode);
        return JNI_FALSE;
    }

    Android_Log_d("Participant deleted!");

    return JNI_TRUE;
}

/*************************************************************************
 */
JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1create_1publisher(
        JNIEnv* env, jobject this, jint jparticipant)
{
    DDS_DomainParticipant *participant = (DDS_DomainParticipant *) jparticipant;
    DDS_Publisher *publisher = NULL;

    publisher = DDS_DomainParticipant_create_publisher(participant,
            &DDS_PUBLISHER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE );

    if (publisher == NULL )
    {
        Android_Log_e("Could not create DDSPublisher.");
        return 0;
    }

    return (jint) publisher;
}

/*************************************************************************
 */
JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1create_1subscriber(
        JNIEnv* env, jobject this, jint jparticipant)
{
    DDS_DomainParticipant *participant = (DDS_DomainParticipant *) jparticipant;
    DDS_Subscriber *subscriber = NULL;

    subscriber = DDS_DomainParticipant_create_subscriber(participant,
            &DDS_SUBSCRIBER_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE );

    if (subscriber == NULL )
    {
        Android_Log_e("Could not create DDSPublisher.");
        return 0;
    }

    return (jint) subscriber;
}


/*************************************************************************
 */
JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1create_1topic(
        JNIEnv* env, jobject this, jint jparticipant, jstring jtopicName,
        jstring jtypeName)
{
    DDS_Topic *topic = NULL;
    DDS_DomainParticipant *participant = (DDS_DomainParticipant *) jparticipant;
    char* topicName = (char *) (*env)->GetStringUTFChars(env, jtopicName,
            NULL );
    char* typeName = (char *) (*env)->GetStringUTFChars(env, jtypeName, NULL );

    topic = DDS_DomainParticipant_create_topic(participant, topicName, typeName,
            &DDS_TOPIC_QOS_DEFAULT, NULL, DDS_STATUS_MASK_NONE );

    if (topic == NULL )
    {
        Android_Log_e("Could not create topic %s.", topicName);
    }

    (*env)->ReleaseStringUTFChars(env, jtopicName, topicName);
    (*env)->ReleaseStringUTFChars(env, jtypeName, typeName);

    return (jint) topic;
}

/*************************************************************************
 */
JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1create_1datawriter(
        JNIEnv* env, jobject this, jint jpublisher, jint jtopic)
{
    DDS_Publisher *publisher = (DDS_Publisher *) jpublisher;
    DDS_Topic *topic = (DDS_Topic*) jtopic;
    struct DDS_DataWriterQos dw_qos = DDS_DataWriterQos_INITIALIZER;
    DDS_DataWriter *datawriter;

    dw_qos.reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
    dw_qos.resource_limits.max_samples = 256;
    dw_qos.resource_limits.max_samples_per_instance = 1;
    dw_qos.resource_limits.max_instances = 256;
    dw_qos.history.kind = DDS_KEEP_LAST_HISTORY_QOS;
    dw_qos.history.depth = 1;

    datawriter = DDS_Publisher_create_datawriter(publisher, topic, &dw_qos,
            NULL, DDS_STATUS_MASK_NONE );

    if (datawriter == NULL )
    {
        Android_Log_e("Could not create datawriter for topic %s.",
                DDS_TopicDescription_get_name(DDS_Topic_as_topicdescription(topic)));
    }

    Android_Log_d("Created datawriter for topic %s", DDS_TopicDescription_get_name(
            DDS_Topic_as_topicdescription(topic)));

    return (jint) datawriter;
}

JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1SHAPETYPE_1datawriter_1write(
        JNIEnv* env, jobject this, jint jwriter, jstring jcolor, jint x, jint y, jint size)
{
    DDS_DataWriter *datawriter = (DDS_DataWriter *) jwriter;
    ShapeTypeDataWriter *shapedatawriter;
    ShapeType sample;
    DDS_ReturnCode_t retcode;

    shapedatawriter = ShapeTypeDataWriter_narrow(datawriter);

    if (shapedatawriter == NULL )
    {
        Android_Log_e("Invalid datawriter for ShapeDataWriter.write");
        return DDS_RETCODE_ERROR;
    }

    sample.color = (char *) (*env)->GetStringUTFChars(env, jcolor, NULL );
    sample.x = x;
    sample.y = y;
    sample.shapesize = size;

    retcode = ShapeTypeDataWriter_write(shapedatawriter, &sample,
            &DDS_HANDLE_NIL);

    (*env)->ReleaseStringUTFChars(env, jcolor, sample.color);

    return retcode;
}

struct JavaCallbackData
{
    jobject obj;
    jmethodID mID;
};

/*************************************************************************
 */
void ShapesDDS_RTIDDSWrapper_on_data_available(void *listener_data,
        DDS_DataReader * reader)
{

    JNIEnv *env = NULL;
    struct JavaCallbackData *callback =
            (struct JavaCallbackData *) listener_data;
    ShapeTypeDataReader *datareader = ShapeTypeDataReader_narrow(reader);

    ShapeType*sample = NULL;
    struct DDS_SampleInfo *sample_info = NULL;

    struct DDS_SampleInfoSeq info_seq =
            DDS_SEQUENCE_INITIALIZER(struct DDS_SampleInfo);
    struct ShapeTypeSeq data_seq = DDS_SEQUENCE_INITIALIZER(ShapeType);

    DDS_ReturnCode_t retcode;
    const DDS_Long TAKE_MAX_SAMPLES = 32;
    int i;

    int attached = 0;

    if (datareader == NULL )
    {
        Android_Log_e("on_data_available did not get ShapeTypeDataReader");
        return;
    }

    retcode = ShapeTypeDataReader_take(datareader, &data_seq, &info_seq,
            TAKE_MAX_SAMPLES, DDS_ANY_SAMPLE_STATE, DDS_ANY_VIEW_STATE,
            DDS_ANY_INSTANCE_STATE);

    if (retcode != DDS_RETCODE_OK)
    {
        Android_Log_e("on_data_available failed to take data, retcode(%d)\n",
                retcode);
        return;
    }

    /* Need to get the environment from the JVM */
    switch ((*gJvm)->GetEnv(gJvm, (void **) &env, JNI_VERSION_1_6))
    {
        case JNI_OK :
            /* thread was already attached to Jvm */
            break;

        case JNI_EDETACHED :
            /* Need to attach thread to Jvm to get env */
            if ((*gJvm)->AttachCurrentThread(gJvm, &env, NULL) != 0)
            {
                Android_Log_e("on_data_available could not attach thread");
                return;
            }
            attached = 1;
            break;

        default :
            Android_Log_e("on_data_available wrong JNI version");
            return;
            break;
    }

    if (env == NULL )
    {
        goto done;
    }

    /* process the data received */
    for (i = 0; i < ShapeTypeSeq_get_length(&data_seq); ++i)
    {
        sample_info = DDS_SampleInfoSeq_get_reference(&info_seq, i);

        /* only process data that is valid */
        if (sample_info->valid_data)
        {
            jstring jColorStr;
            sample = ShapeTypeSeq_get_reference(&data_seq, i);

            /* to pass a string into Java, need to allocate */
            jColorStr = (*env)->NewStringUTF(env, sample->color);

            /* Call Java method with parameters */
            (*env)->CallVoidMethod(env, callback->obj, callback->mID,
                    jColorStr, sample->x, sample->y, sample->shapesize);

        }
    }

    done:

    /* If thread had to be attached to Jvm, now detach */
    if ( attached )
    {
        (*gJvm)->DetachCurrentThread(gJvm);
    }

    /* Return the memory loaned by DDS for storing data/info */
    ShapeTypeDataReader_return_loan(datareader, &data_seq, &info_seq);

    ShapeTypeSeq_finalize(&data_seq);
    DDS_SampleInfoSeq_finalize(&info_seq);

}

/*************************************************************************
 */
JNIEXPORT jint JNICALL Java_com_rti_android_shapesdemo_ShapesDDS_RTIDDSWrapper_1create_1datareader(
        JNIEnv* env, jobject this, jint jsubscriber, jint jtopic, jobject jlistener)
{
    DDS_Subscriber *subscriber = (DDS_Subscriber *) jsubscriber;
    DDS_Topic *topic = (DDS_Topic*) jtopic;
    struct DDS_DataReaderQos dr_qos = DDS_DataReaderQos_INITIALIZER;
    DDS_DataReader *datareader;
    struct DDS_DataReaderListener default_listener =
            DDS_DataReaderListener_INITIALIZER;
    struct DDS_DataReaderListener *listener =
            (struct DDS_DataReaderListener*) malloc(
                    sizeof(struct DDS_DataReaderListener));
    struct JavaCallbackData *callback = (struct JavaCallbackData *) malloc(
            sizeof(struct JavaCallbackData));
    jclass cls;

    *listener = default_listener;

    /* Need to allocate global reference to callback object since JNI
       passes a local ref */
    callback->obj =  (*env)->NewGlobalRef(env, jlistener);

    /* Need to get class of the Java listener object */
    cls = (*env)->GetObjectClass(env, jlistener);

    /* Cache the ID of the on_data_available() method of the Java class */
    callback->mID = (*env)->GetMethodID(env, cls, "on_data_available",
            "(Ljava/lang/String;III)V");

    if (callback->mID == 0)
    {
        Android_Log_e(
                "create_datareader: method \"void on_data_available(String,int,int,int)\" not found.");
    }

    /* Set C callback data and function */
    listener->on_data_available = ShapesDDS_RTIDDSWrapper_on_data_available;
    listener->as_listener.listener_data = callback;

    dr_qos.reliability.kind = DDS_BEST_EFFORT_RELIABILITY_QOS;
    dr_qos.resource_limits.max_samples = 256;
    dr_qos.resource_limits.max_samples_per_instance = 1;
    dr_qos.resource_limits.max_instances = 256;
    dr_qos.history.kind = DDS_KEEP_LAST_HISTORY_QOS;
    dr_qos.history.depth = 1;
    dr_qos.reader_resource_limits.max_remote_writers = 256;
    dr_qos.reader_resource_limits.max_remote_writers_per_instance = 256;

    datareader = DDS_Subscriber_create_datareader(subscriber,
            DDS_Topic_as_topicdescription(topic), &dr_qos, listener,
            DDS_DATA_AVAILABLE_STATUS);

    if (datareader == NULL )
    {
        Android_Log_e("Could not create datareader for topic %s.",
                DDS_TopicDescription_get_name(
                        DDS_Topic_as_topicdescription(topic)));
    }

    Android_Log_d("Created datareader for topic %s", DDS_TopicDescription_get_name(
            DDS_Topic_as_topicdescription(topic)));

    return (jint) datareader;
}

