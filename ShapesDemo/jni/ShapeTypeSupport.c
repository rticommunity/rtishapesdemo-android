/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from ShapeType.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Data Distribution Service distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Data Distribution Service manual.
*/

#include <stdlib.h>

#include "ShapeTypeSupport.h"
#include "ShapeTypePlugin.h"

/*** SOURCE_BEGIN ***/

/* ========================================================================= */
/* DDSDataWriter
   Defines:   TDataWriter, TData
*/

/* Requires */
#define TTYPENAME   ShapeTypeTYPENAME

/* Defines */
#define TDataWriter ShapeTypeDataWriter
#define TData       ShapeType
#undef dds_c_tdatawriter_gen_h  /* allow reuse of include file */
#include "dds_c/dds_c_tdatawriter_gen.h"
#undef TDataWriter
#undef TData

#undef TTYPENAME

/* ========================================================================= */
/* DDSDataReader
   Defines:   TDataReader, TData
*/

/* Requires */
#define TTYPENAME   ShapeTypeTYPENAME

/* Defines */
#define TDataReader ShapeTypeDataReader
#define TDataSeq    ShapeTypeSeq
#define TData       ShapeType
#undef dds_c_tdatareader_gen_h  /* allow reuse of include file */
#include "dds_c/dds_c_tdatareader_gen.h"
#undef TDataReader
#undef TDataSeq
#undef TData

#undef TTYPENAME

