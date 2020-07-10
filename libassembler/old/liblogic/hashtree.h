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
#ifndef _HASHTREE_V_1_0_H_
#define _HASHTREE_V_1_0_H_

/*
 * ͷ�ļ�
 */
#include "headfile.h"

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
#define _copy_node_hashtree_(d, s) memcpy(d, s, sizeof(HASHTREE_NODE))
/* ��սڵ� */
#define _clean_node_hashtree_(d) memset(d, 0, sizeof(HASHTREE_NODE))
/* �ͷ�ֵ */
#define _free_value_hashnode_(d)\
	if ((d)->value) {\
		if ((d)->free_value) (d)->free_value((d)->value);\
		else logic_free((d)->value);\
	(d)->value = NULL;}

/*
 * �����ṹ����
 */
typedef void (__cdecl *FPHashNodeValueFreeCallBack)(void *);

/*
 * �ṹ����
 */
/* ��ϣ�ڵ����ֳ��� */
#define HASHTREE_NODE_NAME_SIZE		512
/* ��ϣ���ڵ� */
typedef struct _HASHTREE_NODE {
	/* �ڵ��Ƿ�Ϊ�� */
	bool_t empty;
	/* �ڵ��� */
	byte_t *name;
	/* �ڵ������� */
	dword_t name_len;
	/* ֵ�ڴ�ָ�� */
	void *value;
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
	dword_t prime;
	/* ��ϣ�ڵ㼯�� */
	PHASHTREE_NODE node_set;
	/* �ڵ����� */
	dword_t node_count;
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
PHASHTREE init_hashtree(dword_t, FPHashNodeValueFreeCallBack);
/* �ͷŹ�ϣ�� */
void free_hashtree(PHASHTREE *);
/* ����һ���ڵ㵽�� */
PHASHTREE_NODE add_node_hashtree(PHASHTREE, byte_t *, dword_t, void *);
/* ��ѯ�ڵ� */
PHASHTREE_NODE query_node_hashtree(PHASHTREE, byte_t *, dword_t);
/* ɾ���ڵ� */
bool_t del_node_hashtree(PHASHTREE, byte_t *, dword_t);
/* �����ϣ���ڵ� */
dword_t count_node_hashtree(PHASHTREE);
/* ��ϣ��ת������ */
PHASHLIST_NODE hashtree_2_linkedlist(PHASHTREE);
/* �ͷŹ�ϣ������ */
void free_hashtree_linkedlist(PHASHLIST_NODE *);

#endif
