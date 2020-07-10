#if !defined(__PROTOTYPECONFIGURE_H__)
#define __PROTOTYPECONFIGURE_H__

#include "Common.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define __MAX_CIPHER_KEY__			32

typedef struct _PROTOTYPE_MATHEXP_ASM_CONFIGURE {
	ASMER_BIT Bit;//������λ
	ASMER_TYPE ResultReg;//����Ĵ���
	ASMER_TYPE VarReg;//�������õļĴ���
	union{
		ASMER_TYPE KeyReg;//��Կ�Ĵ���
		ASMER_TYPE TmpReg;//��ʱ�Ĵ���
	};
} PROTOTYPE_MATHEXP_ASM_CONFIGURE, *PPROTOTYPE_MATHEXP_ASM_CONFIGURE;
typedef PROTOTYPE_MATHEXP_ASM_CONFIGURE PROTOTYPE_CIPHEREXP_ASM_CONFIGURE, *PPROTOTYPE_CIPHEREXP_ASM_CONFIGURE;

typedef struct _PROTOTYPE_CONFIGURE {
	union {
		__bool MathExpRollArray[100];//��ѧ�������Ӷ���
		__bool CipherExpRollArray[100];//�������ǹ��Ӷ���
	};
} PROTOTYPE_CONFIGURE, *PPROTOTYPE_CONFIGURE;

#if defined(__cplusplus)
}
#endif

#endif
