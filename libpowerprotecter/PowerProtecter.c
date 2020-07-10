#include "PowerProtecter.h"
#include "lcrypto.h"

/*
 * Ҫ������ȫ�ֱ���
 */
__EXPORT__ __integer g_iPowerProtecterEncryptProcedures = 0;
__EXPORT__ PPOWER_PROTECTER_ENCRYPT_PROCEDURE g_pPowerProtecterEncryptProcedures = NULL;

/*
 * �Լ�ʹ�õ�ȫ�ֱ���
 */
POWER_PROTECTER_INFO g_PowerProtecterInfo = {0};//��������������Ϣ
POWER_PROTECTER_CONFIGURE g_PowerProtecterConfigure = {0};//���������������ýṹ
CRITICAL_SECTION g_PowerProtecterCriticalSection = {0};//ȫ��ͬ������
__address g_addrInvokeProcAddress = 0;
__dword g_dwEFlag = 0;
__dword g_dwEAX, g_dwECX, g_dwEDX, g_dwEBX;
__dword g_dwESP, g_dwEBP, g_dwESI, g_dwEDI;
__address g_addrNowStackPointer = 0;

#define __POWER_PROTECTER_OFFSET_TO_INVOKE_INDEX_FIX__				0x04
#define __POWER_PROTECTER_OFFSET_TO_INVOKE_RANDNUMBER_FIX__			0x09
#define __POWER_PROTECTER_OFFSET_TO_INVOKE_RETADDR_FIX__			0x0E

/*
 * ����:
 *	iIndex:����������������
 *
 * ����:
 *	��ȡ����ָ�����������ܺ����ṹ
 */
__INLINE__ PPOWER_PROTECTER_ENCRYPT_PROCEDURE __INTERNAL_FUNC__ GetCurrEncryptProcedureStruct(__integer iIndex) {
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pRet = g_pPowerProtecterEncryptProcedures;
	__integer i = 0;
	if (iIndex >= g_iPowerProtecterEncryptProcedures) return NULL;
	for (i = 0; i < g_iPowerProtecterEncryptProcedures; i++) {
		if (i == iIndex) break;
		pRet = (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)((__memory)pRet + pRet->iTotalSize);
	}
	return pRet;
}

/*
 * ����:
 *	iIndex:���ܺ���������
 *	pPowerProtecterRunContext:������¼��ǰ�ļĴ���״��
 *
 * ����:
 *	���︺�������ջ,����ռ����Ҫ����,�˺����Ǳ��������ú�����
 */
