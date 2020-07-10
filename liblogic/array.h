/*
 * ����:���в���
 * ����:����
 * ��ʼ����:2011-7-11
 * ��������:2011-7-11
 *
 */
#if !defined(__ARRAY_H__)
#define __ARRAY_H__

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
#define __DEF_MAX_ARRAY_SIZE__	1024

/*
 * �꺯��
 */

/*
 * �ṹ����
 */
/* ���нṹ���� */
typedef struct _ARRAY {
	__bool auto_plus;//�Զ�����,�����кľ�ʱ
	__memory buffer;//�����ڴ�
	__integer i;//��ǰ����
	__integer j;//ȡֵ������
	__integer size;//���д�С
} ARRAY, *PARRAY;

/*
 * ��������
 */
/* ��ʼ������ */
PARRAY __API__ init_array(__integer, __bool);
/* ѹ�� */
__memory __API__ set_array(PARRAY, __void *, __integer);
/* ���� */
__memory __API__ get_array(PARRAY, __integer);
/* �ͷŶ����ڴ� */
__void __API__ free_array(PARRAY *);

#if defined(__cplusplus)
}
#endif

#endif
