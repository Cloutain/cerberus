#if !defined(__DISASM_H__)
#define __DISASM_H__

#include "Common.h"

typedef struct _DISASM_CONFIGURE {
	__bool bATT;//ʹ��AT&T���﷨��ʽ

	__bool bDisAsm;//�����
} DISASM_CONFIGURE, *PDISASM_CONFIGURE;

__bool __INTERNAL_FUNC__ DisAsm(__memory pMem, __tchar *pResultFilePath, PDISASM_CONFIGURE pConfigure);

#endif