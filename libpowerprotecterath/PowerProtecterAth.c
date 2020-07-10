#include "PowerProtecterAth.h"
#include "Support.h"
#include "lcrypto.h"

#include "PowerProtecterThunkCoder.c"

/*
 * ȫ�ֱ���
 */
POWER_PROTECTER_ATTACH_CONFIGURE g_PowerProtecterAttachConfigure = {0};

/*
 * ����:
 *	ofRVA:ָ���RVA
 *	pInstructions:��¼��ָ����
 *	iCount:ָ�����ļ���
 *
 * ����:
 *	ȷ��ָ���ĵ�ַ�Ƿ�����Ҫ���ܵ�ָ��
 */
__INLINE__ PPOWER_PROTECTER_INSTRUCTION __INTERNAL_FUNC__ ThisInstructionIsNeedProtect(__offset ofRVA, PPOWER_PROTECTER_INSTRUCTION pInstructions, __integer iCount) {
	__integer i = 0;
	for (i = 0; i < iCount; i++) {
		if (ofRVA == pInstructions->Instruction.ofMemRVA)
			return pInstructions;
		pInstructions++;
	}
	return NULL;
}

/*
 * ����:
 *	ofStartRVA:��������ʼRVA
 *	ofEndRVA:��������RVA
 *	ofTargetRVA:��ת��Ŀ���ڴ�RVA
 *
 * ����:
 *	��תָ���Ƿ�������ת
 */
__INLINE__ __bool __INTERNAL_FUNC__ IsJmpGotoOut(__offset ofStartRVA, __offset ofEndRVA, __offset ofTargetRVA) {
	if ((ofTargetRVA < ofStartRVA) || (ofTargetRVA >= ofEndRVA))
		return TRUE;
	return FALSE;
}

/*
 * ����:
 *	pud_obj:�����ṹ,������¼ָ��Ļ����Ϣ
 *
 * ����:
 *	�ж�һ��ָ���Ƿ�������ָ��
 */
__INLINE__ POWER_PROTECTER_FLOW_TYPE __INTERNAL_FUNC__ IsFlowInstruction(ud_t *pud_obj) {
	switch (pud_obj->mnemonic) {
	case UD_Ijo: return PPFT_JO; break;
	case UD_Ijno: return PPFT_JNO; break;
	case UD_Ijb: return PPFT_JB; break;
	case UD_Ijae: return PPFT_JAE; break;
	case UD_Ijz: return PPFT_JZ; break;
	case UD_Ijnz: return PPFT_JNZ; break;
	case UD_Ijbe: return PPFT_JBE; break;
	case UD_Ija: return PPFT_JA; break;
	case UD_Ijs: return PPFT_JS; break;
	case UD_Ijns: return PPFT_JNS; break;
	case UD_Ijp: return PPFT_JP; break;
	case UD_Ijnp: return PPFT_JNP; break;
	case UD_Ijl: return PPFT_JL; break;
	case UD_Ijge: return PPFT_JGE; break;
	case UD_Ijle: return PPFT_JLE; break;
	case UD_Ijg: return PPFT_JG; break;
	case UD_Ijcxz: return PPFT_JCXZ; break;
	case UD_Ijecxz: return PPFT_JECXZ; break;
	case UD_Ijrcxz: return PPFT_JRCXZ; break;
	case UD_Iloopnz: return PPFT_LOOPNE; break;
	case UD_Iloope: return PPFT_LOOPE; break;
	case UD_Iloop: return PPFT_LOOP; break;
	case UD_Ijmp: return PPFT_JMP; break;
	case UD_Icall: return PPFT_CALL; break;
	case UD_Iret: return PPFT_RET; break;
	}
	return PPFT_NONE;
}

/*
 * ����:
 *	pud_obj:�����ṹ,������¼ָ��Ļ����Ϣ
 *
 * ����:
 *	�������ָ���Ƿ���ƫ�����Ѱַ����,������򷵻���������ָ��ľ�������,���򷵻�PPFT_NONE
 */