typedef __void (__API__ *FPPowerProtecterSetupLocalHandler)(__integer iIndex, PPOWER_PROTECTER_RUN_CONTEXT pPowerProtecterRunContext);
__void __API__ PowerProtecterSetupLocalHandler(__integer iIndex, PPOWER_PROTECTER_RUN_CONTEXT pPowerProtecterRunContext) {
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure = NULL;
	__memory pNewStack = NULL;
	__memory pNewEsp = NULL;
	__memory pEncryptProcedureBuffer = NULL;
	__dword dwRetAddress = 0;
	__dword *pStackPoint = NULL;
	__integer i = 0;

	// ��ȡ����ļ��ܺ�����
	pEncryptProcedure = GetCurrEncryptProcedureStruct(iIndex);

	// �Ƿ��ʼ����
	if (!(pEncryptProcedure->bInit)) {
		InitializeCriticalSection(&(pEncryptProcedure->LocalCriticalSection));
		pEncryptProcedure->bInit = TRUE;
	}

	EnterCriticalSection(&(pEncryptProcedure->LocalCriticalSection));
	// �����Ƿ���ͷһ�ν����,�����ͷһ������������ڴ�,�����н���
	if (pEncryptProcedure->iHandleCount == 0) {
		__integer iDecryptSpaceSize = 0;
		__dword dwKeyX = 0;
		__dword dwKeyZ = 0;
		__dword dwKeyY = 0;
		__memory pKeyProcedure = NULL;
		__integer iKeyProcedureSize = 0;
		PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pInstructions = NULL;
		__integer iInstructionSize = 0;
		pEncryptProcedure->pRunSpace = (__memory)__logic_new_size__(pEncryptProcedure->iProcSize);
		pEncryptProcedureBuffer = (__memory)pEncryptProcedure + sizeof(POWER_PROTECTER_ENCRYPT_PROCEDURE);//��ʱ������
		// ������ܿռ�
		iDecryptSpaceSize = sizeof(POWER_PROTECTER_DECRYPT_PROCEDURE) + sizeof(POWER_PROTECTER_ENCRYPT_INSTRUCTION) * pEncryptProcedure->iEncryptInstCount;
		(__memory)(pEncryptProcedure->pDecryptProcedure) = __logic_new_size__(iDecryptSpaceSize);

		dwKeyZ = crc32(pEncryptProcedureBuffer, pEncryptProcedure->iProcSize);//ͨ�������������KeyZ
		// ����Ŀ���ַ�ṹ
		XorArray(dwKeyZ, \
				(__memory)&(pEncryptProcedure->KeyProcedure), \
				(__memory)&(pEncryptProcedure->pDecryptProcedure->KeyProcedure), \
				sizeof(POWER_PROTECTER_WATCH_RECORD));
		//// ���ܺ��������ĵ�ַ
		//XorArray(dwKeyZ, \
		//		(__memory)&(pEncryptProcedure->addrProcAddress), \
		//		(__memory)&(pEncryptProcedure->pDecryptProcedure->addrProcAddress), \
		//		sizeof(__address));

		// ������Կ��������ȷ���ڴ��ַ
		pEncryptProcedure->pDecryptProcedure->KeyProcedure.addrMemAddress = \
			pEncryptProcedure->pDecryptProcedure->KeyProcedure.ofMemRVA + g_PowerProtecterInfo.addrNowTargetImageBase;

		// �������ܺ������KeyY
		pKeyProcedure = (__memory)(pEncryptProcedure->pDecryptProcedure->KeyProcedure.addrMemAddress);
		iKeyProcedureSize = pEncryptProcedure->pDecryptProcedure->KeyProcedure.iSize;
		dwKeyY = crc32(pKeyProcedure, iKeyProcedureSize);
		// ���ܺ�����
		XorArray(dwKeyY, pEncryptProcedureBuffer, pEncryptProcedure->pRunSpace, pEncryptProcedure->iProcSize);
		// ������ԿX
		dwKeyX = crc32(pEncryptProcedure->pRunSpace, pEncryptProcedure->iProcSize);
		// ����ָ��ṹ
		pInstructions = (PPOWER_PROTECTER_ENCRYPT_INSTRUCTION)(pEncryptProcedureBuffer + pEncryptProcedure->iProcSize);
		iInstructionSize = sizeof(POWER_PROTECTER_ENCRYPT_INSTRUCTION) * pEncryptProcedure->iEncryptInstCount;
		XorArray(dwKeyX, \
				(__memory)pInstructions, \
				((__memory)(pEncryptProcedure->pDecryptProcedure) + sizeof(POWER_PROTECTER_DECRYPT_PROCEDURE)), \
				iInstructionSize);

		// ������ָ��ṹ�еĵ�ַ����Ϊ��ǰ�Ļ���ַ
		for (i = 0; i < pEncryptProcedure->iEncryptInstCount; i++) {
			// ���õ�ǰָ����ڴ��ַ
			pInstructions->addMemAddress = pInstructions->ofMemRVA + g_PowerProtecterInfo.addrNowTargetImageBase;
			// ���������ָ��
			if (pInstructions->JmpType != PPFT_NONE) 
				pInstructions->addrTargetMemAddress = pInstructions->ofTargetMemRVA + g_PowerProtecterInfo.addrNowTargetImageBase;
			// ����м����ڴ����趨�����ڴ浱ǰ�ĵ�ַ
			if (pInstructions->bIsWatched) {
				pInstructions->WatchRecord.addrMemAddress = pInstructions->WatchRecord.ofMemRVA + g_PowerProtecterInfo.addrNowTargetImageBase;
			}
			pInstructions++;
		}

		// ���Ӿ������
		pEncryptProcedure->iHandleCount++;
	}

	// ���������Ľṹ
	//__logic_memcpy__((__memory)&(pEncryptProcedure->Context), (__memory)pPowerProtecterRunContext, sizeof(POWER_PROTECTER_RUN_CONTEXT));
	LeaveCriticalSection(&(pEncryptProcedure->LocalCriticalSection));

	// ���ﱣ����Ϣ��ȫ�ֱ���
	EnterCriticalSection(&g_PowerProtecterCriticalSection);
	g_dwEFlag = pPowerProtecterRunContext->dwEFlag;
	g_dwEAX = pPowerProtecterRunContext->EAX;
	g_dwECX = pPowerProtecterRunContext->ECX;
	g_dwEDX = pPowerProtecterRunContext->EDX;
	g_dwEBX = pPowerProtecterRunContext->EBX;
	g_dwESP = pPowerProtecterRunContext->ESP;
	g_dwEBP = pPowerProtecterRunContext->EBP;
	g_dwESI = pPowerProtecterRunContext->ESI;
	g_dwEDI = pPowerProtecterRunContext->EDI;
	g_addrInvokeProcAddress = (__address)(pEncryptProcedure->pRunSpace);//��ȡ��ԭ���ĺ�����ַ

	// ��Ҫ����ջ
	__asm {
		mov esp, g_dwESP								; ����ָ��ִ��֮�����еľֲ�������������
		push g_addrInvokeProcAddress					; ���õ�ַ
		;;
		;; �ָ����������л���
		;;
		push g_dwEFlag									; ��־�Ĵ���
		push g_dwEAX
		push g_dwECX
		push g_dwEDX
		push g_dwEBX
		push g_dwESP
		push g_dwEBP
		push g_dwESI
		push g_dwEDI
	}
	// �ͷ�ͬ������
	LeaveCriticalSection(&g_PowerProtecterCriticalSection);

	__asm {
		popad
		popfd
		ret												; ���뵽����ִ��,����Ҳ�ص������������
	}
}

