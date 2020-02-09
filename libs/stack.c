
#ifdef TYPE

#ifndef TYPED
#define TYPED(THING) THING
#endif

#ifndef NULL
#define NULL 0
#endif 

/**
 * The nodes in our stack.
 */
struct StackNode
  {
    TYPE val;                    // The value at the top of the stack
    struct StackNode *next;     // The rest of the values in the stack.
  };

/**
 * The stack itself.
 */
struct TYPED(Stack)
  {
    int size;                   // How many elements are in the stack?
    struct StackNode *top;      // What's at the top of the stack?
  };

//Stack *
//stack_new (void)
TYPED(Stack)* TYPED(stack_new) (void)
{
  struct TYPED(Stack) *new_stack = (struct TYPED(Stack) *) MEM_alloc (sizeof (struct TYPED(Stack)));
  new_stack->size = 0;
  new_stack->top = NULL;
  return new_stack;
} // stack_new

//void 
//stack_free (Stack *stack)
void TYPED(stack_free) (TYPED(Stack) *stack)
{
  // Free all the nodes in the stack
  struct StackNode *tmp;
  while ((tmp = stack->top) != NULL)
    {
      stack->top = tmp->next;
      MEM_free (tmp);
    } // while
  // Free the stack itself
  MEM_free (stack);
} // stack_free

void TYPED(stack_to_list) (TYPED(Stack) *stack, TYPE* thislist) {
    struct StackNode *tmp;
    int i = 0;
    while ((tmp = stack->top) != NULL) {
        stack->top = tmp->next;
        thislist[i] = tmp->val;        
    } // while
}

//int 
//stack_size (Stack *stack)
int TYPED(stack_size) (TYPED(Stack) *stack)
{ 
  return stack->size;
} // stack_size
 
//int 
//stack_top (Stack *stack)
TYPE TYPED(stack_top) (TYPED(Stack) *stack)
{
  return stack->top->val;
} // stack_top

//int 
//stack_pop (Stack *stack)
TYPE TYPED(stack_pop) (TYPED(Stack) *stack)
{
  // Remember the top node and its value
  struct StackNode *tmp = stack->top;
  int top = tmp->val;
  // Drop that node
  stack->top = stack->top->next;
  MEM_free (tmp);
  --stack->size;
  // And return the saved value
  return top;
} // stack_pop

//void 
//stack_push (Stack *stack, int val)
void TYPED(stack_push) (TYPED(Stack) *stack, TYPE val)
{
    struct StackNode *new_top = 
        (struct StackNode *) MEM_alloc (sizeof (struct StackNode));

    new_top->val = val;
    new_top->next = stack->top;

    stack->top = new_top;
    stack->size++;
} // stack_push
#endif // TYPE
