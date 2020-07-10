/*
 * ����:���Ա�
 * ����:����
 * ��ʼ����:2009-1-1
 * ��������:2009-1-1
 * 
 * ����汾
 * �޸���:-
 * ��ʼ����:-
 * ��������:-
 * �޸Ĳ���:-
 *
 */
#if !defined(__LINKEDLIST_H__)
#define __LINKEDLIST_H__

/*
 * ͷ�ļ�
 */
#include "headfile.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * �ļ���ʶ
 */
#define __LOGIC_LINKEDLIST__		0xFFFFFFFF

/*
 * ����ѡ��
 */
//#define DEBUG_LINKEDLIST		1

/*
 * �ṹ����
 */
/* ����ڵ� */
typedef struct _LINKEDLIST_NODE {
	/* ��һ���ڵ� */
	struct _LINKEDLIST_NODE *previous;
	/* ��һ���ڵ� */
	struct _LINKEDLIST_NODE *next;
} LINKEDLIST_NODE, *PLINKEDLIST_NODE;

/*
 * �꺯������
 */
/* ����һ������ڵ� */
#define _create_linkedlist_node_(p_linkedlist_node) {\
	p_linkedlist_node = (PLINKEDLIST_NODE)logic_malloc(sizeof(LINKEDLIST_NODE));\
	p_linkedlist_node->previous = NULL;\
	p_linkedlist_node->next = NULL;\
}
/* ����һ������ڵ� */
#define _copy_linkedlist_node_(d, s) __logic_memcpy__(d, s, sizeof(LINKEDLIST_NODE))
/* ���һ������ڵ� */
#define _clean_linkedlist_node_(d) __logic_memset__(d, 0, sizeof(LINKEDLIST_NODE))

/*
 * ��������
 */
/* ��ʼ��һ������ */
PLINKEDLIST_NODE __API__ init_linkedlist(__integer);
/* ɾ��һ������ */
__void __API__ free_linkedlist(PLINKEDLIST_NODE *);
/* ��ĳ��λ�����һ���ڵ� */
PLINKEDLIST_NODE __API__ add_node_linkedlist(PLINKEDLIST_NODE, PLINKEDLIST_NODE, __integer);
/* ɾ��ĳ��λ���ϵĽڵ� */
__bool __API__ del_node_linkedlist(PLINKEDLIST_NODE *, __integer);
/* ��ȡĳλ�õĽڵ� */
PLINKEDLIST_NODE __API__ query_node_linkedlist(PLINKEDLIST_NODE, __integer);
/* ��ȡ����ڵ��� */
__integer __API__ get_count_linkedlist(PLINKEDLIST_NODE);
/* ��ĩβ���һ���ڵ� */
__void __API__ add_node_tail_linkedlist(PLINKEDLIST_NODE, PLINKEDLIST_NODE);

#if defined(__cplusplus)
}
#endif

#endif
