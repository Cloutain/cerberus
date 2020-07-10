#if !defined(__CHAOSVM_CPU_STATUS_H__)
#define __CHAOSVM_CPU_STATUS_H__

#include "Common.h"

// �ɹ����͵ķ�����,��ʹCPU��������,�������͵ķ�����,������CPU��ֹ����
typedef __dword CPU_STATUS;

#define __CPU_STATUS_SUCCESS__(Status)				(((__sdword)(Status)) >= 0 )//�Ƿ�ִ�гɹ�

// �ж��Ƿ����ж�
#define __CPU_IS_INTERRUPT_OCCUR__(pCPU)			(1 == (pCPU)->bIsInterruptOccur)
#define __CPU_SET_INTERRUPT_OCCUR__(pCPU)			((pCPU)->bIsInterruptOccur = 1)

// �˳�
#define __CPU_STATUS_IMMEDIATELY_EXIT__				(0x10000000)
#define __CPU_STATUS_IS_SUCCESS_EXIT__(Status)		((((__dword)(Status)) >= 0x10000000)&& (((__dword)(Status)) < 0x20000000))

// ����ָ���д����ж�,���жϴ���ɹ�
#define __CPU_STATUS_INTERRRUPT_HANDLE_SUCCESS__	10

// ָ��ִ�гɹ�
#define __CPU_STATUS_EXECUTE_SUCCESS__				0

// ���ô��󷵻�������Ϊ"�ж�"
#define __CPU_STATUS_INTERRRUPT_OCCUR__				(0x50000100)

// GP���쳣�ķ���ֵ
//__CPU_STATUS_MM_INVALID_ADDRESS__
//__CPU_STATUS_MM_INVALID_ADDRESS_2__

// UD���쳣�ķ���ֵ
#define __CPU_STATUS_INVALID_OPCODE__				__CPU_STATUS_INTERRRUPT_OCCUR__
// DE���쳣�ķ���ֵ
#define __CPU_STATUS_DIVIDE_ERROR__					__CPU_STATUS_INTERRRUPT_OCCUR__
// BP�쳣�ķ���ֵ
#define __CPU_STATUS_BREAK_POINT__					__CPU_STATUS_INTERRRUPT_OCCUR__

// ����ָ���д����ж�,�жϴ����ɹ�
#define __CPU_STATUS_INTERRRUPT_UNHANDLE__			(0x80000001)
// ����δʶ����Ļ�����
#define __CPU_STATUS_UNIMPLEMENT_OPCODE__			(0x80000002)
// �����������Ļ�����
#define __CPU_STATUS_INCOMPLETE_OPCODE__	        (0x80000004)
// MODRMλ����ʱ����
#define __CPU_STATUS_MODRM_DATA_LENGTH_INCORRECT__  (0x80000103)

#define __CPU_STATUS_IGNORE__                       (0x80000303)

// �ڴ�ӿ�ʹ��
#define __CPU_STATUS_MM_ACCESS_SUCCESS__			__CPU_STATUS_EXECUTE_SUCCESS__
#define __CPU_STATUS_MM_INVALID_ADDRESS__			(0xC0000001)
#define __CPU_STATUS_MM_BAD_PRIVILEGE__				(0xC0000002)
#define __CPU_STATUS_MM_INVALID_ADDRESS_2__			(0xC0000003)

// HOOK������ʹ�õķ���ֵ
// ����ִ����������
#define __CPU_STATUS_HOOK_NOT_HANDLE__				(0x60000003)
// ������ǰָ��
#define __CPU_STATUS_HOOK_HANDLE__				    (0x60000004)
// �˳�
#define __CPU_STATUS_HOOK_STOP_CPU__				(0x60000001)

#endif
