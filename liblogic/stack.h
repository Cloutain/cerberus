/*
 * ����:��ջ����
 * ����:����
 * ��ʼ����:2010-4-21
 * ��������:2010-4-21
 *
 * ����汾
 * �޸���:����
 * ��ʼ����:2010-6-18
 * ��������:2010-6-18
 * �޸Ĳ���:�����˶�ջ��̬��������,��ѹջʱ�ڴ�ľ�
 * �������Զ������ڴ�
 */
#if !defined(__STACK_H__)
#define __STACK_H__

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
#define __LOGIC_STACK__		0xFFFFFFFF

/* 
 * ����ѡ��
 */
//#define DEBUG_STACK		1

/*
 * �궨��
 */
#define __DEF_MAX_STACK_SIZE__	1024

/*
 * �꺯��
 */

/*
 * �ṹ����
 */
/* ջ�ṹ���� */
typedef struct _STACK {
	__bool auto_plus;//�Զ�����,����ջ�ľ�ʱ
	__memory stack;//ջ�ڴ�
	__integer i;//��ǰ����
	__integer size;//ջ��С
} STACK, *PSTACK;

/*
 * ��������
 */
/* ��ʼ����ջ */
PSTACK __API__ init_stack(__integer, __bool);
/* ѹ�� */
__memory __API__ push_stack(PSTACK, __void *, __integer);
/* ���� */
__memory __API__ pop_stack(PSTACK, __integer);
/* �ͷŶ�ջ�ڴ� */
__void __API__ free_stack(PSTACK *);

#if defined(__cplusplus)
}
#endif

#endif
