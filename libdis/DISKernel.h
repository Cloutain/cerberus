#if !defined(__DIS_KERNEL_H__)
#define __DIS_KERNEL_H__

#include "Support.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * ����ṹ����
 */
// DIS�����ýṹ
typedef struct _DIS_CONFIGURE {
	//////////////////////////////////////////////////////////////////////////
	// ȫ�ֱ���
	__offset ofOrigEntryRVA;//ԭʼ��ڵ��RVA
	__address addrOrigImageBase;//ԭʼӳ�����ַ
	__integer iOrigSizeOfImage;//ԭʼӳ�䳤��
	__integer iNowSizeOfImage;//��ǰӳ��ĳ���

	IMPORT_PROTECT DISImportProtect;//DIS����������ṹ
	IMPORT_PROTECT TargetImportProtect;//Ŀ���ļ�����������ṹ

	__address addrDISNewImportTable[__MAX_JMP_ADDRESS_TABLE_COUNT__];//DIS�µ������ַ������
	__address addrTargetNewImportTable[__MAX_JMP_ADDRESS_TABLE_COUNT__];//Ŀ���ļ��µ������ַ������

	//////////////////////////////////////////////////////////////////////////
	// ����ѡ��
	__bool bFirstRun;//��һ������
	__bool bProtectMyIAT;//��������������
	__bool bProtectTargetIAT;//����Ŀ�����������

} DIS_CONFIGURE, *PDIS_CONFIGURE;

/*
 * ʹ��ʱ��Ҫ��������
 */
__NEED_EXPORT__ __EXTERN__ DIS_CONFIGURE g_DISConfigure;																	//DIS�����ýṹ

/*
 * ֻ�������ĺ���
 */
__memory __INTERNAL_FUNC__ DISAddThunkCodeStub(__address addrNowApiAddress);

/*
 * ���ýӿ�
 */
//������EXE����
__bool __API__ DISEntry();
__bool __API__ DISEntryInDll(__dword hModule, __dword ul_reason_for_call, __void *lpReserved);


// DNA�趨
#include "DISDNA.h"

// �ⲿ���̺�������
#include "DISSteps.h"

#if defined(__cplusplus)
}
#endif

#endif
