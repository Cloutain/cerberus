#if !defined(__THUNKCODERRUNTIME_H__)
#define __THUNKCODERRUNTIME_H__

#include "Common.h"
#include "Assembler.h"
#include "ThunkCoderRandRoll.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define __MAX_THUNKCODER_REGISTER__					8

/*
 * ���ɻ�ָ�����й����б�ʾ�Ĵ����ı�ʾ
 */
typedef enum _THUNKCODER_REGISTER {
	TC_REG_EAX = 0,
	TC_REG_ECX,
	TC_REG_EDX,
	TC_REG_EBX,
	TC_REG_ESP,
	TC_REG_EBP,
	TC_REG_ESI,
	TC_REG_EDI
} THUNKCODER_REGISTER;

/*
 * ��ָ������ʱ
 */
typedef struct _THUNKCODER_RUNTIME {
	__integer iVariableBase;//��׼�����Ļ�
	__integer iTmpVariableBase;//��ʱ�����Ļ�
	__integer iTmpVariableCount;//��ʱ��������
	__bool *pUseTmpVar;//��ʱ��������
	__bool bUseRegisters[__MAX_THUNKCODER_REGISTER__];//��ʹ�õļĴ���
} THUNKCODER_RUNTIME, *PTHUNKCODER_RUNTIME;

THUNKCODER_REGISTER __INTERNAL_FUNC__ AsmTokenToThunkCoderRegiste(ASMER_TYPE AsmToken);
THUNKCODER_REGISTER __INTERNAL_FUNC__ ChooseRegisterToken();
PTHUNKCODER_RUNTIME __INTERNAL_FUNC__ GenerateThunkCoderRuntime(__integer iTmpVarCount, __integer iTmpVarBase, __integer iVarBase);
__void __INTERNAL_FUNC__ DestoryThunkCoderRuntime(PTHUNKCODER_RUNTIME pThunkCoderRuntime);

#if defined(__cplusplus)
}
#endif

#endif