__INLINE__ POWER_PROTECTER_FLOW_TYPE __INTERNAL_FUNC__ IsFlowInstructionByOffset(ud_t *pud_obj) {
	POWER_PROTECTER_FLOW_TYPE Type = PPFT_NONE;

	Type = IsFlowInstruction(pud_obj);
	if (Type != PPFT_NONE) {
		// ����Ƿ���ֱ�ӵ�ƫ��
		if (pud_obj->operand[0].type == UD_OP_JIMM)
			return Type;
		else if (Type == PPFT_RET)
			return Type;
	}

	return PPFT_NONE;
}

/*
 * ����:
 *	pMem:������������ļ�ӳ��
 *	pud_obj:�����ṹ
 *	ofProcMemRVA:�������ڴ�RVA
 *	iProcSize:�����ĳ���
 *	pInstruction:Ҫ���ܵ�ָ��
 *	pEncryptInstruction:����ָ��Ľṹ��ָ��(д���)
 *
 * ����:
 *	����ָ����ָ��
 */
__INLINE__ __void __INTERNAL_FUNC__ PowerProtectThisInstruction(__memory pMem, ud_t *pud_obj, __offset ofProcMemRVA, __integer iProcSize, \
	PPOWER_PROTECTER_INSTRUCTION pInstruction, PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pEncryptInstruction) {
	__memory pFileAddress = NULL;
	__integer iInstLength = 0;
	__offset ofRVA = 0;
	POWER_PROTECTER_FLOW_TYPE JmpType = PPFT_NONE;
	__offset ofProcEndMemRVA = 0;

	// ����������Ľ�β
	ofProcEndMemRVA = ofProcMemRVA + iProcSize;

	iInstLength = pud_obj->inp_ctr;
	pFileAddress = pud_obj->inp_buff;
	pFileAddress -= iInstLength;

	// ���û�����Ϣ
	pEncryptInstruction->ofMemRVA = pInstruction->Instruction.ofMemRVA;
	ofRVA = pEncryptInstruction->ofMemRVA;

	// ��ȡ����ָ�������
	JmpType = IsFlowInstructionByOffset(pud_obj);

	/*
	 * �����������������ָ��õ����ò���
	 */

	// ��������ָ�����ת����
	pEncryptInstruction->JmpType = JmpType;

	// ָ���
	pEncryptInstruction->iInstLen = iInstLength;

	// �������ָ����ƫ����תָ��
	if (JmpType != PPFT_NONE) {

		// ���������ָ��ֱ�ӽ����쳣����
		if (iInstLength == 1) {
			*(__byte *)pFileAddress = PowerProtecterRandGetExceptionInst();
		} else {
			__integer iRemainSize = 0;
			*(__byte *)pFileAddress = PowerProtecterRandGetExceptionInst();
			iRemainSize = iInstLength - 1;
			PowerProtecterGetJmpThunkCode(pFileAddress + 1, iRemainSize);
		}

		// ����Ƿ���ָ��
		if (JmpType == PPFT_RET) {
			// ��¼Ҫ���صĲ�������
			pEncryptInstruction->iRetSize = pud_obj->operand[0].lval.udword;//��¼���Ĳ�������
			pEncryptInstruction->bOut = TRUE;
			pEncryptInstruction->ofTargetMemRVA = 0;//RETָ���Ŀ���ַΪ0
		} else {
			// �������ǵ�Ŀ�ĵ�ַ
			__offset ofTargetRVA = 0;
			__offset ofOffset = 0;
			ofOffset = pud_obj->operand[0].lval.udword;
			if (pud_obj->operand[0].size == 8) {
				__byte bOffset = 0;
				bOffset = pud_obj->operand[0].lval.ubyte;
				if (__IsNegative8__(bOffset)) {
					ofTargetRVA = (__offset)CalcTargetAddress(8, (__address)ofRVA, bOffset, iInstLength, TRUE);
				} else {
					ofTargetRVA = (__offset)CalcTargetAddress(8, (__address)ofRVA, bOffset, iInstLength, FALSE);
				}
			} else if (pud_obj->operand[0].size == 16) {
				__word wOffset = 0;
				wOffset = pud_obj->operand[0].lval.uword;
				if (__IsNegative16__(wOffset)) {
					ofTargetRVA = (__offset)CalcTargetAddress(16, (__address)ofRVA, wOffset, iInstLength, TRUE);
				} else {
					ofTargetRVA = (__offset)CalcTargetAddress(16, (__address)ofRVA, wOffset, iInstLength, FALSE);
				}
			} else if (pud_obj->operand[0].size == 32) {
				__dword dwOffset = 0;
				dwOffset = pud_obj->operand[0].lval.udword;
				if (__IsNegative32__(dwOffset)) {
					ofTargetRVA = (__offset)CalcTargetAddress(32, (__address)ofRVA, dwOffset, iInstLength, TRUE);
				} else {
					ofTargetRVA = (__offset)CalcTargetAddress(32, (__address)ofRVA, dwOffset, iInstLength, FALSE);
				}
			}
			// ����Ƿ���������
			if (IsJmpGotoOut(ofProcMemRVA, ofProcEndMemRVA, ofTargetRVA)) {
				if (JmpType == PPFT_CALL)//CALLָ��������ⲿ��ת��ָ��
					pEncryptInstruction->bOut = FALSE;
				else
					pEncryptInstruction->bOut = TRUE;
			} else 
				pEncryptInstruction->bOut = FALSE;

			// ����Ŀ�ĵ�ַ
			pEncryptInstruction->ofTargetMemRVA = ofTargetRVA;
		}/* end else */
	} else {
		// ��Щ�Ƿ�����ָ��
		// ��������ָ��
		__logic_memset__(pEncryptInstruction->InstBuf, 0x90, __POWER_PROTECTER_MAX_INSTRUCTION_BUFFER_SIZE__);
		__logic_memcpy__(pEncryptInstruction->InstBuf, pFileAddress, iInstLength);

		// ����ָ���Ƿ��м��ӵ��ڴ��ַ
		if (pInstruction->bWatched) {
			__memory pWatchFileAddress = NULL;
			__integer iWatchSize = 0;
			__dword dwKey = 0;

			pEncryptInstruction->bIsWatched = TRUE;
			pEncryptInstruction->WatchRecord.ofMemRVA = pInstruction->WatchRecord.ofMemRVA;
			pEncryptInstruction->WatchRecord.iSize = pInstruction->WatchRecord.iSize;

			pWatchFileAddress = pMem + Rva2Raw(pMem, pEncryptInstruction->WatchRecord.ofMemRVA);
			iWatchSize = pEncryptInstruction->WatchRecord.iSize;

			// ��������ڴ��Key
			dwKey = crc32(pWatchFileAddress, iWatchSize);
			XorArray(dwKey, pEncryptInstruction->InstBuf, pEncryptInstruction->InstBuf, iInstLength);

		} else {
			// ʹ��Ĭ�ϵ�KEY���м���
			__dword dwKey = 0;

			pEncryptInstruction->bIsWatched = FALSE;
			dwKey = GenerateRandomNumber();//���������Կ
			XorArray(dwKey, pEncryptInstruction->InstBuf, pEncryptInstruction->InstBuf, iInstLength);
			pEncryptInstruction->dwKey = dwKey;//������Կ
		}

		// ������ָ��,��������쳣ָ��
		if (iInstLength == 1) {
			*(__byte *)pFileAddress = PowerProtecterRandGetExceptionInst();
		} else {
			__integer iRemainSize = 0;
			*(__byte *)pFileAddress = PowerProtecterRandGetExceptionInst();
			iRemainSize = iInstLength - 1;
			PowerProtecterGetJmpThunkCode(pFileAddress + 1, iRemainSize);
		}

		pEncryptInstruction->bOut = FALSE;//����ת
	}/* end else */
}

