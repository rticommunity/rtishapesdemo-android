/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from ShapeType.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Data Distribution Service manual.
*/

#include <stdlib.h>

#include "ShapeType.h"
#include "ShapeTypePlugin.h"

/*** SOURCE_BEGIN ***/
/* --------------------------------------------------------------------------
    Support functions:
* -------------------------------------------------------------------------- */

RTI_BOOL
ShapeTypePlugin_create_sample(
    struct NDDS_Type_Plugin *plugin, void **sample, void *param)
{
    *sample = (void *) ShapeType_create();
    if (*sample == NULL) {
        return RTI_FALSE; 
    }
    return RTI_TRUE;
}

RTI_BOOL
ShapeTypePlugin_delete_sample(
    struct NDDS_Type_Plugin *plugin, void *sample, void *param)
{
    ShapeType_delete((ShapeType *) sample);
    return RTI_TRUE;
}

RTI_BOOL 
ShapeTypePlugin_copy_sample(
    struct NDDS_Type_Plugin *plugin, void *dst, const void *src, void *param)
{
    ShapeType_copy((ShapeType *)dst,(const ShapeType *)src);
    return RTI_TRUE;
} 

/* --------------------------------------------------------------------------
    (De)Serialize functions:
* -------------------------------------------------------------------------- */

RTI_BOOL 
ShapeType_CDRSerialize(
    struct CDR_Stream_t *stream, const void *void_sample, void *param)
{
    ShapeType *sample = (ShapeType *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
        {
        return RTI_FALSE;
    }

    if (!CDR_Stream_serialize_String(
            stream, sample->color, (128))) {
        return RTI_FALSE;
    }
    if (!CDR_Stream_serialize_Long(
            stream, &sample->x)) {
        return RTI_FALSE;
    }  
    if (!CDR_Stream_serialize_Long(
            stream, &sample->y)) {
        return RTI_FALSE;
    }  
    if (!CDR_Stream_serialize_Long(
            stream, &sample->shapesize)) {
        return RTI_FALSE;
    }  

    return RTI_TRUE;
}

RTI_BOOL 
ShapeType_CDRDeserialize(
    struct CDR_Stream_t *stream, void *void_sample, void *param)
{
    ShapeType *sample = (ShapeType *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
        {
        return RTI_FALSE;
    }

    if (!CDR_Stream_deserialize_String(
            stream, sample->color, (128))) {
        return RTI_FALSE;
    }
    if (!CDR_Stream_deserialize_Long(
            stream, &sample->x)) {
        return RTI_FALSE;
    }  
    if (!CDR_Stream_deserialize_Long(
            stream, &sample->y)) {
        return RTI_FALSE;
    }  
    if (!CDR_Stream_deserialize_Long(
            stream, &sample->shapesize)) {
        return RTI_FALSE;
    }  

    return RTI_TRUE;

}

RTI_UINT32
ShapeType_get_serialized_sample_max_size(
    struct NDDS_Type_Plugin *plugin,
    RTI_UINT32 current_alignment,
    void *param)
{
    RTI_UINT32 initial_alignment = current_alignment;

    current_alignment += CDR_get_maxSizeSerialized_String(
            current_alignment, (128)+1);

    current_alignment += CDR_get_maxSizeSerialized_Long(
            current_alignment);

    current_alignment += CDR_get_maxSizeSerialized_Long(
            current_alignment);

    current_alignment += CDR_get_maxSizeSerialized_Long(
            current_alignment);

    return  current_alignment - initial_alignment;
}

/* --------------------------------------------------------------------------
    Key Management functions:
* -------------------------------------------------------------------------- */
NDDS_TypePluginKeyKind 
ShapeType_get_key_kind(
    struct NDDS_Type_Plugin *plugin,
    void *param)
{
    return NDDS_TYPEPLUGIN_USER_KEY;
}

RTI_BOOL
ShapeType_CDRSerialize_key(
    struct CDR_Stream_t *stream, const void *void_sample, void *param)
{
    const ShapeType *sample = (ShapeType *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
        {
        return RTI_FALSE;
    }

    if (!CDR_Stream_serialize_String(
            stream, sample->color, (128))) {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

RTI_BOOL
ShapeType_CDRDeserialize_key(
    struct CDR_Stream_t *stream, void *void_sample, void *param)
{
    ShapeType *sample = (ShapeType *)void_sample;

    if ((stream == NULL) || (void_sample == NULL))
        {
        return RTI_FALSE;
    }

    if (!CDR_Stream_deserialize_String(
            stream, sample->color, (128))) {
        return RTI_FALSE;
    }

    return RTI_TRUE;
}

RTI_UINT32 
ShapeType_get_serialized_key_max_size(
    struct NDDS_Type_Plugin *plugin,
    RTI_UINT32 current_alignment,
    void *param)
{
    RTI_UINT32 initial_alignment = current_alignment;

    current_alignment +=  CDR_get_maxSizeSerialized_String(
            current_alignment, (128)+1);

    return current_alignment - initial_alignment;
}

/* --------------------------------------------------------------------------
 *  Type ShapeType Plugin Instanciation
 * -------------------------------------------------------------------------- */

NDDSCDREncapsulation ShapeTypeEncapsulationKind[] =
    { {0,0} };

struct NDDS_Type_Plugin ShapeTypeTypePlugin =
    {
    {0, 0},                     /* NDDS_Type_PluginVersion */
    NULL,                       /* DDS_TypeCode_t* */
    ShapeTypeEncapsulationKind,
        NDDS_TYPEPLUGIN_USER_KEY,   /* NDDS_TypePluginKeyKind */
    ShapeType_CDRSerialize,
        ShapeType_CDRDeserialize,
        ShapeType_get_serialized_sample_max_size,
        ShapeType_CDRSerialize_key,
        ShapeType_CDRDeserialize_key,
        ShapeType_get_serialized_key_max_size,
        ShapeTypePlugin_create_sample,
        ShapeTypePlugin_delete_sample,
        ShapeTypePlugin_copy_sample,
        PluginHelper_get_sample,
        PluginHelper_return_sample,
        PluginHelper_get_buffer,
        PluginHelper_return_buffer,
        PluginHelper_on_participant_attached,
        PluginHelper_on_participant_detached,
        PluginHelper_on_endpoint_attached,
        PluginHelper_on_endpoint_detached,
        PluginHelper_get_key_kind,
        PluginHelper_instance_to_keyhash,
        };

/* --------------------------------------------------------------------------
 * Plug-in Installation Methods
 * -------------------------------------------------------------------------- */
struct NDDS_Type_Plugin *ShapeTypeTypePlugin_get(void) 
{ 
    return &ShapeTypeTypePlugin;
} 

