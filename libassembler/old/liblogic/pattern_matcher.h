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
#ifndef _PATTERN_MATCHER_V_1_0_H_
#define _PATTERN_MATCHER_V_1_0_H_
/*
 * ͷ�ļ�
 */
#include "headfile.h"
#include "hashtree.h"

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
#define HASH_TYPE				word_t
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
typedef bool_t (*wm_get1)(dword_t, dword_t, const byte_t *, dword_t, const byte_t *);

/*
 * �ṹ����
 */
/* BMģʽ�ṹ */
typedef struct _BM_PATTERN {
	/* ���ַ����� */
	dword_t *bc;
	/* �ú�׺���� */
	dword_t *gs;
} BM_PATTERN, *PBM_PATTERN;

/* WMģʽ�ṹ */
typedef struct _WM_PATTERN {
	/* ��һ��ģʽ */
	struct _WM_PATTERN *next;
	/* ģʽ��ID */
	dword_t id;
	/* ģʽ */
	byte_t *ptn;
	/* ģʽ���� */
	dword_t ptnlen;
	/* BMģʽ */
	PBM_PATTERN bm_ptn;
} WM_PATTERN, *PWM_PATTERN;

/* WM�ṹ */
typedef struct _WM {
	/* ���� */
	dword_t method;
	/* ģʽ���� */
	PWM_PATTERN ptn_list;
	/* ģʽ������ */
	dword_t max_ptn;
	/* ģʽ���� */
	PWM_PATTERN ptn_array;
	/* ��ϣ��ÿ������� */
	dword_t *group_nums;
	/* �ֹ�ϣ��ڵ����� */
	dword_t hash_entries;
	/* �ֽڹ�ϣ��*/
	HASH_TYPE bhash[0x100];
	/* �ֹ�ϣ�� */
	HASH_TYPE *whash;
	/* ��¼��ͬ����ģʽ�ĸ��� */
	dword_t *lengths;
	/* ���ֽ��ƶ��� */
	word_t bshift[0x100];
	/* ���ƶ��� */
	byte_t *wshift;
	/* �ƶ����ƶ��ĳ��� */
	word_t shift_len;
	/* ʹ�ÿ��ƶ��㷨 */
	bool_t shift_large;
	/* ����ģʽ����С�ĳ��� */
	dword_t smallest;
	/* ����ģʽ�����ĳ��� */
	dword_t largest;
	/* ����ģʽ�г��ȵ�ƽ��ֵ */
	dword_t avg;
	/* ����ģʽ�ܳ��� */
	dword_t total;
	/* �����㷨 */
	dword_t (*search)(struct _WM *, const byte_t *, dword_t, wm_get1);
} WM, *PWM;

/*
 * ��������
 */
/* BM�㷨���ɻ��ַ����� */
dword_t *bm_make_bc(const byte_t *, dword_t);
/* BM�㷨���ɺú�׺���� */
dword_t *bm_make_gs(const byte_t *, dword_t);
/* BM�㷨ƥ�� */
dword_t bm_matcher(const byte_t *, dword_t, const byte_t *, dword_t);
/* BM�㷨ƥ�� */
dword_t bm_matcherex(const byte_t *, dword_t, const byte_t *, dword_t, dword_t *, dword_t *);
/* WM��ʼ�� */
PWM init_wm(void);
/* WM�ͷ� */
void free_wm(PWM *);
/* WM����ģʽ */
PWM_PATTERN add_wm_pattern(PWM, dword_t, const byte_t *, dword_t);
/* WMƥ�� */
dword_t wm_matcher(PWM, const byte_t *, dword_t, wm_get1);

#endif