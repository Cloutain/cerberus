#include <stdlib.h>

#include "Common.h"
#include "ChaosVmAth.h"

// ������ָ��ʱ����Ĵ���ص���������
__void __INTERNAL_FUNC__ VmpThisInstructionErrorNotification(__integer iError, __char *pInstHex, __char *pInst, __integer iInstLength, __offset ofOffset) {
	if (iError == __VMP_INST_NOT_SUPPORT__) {
		printf("[-] Vmp instruction:[%s]%s(%d bytes) in %d offset not support\r\n", pInstHex, pInst, iInstLength, ofOffset);
	}
}
