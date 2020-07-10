/*
 * ����:ģʽƥ���㷨.
 * ����:����
 * ��ʼ����:2009-7-5
 * ��������:2009-7-19
 * 
 * ����汾
 * �޸���:-
 * ��ʼ����:-
 * ��������:-
 * �޸Ĳ���:-
 *
 */
#if !defined(__PATTERN_MATCHER_H__)
#define __PATTERN_MATCHER_H__
/*
 * ͷ�ļ�
 */
#include "headfile.h"
#include "hashtree.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/* 
 * �ļ���ʶ
 */
#define __LOGIC_PATTERN_MATCHER__		0xFFFFFFFF

/* 
 * ����ѡ��
 */
//#define DEBUG_PATTERN_MATCHER		1

/*
 * �궨��
 */
/* BM�㷨 */
#define LPM_BM					0x01
/* WM�㷨 */
#define LPM_WM					0x02
/* ��ϣ������ */
#define HASH_TYPE				__word
/* �ƶ����� */
#define BWSHIFTABLESIZE			(64*1024)
/* ��ϣ���� */
#define HASHTABLESIZE			BWSHIFTABLESIZE
/* ��ϣ�ֽڳ��� */
#define HASHBYTES16				2

/*
 * ��������ָ��
 */
/* ������� */
/* 1:ģʽID,2:ƫ��,3:ģʽ,4:ģʽ����,5:�ı�ָ�� */
/* ����T:�ҵ�һ��ֱ�ӷ���,����F:��������ֱ��ȫ���ı�������� */
typedef __bool (__API__ *wm_get1)(__integer, __integer, const __byte *, __integer, const __byte *);

/*
 * �ṹ����
 */
/* BMģʽ�ṹ */
typedef struct _BM_PATTERN {
	/* ���ַ����� */
	__integer *bc;
	/* �ú�׺���� */
	__integer *gs;
} BM_PATTERN, *PBM_PATTERN;

/* WMģʽ�ṹ */
typedef struct _WM_PATTERN {
	/* ��һ��ģʽ */
	struct _WM_PATTERN *next;
	/* ģʽ��ID */
	__integer id;
	/* ģʽ */
	__byte *ptn;
	/* ģʽ���� */
	__integer ptnlen;
	/* BMģʽ */
	PBM_PATTERN bm_ptn;
} WM_PATTERN, *PWM_PATTERN;

/* WM�ṹ */
typedef struct _WM {
	/* ���� */
	__integer method;
	/* ģʽ���� */
	PWM_PATTERN ptn_list;
	/* ģʽ������ */
	__integer max_ptn;
	/* ģʽ���� */
	PWM_PATTERN ptn_array;
	/* ��ϣ��ÿ������� */
	__integer *group_nums;
	/* �ֹ�ϣ��ڵ����� */
	__integer hash_entries;
	/* �ֽڹ�ϣ��*/
	HASH_TYPE bhash[0x100];
	/* �ֹ�ϣ�� */
	HASH_TYPE *whash;
	/* ��¼��ͬ����ģʽ�ĸ��� */
	__integer *lengths;
	/* ���ֽ��ƶ��� */
	__word bshift[0x100];
	/* ���ƶ��� */
	__byte *wshift;
	/* �ƶ����ƶ��ĳ��� */
	__word shift_len;
	/* ʹ�ÿ��ƶ��㷨 */
	__bool shift_large;
	/* ����ģʽ����С�ĳ��� */
	__integer smallest;
	/* ����ģʽ�����ĳ��� */
	__integer largest;
	/* ����ģʽ�г��ȵ�ƽ��ֵ */
	__integer avg;
	/* ����ģʽ�ܳ��� */
	__integer total;
	/* �����㷨 */
	__integer (__INTERNAL_FUNC__ *search)(struct _WM *, const __byte *, __integer, wm_get1);
} WM, *PWM;

/*
 * ��������
 */
/* BM�㷨���ɻ��ַ����� */
__integer * __API__ bm_make_bc(const __byte *, __integer);
/* BM�㷨���ɺú�׺���� */
__integer * __API__ bm_make_gs(const __byte *, __integer);
/* BM�㷨ƥ�� */
__integer __API__ bm_matcher(const __byte *, __integer, const __byte *, __integer);
/* BM�㷨ƥ�� */
__integer __API__ bm_matcherex(const __byte *, __integer, const __byte *, __integer, __integer *, __integer *);
/* WM��ʼ�� */
PWM __API__ init_wm(__void);
/* WM�ͷ� */
__void __API__ free_wm(PWM *);
/* WM����ģʽ */
PWM_PATTERN __API__ add_wm_pattern(PWM, __integer, const __byte *, __integer);
/* WMƥ�� */
__integer __API__ wm_matcher(PWM, const __byte *, __integer, wm_get1);

#if defined(__cplusplus)
}
#endif

#endif
