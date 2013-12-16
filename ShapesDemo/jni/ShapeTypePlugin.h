/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from ShapeType.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Data Distribution Service manual.
*/

#ifndef ShapeTypePlugin_434673458_h
#define ShapeTypePlugin_434673458_h

#include "ShapeType.h"

struct CDR_Stream_t;

#if defined(RTI_WIN32) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, start exporting symbols.
*/
#undef NDDSUSERDllExport
  #define NDDSUSERDllExport __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
    #endif

    /* --------------------------------------------------------------------------
        Untyped interfaces to the typed sample management functions
     * -------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTI_BOOL
    ShapeTypePlugin_create_sample(
        struct NDDS_Type_Plugin *plugin, void **sample,void *param);

    NDDSUSERDllExport extern RTI_BOOL 
    ShapeTypePlugin_delete_sample(
        struct NDDS_Type_Plugin *plugin, void *sample,void *param);

    NDDSUSERDllExport extern RTI_BOOL 
    ShapeTypePlugin_copy_sample(
        struct NDDS_Type_Plugin *plugin, void *dst, const void *src, void *param);

    /* --------------------------------------------------------------------------
        (De)Serialize functions:
    * -------------------------------------------------------------------------- */

    NDDSUSERDllExport extern RTI_BOOL 
    ShapeType_CDRSerialize(
        struct CDR_Stream_t *stream, const void *void_sample, void *param);

    NDDSUSERDllExport extern RTI_BOOL 
    ShapeType_CDRDeserialize(
        struct CDR_Stream_t *stream, void *void_sample, void *param);

    NDDSUSERDllExport extern RTI_UINT32
    ShapeType_get_serialized_sample_max_size(
        struct NDDS_Type_Plugin *plugin,
        RTI_UINT32 current_alignment,
        void *param);

    NDDSUSERDllExport extern NDDS_TypePluginKeyKind 
    ShapeType_get_key_kind(
        struct NDDS_Type_Plugin *plugin,
        void *param);

    /* --------------------------------------------------------------------------
        Key Management functions:
    * -------------------------------------------------------------------------- */
    NDDSUSERDllExport extern RTI_BOOL 
    ShapeType_CDRSerialize_key(
        struct CDR_Stream_t *keystream, const void *sample,
        void *param);

    NDDSUSERDllExport extern RTI_BOOL 
    ShapeType_CDRDeserialize_key(
        struct CDR_Stream_t *keystream, void *sample,
        void *param);

    NDDSUSERDllExport extern RTI_UINT32
    ShapeType_get_serialized_key_max_size(
        struct NDDS_Type_Plugin *plugin,
        RTI_UINT32 current_alignment,
        void *param);

    NDDSUSERDllExport extern RTI_BOOL 
    ShapeType_instance_to_keyhash(
        struct NDDS_Type_Plugin *plugin,
        struct CDR_Stream_t *stream, DDS_KeyHash_t *keyHash, const void *instance,
        void *param);

    #ifdef __cplusplus
}
#endif

#if defined(RTI_WIN32) && defined(NDDS_USER_DLL_EXPORT)
/* If the code is building on Windows, stop exporting symbols */
#undef NDDSUSERDllExport
  #define NDDSUSERDllExport
#endif

#endif /* ShapeTypePlugin_434673458_h */

