/*
 * ����:������Ϣ���
 * ����:����
 * ��ʼ����:2009-6-13
 * ��������:2008-6-13
 * 
 * ����汾
 * �޸���:����
 * ��ʼ����:2008-12-29
 * ��������:2008-12-29
 * �޸Ĳ���:�����˵������õ��ڴ���亯��,���ڼ�¼
 * �ڴ�й©
 *
 */
#ifndef _LOGICDBG_V_1_0_H_
#define _LOGICDBG_V_1_0_H_

/*
 * ͷ�ļ�
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/*
 * �ļ���ʶ
 */
#define __LOGIC_DEBUG__		0xFFFFFFFF

/*
 * �꺯������
 */
/* ���Կ��� */
#ifdef _DEBUG
#define logic_malloc(l) debug_memory_alloc((unsigned long)(l), __FILE__, __LINE__)
#define logic_free(p) debug_memory_free((void *)(p))
#define logic_leak() debug_check_leak()
#else
#define logic_malloc(l) malloc(l)
#define logic_free(p) free(p)
#define logic_leak()
#endif

/*
 * ��������
 */
/* �ڴ���� */
void *debug_memory_alloc(unsigned long, const char *, unsigned long);
/* �ڴ��ͷ� */
void debug_memory_free(void *);
/* ��ʾ�ڴ��� */
void debug_check_leak(void);

#endif