/*
 * ����:
 *	pMem:������������ļ�ӳ��
 *	pProcFileAddress:�������ļ���ַ
 *	ofProcMemRVA:������RVA
 *	iProcSize:�����ĳ���
 *	pProtectInstructions:Ҫ���ܵ�ָ��
 *	iProtectInstCount:Ҫ����ָ��ĸ���
 *	pPowerProtecterEncryptInstruction:����ָ��ṹ��ָ��(д���)
 *	pPowerProtecterEncryptProcedure:���ܺ����Ľṹ��ָ��(д���)
 *
 * ����:
 *	����ָ���ĺ�������
 */
__INLINE__ __integer __INTERNAL_FUNC__ PowerProtectProcedure(__memory pMem, __memory pProcFileAddress, __offset ofProcMemRVA, __integer iProcSize, \
	PPOWER_PROTECTER_INSTRUCTION pProtectInstructions, __integer iProtectInstCount, PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pPowerProtecterEncryptInstruction, \
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pPowerProtecterEncryptProcedure) {
	__integer iFlowInstCount = 0;
	__integer iInstLength = 0;
	__offset ofOffset = 0;
	__memory pCurr = NULL;
	__offset ofCurrMemRVA = 0;
	PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pEncryptInstruction = NULL;
	PPOWER_PROTECTER_INSTRUCTION pNowProtectInstruction = NULL;
	ud_t ud_obj = {0};

	pCurr = pProcFileAddress;
	ofCurrMemRVA = ofProcMemRVA;

	pEncryptInstruction = pPowerProtecterEncryptInstruction;

	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, pProcFileAddress, iProcSize);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	while (ud_disassemble(&ud_obj)) {
		iInstLength = ud_obj.inp_ctr;//ָ���

		// ���������ָ�����ж��Ƿ��ڱ�����Χ������
		pNowProtectInstruction = ThisInstructionIsNeedProtect(ofCurrMemRVA, pProtectInstructions, iProtectInstCount);
		if (pNowProtectInstruction) {
			/*
			 * ������û�ָ���ı���ָ��
			 */
			PowerProtectThisInstruction(pMem, &ud_obj, ofCurrMemRVA, iProcSize, pNowProtectInstruction, pEncryptInstruction);
			pEncryptInstruction++;//�ƶ�����һ������ָ��ṹ
		} else if (IsFlowInstructionByOffset(&ud_obj) != PPFT_NONE) {
			/*
			 * ���������ָ����ؽ��б���
			 */
			POWER_PROTECTER_INSTRUCTION PowerProtecterInstruction = {0};
			PowerProtecterInstruction.bWatched = FALSE;
			PowerProtecterInstruction.Instruction.ofMemRVA = ofCurrMemRVA;
			PowerProtecterInstruction.Instruction.iSize = iInstLength;
			pNowProtectInstruction = &PowerProtecterInstruction;

			PowerProtectThisInstruction(pMem, &ud_obj, ofCurrMemRVA, iProcSize, pNowProtectInstruction, pEncryptInstruction);
			pEncryptInstruction++;//�ƶ�����һ������ָ��ṹ
			iFlowInstCount++;
		}
		
		// ��¼�����Ϣ
		ofOffset += iInstLength;
		pCurr += iInstLength;
		ofCurrMemRVA += iInstLength;
	}

	return iFlowInstCount;
}

