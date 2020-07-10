#if !defined(__THUNKCODERCONFIGURE_H__)
#define __THUNKCODERCONFIGURE_H__

#include "Common.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * ��ָ������ýṹ
 */
typedef struct _THUNKCODER_CONFIGURE {
	struct {
		__integer iJunkCodeMinSize;//����ָ����С����
		__integer iJunkCodeMaxSize;//����ָ����󳤶�
	} JunkCodeSize;
	__integer iThunkCoderAlignment;//��ָ�������������
	__integer iThunkCoderFrameArgumentsMax;//��ָ�������������
	__integer iMathExpRollProb;//��ѧ���ʽ���Ӹ���

	// �������Ӷ���
	__bool ThunkCoderLayoutRollArray[100];//��ָ���ͼ���Ӷ���
	__bool FlowCoderRollArray[100];//ָ���������Ӷ���
	__bool JunkCoderRollArray[100];//����ָ�����Ӷ���
	__bool MathExpRollArray[100];//��ѧ���ʽ���Ӷ���
} THUNKCODER_CONFIGURE, *PTHUNKCODER_CONFIGURE;

#if defined(__cplusplus)
}
#endif

#endif
