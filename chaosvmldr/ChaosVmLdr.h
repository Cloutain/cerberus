#if !defined(__CHAOSVM_LDR_H__)
#define __CHAOSVM_LDR_H__

#include "Common.h"
#include "Support.h"
#include "ChaosVm.h"

#define __CHAOSVM_LOADER_XFILE_DNA__								0x7E83ACFB//�ô�ֵ��Ψһ��ʶ 

// �������������������Ϣ�ṹ
typedef struct _CHAOSVM_LOADER_DATA {
	__offset ofChaosVmRuntimeRVA;//������������л���ʱ��RVA
} CHAOSVM_LOADER_DATA, *PCHAOSVM_LOADER_DATA;

// ����������������ṹ
typedef struct _CHAOSVM_LOADER_CONFIGURE {
	__bool bUseChaosVmEmulationInResource;//ʹ����Դ�еĻ��������������
	__tchar szByteCodeFileName[0x20];//�ֽ����ļ�������
	__tchar szChaosVmEmulationFileName[0x20];//���������������������
	CHAOSVM_EMULATION_CONFIGURE ChaosVmEmulationConfigure;//������������������ýṹ
	CHAOSVM_LOADER_DATA ChaosVmLoaderData;//���������������������

	//////////////////////////////////////////////////////////////////////////
	// ��ʾ��ѡ��,��cbc�ļ�ȷʵʱ����
	__tchar szMessageBoxTitle[0x40];
	__tchar szMessageBoxContextOnByteCodeFileNotExist[0x100];
	__tchar szMessageBoxContextOnEmulationFileNotExist[0x100];
	__dword dwMessageStyle;//��Ϣ����

	//////////////////////////////////////////////////////////////////////////
	// ���������ѡ��
	// 2012.2.9 ����
	CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE DebugConfigure;
} CHAOSVM_LOADER_CONFIGURE, *PCHAOSVM_LOADER_CONFIGURE;

#endif