/*
 * ����:
 *	pProcedure:�����ļ���ַ
 *	iProcSize:��������
 *	pKeyProcedure:��Կ�����ļ���ַ
 *	iKeyProcSize:��Կ��������
 *	pInstructions:ָ����ļ���ַ
 *	iInstructionSize:ָ��ĳ���
 *	pPowerProtecterEncryptProcedure:���ܺ����ṹ
 *
 * ����:
 *	ʹ�ú��������Ĳ���һ����ԿX, ��X����ָ����ṹ
 *	ʹ����Կ����KP����һ����ԿY, ��Y���ܺ���P��������C0
 *	ʹ��    C0    ����һ����ԿZ, ��Z���ܼ�¼��Կ������ַ�������ڴ��������C1 //��Z���ܵ�ǰ������ԭʼ��ַ
 */
__INLINE__ __void __INTERNAL_FUNC__ PowerProtecterEncryptProcedure(__memory pProcedure, __integer iProcSize, __memory pKeyProcedure, __integer iKeyProcSize, \
	__memory pInstructions, __integer iInstructionSize, PPOWER_PROTECTER_ENCRYPT_PROCEDURE pPowerProtecterEncryptProcedure) {
	__dword dwKeyX = 0;
	__dword dwKeyY = 0;
	__dword dwKeyZ = 0;
	__memory pKeyProcedureRecord = NULL;

	// ��ȡ��Կ������¼�ṹ��ַ
	pKeyProcedureRecord = (__memory)&(pPowerProtecterEncryptProcedure->KeyProcedure);

	/*
	 * ������ԿX,Y
	 * ʹ�ñ������������Ĳ�����ԿX
	 * ʹ����Կ���������Ĳ�����ԿY
	 */
	dwKeyX = crc32(pProcedure, iProcSize);
	dwKeyY = crc32(pKeyProcedure, iKeyProcSize);

	/*
	 * ����ָ��ṹ
	 * ʹ�ñ�����������������Ϊ��Կ�����Ӽ���ָ��ṹ
	 * Ҫ�����걻����������ſ���ʹ���������Ľ��ܳ�ָ��ṹ
	 */
	XorArray(dwKeyX, pInstructions, pInstructions, iInstructionSize);

	/*
	 * ���ܺ�����
	 * ʹ����Կ������Ϊ���Ӽ��ܱ���������
	 */
	XorArray(dwKeyY, pProcedure, pProcedure, iProcSize);

	/*
	 * ������ԿZ
	 * ʹ�ñ��������������Ĳ�����ԿZ
	 */
	dwKeyZ = crc32(pProcedure, iProcSize);

	/*
	 * ���ܼ�¼�ṹ
	 */
	XorArray(dwKeyZ, pKeyProcedureRecord, pKeyProcedureRecord, sizeof(POWER_PROTECTER_WATCH_RECORD));
	//XorArray(dwKeyZ, (__memory)&(pPowerProtecterEncryptProcedure->ofProcRVA), (__memory)&(pPowerProtecterEncryptProcedure->ofProcRVA), sizeof(__offset));

	return;
}

