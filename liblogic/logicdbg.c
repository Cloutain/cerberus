/*
 * ͷ�ļ�
 */
#include "logicdbg.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * �ṹ����
 */
/* �ڴ��¼ָ�� */
typedef struct _ALLOCRCD {
	/* ������ڴ� */
	__memory memory;
	/* �ڴ�ĳ��� */
	__integer size;
	/* ���ڵ��ļ� */
	const __char *file;
	/* �к� */
	__integer line;
	/* ��һ���ڵ� */
	struct _ALLOCRCD *next;
} ALLOCRCD, *PALLOCRCD;

/*
 * ��������
 */
PALLOCRCD alloc_header = NULL;
PALLOCRCD *alloc_current = &alloc_header;
__integer alloc_count = 0;

/*
 * ��������
 */
/*
 * ����:����
 * ��ʼ����:2009-6-14
 * �������:2009-6-14
 * ����:
 *    sz:Ҫ������ڴ�.
 *    file:Դ�ļ���.
 *    line:�к�.
 * ����ֵ:
 *	  ������ڴ�ָ��.
 *    ʧ�ܷ���NULL.
 * ˵��:
 *    �����ڴ沢��¼���ڴ�����.
 */
__memory __INTERNAL_FUNC__ debug_memory_alloc(__integer sz, const __char *file, __integer line) {
	__memory p = (__memory)__logic_new_size__(sz);
	if (!p) return NULL;
	/* ���뵽���� */
	(*alloc_current) = (PALLOCRCD)__logic_new_size__(sizeof(ALLOCRCD));
	(*alloc_current)->file = file;
	(*alloc_current)->line = line;
	(*alloc_current)->memory = p;
	(*alloc_current)->size = sz;
	(*alloc_current)->next = NULL;
	alloc_current = &((*alloc_current)->next);
	alloc_count++;
	return p;
}/* end debug_memory_alloc */

/*
 * ����:����
 * ��ʼ����:2009-6-14
 * �������:2009-6-14
 * ����:
 *    p:Ҫ�ͷŵ��ڴ�.
 * ����ֵ:
 * ˵��:
 *    �ͷ��ڴ�.
 */
__void __INTERNAL_FUNC__ debug_memory_free(__memory p) {
	PALLOCRCD t = alloc_header;PALLOCRCD b = NULL;
	/* �������� */
	while (t) {
		/* �ҵ�Ҫɾ���Ľڵ� */
		if (t->memory == p) {
			/* �����ͷ�ڵ� */
			if (!b) {
				b = t->next;
				if (t->memory) __logic_delete__(t->memory);
				__logic_delete__(t);
				alloc_header = b;
				/* ���������ֻ��һ��ͷ�ڵ� */
				if (!alloc_header) alloc_current = &alloc_header;
			/* �����β�ڵ� */
			} else if (!(t->next)) {
				if (t->memory) __logic_delete__(t->memory);
				__logic_delete__(t);
				b->next = NULL;
				alloc_current = &(b->next);
			/* ���ɾ���Ľڵ������м�λ�� */
			} else {
				b->next = t->next;
				if (t->memory) __logic_delete__(t->memory);
				__logic_delete__(t);
			}/* end else */
			alloc_count--;
			return;
		}/* end if */
		b = t;
		t = t->next;
	}/* end while */
}/* debug_memory_free */

/*
 * ����:����
 * ��ʼ����:2009-6-14
 * �������:2009-6-14
 * ����:
 * ����ֵ:
 * ˵��:
 *    ����ڴ�й©,����ڳ����������ʱʹ��.
 */
__void __INTERNAL_FUNC__ debug_check_leak(__void) {
	PALLOCRCD t = alloc_header;
	/* ���ڴ�й© */
	if (!alloc_count) {
		printf("aha... no memory leak! ^_^\n");
		return;
	}
	printf("%d memory not logic_free\n", alloc_count);
	while (t) {
		printf("at source code[%s:%lu] -> %p,size=%lu\n", t->file, t->line, t->memory, t->size);
		t = t->next;
	}
}/* end debug_show_alloc */

#if defined(__cplusplus)
}
#endif
