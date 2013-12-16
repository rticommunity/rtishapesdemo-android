/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from ShapeType.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Connext Micro distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Connext manual.
*/

#ifndef ShapeType_h
#define ShapeType_h

#ifndef rti_me_c_h
  #include "rti_me_c.h"
#endif

#if defined(RTI_WIN32) && defined(NDDS_USER_DLL_EXPORT)
  /* If the code is building on Windows, start exporting symbols. */
#undef NDDSUSERDllExport
  #define NDDSUSERDllExport __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
    #endif

    extern const char *ShapeTypeTYPENAME;

    #ifdef __cplusplus
    struct ShapeTypeSeq;
    #ifndef NDDS_STANDALONE_TYPE
        class ShapeTypeTypeSupport;
    class ShapeTypeDataWriter;
    class ShapeTypeDataReader;
    #endif
    #endif

    typedef struct ShapeType 
    {
        char* color;
        CDR_Long x;
        CDR_Long y;
        CDR_Long shapesize;
    } ShapeType ;

    REDA_DEFINE_SEQUENCE(ShapeTypeSeq, ShapeType);

    NDDSUSERDllExport extern struct NDDS_Type_Plugin*
    ShapeTypeTypePlugin_get(void);

    NDDSUSERDllExport extern RTI_BOOL
    ShapeType_initialize(ShapeType* sample);

    NDDSUSERDllExport extern RTI_BOOL
    ShapeType_finalize(ShapeType* sample);

    NDDSUSERDllExport extern ShapeType*
    ShapeType_create();

    NDDSUSERDllExport extern void
    ShapeType_delete(ShapeType* sample);

    NDDSUSERDllExport extern RTI_BOOL
    ShapeType_copy(ShapeType* dst,const ShapeType* src);
    #ifdef __cplusplus
}
#endif

#if defined(RTI_WIN32) && defined(NDDS_USER_DLL_EXPORT)
  /* If the code is building on Windows, stop exporting symbols.
   */
#undef NDDSUSERDllExport
  #define NDDSUSERDllExport
#endif

#endif /* ShapeType_h */