/*
 * ����:
 *	iIndex:��ǰ����������
 *	pMem:������Ŀ�������ļ�ӳ��ָ��
 *	pProcedure:�����������ļ�¼�ṹ
 *	pKeyProcedure:��Կ�����ļ�¼�ṹ
 *	pInstructions:������ָ��ļ�¼�ṹ
 *	iEncryptInstCount:������ָ��ļ���
 *	pEncryptProcedure:���ܺ���
 *
 * ����:
 *	������ǰָ���ĺ���,ʧ�ܷ���NULL,�ɹ�������һ�����ܺ����ṹ
 */
PPOWER_PROTECTER_ENCRYPT_PROCEDURE __INTERNAL_FUNC__ PowerProtectThisProcedure(__integer iIndex, __memory pMem, PPOWER_PROTECTER_PROCEDURE_RECORD pProcedure, \
	PPOWER_PROTECTER_WATCH_RECORD pKeyProcedure, PPOWER_PROTECTER_INSTRUCTION pInstructions, __integer iEncryptInstCount, \
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedure) {
	
	__memory pProcedureFileAddress = NULL;
	__offset ofProcMemRVA = 0;
	__memory pKeyProcedureFileAddress = NULL;
	__offset ofKeyProcMemRVA = 0;
	__memory pEncryptProcedureBuffer = NULL;
	PPOWER_PROTECTER_ENCRYPT_INSTRUCTION pEncryptInstructions = NULL;
	__integer iProcSize = 0;
	__integer iKeyProcSize = 0;
	__integer iTotalSize = 0;
	__integer iFlowInstCount = 0;

	pProcedureFileAddress = pProcedure->pFileAddress;
	ofProcMemRVA = pProcedure->ofMemRVA;
	pKeyProcedureFileAddress = pKeyProcedure->pFileAddress;
	ofKeyProcMemRVA = pKeyProcedure->ofMemRVA;
	iProcSize = pProcedure->iSize;
	iKeyProcSize = pKeyProcedure->iSize;

	// �õ���Ӧ�Ľṹ��ַ
	pEncryptProcedureBuffer = (__memory)pEncryptProcedure + sizeof(POWER_PROTECTER_ENCRYPT_PROCEDURE);
	pEncryptInstructions = (PPOWER_PROTECTER_ENCRYPT_INSTRUCTION)(pEncryptProcedureBuffer + iProcSize);

	// ���������������
	iFlowInstCount = 0;
	iFlowInstCount = PowerProtectProcedure(pMem, pProcedureFileAddress, ofProcMemRVA, iProcSize, pInstructions, iEncryptInstCount, pEncryptInstructions, pEncryptProcedure);
	iEncryptInstCount += iFlowInstCount;//����������ָ��

	// ���Ʊ�����Ľ��
	__logic_memcpy__(pEncryptProcedureBuffer, pProcedureFileAddress, iProcSize);

	// ���û�ָ��
	if (g_PowerProtecterAttachConfigure.pGenPowerProtecterInvokeThunkCode) {
		__memory pInvokeThunkCode = NULL;
		FPGenPowerProtecterInvokeThunkCode pGenInvokeThunkCode = g_PowerProtecterAttachConfigure.pGenPowerProtecterInvokeThunkCode;
		pInvokeThunkCode = pGenInvokeThunkCode(iProcSize);
		if (!pInvokeThunkCode) {
			// ����ռ�ʧ��
		}
		__logic_memcpy__(pProcedureFileAddress, pInvokeThunkCode, iProcSize);
		if (pInvokeThunkCode) __logic_delete__(pInvokeThunkCode);
	} else {
		__logic_memset__(pProcedureFileAddress, 0x90, iProcSize);
	}

	// �����ܺ����ṹ
	pEncryptProcedure->iProcSize = iProcSize;
	pEncryptProcedure->KeyProcedure.ofMemRVA = pKeyProcedure->ofMemRVA;
	pEncryptProcedure->KeyProcedure.iSize = pKeyProcedure->iSize;
	pEncryptProcedure->iEncryptInstCount = iEncryptInstCount;
	pEncryptProcedure->bInit = FALSE;
	__logic_memset__(&(pEncryptProcedure->LocalCriticalSection), 0, sizeof(CRITICAL_SECTION));
	pEncryptProcedure->iHandleCount = 0;
	pEncryptProcedure->pRunSpace = NULL;
	iTotalSize = sizeof(POWER_PROTECTER_ENCRYPT_PROCEDURE) + iProcSize + sizeof(POWER_PROTECTER_ENCRYPT_INSTRUCTION) * iEncryptInstCount;
	iTotalSize = Alig(iTotalSize, __POWER_PROTECT_ENCRYPT_PROCEDURE_ALIGN__, TRUE);//�ܳ���
	pEncryptProcedure->iTotalSize = iTotalSize;
	pEncryptProcedure->ofProcRVA = pProcedure->ofMemRVA;

	// ���ܺ����벿�ֽṹ
	PowerProtecterEncryptProcedure(pEncryptProcedureBuffer, iProcSize, pKeyProcedureFileAddress, iKeyProcSize, (__memory)pEncryptInstructions, iEncryptInstCount * sizeof(POWER_PROTECTER_ENCRYPT_INSTRUCTION), pEncryptProcedure);

	// ������һ�������ļ��ܽṹָ��
	return (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)((__memory)pEncryptProcedure + pEncryptProcedure->iTotalSize);
}

