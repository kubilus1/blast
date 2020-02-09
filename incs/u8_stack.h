#ifndef _U8_STACK_H_
#define _U8_STACK_H_
#include "blast.h"
/**
 * double-stack.h
 *   Stacks of doubles.
 *
 * <insert MIT license>
 */

#undef TYPE
#undef TYPED

#define TYPE u8
#define TYPED(THING) u8_ ## THING

#include "stack.h"

#endif // _U8_STACK_H_
