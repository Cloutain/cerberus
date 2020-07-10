#include "lcrypto.h"

/*
 * ŷ������㷨
 *
 * E1.[������] ��n��m����rΪ�������� (���ǽ���0<=r<n)
 * E2.[����Ϊ��?] ��r = 0,�㷨����,n��Ϊ��
 * E3.[����] ��m<-n, n<-r, �����ز���E1
 */
__integer __API__ EuclidAlgorithm(__integer m, __integer n) {
	__integer r = 0;

	while (1) {
		r = m % n;

		if (r == 0)
			return n;

		m = n;
		n = r;
	}

	return 1;
}

/*
 * ��չŷ������㷨
 * ��������������m��n,���Ǽ������ǵ����Լ����d����������a��b,ʹ��am+bn=d
 *
 * E1.[��ʼ��] ��a'<-b<-1, a<-b'<-0,c<-m,d<-n
 * E2.[��] ��q��r�ֱ���c����d���̺�������(������c=qd+r,��0<=r<d)
 * E3.[����Ϊ0?] ���r = 0,�㷨��ֹ;�ڴ������,������Ը����am+bn=d
 * E4.[ѭ��] ��c<-d,d<-r,t<-a',a'<-a,a<-t-qa,t<-b',b'<-b,b<-t-qb,������E2
 */
__integer __API__ EuclidAlgorithmEx(__integer m, __integer n, __integer *pa, __integer *pb) {
	__integer a = 0, b = 0, c = 0, d = 0, a_ = 0, b_ = 0, q = 0, r = 0, t = 0;


	// ��ʼ��
	a = 0;
	a_ = 1;
	b = 1;
	b_ = 0;
	c = m;
	d = n;

	while (1) {
		q = c / d;
		r = c / d;

		// ����Ϊ0
		if (r == 0) {
			*pa = a;
			*pb = b;
			return d;
		}

		c = d;
		d = r;
		t = a_;
		a_ = a;
		a = t - q * a;
		t = b_;
		b_ = b;
		b = t - q * b;
	}

	return d;
}
