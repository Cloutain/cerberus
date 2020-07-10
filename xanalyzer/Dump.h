#if !defined(__DUMP_H__)
#define __DUMP_H__

#include "Common.h"

typedef struct _DUMP_CONFIGURE {
	__bool bDump;//ת��
	__integer iDumpSize;//ת��ĳ���
	__bool bEntry;//ת����ڵ�
	__integer iEntrySize;//ת����ڵ㳤��

	// ���ֵ����Ҫ���õ�
	__integer iFileSize;//�ļ��ĳ���
} DUMP_CONFIGURE, *PDUMP_CONFIGURE;

__bool __INTERNAL_FUNC__ Dump(__memory pMem, __tchar *pResultFilePath, PDUMP_CONFIGURE pConfigure);

#endif