/*
 * ����:�౶���ȳ˷�
 * ����:����
 * ��ʼ����:2011-7-28
 * ��������:2011-7-28
 *
 */
//#if !defined(__MP_H__)
//#define __MP_H__

///*
// * ͷ�ļ�
// */
//#include "headfile.h"
//
//#if defined(__cplusplus)
//extern "C"
//{
//#endif
//
///*
// * �ļ���ʶ
// */
//#define __LOGIC_MP__		0xFFFFFFFF
//
///* 
// * ����ѡ��
// */
////#define DEBUG_MP		1
//
///*
// * �궨��
// */
///* Ĭ�Ϲ�ϣ���������� */
//#define MAX_MP_NUMBER				32
///* Ĭ�Ϲ�ϣ���������� */
//#define MP_RADIX					4294967296.0//0x100000000
///* ���� */
//#define POSITIVE					0
///* ���� */
//#define NEGATIVE					1
//
///*
// * �꺯��
// */
///* ������0 */
//#define _mp_is_zero_(a) ((a)->n == 0)
///* ������1 */
//#define _mp_is_one_(a) ((a)->n == 1 && (a)->v[0] == 1)
//
///*
// * �����ṹ����
// */
//
//
///*
// * �ṹ����
// */
///* �ྫ�ȱ��˽ṹ */
//typedef struct _MP_TYPE{
//	/* ����ṹ�ж��ٸ����� */
//	__integer n;
//	/* �ṹ����ֵ */
//	__dword v[MAX_MP_NUMBER];
//} MP_TYPE, *PMP_TYPE;
//
///* ����Ķྫ�ȱ��˽ṹ */
//typedef struct _EX_MP_TYPE{
//	/* ����ṹ�ж��ٸ����� */
//	__integer n;
//	/* �ṹ����ֵ */
//	__dword v[2 * MAX_MP_NUMBER];
//} EX_MP_TYPE, *PEX_MP_TYPE;
//
///* �����ŵĶྫ�ȱ��˽ṹ */
//typedef struct _SIGNED_MP_TYPE {
//	/* ����λ */
//	__byte sign;
//	/* ��ֵ */
//	MP_TYPE num;
//} SIGNED_MP_TYPE, *PSIGNED_MP_TYPE;
//
///* �����ŵĸ���ྫ�ȱ��˽ṹ */
//typedef struct _SIGNED_MP_TYPE {
//	/* ����λ */
//	__byte sign;
//	/* ��ֵ */
//	EX_MP_TYPE num;
//} SIGNED_MP_TYPE, *PSIGNED_MP_TYPE;
//
///*
// * ��������
// */
///* ���MP�ṹ */
//__void __API__ mp_clear(PMP_TYPE);
///* ��մ���ǵ�MP�ṹ */
//__void __API__ signed_mp_clear(PSIGNED_MP_TYPE);
///* ����MP�ṹ */
//__void __API__ mp_copy(PMP_TYPE, PMP_TYPE);
///* ���Ʒ���MP�ṹ */
//__void __API__ signed_mp_copy(PSIGNED_MP_TYPE, PSIGNED_MP_TYPE);
///* ����MP�ṹ��λ�� */
//__integer __API__ mp_bits(PMP_TYPE);
///* ������eΪ����MPΪ���Ķ���ֵ */
//__double __API__ mp_e_log(PMP_TYPE);
///* ����MP�ṹ����MP�ṹ */
//__void __API__ mp_add_mp(PMP_TYPE, PMP_TYPE, PMP_TYPE);
///* ����MP�ṹ����һ������ */
//__void __API__ mp_add_integer(PMP_TYPE, __integer, PMP_TYPE);
///* ����MP�ṹ��ȥMP�ṹ */
//__void __API__ mp_sub_mp(PMP_TYPE, PMP_TYPE, PMP_TYPE);
///* ����MP�ṹ��ȥһ������ */
//__void __API__ mp_sub_integer(PMP_TYPE, __integer, PMP_TYPE);
///* ������λMP�ṹ�ӷ� */
//__void __API__ signed_mp_add(PSIGNED_MP_TYPE, PSIGNED_MP_TYPE, PSIGNED_MP_TYPE);
///* ������λMP�ṹ���� */
//__void __API__ signed_mp_sub(PSIGNED_MP_TYPE, PSIGNED_MP_TYPE, PSIGNED_MP_TYPE);
///* ��MP��ֵ����nλ,����ƶ�λ��Ϊ32 * MAX_MP_NUMBER */
//__void __API__ mp_rshift(PMP_TYPE, __integer, PMP_TYPE);

//#if defined(__cplusplus)
//}
//#endif
//
//#endif