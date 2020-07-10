///*
// * ͷ�ļ�
// */
//#include "mp.h"
//#include <math.h>
//
//#if defined(__cplusplus)
//extern "C"
//{
//#endif
//
///*
// * ��������
// */
///* ���ص�һ����0ֵ������ */
//__STATIC__ __integer __INTERNAL_FUNC__ get_nonzero_index(__integer *, __integer);
///* �Ƚ�MP�ṹ�Ĵ�С */
//__STATIC__ __integer __INTERNAL_FUNC__ mp_cmp_mp(__CONST__ PMP_TYPE, __CONST__ PMP_TYPE);
//
///*
// * ��������
// */
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    x:ָ����������.
// *	  max_number:�������ļ���.
// * ����ֵ:
// *	  ��һ����0��������.
// * ˵��:
// *    ���ص�һ����0��ֵ������.
// */
//__STATIC__ __integer __INTERNAL_FUNC__ get_nonzero_index(__integer *x, __integer max_number) {
//	__integer i = 0;
//	for (i = max_number; i && !x[i-1]; i--);
//	return i;
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:���Ƚϵ���1.
// *	  b:���Ƚϵ���2
// * ����ֵ:
// *	  0,1,-1.
// * ˵��:
// *    ����MP�����Ƚ�.
// */
//__STATIC__ __integer __INTERNAL_FUNC__ mp_cmp_mp(__CONST__ PMP_TYPE a, __CONST__ PMP_TYPE b) {
//	__integer i = 0;
//
//	if (a->n > b->n)
//		return 1;
//	if (a->n < b->n)
//		return -1;
//
//	for (i = a->n; i ; i--) {
//		if (a->v[i-1] > b->v[i-1])
//			return 1;
//		if (a->v[i-1] < b->v[i-1])
//			return -1;
//	}
//
//	return 0;
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-30
// * �������:2011-7-30
// * ����:
// *    v:�����ṹָ��.
// * ����ֵ:
// * ˵��:
// *    ���MP�ṹ.
// */
//__void __API__ mp_clear(PMP_TYPE v) {
//	__logic_memset__(v, 0, sizeof(MP_TYPE));
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-30
// * �������:2011-7-30
// * ����:
// *    v:�����ŵĴ����ṹָ��.
// * ����ֵ:
// * ˵��:
// *    ��մ����ŵ�MP�ṹ.
// */
//__void __API__ signed_mp_clear(PSIGNED_MP_TYPE v) {
//	__logic_memset__(v, 0, sizeof(SIGNED_MP_TYPE));
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-30
// * �������:2011-7-30
// * ����:
// *    d:�����ṹָ���Ŀ���ַ.
// *    s:�����ṹָ���Դ��ַ.
// * ����ֵ:
// * ˵��:
// *    ����MP�ṹ.
// */
//__void __API__ mp_copy(PMP_TYPE d, PMP_TYPE s) {
//	__logic_memcpy__(d, s, sizeof(MP_TYPE));
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-30
// * �������:2011-7-30
// * ����:
// *    d:�����ŵĴ����ṹָ���Ŀ���ַ.
// *    s:�����ŵĴ����ṹָ���Դ��ַ.
// * ����ֵ:
// * ˵��:
// *    ���ƴ����ŵ�MP�ṹ.
// */
//__void signed_mp_copy(PSIGNED_MP_TYPE d, PSIGNED_MP_TYPE s) {
//	__logic_memcpy__(d, s, sizeof(SIGNED_MP_TYPE));
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-30
// * �������:2011-7-30
// * ����:
// *    v:MP�ṹָ��.
// * ����ֵ:
// *	  MP��λ��.
// * ˵��:
// *	  ����MP�ṹ����ֵ��λ��.
// */
//__integer __API__ mp_bits(PMP_TYPE v) {
//	__integer i = 0, bits = 0, mask = 0, top_word = 0;
//
//	if (_mp_is_zero_(v))
//		return 0;
//
//	i = v->n;
//	bits = 32 * i;
//	top_word = v->v[i - 1];
//	mask = 0x80000000;
//	if ((top_word >> 16) == 0) {
//		mask = 0x8000;
//		bits -= 16;
//	}
//
//	while (!(top_word & mask)) {
//		bits--;
//		mask >>= 1;
//	}
//
//	return bits;
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-30
// * �������:2011-7-30
// * ����:
// *    v:MP�ṹָ��.
// * ����ֵ:
// *	  ��eΪ����MPΪ���Ķ���ֵ.
// * ˵��:
// *	  MP����Ȼ��������.
// */
//__double __API__ mp_e_log(PMP_TYPE v) {
//	__integer i = v->n;
//
//	switch(i) {
//	case 0:
//		return 0;
//	case 1:
//		return log((__double)(v->v[0]));
//	case 2:
//		return log((__double)(v->v[0]) + MP_RADIX * v->v[1]);
//	default:
//		return 32 * (i-3) * M_LN2 + \
//			log((__double)(v->v[i-3]) + MP_RADIX * ( \
//			((__double)v->v[i-2] + MP_RADIX * \
//			v->v[i-1])));
//	}
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  b:MP�ṹָ��.
// *	  sum:������ĺ�.
// * ����ֵ:
// * ˵��:
// *	  ����MP�ṹ���.
// */
//__void __API__ mp_add_mp(PMP_TYPE a, PMP_TYPE b, PMP_TYPE sum) {
//	__integer max_words = 0;
//	__integer i = 0, carry = 0, acc = 0;
//
//	max_words = __max__(a->n, b->n);
//	for (i = 0; i < max_words; i++) {
//		acc = a->v[i] + carry;
//		carry = (acc < a->v[i]);
//		sum->v[i] = acc + b->v[i];
//		carry += (sum->v[i] < acc);
//	}
//	if (carry)
//		sum->v[i++] = carry;
//
//	sum->n = i;
//	for (; i < MAX_MP_NUMBER; i++)
//		sum->v[i] = 0;
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  b:����.
// *	  sum:MP�ṹ�����.
// * ����ֵ:
// * ˵��:
// *	  MP�ṹ���ϳ����Ľ������MP�ṹ��.
// */
//__void __API__ mp_add_integer(PMP_TYPE a, __integer b, PMP_TYPE sum) {
//	__integer max_words = a->n;
//	__integer i, carry = b, acc = 0;
//
//	for (i = 0; carry && i < max_words; i++) {
//		acc = a->v[i] + carry;
//		carry = (acc < a->v[i]);
//		sum->v[i] = acc;
//	}
//	if (carry)
//		sum->v[i++] = carry;
//
//	for (; i < MAX_MP_NUMBER; i++)
//		sum->v[i] = a->v[i];
//
//	sum->n = max_words;
//	if (max_words < MAX_MP_NUMBER && sum->v[max_words])
//		sum->n++;
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  b:MP�ṹָ��.
// *	  diff:���ڱ���a��b�Ĳ�.
// * ����ֵ:
// * ˵��:
// *	  ����MP�ṹ���.
// */
//__void __API__ mp_sub_mp(PMP_TYPE a, PMP_TYPE b, PMP_TYPE diff) {
//	__integer max_number = a->n;
//	__integer borrow = 0, acc = 0, i = 0;
//
//	for (i = 0; i < max_number; i++) {
//		acc = a->v[i] - borrow;
//		borrow = (acc > a->v[i]);
//		diff->v[i] = acc - b->v[i];
//		borrow += (diff->v[i] > acc);
//	}
//	for (; i < MAX_MP_NUMBER; i++)
//		diff->v[i] = 0;
//
//	diff->n = get_nonzero_index(diff->v, max_number);
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  b:����.
// *	  diff:���ڱ���a��b�Ĳ�.
// * ����ֵ:
// * ˵��:
// *	  һ��MP�ṹָ����һ���������.
// */
//__void __API__ mp_sub_integer(PMP_TYPE a, __integer b, PMP_TYPE diff) {
//	__integer max_number = a->n;
//	__integer i = 0, borrow = b, acc = 0;
//
//	for (i = 0; borrow; i++) {
//		acc = a->v[i] - borrow;
//		borrow = (acc > a->v[i]);
//		diff->v[i] = acc;
//	}
//
//	for (; i < MAX_MP_NUMBER; i++)
//		diff->v[i] = a->v[i];
//
//	diff->n = get_nonzero_index(diff->v, max_number);
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  b:MP�ṹָ��.
// *	  sum:���ڱ���a��b�ĺ�.
// * ����ֵ:
// * ˵��:
// *	  ���������ŵ�MP�ṹ���.
// */
//__void __API__ signed_mp_add(PSIGNED_MP_TYPE a, PSIGNED_MP_TYPE b, PSIGNED_MP_TYPE sum) {
//	switch(2 * a->sign + b->sign) {
//
//	case 2 * POSITIVE + POSITIVE:
//	case 2 * NEGATIVE + NEGATIVE:
//		mp_add_mp(&a->num, &b->num, &sum->num);
//		sum->sign = a->sign;
//	break;
//
//	case 2 * POSITIVE + NEGATIVE:
//		if (mp_cmp_mp(&a->num, &b->num) >= 0) {
//			mp_sub_mp(&a->num, &b->num, &sum->num);
//			sum->sign = POSITIVE;
//		}
//		else {
//			mp_sub_mp(&b->num, &a->num, &sum->num);
//			sum->sign = NEGATIVE;
//		}
//	break;
//
//	case 2 * NEGATIVE + POSITIVE:
//		if (mp_cmp_mp(&a->num, &b->num) > 0) {
//			mp_sub_mp(&a->num, &b->num, &sum->num);
//			sum->sign = NEGATIVE;
//		}
//		else {
//			mp_sub_mp(&b->num, &a->num, &sum->num);
//			sum->sign = POSITIVE;
//		}
//		break;
//	}/* end switch */
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  b:MP�ṹָ��.
// *	  diff:���ڱ���a��b�Ĳ�.
// * ����ֵ:
// * ˵��:
// *	  һ�������ŵ�MP�ṹָ����һ���������.
// */
//__void __API__ signed_mp_sub(PSIGNED_MP_TYPE a, PSIGNED_MP_TYPE b, PSIGNED_MP_TYPE diff) {
//	switch(2 * a->sign + b->sign) {
//
//	case 2 * POSITIVE + POSITIVE:
//		if (mp_cmp_mp(&a->num, &b->num) >= 0) {
//			mp_sub_mp(&a->num, &b->num, &diff->num);
//			diff->sign = POSITIVE;
//		}
//		else {
//			mp_sub_mp(&b->num, &a->num, &diff->num);
//			diff->sign = NEGATIVE;
//		}
//	break;
//
//	case 2 * NEGATIVE + NEGATIVE:
//		if (mp_cmp_mp(&a->num, &b->num) > 0) {
//			mp_sub_mp(&a->num, &b->num, &diff->num);
//			diff->sign = NEGATIVE;
//		}
//		else {
//			mp_sub_mp(&b->num, &a->num, &diff->num);
//			diff->sign = POSITIVE;
//		}
//	break;
//
//	case 2 * POSITIVE + NEGATIVE:
//	case 2 * NEGATIVE + POSITIVE:
//		mp_add_mp(&a->num, &b->num, &diff->num);
//		diff->sign = a->sign;
//	break;
//	}/* end switch */
//}
//
///*
// * ����:����
// * ��ʼ����:2011-7-31
// * �������:2011-7-31
// * ����:
// *    a:MP�ṹָ��.
// *	  shift:Ҫ�ƶ���λ��.
// *	  res:���ڱ���λ�ƺ�Ľ��.
// * ����ֵ:
// * ˵��:
// *	  ��MP��ֵ����shiftλ,����ƶ�32 * MAX_MP_NUMBER.
// */
//__void __API__ mp_rshift(PMP_TYPE a, __integer shift, PMP_TYPE res) {
//	__integer i = 0;
//	__integer number = a->n;
//	__integer start_word = shift / 32;
//	__integer word_shift = shift & 31;
//	__integer comp_word_shift = 32 - word_shift;
//
//	if (start_word > number) {
//		mp_clear(res);
//		return;
//	}
//
//	if (word_shift == 0) {
//		for (i = 0; i < (number - start_word); i++)
//			res->v[i] = a->v[start_word + i];
//	}
//	else {
//		for (i = 0; i < (number - start_word - 1); i++)
//			res->v[i] = a->v[start_word + i] >> word_shift |
//			a->v[start_word + i + 1] << comp_word_shift;
//		res->v[i] = a->v[start_word + i] >> word_shift;
//		i++;
//	}
//
//	for (; i < MAX_MP_NUMBER; i++)
//		res->v[i] = 0;
//
//	res->n = get_nonzero_index(res->v, (__integer)(number - start_word));
//}
//
//#if defined(__cplusplus)
//}
//#endif