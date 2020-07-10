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
#if !defined(__LOGICDBG_H__)
#define __LOGICDBG_H__

/*
 * ͷ�ļ�
 */
#include "Common.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * �ļ���ʶ
 */
#define __LOGIC_DEBUG__		0xFFFFFFFF

/*
 * �꺯������
 */
/* ���Կ��� */
#ifdef _DEBUG
#define logic_malloc(l) debug_memory_alloc((__integer)(l), __FILE__, __LINE__)
#define logic_free(p) debug_memory_free((__memory)(p))
#define logic_leak() debug_check_leak()
#else
#define logic_malloc(l) __logic_new_size__(l)
#define logic_free(p) __logic_delete__(p)
#define logic_leak()
#endif

/*
 * ��������
 */
/* �ڴ���� */
__memory __INTERNAL_FUNC__ debug_memory_alloc(__integer, const __memory, __integer);
/* �ڴ��ͷ� */
__void __INTERNAL_FUNC__ debug_memory_free(__memory);
/* ��ʾ�ڴ��� */
__void __INTERNAL_FUNC__ debug_check_leak(__void);

#if defined(__cplusplus)
}
#endif

#endif