/*
 *
 * ����:
 *	pMem:����������ӳ����ڴ�ָ��
 *	pRecord:��¼��Ҫ��������Ϣ
 *
 * ����:
 *	�������м��ܺ����ṹ�ĳ���
 */
__integer __API__ PowerProtecterAthCountAllEncryptProceduresSize(__memory pMem, PPOWER_PROTECTER_PROCEDURE pRecord) {
	__integer iTotalSize = 0;
	__integer i = 0;
	__integer iCount = 0;
	__integer iEncryptInstSize = 0;
	__integer iSize = 0;
	__address addrImageBase = 0;
	__integer iFlowCount = 0;
	ud_t ud_obj = {0};

	addrImageBase = GetNtHeader(pMem)->OptionalHeader.ImageBase;
	iCount = pRecord->iCount;

	for (i = 0; i < iCount; i++) {
		__memory pProcFileAddress = NULL;
		__integer iProcSize = 0;
		__address addrProcMemAddress = 0;

		addrProcMemAddress = pRecord->Procedure[i].addrMemAddress;
		pProcFileAddress = pMem + Rva2Raw(pMem, addrProcMemAddress - addrImageBase);
		iProcSize = pRecord->Procedure[i].iSize;

		// ��������������������ָ�����
		ud_init(&ud_obj);
		ud_set_input_buffer(&ud_obj, pProcFileAddress, iProcSize);
		ud_set_mode(&ud_obj, 32);
		ud_set_syntax(&ud_obj, UD_SYN_INTEL);
		while (ud_disassemble(&ud_obj)) {
			POWER_PROTECTER_FLOW_TYPE Type = PPFT_NONE;
			Type = IsFlowInstructionByOffset(&ud_obj);
			if (Type != PPFT_NONE)
				iFlowCount++;
		}

		// �����������������
		iEncryptInstSize = (iFlowCount + pRecord->iInstructionCount[i]) * sizeof(POWER_PROTECTER_ENCRYPT_INSTRUCTION);//�������ָ���ܳ���
		// ���ܺ����ĳ��� + �������ܽṹ���� + ����ָ��ṹ�ܳ���
		iSize = pRecord->Procedure[i].iSize + sizeof(POWER_PROTECTER_ENCRYPT_PROCEDURE) + iEncryptInstSize;
		iSize = Alig(iSize, __POWER_PROTECT_ENCRYPT_PROCEDURE_ALIGN__, TRUE);
		iTotalSize += iSize;
	}

	return iTotalSize;
}

