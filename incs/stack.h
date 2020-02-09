/**
 * generic-stack.h
 *   Declarations for a simple generic implementation of stacks.
 *
 * <insert MIT license>
 */

#ifdef TYPE

// +--------+--------------------------------------------------------
// | Macros |
// +--------+

/**
 * By default, we do not rename procedures and structs.
 */
#ifndef TYPED
#define TYPED(THING) THING
#endif

#ifndef NULL
#define NULL 0
#endif
// +-------+---------------------------------------------------------
// | Types |
// +-------+

typedef struct TYPED(Stack) TYPED(Stack);

// +------------+----------------------------------------------------
// | Procedures |
// +------------+

/**
 * Create a new stack.
 */
TYPED(Stack)* TYPED(stack_new) (void);

/**
 * Free the space allocated to a stack.
 */
void TYPED(stack_free) (TYPED(Stack) *stack);

void TYPED(stack_to_list) (TYPED(Stack) *stack, TYPE* thislist);

/**
 * Determine the number of elements in the stack.
 */
int TYPED(stack_size) (TYPED(Stack) *stack);

/**
 * Look at the top element of the stack.  Requires that the
 * stack have at least one element.
 */
TYPE TYPED(stack_top) (TYPED(Stack) *stack);

/**
 * Remove and return the top element of the stack.  Requires that
 * the stack have at least one element.
 */
TYPE TYPED(stack_pop) (TYPED(Stack) *stack);

/**
 * Add an element to the stack.
 */
void TYPED(stack_push) (TYPED(Stack) *stack, TYPE val);

#endif // TYPE
