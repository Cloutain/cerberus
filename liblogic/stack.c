/*
 * ͷ�ļ�
 */
#include "stack.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * ����:����
 * ��ʼ����:2010-4-21
 * �������:2010-4-21
 * ����:
 *    size:��ջ����
 *    auto_plus:�Ƿ����Զ�����,����ջ�þ�ʱ
 * ����ֵ:
 * ˵��:
 *	  ��ʼ��ջ�ڴ�.
 */
PSTACK __API__ init_stack(__integer size, __bool auto_plus) {
	PSTACK keep = NULL;
	__memory stack = NULL;
	if (size == 0) size = __DEF_MAX_STACK_SIZE__;
	stack = logic_malloc(size);
	keep = logic_malloc(sizeof(STACK));
	keep->i = 0;
	keep->size = __DEF_MAX_STACK_SIZE__;
	keep->stack = stack;
	keep->auto_plus = auto_plus;
	return keep;
}

/*
 * ����:����
 * ��ʼ����:2010-4-21
 * �������:2010-4-21
 * ����:
 *    stack:��ջ�ṹ.
 *    v:ѹ�����ݵ�ָ��.
 *    size:Ҫѹ��ĳ���.
 * ����ֵ:
 * ˵��:
 *	  ѹ�����ݵ�ջ.
 */
__memory __API__ push_stack(PSTACK stack, __void *v, __integer size) {
	__integer curr_size = stack->i + size;
	__memory p = stack->stack + stack->i;
	if (curr_size >= stack->size) {
		if (stack->auto_plus) {
			// �����Զ�����
			__memory tmp = logic_malloc(__DEF_MAX_STACK_SIZE__ + stack->size);
			__logic_memcpy__(tmp, stack->stack, stack->size);
			logic_free(stack->stack);
			stack->stack = tmp;//�µĶ�ջ
			stack->size += __DEF_MAX_STACK_SIZE__;
		} else 
			return NULL;
	}
	
	__logic_memcpy__(p, (__memory)v, size);
	stack->i += size;
	return (__memory)v;
}

/*
 * ����:����
 * ��ʼ����:2010-4-21
 * �������:2010-4-21
 * ����:
 *    stack:��ջ�ṹ.
 *    size:Ҫ�����ĳ���.
 * ����ֵ:
 * ˵��:
 *	  �������ݴ�ջ.
 */
__memory  __API__ pop_stack(PSTACK stack, __integer size) {
	__memory v;
	__integer i = stack->i - size;
	__integer curr_size = stack->i;
	if (curr_size < size) return NULL;
	v = stack->stack + i;
	stack->i -= size;
	return v;
}

/*
 * ����:����
 * ��ʼ����:2010-4-21
 * �������:2010-4-21
 * ����:
 *    stack:ջ�ڴ�ָ��.
 * ����ֵ:
 * ˵��:
 *	  �ͷŶ�ջ�ڴ�.
 */
__void __API__ free_stack(PSTACK *stack) {
	if ((*stack)->stack) logic_free((*stack)->stack);
	logic_free(*stack);
	(*stack) = NULL;
}

#if defined(__cplusplus)
}
#endif