/*
 * ����:
 *	pMem:������Ŀ�������ļ�ӳ��ָ��
 *	addrImageBase:ӳ��Ļ���ַ
 *	pRecord:������Ϣ��¼�ṹ
 *
 * ����:
 *	���ñ������������ļ���ַ
 */
__INLINE__ __void __INTERNAL_FUNC__ PowerProtecterSetFileAddress(__memory pMem, __address addrImageBase, PPOWER_PROTECTER_PROCEDURE pRecord) {
	__integer i = 0;
	__integer iCount = pRecord->iCount;

	// ����ÿ�������ļ���ַָ��
	for (i = 0; i < iCount; i++) {
		// ����������
		pRecord->Procedure[i].ofMemRVA = pRecord->Procedure[i].addrMemAddress - addrImageBase;
		pRecord->Procedure[i].pFileAddress = pMem + Rva2Raw(pMem, pRecord->Procedure[i].ofMemRVA);

		// ��Կ����
		pRecord->KeyProcedure[i].ofMemRVA = pRecord->KeyProcedure[i].addrMemAddress - addrImageBase;
		pRecord->KeyProcedure[i].pFileAddress = pMem + Rva2Raw(pMem, pRecord->KeyProcedure[i].ofMemRVA);
	}
}

/*
 * ����:
 *	pMem:������Ŀ�������ļ�ӳ��ָ��
 *	addrImageBase:ӳ��Ļ���ַ
 *	pRecord:������Ϣ��¼�ṹ
 *	pEncryptProcedures:���ܺ����ṹ��ָ��
 *
 * ����:
 *	����ȷ����ť,����ѡ���ĺ���
 */
