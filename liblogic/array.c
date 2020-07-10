/*
 * ͷ�ļ�
 */
#include "array.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * ����:����
 * ��ʼ����:2011-7-11
 * �������:2010-7-11
 * ����:
 *    size:���г���
 *    auto_plus:�Ƿ����Զ�����,�������þ�ʱ
 * ����ֵ:
 * ˵��:
 *	  ��ʼ�������ڴ�.
 */
PARRAY __API__ init_array(__integer size, __bool auto_plus) {
	PARRAY keep = NULL;
	__memory buffer = NULL;
	if (size == 0) size = __DEF_MAX_ARRAY_SIZE__;
	buffer = logic_malloc(size);
	keep = logic_malloc(sizeof(ARRAY));
	keep->i = 0;
	keep->j = 0;
	keep->size = __DEF_MAX_ARRAY_SIZE__;
	keep->buffer = buffer;
	keep->auto_plus = auto_plus;
	return keep;
}

/*
 * ����:����
 * ��ʼ����:2011-7-11
 * �������:2011-7-11
 * ����:
 *    a:���нṹ.
 *    v:ѹ�����ݵ�ָ��.
 *    size:Ҫѹ��ĳ���.
 * ����ֵ:
 * ˵��:
 *	  ѹ�����ݵ�ջ.
 */
__memory __API__ set_array(PARRAY a, __void *v, __integer size) {
	__integer curr_size = a->i + size;
	__memory p = a->buffer + a->i;
	if (curr_size >= a->size) {
		if (a->auto_plus) {
			// �����Զ�����
			__memory tmp = logic_malloc(__DEF_MAX_ARRAY_SIZE__ + (a->size - a->j));
			__logic_memcpy__(tmp, (a->buffer + a->j), (a->size - a->j));
			logic_free(a->buffer);
			a->buffer = tmp;//�µĶ�ջ
			a->size = __DEF_MAX_ARRAY_SIZE__;
			a->i = a->j = 0;
		} else 
			return NULL;
	}
	
	__logic_memcpy__(p, (__memory)v, size);
	a->i += size;
	return (__memory)v;
}

/*
 * ����:����
 * ��ʼ����:2011-7-11
 * �������:2011-7-11
 * ����:
 *    a:���нṹ.
 *    size:Ҫ�����ĳ���.
 * ����ֵ:
 * ˵��:
 *	  �������ݴ�ջ.
 */
__memory  __API__ get_array(PARRAY a, __integer size) {
	__memory v = NULL;
	__integer i = a->i, j = a->j;

	if (j + size > i) return NULL;
	v = (__memory)(a->buffer + j);
	a->j += size;//��������
	return v;
}

/*
 * ����:����
 * ��ʼ����:2011-7-11
 * �������:2011-7-11
 * ����:
 *    a:�����ڴ�ָ��.
 * ����ֵ:
 * ˵��:
 *	  �ͷŶ����ڴ�.
 */
__void __API__ free_array(PARRAY *a) {
	if ((*a)->buffer) logic_free((*a)->buffer);
	logic_free(*a);
	(*a) = NULL;
}

#if defined(__cplusplus)
}
#endif