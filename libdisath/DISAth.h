#if !defined(__DIS_ATTACH_H__)
#define __DIS_ATTACH_H__

#include "DISKernel.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * �ṹ����
 */
typedef struct _DISATH_CONFIGURE {
	__bool bIfXFileExistFailed;//���X�ļ�������ʧ��

	// DISģ�����
	union {
		__word wResourceID;//��ԴID
		__tchar *szDISFilePath;//DIS��·��
	};
	__dword dwDISResourceKey;//DIS��Ϊ��Դʱ��Key
	__tchar *szResourceType;//DIS��Դ������

	__bool bProtectMyIAT;//��������������
	__bool bProtectTargetIAT;//����Ŀ�����������

	XFILE_ATTACH_INFO XFileAttachInfo;//X�ļ�������Ϣ

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�ΪҪ��ӳ��DLL��������ֶ�
	__dword dwDISProtectDllCrc32List[__MAX_LIBRARY_COUNT__];//��Ҫ������ӳ���DLL��CRCֵ
	__integer iDISProtectDllCrc32Count;//��Ҫ������ӳ���DLL��CRCֵ�б����
	__dword dwDISProtectDllCallDllMainCrc32List[__MAX_LIBRARY_COUNT__];//ִ��DllMain�Ŀ�
	__integer iDISProtectDllCallDllMainCrc32Count;//ִ��DllMain�Ŀ��б����

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�Ϊ����API��ȡ�����ֶ�
	__dword dwDISProtectApiCrc32List[__MAX_API_NAME_CRC_COUNT__];//��Ҫ����API��ȡ��API, CRC32ֵ
	__integer iDISProtectApiCrc32Count;//��Ҫ����API��ȡ��API����

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�ΪҪ��ӳ��DLL��������ֶ�
	__dword dwTargetProtectDllCrc32List[__MAX_LIBRARY_COUNT__];//��Ҫ������ӳ���DLL��CRCֵ
	__integer iTargetProtectDllCrc32Count;//��Ҫ������ӳ���DLL��CRCֵ�б����
	__dword dwTargetProtectDllCallDllMainCrc32List[__MAX_LIBRARY_COUNT__];//ִ��DllMain�Ŀ�
	__integer iTargetProtectDllCallDllMainCrc32Count;//ִ��DllMain�Ŀ��б����

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�Ϊ����API��ȡ�����ֶ�
	__dword dwTargetProtectApiCrc32List[__MAX_API_NAME_CRC_COUNT__];//��Ҫ����API��ȡ��API, CRC32ֵ
	__integer iTargetProtectApiCrc32Count;//��Ҫ����API��ȡ��API����

	// �Զ�������
	__void *pUserData0;
	__void *pUserData1;
	__void *pUserData2;
	__void *pUserData3;
} DISATH_CONFIGURE, *PDISATH_CONFIGURE;

/*
 * �����ӿں���
 */
__dword __API__ DISAthAttach(PDISATH_CONFIGURE pConfigure);

// �ⲿ���̺���
#include "DISAthSteps.h"

#if defined(__cplusplus)
}
#endif

#endif
