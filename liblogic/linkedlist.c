/*
 * ͷ�ļ�
 */
#include "linkedlist.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * ��������
 */
/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    n:Ҫ��������Ľڵ���Ŀ.
 * ����ֵ:
 *    ����ͷ�ڵ�ָ��.
 * ˵��:
 *    ��ʼ��һ������.
 */
PLINKEDLIST_NODE __API__ init_linkedlist(__integer n) {
	PLINKEDLIST_NODE node;
	if (!n) return NULL;
	node = (PLINKEDLIST_NODE)logic_malloc(sizeof(LINKEDLIST_NODE));
	node->next = init_linkedlist(n-1);
	if (node->next) (node->next)->previous = node;
	node->previous = NULL;	
	return node;
}/* end init_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    pll:ָ������ָ���ָ��.
 * ����ֵ:
 *    ��.
 * ˵��:
 *    �ͷ�һ������.
 */
__void __API__ free_linkedlist(PLINKEDLIST_NODE *pll) {
	if ((*pll)->next) free_linkedlist(&((*pll)->next));
	logic_free(*pll);*pll = NULL;
	return;
}/* end free_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    ppl:ָ������ͷָ���ָ��.
 *	  i:λ��,������1��ʼ.
 * ����ֵ:
 *    T:ɾ���ɹ�.
 *    F:ɾ��ʧ��.
 * ˵��:
 *	  ɾ��ĳ��λ���ϵĽڵ�.
 */
__bool __API__ del_node_linkedlist(PLINKEDLIST_NODE *ppl, __integer i) {
	PLINKEDLIST_NODE now = NULL;
	if (!i) return FALSE;
	now = query_node_linkedlist(*ppl, i);
	if (!now) return FALSE;
	/* �����ͷ�ڵ� */
	if (now == *ppl) {
		PLINKEDLIST_NODE exchange = now->next;
		/* ���ֻ��ͷ�ڵ� */
		if (!exchange) {
			logic_free(now); *ppl = NULL;
			return TRUE;
		}/* end if */
		logic_free(now);
		_copy_linkedlist_node_(now, exchange);
		now->previous = NULL;
		return TRUE;
	} else {
		(now->previous)->next = (now)->next;
		if (now->next) (now->next)->previous = (now)->previous;
		logic_free(now);
		return TRUE;
	}
}/* end del_node_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    pll:ָ������ͷָ��.
 *	  i:λ��,������1��ʼ.
 * ����ֵ:
 *    ��ѯ�Ľڵ�.
 * ˵��:
 *	  ��ȡһ��λ�õĽڵ�,
 *	  ����������NULL.
 */
PLINKEDLIST_NODE __API__ query_node_linkedlist(PLINKEDLIST_NODE pll, __integer i) {
	PLINKEDLIST_NODE now = pll;
	/* ���i = 0 */
	if (!i) return NULL;
	now = query_node_linkedlist(now->next, i-1);
	return now;
}/* end query_node_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    pll:ָ������ͷָ��.
 * ����ֵ:
 *    ����ڵ������.
 * ˵��:
 *	  �ڵ�����.
 */
__integer __API__ get_count_linkedlist(PLINKEDLIST_NODE pll) {
	__integer count = 1;
	if (!pll) return 0;
	count += get_count_linkedlist(pll->next);
#ifdef DEBUG_LINKEDLIST
	printf("linkedlist_node Index = %d\n", count-1);
	printf("linkedlist_node->previous = 0x%X\n", pll->previous);
	printf("linkedlist_node->next = 0x%X\n", pll->next);
	printf("--------------------\n");
#endif
	return count;
}/* end get_count_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    pllh:ָ������ͷָ��.
 *	  plli:ָ��Ҫ����Ľڵ�.
 *	  i:Ҫ���������.
 * ����ֵ:
 *    �����ڵ��ָ��.
 * ˵��:
 *    ��ĳ��λ�ò���ڵ�,
 *	  ���λ�ô�0��ʼ����.
 */
PLINKEDLIST_NODE __API__ add_node_linkedlist(PLINKEDLIST_NODE pllh, PLINKEDLIST_NODE plli, __integer i) {
	PLINKEDLIST_NODE now = NULL;
	now = query_node_linkedlist(pllh, i);
	if (!now) return NULL;
	plli->previous = now->previous;
	plli->next = now;
	now->previous = plli;
	return now;
}/* end add_node_linkedlist */

/*
 * ����:����
 * ��ʼ����:2009-1-1
 * �������:2009-1-1
 * ����:
 *    pllh:ָ������ͷָ��.
 *	  plli:ָ��Ҫ����Ľڵ�.
 * ����ֵ:
 *    �����ڵ��ָ��.
 * ˵��:
 *	  ��.
 */
__void __API__ add_node_tail_linkedlist(PLINKEDLIST_NODE pllh, PLINKEDLIST_NODE plli) {
	if (pllh->next) add_node_tail_linkedlist(pllh->next, plli);
	else {
		pllh->next = plli;
		plli->previous = pllh;
		plli->next = NULL;
	}
	return;
}/* end add_node_tail_linkedlist */

#if defined(__cplusplus)
}
#endif