#include "ChaosVmByteCodeFile.h"
#include "lcrypto.h"
#include "Support.h"

/*
 * ����:
 *	����������������ֽ����ļ�,�ɹ������ֽ����ļ��Ľ�ͷ,ʧ�ܷ���0
 *
 * ByteCode�ļ��ṹ:
 *	CHAOSVM_EMULATION_BYTECODE_FILE
 *	N��CHAOSVM_EMULATION_BYTECODE_PROCEDURE + VmpProcedure
 */
PCHAOSVM_EMULATION_BYTECODE_FILE __API__ ChaosVmByteCodeFileGenByteCodeFile() {
	PCHAOSVM_EMULATION_BYTECODE_FILE pFile = NULL;
	pFile = (PCHAOSVM_EMULATION_BYTECODE_FILE)__logic_new__(CHAOSVM_EMULATION_BYTECODE_FILE, 1);
	pFile->dwSign = __CHAOSVM_BYTECODE_SIGN__;
	pFile->dwSize = sizeof(CHAOSVM_EMULATION_BYTECODE_FILE);
	return pFile;
}

/*
 * ����:
 *	pFilePoint:ָ���ֽ����ļ��ṹ��ָ���ָ��
 *
 * ����:
 *	�ͷ�ָ�����ֽ����ļ����ڴ�
 */
__void __API__ ChaosVmByteCodeFileReleaseByteCodeFile(PCHAOSVM_EMULATION_BYTECODE_FILE *pFilePoint) {
	__logic_delete__(*pFilePoint);
	*pFilePoint = NULL;
}

/*
 * ����:
 *	pFilePoint:ָ���ֽ����ļ��ṹ��ָ���ָ��
 *	pProcedure:Ҫ��ӵĺ���
 *	iProcSize:�����ĳ���
 *
 * ����:
 *	����һ���������ĺ������ֽ����ļ�
 */
PCHAOSVM_EMULATION_BYTECODE_PROCEDURE __API__ ChaosVmByteCodeFileAddByteCodeProcedureToFile(PCHAOSVM_EMULATION_BYTECODE_FILE *pFilePoint, __memory pProcedure, __integer iProcSize) {
	PCHAOSVM_EMULATION_BYTECODE_PROCEDURE pProc = NULL;
	__memory pNew = NULL, pOut = NULL;
	__integer iNewSize = 0;

	iNewSize = (*pFilePoint)->dwSize + sizeof(CHAOSVM_EMULATION_BYTECODE_PROCEDURE) + iProcSize;
	pNew = (__memory)__logic_new_size__(iNewSize);
	if (!pNew) return NULL;

	__logic_memcpy__(pNew, *pFilePoint, (*pFilePoint)->dwSize);
	pProc = (PCHAOSVM_EMULATION_BYTECODE_PROCEDURE)(pNew + (*pFilePoint)->dwSize);
	pProc->dwAttribute = 0;
	pProc->dwSize = sizeof(CHAOSVM_EMULATION_BYTECODE_PROCEDURE) + iProcSize;
	pOut = (__memory)pProc + sizeof(CHAOSVM_EMULATION_BYTECODE_PROCEDURE);
	__logic_memcpy__(pOut, pProcedure, iProcSize);//���ƺ�����
	__logic_delete__(*pFilePoint);//�ͷ�ԭ�͵��ڴ�
	*pFilePoint = (PCHAOSVM_EMULATION_BYTECODE_FILE)pNew;
	(*pFilePoint)->dwProcedureCount++;
	(*pFilePoint)->dwSize = iNewSize;
	pOut = (__memory)*pFilePoint + sizeof(CHAOSVM_EMULATION_BYTECODE_FILE);
	(*pFilePoint)->dwCrc32 = crc32(pOut, (*pFilePoint)->dwSize - sizeof(CHAOSVM_EMULATION_BYTECODE_FILE));
	return pProc;
}

/*
 * ����:
 *	pFile:�ֽ����ļ�ָ��
 *	pdwCrc32:�����ǰ�ֽ����ļ�ʵʱ�������CRC32ֵ
 *
 * ����:
 *	���׵�ǰ���ֽ����ļ���CRC32ֵ
 */
__bool __API__ ChaosVmByteCodeFileVerifySign(PCHAOSVM_EMULATION_BYTECODE_FILE pFile, __dword *pdwCrc32) {
	__memory pPoint = NULL;
	__integer iSize = 0;
	__dword dwCrc32 = 0;

	pPoint = (__memory)pFile + sizeof(CHAOSVM_EMULATION_BYTECODE_FILE);
	iSize = pFile->dwSize - sizeof(CHAOSVM_EMULATION_BYTECODE_FILE);

	dwCrc32 = crc32(pPoint, iSize);
	*pdwCrc32 = dwCrc32;

	if (dwCrc32 == pFile->dwCrc32)
		return TRUE;
	
	return FALSE;
}

/*
 * ����:
 *	szFilePath:Ҫ������ļ��ĵ��ļ���
 *	pFile:�ֽ����ļ�ָ��
 *
 * ����:
 *	���ֽ����ļ������������ļ���
 */
__bool __API__ ChaosVmByteCodeFileOutputByteCodeFileToFile(__tchar *szFilePath, PCHAOSVM_EMULATION_BYTECODE_FILE pFile) {
	__memory pMem = 0;
	pMem = (__memory)MappingFile(szFilePath, NULL, TRUE, 0, pFile->dwSize);
	if (!pMem) {
		return FALSE;
	}

	__logic_memcpy__(pMem, pFile, pFile->dwSize);

	UnMappingFile(pMem);
	return TRUE;
}

/*
 * ����:
 *	pByteCodeFile:�ֽ����ļ�ָ��
 *
 * ����:
 *	��ȡ���ļ����ж��ٸ�����
 */
__integer __API__ ChaosVmByteCodeFileGetProcedureCount(PCHAOSVM_EMULATION_BYTECODE_FILE pByteCodeFile) {
	return pByteCodeFile->dwProcedureCount;
}

/*
 * ����:
 *	iIndex:Ҫȡ�ú���������
 *	pByteCodeFile:�ֽ����ļ�ָ��
 *
 * ����:
 *	��ȡ����ָ��ĺ���,�ɹ�������������������,ʧ�ܷ���NULL
 */
__memory __API__ ChaosVmByteCodeFileGetProcedureByteCode(__integer iIndex, PCHAOSVM_EMULATION_BYTECODE_FILE pByteCodeFile) {
	__memory pPoint = NULL, pVmpProcedureByteCode = NULL;
	__integer iCount = 0, i = 0;
	PCHAOSVM_EMULATION_BYTECODE_PROCEDURE pProcedure = NULL;

	iCount = (__integer)(pByteCodeFile->dwProcedureCount);
	if (iIndex >= iCount) return NULL;

	pPoint = (__memory)pByteCodeFile;
	pPoint += sizeof(CHAOSVM_EMULATION_BYTECODE_FILE);
	pProcedure = (PCHAOSVM_EMULATION_BYTECODE_PROCEDURE)pPoint;//��һ���ֽ��뺯��ͷ

	for (i = 0; i < iIndex; i++)
		pProcedure = (PCHAOSVM_EMULATION_BYTECODE_PROCEDURE)((__memory)pProcedure + pProcedure->dwSize);

	pVmpProcedureByteCode = (__memory)pProcedure + sizeof(CHAOSVM_EMULATION_BYTECODE_PROCEDURE);
	return pVmpProcedureByteCode;
}