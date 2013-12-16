/*
  WARNING: THIS FILE IS AUTO-GENERATED. DO NOT MODIFY.

  This file was generated from ShapeType.idl using "rtiddsgen".
  The rtiddsgen tool is part of the RTI Connext Micro distribution.
  For more information, type 'rtiddsgen -help' at a command shell
  or consult the RTI Connext manual.
*/

#include "ShapeType.h"

/*** SOURCE_BEGIN ***/

/* ========================================================================= */

const char *ShapeTypeTYPENAME = "ShapeType";

RTI_BOOL
ShapeType_initialize(ShapeType* sample)
{
    if (!CDR_String_initialize(&sample->color, (128)))
        {
        return RTI_FALSE;
    }
    CDR_Primitive_init_Long(&sample->x);
    CDR_Primitive_init_Long(&sample->y);
    CDR_Primitive_init_Long(&sample->shapesize);
    return RTI_TRUE;
}

RTI_BOOL
ShapeType_finalize(ShapeType* sample)
{
    CDR_String_finalize(&sample->color);
    return RTI_TRUE;
}

ShapeType *
ShapeType_create()
{
    ShapeType* sample;
    OSAPI_Heap_allocate_struct(&sample, ShapeType);
    if (sample != NULL) {
        if (!ShapeType_initialize(sample)) {
            OSAPI_Heap_free_struct(sample);
            return NULL;
        }
    }
    return sample;
}

void
ShapeType_delete(ShapeType*sample)
{

    if (sample != NULL) {
        ShapeType_finalize(sample);
        OSAPI_Heap_free_struct(sample);
    }
}

RTI_BOOL
ShapeType_copy(ShapeType* dst,const ShapeType* src)
{        
    if (!CDR_String_copy(&dst->color, &src->color, (128)))
        {
        return RTI_FALSE;
    }
    CDR_Primitive_copy_Long(&dst->x, &src->x);
    CDR_Primitive_copy_Long(&dst->y, &src->y);
    CDR_Primitive_copy_Long(&dst->shapesize, &src->shapesize);

    return RTI_TRUE;
}

/**
 * <<IMPLEMENTATION>>
 *
 * Defines:  TSeq, T
 *
 * Configure and implement 'ShapeType' sequence class.
 */

#define T ShapeType
#define TSeq ShapeTypeSeq
#define T_initialize ShapeType_initialize
#define T_finalize   ShapeType_finalize
#define T_copy       ShapeType_copy

#undef reda_sequence_gen_h
#include "reda/reda_sequence_gen.h"

#undef T_copy
#undef T_finalize
#undef T_initialize
#undef TSeq
#undef T