// ����ͷ
//__NAKED__ __void __API__ LocalPowerProtecterInvoke() {
//	__asm {
//		;;
//		;; �ڱ����˳�ʱ,�ָ���������,��ջ���л����˿�
//		;; ��ʱesp�պ�ָ����ñ��������ķ��ص�ַ,ֱ��ret���ɷ���
//		;;
//		pushad													;1
//		pushfd													;1
//		push esp												;1
//		push 0x19831210;����Ҫ�޸�Ϊ�������ĺ�������			;5
//		push 0x19831210;���ѹ��һ��ֵ,����ƽ���ջ				;5
//		push 0x19831204;������Ҫ�޸�,SetupLocalPowerProtecter���ڴ��ַ
//		ret
//	}
//}
//__NAKED__ __void __API__ EndLocalPowerProtecterInvoke() {
//	if (IsDebuggerPresent() == TRUE)
//		ExitProcess(0);
//}

// Ĭ�ϵĵ���ͷ
#define __POWER_PROTECTER_DEF_INVOKE_STUB_SIZE__					19
__byte g_DefaultPowerProtecterInvokeStub[0x20] = {
	"\x60\x9C\x54\x68\x10\x12\x83\x19\x68\x10\x12\x83\x19\x68\x04\x12\x83\x19\xC3"
};
__bool __INTERNAL_FUNC__ PowerProtecterGenInvokeAlgorithm(__integer iIndex, __memory pProcAddress, __integer iProcSize, \
														FPPowerProtecterSetupLocalHandler pPowerProtecterSetupLocalHandler) {
	__memory pInvoke = NULL;
	__integer iInvokeSize = 0;

	//pInvoke = (__memory)LocalPowerProtecterInvoke;
	//iInvokeSize = (__integer)EndLocalPowerProtecterInvoke - (__integer)LocalPowerProtecterInvoke;
	pInvoke = (__memory)g_DefaultPowerProtecterInvokeStub;
	iInvokeSize = __POWER_PROTECTER_DEF_INVOKE_STUB_SIZE__;
	if (iInvokeSize > iProcSize) return FALSE;

	// ���Ƶ���ͷ,������ƫ��
	__logic_memcpy__(pProcAddress, pInvoke, iInvokeSize);
	*(__dword *)(pProcAddress + __POWER_PROTECTER_OFFSET_TO_INVOKE_INDEX_FIX__) = (__dword)iIndex;
	*(__dword *)(pProcAddress + __POWER_PROTECTER_OFFSET_TO_INVOKE_RANDNUMBER_FIX__) = (__dword)GenerateRandomNumber(); 
	*(__dword *)(pProcAddress + __POWER_PROTECTER_OFFSET_TO_INVOKE_RETADDR_FIX__) = (__dword)pPowerProtecterSetupLocalHandler;

	return TRUE;
}

