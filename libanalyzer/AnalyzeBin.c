#include "AnalyzeBin.h"
#include <string.h>

/*
 * ȫ�ֱ���
 */
FPRva2Raw AnalyzerRva2Raw = NULL;
FPRaw2Rva AnalyzerRaw2Rva = NULL;
FPGetCodeSectionHeader AnalyzerGetCodeSectionHeader = NULL;
FPGetNtHeader AnalyzerGetNtHeader = NULL;

/*
 * !ע:����˳��ͳһ��������,˳���ܴ���
 */
#include "AnalyzerDebug.c"//���Ժ���
#include "CheckValidAddress.c"//����ַ����Ч��
#include "ImportTableReference.c"//�������������
#include "AnalyzeInstruction.c"//����ָ��
#include "AnalyzeProcedure.c"//��������

PPROGRAM __INTERNAL_FUNC__ InitProgram(PANALYZE_CONFIGURE pAnalyzeConfigure) {
	PPROGRAM pProgram = __logic_new__(PROGRAM, 1);
	if (!pProgram) return NULL;
	__logic_memset__(pProgram, 0, sizeof(PROGRAM));
	__logic_memcpy__(&(pProgram->AnalyzeConfigure), pAnalyzeConfigure, sizeof(ANALYZE_CONFIGURE));
	return pProgram;
}

__void __API__ ReleaseProgram(PPROGRAM *pProgramPoint) {
	PPROGRAM pProgram = NULL;

	pProgram = *pProgramPoint;
	ReleaseImportTableReference(&(pProgram->pImportTableReference));
	if (pProgram->pProcedureList)
		ReleaseProcedureList(&(pProgram->pProcedureList));
	__logic_delete__(*pProgramPoint);
	*pProgramPoint = NULL;
}

/*
 * ͨ��һ��PE�ļ�ӳ��ָ��(pMem),�õ�һ��PROGRAM�ṹ��ָ��
 * ���صĽṹָ����ڴ��ɱ��������,ʹ����Ϻ�ͨ��
 * ReleaseProgram�����ͷ���
 */
PPROGRAM __API__ AnalyzeProgram(PANALYZE_CONFIGURE pAnalyzeConfigure, PANALYZE_BASIC_INFO pAnalyzeBasicInfo) {
	__integer iProcCount = 0;
	PPROCEDURE pCurrProcCount = NULL;
	PPROGRAM pProgram = InitProgram(pAnalyzeConfigure);
	if (!pProgram) return NULL;
	
	pProgram->pMem = pAnalyzeBasicInfo->pMem;
	pProgram->ImageBase = pAnalyzeBasicInfo->ImageBase;
	pProgram->pCodeSeg = pAnalyzeBasicInfo->pCodeSeg;
	pProgram->pTailData = pAnalyzeBasicInfo->pTailData;
	pProgram->addrCodeSegMemoryAddress = pAnalyzeBasicInfo->addrCodeSegMemoryAddress;
	pProgram->iBinSize = pAnalyzeBasicInfo->iBinSize;
	pProgram->iCodeSegSize = pAnalyzeBasicInfo->iCodeSegSize;
	pProgram->iTailDataSize = pAnalyzeBasicInfo->iTailDataSize;
	// �鿴�Ƿ�ӵ��ָ������ڵ�,�����ָ����ڵ�,��ʹ��ָ����ڵ����
	if (pAnalyzeConfigure->addrFromHereStartAnalyze == 0)
		pProgram->iEntryRva = pAnalyzeBasicInfo->iEntryRva;
	else
		pProgram->iEntryRva = pAnalyzeConfigure->addrFromHereStartAnalyze - pAnalyzeBasicInfo->ImageBase;
	AnalyzerRva2Raw = pAnalyzeBasicInfo->pRva2Raw;
	AnalyzerRaw2Rva = pAnalyzeBasicInfo->pRaw2Rva;
	AnalyzerGetCodeSectionHeader = pAnalyzeBasicInfo->pGetCodeSectionHeader;
	AnalyzerGetNtHeader = pAnalyzeBasicInfo->pGetNtHeader;

	// �Ƿ������������
	if (pAnalyzeConfigure->bEnableImportTableReference)
		pProgram->pImportTableReference = GenerateImportTableReference(pProgram->pMem);
		
	// �����һ�α�������,�ҳ�����ڵ㺯�����еĽ������ú���
	pProgram->pProcedureList = AnalyzeProcedurePass1(pProgram);
	pProgram->pEntryProcedure = FindEntryProcedure(pProgram->pProcedureList);

	// ���㺯��������
	pCurrProcCount = pProgram->pProcedureList;
	pProgram->iProcCount = CountProcedureList(pCurrProcCount);
	pProgram->iAllBlockCount = CountAllBloackList(pCurrProcCount);

	return pProgram;
}

PX86INSTRUCTION __API__ FindInstructionByAddress(PPROGRAM pProgram, __address addrInstruction) {
	PX86INSTRUCTION pCurrInst = NULL;
	// ����������
	PPROCEDURE pCurrProc = pProgram->pProcedureList;
	while (pCurrProc) {
		if ((pCurrProc->addrMemoryStartAddress >= addrInstruction) && (addrInstruction < (pCurrProc->addrMemoryStartAddress + pCurrProc->iSize))) {
			// ����ָ����
			pCurrInst = pCurrProc->pInstruction;
			while (pCurrInst) {
				if (pCurrInst->addrCurrMemoryAddress == addrInstruction)
					return pCurrInst;
				pCurrInst = pCurrInst->pNext;
			}
		}
		pCurrProc = pCurrProc->pNext;
	}
	return NULL;
}

PPROCEDURE __API__ FindProcedureByAddress(PPROGRAM pProgram, __address addrProcedure) {
	PPROCEDURE pCurrProc = pProgram->pProcedureList;
	while (pCurrProc) {
		if (pCurrProc->addrMemoryStartAddress == addrProcedure)
			return pCurrProc;
		pCurrProc = pCurrProc->pNext;
	}
	return NULL;
}

PPROCEDURE __API__ FindProcedureByInsideAddress(PPROGRAM pProgram, __address addrAddress) {
	PPROCEDURE pCurrProc = pProgram->pProcedureList;
	while (pCurrProc) {
		if ((pCurrProc->addrMemoryStartAddress >= addrAddress) && (addrAddress < (pCurrProc->addrMemoryStartAddress + pCurrProc->iSize)))
			return pCurrProc;
		pCurrProc = pCurrProc->pNext;
	}
	return NULL;
}
