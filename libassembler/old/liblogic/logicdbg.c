/*
 * ͷ�ļ�
 */
#include "logicdbg.h"

/*
 * �ṹ����
 */
/* �ڴ��¼ָ�� */
typedef struct _ALLOCRCD
{
	/* ������ڴ� */
	void *memory;
	/* �ڴ�ĳ��� */
	unsigned long size;
	/* ���ڵ��ļ� */
	const char *file;
	/* �к� */
	unsigned long line;
	/* ��һ���ڵ� */
	struct _ALLOCRCD *next;
} ALLOCRCD, *PALLOCRCD;

/*
 * ��������
 */
PALLOCRCD alloc_header = NULL;
PALLOCRCD *alloc_current = &alloc_header;
unsigned long alloc_count = 0;

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
void *debug_memory_alloc(unsigned long sz, const char *file, unsigned long line) {
	void *p = (void *)malloc(sz);
	if (!p) return NULL;
	/* ���뵽���� */
	(*alloc_current) = (PALLOCRCD)malloc(sizeof(ALLOCRCD));
	assert(*alloc_current);
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
void debug_memory_free(void *p) {
	PALLOCRCD t = alloc_header;PALLOCRCD b = NULL;
	assert(p && alloc_header);
	/* �������� */
	while (t) {
		/* �ҵ�Ҫɾ���Ľڵ� */
		if (t->memory == p) {
			/* �����ͷ�ڵ� */
			if (!b) {
				b = t->next;
				if (t->memory) free(t->memory);
				free(t);
				alloc_header = b;
				/* ���������ֻ��һ��ͷ�ڵ� */
				if (!alloc_header) alloc_current = &alloc_header;
			/* �����β�ڵ� */
			} else if (!(t->next)) {
				if (t->memory) free(t->memory);
				free(t);
				b->next = NULL;
				alloc_current = &(b->next);
			/* ���ɾ���Ľڵ������м�λ�� */
			} else {
				b->next = t->next;
				if (t->memory) free(t->memory);
				free(t);
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
void debug_check_leak(void) {
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