/*
 * ����:
 *	iIndex:����������������
 *	pEncryptProcedure:���ܺ����Ľṹ
 *
 * ����:
 *	���������ĵ���ͷ
 */
__bool __INTERNAL_FUNC__ PowerProtecterGenInvoke(__integer iIndex, PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure) {
	__memory pProcAddress = NULL;
	__integer iProcSize = 0;
	__bool bRet = FALSE;
	
	pProcAddress = (__memory)(pEncryptProcedure->addrProcAddress);
	iProcSize = pEncryptProcedure->iProcSize;

	// �����������������С�ڵ���ͷ��С����
	if (__POWER_PROTECTER_DEF_INVOKE_STUB_SIZE__ > iProcSize) return FALSE;

	bRet = PowerProtecterGenInvokeAlgorithm(iIndex, pProcAddress, iProcSize, PowerProtecterSetupLocalHandler);

	return bRet;
}

/*
 * ����:
 *	pEncryptProcedure:�����������Ľṹ
 *	addrInstAddress:ָ����ڴ��ַ
 *
 * ����:
 *	ȷ��ָ����ָ���Ƿ��Ǳ�������ָ��
 */
__INLINE__ __bool __INTERNAL_FUNC__ IsProtectInstruction(PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure, __address addrInstAddress) {
	PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pEncryptInstructions = NULL;
	__integer iCount = 0;
	__integer i = 0;

	iCount = pEncryptProcedure->iEncryptInstCount;
	pEncryptInstructions = \
		(PPOWER_PROTECTER_ENCRYPT_INSTRUCTION)((__memory)pEncryptProcedure + sizeof(POWER_PROTECTER_ENCRYPT_PROCEDURE) + pEncryptProcedure->iProcSize);
	for (i = 0; i < iCount; i++) {
		if (addrInstAddress == pEncryptInstructions->addMemAddress)
			return TRUE;
		pEncryptInstructions++;//�ƶ�����һ��ָ��
	}

	return FALSE;
}

/*
 * ����:
 *	addrExpAddress:�쳣�����ĵ�ַ
 *
 * ����:
 *	Ѱ�ҵ�ָ���ĵ�ַ�����ĸ�����������֮��,����ҵ��򷵻ؼ��ܺ����ṹָ��,���򷵻�NULL
 */
__INLINE__ PPOWER_PROTECTER_ENCRYPT_PROCEDURE __INTERNAL_FUNC__ FindEncryptProcedureByExpInstAddress(__address addrExpAddress) {
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure = NULL;
	__integer i = 0;

	pEncryptProcedure = g_pPowerProtecterEncryptProcedures;
	for (i = 0; i < g_iPowerProtecterEncryptProcedures; i++) {
		if ((addrExpAddress >= (__address)(pEncryptProcedure->pRunSpace)) && (addrExpAddress < (__address)(pEncryptProcedure->pRunSpace + pEncryptProcedure->iTotalSize)))
			return pEncryptProcedure;
		pEncryptProcedure = (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)((__memory)pEncryptProcedure + pEncryptProcedure->iTotalSize);
	}
	return NULL;
}

/*
 * ����:
 *	pEncryptProcedure:���ܺ����ṹ
 *	addrExpAddress:�쳣�����ĵ�ַ
 *
 * ����:
 *	ͨ��ָ�����ڴ��ַ,�ҵ���Ӧ�ļ���ָ��ṹ
 */
