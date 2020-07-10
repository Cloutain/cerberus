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
#ifndef _LINKEDLIST_V_1_0_H_
#define _LINKEDLIST_V_1_0_H_
/*
 * ͷ�ļ�
 */
#include "headfile.h"

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
#define _copy_linkedlist_node_(d, s) memcpy(d, s, sizeof(LINKEDLIST_NODE))
/* ���һ������ڵ� */
#define _clean_linkedlist_node_(d) memset(d, 0, sizeof(LINKEDLIST_NODE))

/*
 * ��������
 */
/* ��ʼ��һ������ */
PLINKEDLIST_NODE init_linkedlist(dword_t);
/* ɾ��һ������ */
void free_linkedlist(PLINKEDLIST_NODE *);
/* ��ĳ��λ�����һ���ڵ� */
PLINKEDLIST_NODE add_node_linkedlist(PLINKEDLIST_NODE, PLINKEDLIST_NODE, dword_t);
/* ɾ��ĳ��λ���ϵĽڵ� */
bool_t del_node_linkedlist(PLINKEDLIST_NODE *, dword_t);
/* ��ȡĳλ�õĽڵ� */
PLINKEDLIST_NODE query_node_linkedlist(PLINKEDLIST_NODE, dword_t);
/* ��ȡ����ڵ��� */
dword_t get_count_linkedlist(PLINKEDLIST_NODE);
/* ��ĩβ���һ���ڵ� */
void add_node_tail_linkedlist(PLINKEDLIST_NODE, PLINKEDLIST_NODE);

#endif