__void __API__ PowerProtectAthGo(__memory pMem, PPOWER_PROTECTER_PROCEDURE pRecord, PPOWER_PROTECTER_ENCRYPT_PROCEDURE pEncryptProcedures) {
	__integer i = 0;
	__integer iCount = 0;
	__integer iEncryptInstCount = 0;
	__address addrImageBase = 0;
	PPOWER_PROTECTER_INSTRUCTION pInstructions = NULL;
	PPOWER_PROTECTER_PROCEDURE_RECORD pProcedure = NULL;
	PPOWER_PROTECTER_WATCH_RECORD pKeyProcedure = NULL;
	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pCurrEncryptProcedure = NULL;
	PIMAGE_NT_HEADERS pNtHdr = NULL;

	// ��ȡNTͷ
	pNtHdr = GetNtHeader(pMem);

	// ��ȡ����ַ
	addrImageBase = pNtHdr->OptionalHeader.ImageBase;

	// ��¼������
	iCount = pRecord->iCount;

	// ����ÿ�������ļ���ַָ��
	PowerProtecterSetFileAddress(pMem, addrImageBase, pRecord);

	pCurrEncryptProcedure = pEncryptProcedures;//ָ���һ�����ܺ����ṹ
	for (i = 0; i < iCount; i++) {
		pProcedure = &(pRecord->Procedure[i]);
		pKeyProcedure = &(pRecord->KeyProcedure[i]);
		pInstructions = (PPOWER_PROTECTER_INSTRUCTION)&(pRecord->Instructions[i]);
		iEncryptInstCount = pRecord->iInstructionCount[i];
		pCurrEncryptProcedure = PowerProtectThisProcedure(i, pMem, pProcedure, pKeyProcedure, pInstructions, iEncryptInstCount, pCurrEncryptProcedure);
	}
}

/*
 * ����:
 *	pRecord:Ҫ���õĲ����ṹ
 *	addrProcedure:�������ڴ��ַ
 *	iSize:�����ĳ���
 *	addrKeyProcedure:��Կ�������ڴ��ַ
 *	iKeySize:��Կ�����ĳ���
 *	pInstruction:Ҫ����ָ��������ṹ
 *	iInstCount:ָ��ļ���
 *
 * ����:
 *	����Ҫ�����ĺ�������ϸ����
 */
__integer __API__ PowerProtectAthSelectProcedure(PPOWER_PROTECTER_PROCEDURE pRecord, __address addrProcedure, __integer iSize, __address addrKeyProcedure, __integer iKeySize, PPOWER_PROTECTER_INSTRUCTION pInstruction, __integer iInstCount) {
	__integer iCount = 0;
	iCount = pRecord->iCount;
	pRecord->Procedure[iCount].addrMemAddress = addrProcedure;
	pRecord->Procedure[iCount].iSize = iSize;
	pRecord->KeyProcedure[iCount].addrMemAddress = addrKeyProcedure;
	pRecord->KeyProcedure[iCount].iSize = iKeySize;
	if (iInstCount > __POWER_PROTECT_MAX_INSTRUCTION__)//���ܳ�����󱣻�����
		iInstCount = __POWER_PROTECT_MAX_INSTRUCTION__;
	__logic_memcpy__((__memory)&(pRecord->Instructions[iCount]), (__memory)pInstruction, sizeof(POWER_PROTECTER_INSTRUCTION) * iInstCount);
	pRecord->iInstructionCount[iCount] = iInstCount;

	(pRecord->iCount)++;
	return pRecord->iCount;
}

/*
 * ����:
 *	pConfigure:���ýṹ
 *
 * ����:
 *	�������ò���
 */
__void __API__ PowerProtecterAthSetArguments(PPOWER_PROTECTER_ATTACH_CONFIGURE pConfigure) {
	__logic_memcpy__(&g_PowerProtecterAttachConfigure, pConfigure, sizeof(POWER_PROTECTER_ATTACH_CONFIGURE));
}