__INLINE__ PPOWER_PROTECTER_ENCRYPT_INSTRUCTION __INTERNAL_FUNC__ FindEncryptInstructionByExpInstAddress(PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure, __address addrExpAddress) {
	PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pInstruction = NULL;
	PPOWER_PROTECTER_DECRYPT_PROCEDURE pDecryptProcedure = NULL;
	__address addrProcAddress = 0;
	__address addrRealAddress = 0;
	__integer i = 0;

	addrProcAddress = pEncryptProcedure->addrProcAddress;
	pDecryptProcedure = pEncryptProcedure->pDecryptProcedure;
	addrRealAddress = addrExpAddress - (__address)(pEncryptProcedure->pRunSpace) + addrProcAddress;
	pInstruction = (PPOWER_PROTECTER_ENCRYPT_INSTRUCTION)((__memory)(pDecryptProcedure) + sizeof(POWER_PROTECTER_DECRYPT_PROCEDURE));

	for (i = 0; i < pEncryptProcedure->iEncryptInstCount; i++) {
		if (addrRealAddress == pInstruction->addMemAddress)
			return pInstruction;
		pInstruction++;
	}
	return NULL;
}

#include "PowerProtecterTestEFlag.c"
/*
 * ����:
 *	pExceptionInfo:�쳣�ṹ
 *
 * ����:
 *	�ֲ����쳣����,�˺�������Ҫ�������б�����������
 */
