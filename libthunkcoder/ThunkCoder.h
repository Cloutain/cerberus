#if !defined(__THUNKCODER_H__)
#define __THUNKCODER_H__

#include "Common.h"
#include "ThunkCoderConfigure.h"
#include "RandRoll.h"
#include "JunkCodeGen.h"
#include "ProtoType.h"
#include "FlowCodeGen.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define __DEF_THUNKCODER_ALIGNMENT__		128
#define __DEF_MAX_THUNKCODER_ARG__			6
#define __DEF_THUNKCODER_VAR_ALIGNMENT__	4

typedef enum _THUNKCODER_TYPE {
	TKT_JUNKCODE = 0,
	TKT_FRAME,
} THUNKCODER_TYPE;

/*
 * �ֲ�����
 */
typedef struct _LOCAL_VARIABLE {
	__integer iOffset;//��EBP��ƫ��
	__integer iSize;//�����Ĵ�С
} LOCAL_VARIABLE, *PLOCAL_VARIABLE;

/*
 * ��ָ���
 */
typedef struct _THUNKCODER_PROCEDURE {
	__offset ofOffset;//���������Ŀ�ͷ��ƫ��
	__integer iSize;//��С
	__integer iArgumentCount;//��������
	__integer iStackSize;//ջ��С
	__integer iLocalVarCount;//�ֲ���������
	PFLOWCODER pFlowCoder;//���̴���
} THUNKCODER_PROCEDURE, *PTHUNKCODER_PROCEDURE;

/*
 * ��ָ���ִ����
 */
typedef struct _THUNKCODER {
	struct _THUNKCODER *pNext;//��һ���ڵ�
	THUNKCODER_TYPE Type;//����
	__offset ofOffset;//���������Ŀ�ͷ��ƫ��
	__integer iSize;//��С
	__memory pJunkCode;//��������
	PLOCAL_VARIABLE pLocalVariable;//�ֲ�����
	struct _THUNKCODER *CallOutList;//���������Ļ�ָ����������
	PFLOWCODER pFlowCoder;//���̴���
} THUNKCODER, *PTHUNKCODER;

PTHUNKCODER __API__ GenerateThunkCode(PTHUNKCODER_CONFIGURE pThunkCoderConfigure);
__memory __API__ AssemblerThunkCode(PTHUNKCODER pThunkCoder, PTHUNKCODER_CONFIGURE pThunkCoderConfigure);
__void __API__ ReleaseThunkCode(PTHUNKCODER *pThunkCodePoint);

#if defined(__cplusplus)
}
#endif

#endif
