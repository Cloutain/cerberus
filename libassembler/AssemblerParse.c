#include "AssemblerParse.h"

/*
 * ȫ�ֱ���
 */
__bool g_bCompilingError = FALSE;
__bool g_bLexAnalyzingError = FALSE;
__integer *g_pAsmTokenList = NULL;
__integer g_iAsmTokenListMaxCount = 0;
__integer g_iAsmTokenListIndex = 0;
FPAssemblerHook g_pAssemblerHookFunc = NULL;
PASM_OBJ *g_pCurrAsmObjPoint = NULL;
__byte g_bInstBuf[16];//ָ����ʱ�ڴ�, һ��ָ���15���ֽ�

/*
 * �Զ����yylex����
 * ��lex����
 * ������-1 
 * �ɹ���������0
 */
int yylex() {
	int iToken = *(int *)(g_pAsmTokenList + g_iAsmTokenListIndex);
	g_iAsmTokenListIndex++;
	if (iToken == END) return 0;
	else if (iToken == ERROR) {
		g_bLexAnalyzingError = TRUE;
		return -1;
	}
	return iToken;
}

void yyerror(char const *pErrorMsg) {
	/* ������ʱʲôҲ���� */
	//fprintf(stderr, pErrorMsg);
}

__INLINE__ __integer __INTERNAL_FUNC__ OutputOneInst(PASM_OBJ pCurrAsmObj, __offset ofCurrentOffset) {
	__logic_memset__(g_bInstBuf, 0, 0x10);
	// ����HOOK���������
	if (g_pAssemblerHookFunc) return g_pAssemblerHookFunc(g_bInstBuf, pCurrAsmObj, ofCurrentOffset);
	return Encoder(g_bInstBuf, pCurrAsmObj, ofCurrentOffset);
}

__INLINE__ PASM_OBJ __INTERNAL_FUNC__ SkipAddrLabelInfo(PASM_OBJ pAsmObj) {
	while (pAsmObj) {
		if (pAsmObj->bAddrLabel)
			pAsmObj = pAsmObj->pNext;//������һ���ڵ�
		else
			break;
	}
	return pAsmObj;
}

#define __OutputToBinFailed__()\
	g_bCompilingError = TRUE;\
		if (pBin) __logic_delete__(pBin);\
	*pBinSize = 0;\
	return NULL;
__memory __INTERNAL_FUNC__ OutputToBin(PASM_OBJ pAsmObjList, __integer *pBinSize) {
	/*
	 * ��ʼ������
	 */ 
	__memory pBin = NULL;
	__integer iInstLength = 0;
	__offset ofOffset = 0;
	PASM_OBJ pCurr = pAsmObjList;
	g_bCompilingError = FALSE;
	g_bLexAnalyzingError = FALSE;
	
	while (pCurr) {		
		/*
		 * ������ַ���
		 */
		pCurr = SkipAddrLabelInfo(pCurr);
		if (!pCurr) break;//��ֹ���һ��ָ���ǵ�ַ��ŵ����
		pCurr->ofOffset = ofOffset;//��ǰָ��ƫ��
		iInstLength = OutputOneInst(pCurr, ofOffset);
		
		/*
		 * ���һ��ָ��ʧ��
		 */ 
		if (!iInstLength) {
			__OutputToBinFailed__();
		}
		/*
		 * ���ָ��ڴ�
		 */ 
		InputValueToMemory(&pBin, (__integer)ofOffset, g_bInstBuf, iInstLength);
		ofOffset += (__offset)iInstLength;
		pCurr->bInstLength = (__byte)iInstLength;
		pCurr = pCurr->pNext;//��һ���ڵ�
	}

	/*
	 * ������ַ�����Ϣ
	 */
	BuildAddrLabelInfo(pAsmObjList);

	*pBinSize = (__integer)ofOffset;
	return pBin;
}