__integer __API__ LocalPowerProtecter(PEXCEPTION_POINTERS pExceptionInfo) {
	PEXCEPTION_RECORD pExceptionRecord = NULL;
	PCONTEXT pContextRecord = NULL;
	__address addrExceptAddress = 0;
	__address addrTargetAddress = 0;
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure = NULL;
	PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pEncryptInstruction = NULL;
	__dword dwEFlags = 0;
	__integer iInstLen = 0;
	__bool bOut = FALSE;

	pExceptionRecord = pExceptionInfo->ExceptionRecord;
	pContextRecord = pExceptionInfo->ContextRecord;

	addrExceptAddress = (__address)(pExceptionRecord->ExceptionAddress);//��ȡ�쳣��ַ

	/*
	 * ��ȡ���ܺ����ṹ
	 * ͨ���쳣�ĵ�ַ���Ҽ��ܺ����ṹ,���ΪNULL,���ʾ��ָ������Ǳ�����
	 * �򷵻�__EXCEPTION_CONTINUE_SEARCH__����Ѱ�Ҷ�Ӧ���쳣����
	 */
	pEncryptProcedure = FindEncryptProcedureByExpInstAddress(addrExceptAddress);
	if (!pEncryptProcedure) return __EXCEPTION_CONTINUE_SEARCH__;

	/*
	 * ��ѯ�˵�ַ��Ӧ�Ľ���ָ��ṹ
	 * ����Ѿ�ȷ����ָ���������ǵļ��ܺ�����,���ҵ���Ӧ�ļ���ָ��ṹ
	 * ���û�ҵ�,������������Ǳ�����ָ��,�򷵻�__EXCEPTION_CONTINUE_SEARCH__����Ѱ�Ҷ�Ӧ���쳣����
	 */
	pEncryptInstruction = FindEncryptInstructionByExpInstAddress(pEncryptProcedure, addrExceptAddress);
	if (!pEncryptInstruction) return __EXCEPTION_CONTINUE_SEARCH__;

	/*
	 * �жϴ�ָ����ʲô����
	 * ���bOut = FALSE���ʾ����һ������ָ��,������һ��ָ��,������ת��Ŀ���ڱ���������Χ��
	 */
	bOut = pEncryptInstruction->bOut;
	dwEFlags = pContextRecord->EFlags;
	switch (pEncryptInstruction->JmpType) {
	case PPFT_JO:{
		// 70
		if (PowerProtecter_Test_00(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JNO:{
		// 71
		if (PowerProtecter_Test_01(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JB:{
		// 72
		if (PowerProtecter_Test_02(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JAE:{
		// 73
		if (PowerProtecter_Test_03(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JZ:{
		// 74
		if (PowerProtecter_Test_04(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JNZ:{
		// 75
		if (PowerProtecter_Test_05(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JBE:{
		// 76
		if (PowerProtecter_Test_06(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JA:{
		// 77
		if (PowerProtecter_Test_07(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JS:{
		// 78
		if (PowerProtecter_Test_08(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JNS:{
		// 79
		if (PowerProtecter_Test_08(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JP:{
		// 7A
		if (PowerProtecter_Test_0A(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JNP:{
		// 7B
		if (PowerProtecter_Test_0B(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JL:{
		// 7C
		if (PowerProtecter_Test_0C(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JGE:{
		// 7D
		if (PowerProtecter_Test_0D(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JLE:{
		// 7E
		if (PowerProtecter_Test_0E(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JG:{
		// 7F
		if (PowerProtecter_Test_0F(dwEFlags)) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JCXZ:{
		// E3
		if (pContextRecord->Ecx == 0) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JECXZ:{
		// E3
		if (pContextRecord->Ecx == 0) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JRCXZ:{
		// E3
		if (pContextRecord->Ecx == 0) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_LOOPNE:{
		if ((0 != pContextRecord->Ecx) && (0 == __POWER_PROTECTER_CPU_EFLAG_GET__(dwEFlags, PP_ER_ZF))) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_LOOPE:{
		if ((0 != pContextRecord->Ecx) && (1 == __POWER_PROTECTER_CPU_EFLAG_GET__(dwEFlags, PP_ER_ZF))) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_LOOP:{
		if (0 != pContextRecord->Ecx) {
			addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		} else {
			addrTargetAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		}
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_JMP:{
		addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_CALL:{
		// �����ֶ�ѹ����һ��ָ��ĵ�ַ����ջ
		__dword *pEsp = NULL;
		__address addrNextInstAddress = 0;
		addrNextInstAddress = addrExceptAddress + pEncryptInstruction->iInstLen;
		pEsp = (__dword *)(pContextRecord->Esp);
		pEsp--;//�ƶ���ջ
		*pEsp = (__dword)addrNextInstAddress;
		pContextRecord->Esp = (__dword)pEsp;

		// ��ת��Ŀ���ַ
		addrTargetAddress = pEncryptInstruction->addrTargetMemAddress;
		pContextRecord->Eip = addrTargetAddress;
	}break;
	case PPFT_RET:{
		__address *paddrCurrEsp = NULL;
		// ���ｫ���ص�ַ�޸�Ϊж�ر��ر����ĺ���
		paddrCurrEsp = (__address *)(pContextRecord->Esp);
		addrTargetAddress = *paddrCurrEsp;
		pContextRecord->Eip = addrTargetAddress;

		// ������ƽ���ջ����
		(__address)paddrCurrEsp = (__address)paddrCurrEsp + sizeof(__address);
		(__address)paddrCurrEsp = (__address)paddrCurrEsp + pEncryptInstruction->iRetSize;
		(__address *)(pContextRecord->Esp) = paddrCurrEsp;
	}break;
	default:{
		__byte *pInstBuf = NULL;
		__integer iInstLen = 0;
		__memory pCurrInstPoint = NULL;
		__address addrContinueAddress = 0;
		iInstLen = pEncryptInstruction->iInstLen;
		pInstBuf = pEncryptInstruction->InstBuf;
		pCurrInstPoint = (__memory)addrExceptAddress;
		// �������ָ�����
		if (pEncryptInstruction->bIsWatched) {
			__dword dwKey = 0;
			__address addrWatchAddress = 0;
			__integer iWatchSize = 0;
			addrWatchAddress = pEncryptInstruction->WatchRecord.addrMemAddress;
			iWatchSize = pEncryptInstruction->WatchRecord.iSize;
			dwKey = crc32((__memory)addrWatchAddress, iWatchSize);
			XorArray(dwKey, pInstBuf, pCurrInstPoint, iInstLen);
		} else {
			// ʹ��ָ����Key���н���
			__dword dwKey = 0;

			dwKey = pEncryptInstruction->dwKey;
			XorArray(dwKey, pInstBuf, pCurrInstPoint, iInstLen);
			__logic_memcpy__(pCurrInstPoint, pInstBuf, iInstLen);
		}
	}/* end default */
	}/* end switch */

	// �������תָ��������
	if (bOut) {
		// ����Ϊ������ת�Ĵ���
		EnterCriticalSection(&(pEncryptProcedure->LocalCriticalSection));
		// ���ټ���
		(pEncryptProcedure->iHandleCount)--;
		// �ͷź������пռ�
		if (!(pEncryptProcedure->iHandleCount)) {
			// �ͷŵ�ǰ�������пռ�
			__logic_delete__(pEncryptProcedure->pRunSpace);
			pEncryptProcedure->pRunSpace = NULL;
			// �ͷŽ��ܽṹ���ܽṹ
			__logic_delete__(pEncryptProcedure->pDecryptProcedure);
		}
		LeaveCriticalSection(&(pEncryptProcedure->LocalCriticalSection));
	}

	/*
	 * ����__EXCEPTION_CONTINUE_EXECUTION__�����쳣�Ѿ��õ��޸�,��������
	 */
	return __EXCEPTION_CONTINUE_EXECUTION__;
}

/*
 * ����:
 *	lpTopLevelExceptionFilter:�쳣�ṹ
 *
 * ����:
 *	ȫ�ֵ��쳣����,��SetUnhandledExceptionFilter���õĴ�����
 *	��ǰû���κ�����
 */
LONG __API__ GlobalPowerProtecter(PEXCEPTION_POINTERS pExceptionPointers) {
	return 0;
}

/*
 * ����:
 *	Ϊÿ�����ܺ�����������ͷ
 */
__void __INTERNAL_FUNC__ PowerProtecterGenInvokeStubs() {
	__integer i = 0;
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pCurrEncryptProcedure = NULL;

	pCurrEncryptProcedure = g_pPowerProtecterEncryptProcedures;
	for (i = 0; i < g_iPowerProtecterEncryptProcedures; i++) {
		// ������ȷ�ĺ�����ַ
		pCurrEncryptProcedure->addrProcAddress = pCurrEncryptProcedure->ofProcRVA + g_PowerProtecterInfo.addrNowTargetImageBase;
		PowerProtecterGenInvoke(i, pCurrEncryptProcedure);
		pCurrEncryptProcedure = (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)((__memory)pCurrEncryptProcedure + pCurrEncryptProcedure->iTotalSize);
	}
}

/*
 * ����:
 *	pConfigure:�������������ýṹ
 *
 * ����:
 *	����������ʼ��
 */
__void __API__ PowerProtecterInit(PPOWER_PROTECTER_CONFIGURE pConfigure, PPOWER_PROTECTER_INFO pInfo) {
	__integer i = 0;

	// ������Ϣ
	__logic_memcpy__(&g_PowerProtecterConfigure, pConfigure, sizeof(POWER_PROTECTER_CONFIGURE));
	__logic_memcpy__(&g_PowerProtecterInfo, pInfo, sizeof(POWER_PROTECTER_INFO));

	// ���µĻ���ַ��ȡ���ܺ��������ڴ��ַ
	g_pPowerProtecterEncryptProcedures = (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)((__offset)g_pPowerProtecterEncryptProcedures + g_PowerProtecterInfo.addrNowTargetImageBase);

	// ��ʼ��ȫ��ͬ������
	InitializeCriticalSection(&g_PowerProtecterCriticalSection);

	// Ϊÿ���������ĺ���,��������ͷ
	PowerProtecterGenInvokeStubs();

	// ����ȫ���쳣����
	SetUnhandledExceptionFilter(GlobalPowerProtecter);

	// ���þֲ��쳣����,û������������ĺ���ִ�в�����
	AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)LocalPowerProtecter);
}

// ��һ����ջ��ѹ��һ��4�ֽ�����
//__INLINE__ __dword * __INTERNAL_FUNC__ PushDwordToStack(__dword *pEsp, __dword dwValue) {
//	__dword *pCurr = NULL;
//	pCurr = pEsp - 1;
//	*pCurr = dwValue;
//	return pCurr;
//}

