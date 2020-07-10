/*
 * ����:�����ϣ��
 * ����:����
 * ��ʼ����:2008-6-13
 * ��������:2009-6-13
 *
 * ����汾2
 * �޸���:����
 * ��ʼ����:2009-12-24
 * ��������:2009-12-24
 * �޸Ĳ���:�����˹�ϣ���ڵ�ֵ����,�����������ͷ�ʱ���õĻص�����
 *
 * ����汾
 * �޸���:����
 * ��ʼ����:2008-12-29
 * ��������:2009-1-8
 * �޸Ĳ���:�����˹�ϵ��ת������ĺ���,�޸���ɾ������
 *
 */
#if !defined(__HASHTREE_H__)
#define __HASHTREE_H__

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
#define __LOGIC_HASHTREE__		0xFFFFFFFF

/* 
 * ����ѡ��
 */
//#define DEBUG_HASHTREE		1

/*
 * �궨��
 */
/* Ĭ�Ϲ�ϣ���������� */
#ifdef DEBUG_HASHTREE
#define DEF_HASHTREE_PRIME				0x01
#else
#define DEF_HASHTREE_PRIME				0x05
#endif

/*
 * �꺯��
 */
/* ���ƽڵ� */
#define _copy_node_hashtree_(d, s)	__logic_memcpy__(d, s, sizeof(HASHTREE_NODE))
/* ��սڵ� */
#define _clean_node_hashtree_(d)	__logic_memset__(d, 0, sizeof(HASHTREE_NODE))
/* �ͷ�ֵ */
#define _free_value_hashnode_(d)\
	if ((d)->value) {\
		if ((d)->free_value) (d)->free_value((d)->value);\
		else logic_free((d)->value);\
	(d)->value = NULL;}

/*
 * �����ṹ����
 */
typedef __void (__INTERNAL_FUNC__ *FPHashNodeValueFreeCallBack)(__memory);

/*
 * �ṹ����
 */
/* ��ϣ�ڵ����ֳ��� */
#define HASHTREE_NODE_NAME_SIZE		512
/* ��ϣ���ڵ� */
typedef struct _HASHTREE_NODE {
	/* �ڵ��Ƿ�Ϊ�� */
	__bool empty;
	/* �ڵ��� */
	__byte *name;
	/* �ڵ������� */
	__integer name_len;
	/* ֵ�ڴ�ָ�� */
	__memory value;
	/* �ͷ�ֵ�ص����� */
	FPHashNodeValueFreeCallBack free_value;
	/* ���ڵ� */
	struct _HASHTREE_NODE *parents;
	/* ��ڵ� */
	struct _HASHTREE_NODE *left;
	/* �ҽڵ� */
	struct _HASHTREE_NODE *right;
} HASHTREE_NODE, *PHASHTREE_NODE;

/* ��ϣ�� */
typedef struct _HASHTREE {
	/* ��ϣ�����ڵ����� */
	__integer prime;
	/* ��ϣ�ڵ㼯�� */
	PHASHTREE_NODE node_set;
	/* �ڵ����� */
	__integer node_count;
	/* �ͷŽڵ�ֵ���� */
	FPHashNodeValueFreeCallBack free_value;
} HASHTREE, *PHASHTREE;

/* ��ϣ������ */
typedef struct _HASHLIST_NODE {
	/* ��ϣ�ڵ� */
	PHASHTREE_NODE hashnode;
	/* ��һ���ڵ� */
	struct _HASHLIST_NODE *next;
} HASHLIST_NODE, *PHASHLIST_NODE;

/*
 * ��������
 */
/* ��ʼ����ϣ�� */
PHASHTREE __API__ init_hashtree(__integer, FPHashNodeValueFreeCallBack);
/* �ͷŹ�ϣ�� */
__void __API__ free_hashtree(PHASHTREE *);
/* ����һ���ڵ㵽�� */
PHASHTREE_NODE __API__ add_node_hashtree(PHASHTREE, __byte *, __integer, __memory);
/* ��ѯ�ڵ� */
PHASHTREE_NODE __API__ query_node_hashtree(PHASHTREE, __byte *, __integer);
/* ɾ���ڵ� */
__bool __API__ del_node_hashtree(PHASHTREE, __byte *, __integer);
/* �����ϣ���ڵ� */
__integer __API__ count_node_hashtree(PHASHTREE);
/* ��ϣ��ת������ */
PHASHLIST_NODE __API__ hashtree_2_linkedlist(PHASHTREE);
/* �ͷŹ�ϣ������ */
__void __API__ free_hashtree_linkedlist(PHASHLIST_NODE *);

#if defined(__cplusplus)
}
#endif

#endif
