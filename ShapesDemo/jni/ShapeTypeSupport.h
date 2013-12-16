/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from ShapeType.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Data Distribution Service manual.
*/

#ifndef ShapeTypeSupport_434673458_h
#define ShapeTypeSupport_434673458_h

#include <stdlib.h>

/* Uses */
#include "ShapeType.h"

#if defined(RTI_WIN32) && defined(NDDS_USER_DLL_EXPORT)
  /* If the code is building on Windows, start exporting symbols. */
#undef NDDSUSERDllExport
  #define NDDSUSERDllExport __declspec(dllexport)
#endif

#ifdef __cplusplus
extern "C" {
    #endif

    /* ========================================================================= */
    /**
       Uses:     T
       Defines:  TTypeSupport, TDataWriter, TDataReader
    */

    #if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
      /* If the code is building on Windows, start exporting symbols. */
    #undef NDDSUSERDllExport
      #define NDDSUSERDllExport __declspec(dllexport)

    #endif

    DDS_DATAWRITER_C(ShapeTypeDataWriter, ShapeType);
    DDS_DATAREADER_C(ShapeTypeDataReader, ShapeTypeSeq, ShapeType);

    #ifdef __cplusplus
}
#endif

#if (defined(RTI_WIN32) || defined (RTI_WINCE)) && defined(NDDS_USER_DLL_EXPORT)
  /* If the code is building on Windows, stop exporting symbols. */
#undef NDDSUSERDllExport
  #define NDDSUSERDllExport
#endif

#endif  /* ShapeTypeSupport_434673458_h */

