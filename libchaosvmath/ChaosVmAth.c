#include "ChaosVmAth.h"
#include "lcrypto.h"
#include "Support.h"

/*
 * ȫ�ֱ���
 */
PCHAOSVMATH g_pChaosVmAth = NULL;

/*
 * �ڲ�ʹ�ú���
 */
__void __INTERNAL_FUNC__ ChaosVmAthSetFileAddressToProcedureList(__memory pMem, PCHAOSVMATH_CONFIGURE pConfigure);//�����ļ���ַ��Ҫ�����ĺ���,��ȫ��Ҫ�����ĺ���ѡ���Ժ����
PCHAOSVMP_PROCEDURE __INTERNAL_FUNC__ ChaosVmAthProtectIt(PCHAOSVMATH pChaosVmAth, __memory pProcedure, __integer iProcedureSize, __offset ofProcedureRVA, __memory pKeyProcedure, __integer iKeyProcedureSize, __offset ofKeyProcedureRVA, __integer iIndex);//����ѡ����
__integer __INTERNAL_FUNC__ ChaosVmAthSelected(PCHAOSVMATH pChaosVmAth);//�൱��ȷ�ϰ�ť

__dword __INTERNAL_FUNC__ ChaosVmAthHash(__memory pPoint, __integer iSize) {
	return crc32(pPoint, iSize);
}

__integer __INTERNAL_FUNC__ ChaosVmAthEncrypt(__memory pIn, __integer iSize, __dword dwKey, __memory pOut) {
	__byte *pBuf = __logic_new_size__(iSize + 0x10);
	__byte *pKey = __logic_new_size__(iSize + 0x10);
	__logic_memcpy__(pBuf, pIn, iSize);
	XorKey32Bits(dwKey, pKey, iSize);
	XorCoder(pKey, pBuf, iSize);
	__logic_memcpy__(pOut, pBuf, iSize);//���
	__logic_delete__(pBuf);
	__logic_delete__(pKey);
	return iSize;
}

__integer __INTERNAL_FUNC__ ChaosVmAthInstRemainEncrypt(__memory pIn, __integer iSize, __dword dwKey, __memory pOut) {
	__byte *pBuf = __logic_new_size__(iSize + 0x10);
	__byte *pKey = __logic_new_size__(iSize + 0x10);
	__logic_memcpy__(pBuf, pIn, iSize);
	XorKey32Bits(dwKey, pKey, iSize);
	XorCoder(pKey, pBuf, iSize);
	__logic_memcpy__(pOut, pBuf, iSize);//���
	__logic_delete__(pBuf);
	__logic_delete__(pKey);
	return iSize;
}

/*
 * ����:
 *	pConfigure:ִ�л�������������������ýṹ
 *
 * ����:
 *	��ʼ��������������������ýṹ
 */
__void __API__ ChaosVmAthInitConfigure(PCHAOSVMATH_CONFIGURE pConfigure) {
	__logic_memset__(pConfigure, 0, sizeof(CHAOSVMATH_CONFIGURE));
}

/*
 * ����:
 *	pConfigure:ִ�л�������������������ýṹ
 *	addrMemoryAddress:Ҫ�����������ڴ��ַ
 *	iSize:Ҫ���������ĳ���
 *
 * ����:
 *	����Ҫ�����ĺ���,��������������ڴ��ַ�볤��,ͨ���˺���������Ҫ�����ĺ���
 */
__void __API__ ChaosVmAthSelectProcedure(PCHAOSVMATH_CONFIGURE pConfigure, __address addrMemoryAddress, __integer iSize, __address addrKeyMemoryAddress, __integer iKeySize) {
	PCHAOSVMATH_PROCEDURE pProcedure = &(pConfigure->ProceduceList);
	pProcedure[pConfigure->iProcedureCount].addrMemoryAddress = addrMemoryAddress;
	pProcedure[pConfigure->iProcedureCount].iSize = iSize;
	pProcedure[pConfigure->iProcedureCount].addrKeyMemoryAddress = addrKeyMemoryAddress;
	pProcedure[pConfigure->iProcedureCount].iKeySize = iKeySize;

	(pConfigure->iProcedureCount)++;
}

/*
 * ����:
 *	pChaosVmAth:����������������ṹָ��
 *	iCount:Ҫ���õĻ�����������л���������
 *
 * ����:
 *	����Ĭ�ϵķ��ɺ������ָ���Ļ�����������л���
 */
__INLINE__ __void __INTERNAL_FUNC__ ChaosVmAthSetDefRunTables(PCHAOSVMATH pChaosVmAth, __integer iCount) {
	PCHAOSVM_RUNTIME pRuntime = &(pChaosVmAth->pChaosVmRuntimeList[iCount]);
	PCHAOSVM_CPU pCPU = &(pRuntime->Cpu);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryFF, pChaosVmAth->pOpcodeExtensionTableForOne_FF, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryFE, pChaosVmAth->pOpcodeExtensionTableForOne_FE, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryC6, pChaosVmAth->pOpcodeExtensionTableForOne_C6, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryC7, pChaosVmAth->pOpcodeExtensionTableForOne_C7, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08 * 2);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryF6, pChaosVmAth->pOpcodeExtensionTableForOne_F6, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryF7, pChaosVmAth->pOpcodeExtensionTableForOne_F7, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntry80, pChaosVmAth->pOpcodeExtensionTableForOne_80, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntry81, pChaosVmAth->pOpcodeExtensionTableForOne_81, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntry83, pChaosVmAth->pOpcodeExtensionTableForOne_83, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntry8F, pChaosVmAth->pOpcodeExtensionTableForOne_8F, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryC0, pChaosVmAth->pOpcodeExtensionTableForOne_C0, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryC1, pChaosVmAth->pOpcodeExtensionTableForOne_C1, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryD0, pChaosVmAth->pOpcodeExtensionTableForOne_D0, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryD1, pChaosVmAth->pOpcodeExtensionTableForOne_D1, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryD2, pChaosVmAth->pOpcodeExtensionTableForOne_D2, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntryD3, pChaosVmAth->pOpcodeExtensionTableForOne_D3, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->PargsDispatchTableEntry0FBA, pChaosVmAth->pOpcodeExtensionTableForTwo_0FBA, sizeof(PARGS_DISPATCH_TABLE_ENTRY) * 0x08);
	__logic_memcpy__(pCPU->OneOpcodeTableEntry, pChaosVmAth->pOneByteOpcodeTable, sizeof(ONE_OPCODE_TABLE_ENTRY) * 0x100);
	__logic_memcpy__(pCPU->TwoByteOpcodeTableEntry, pChaosVmAth->pTwoByteOpcodeTable, sizeof(TWO_BYTE_OPCODE_TABLE_ENTRY) * 0x100);
	__logic_memcpy__(pCPU->EFlagConditionTableEntry, pChaosVmAth->pEFlagContionTable, sizeof(EFLAG_CONDITION_TABLE_ENTRY) * 0x10);
	__logic_memcpy__(pCPU->ModrmByteAnalyseRoutineTableEntry, pChaosVmAth->pModRMAnalyseRoutineTable, sizeof(MODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY) * 0x02);
}

/*
 * ����:
 *	pConfigure:������������������ýṹ
 *
 * ����:
 *	����Ҫ���������к����ĳ����ۺ�,�������ʹ��__VMPPROCEDURE_DATA_SIZE__���Ƚ��ж���
 */
#define __VMPPROCEDURE_DATA_SIZE__		0x1000
__INLINE__ __integer __INTERNAL_FUNC__ ChaosVmAthCountVmpProcedureTotalSize(PCHAOSVMATH_CONFIGURE pConfigure) {
	__integer i = 0, iTotalSize = 0;
	for (i = 0; i < pConfigure->iProcedureCount; i++)
		iTotalSize += pConfigure->ProceduceList[i].iSize;
	iTotalSize = Alig(iTotalSize, __VMPPROCEDURE_DATA_SIZE__, TRUE);
	return iTotalSize;
}

/*
 * ����:
 *	pMem:Ҫ�����������ļ����ļ�ӳ��
 *	pConfigure:������������������ýṹ
 *
 * ����:
 *	�������б������ĺ������ļ���ַͨ����ǰ���ļ�ӳ��
 */
__void __INTERNAL_FUNC__ ChaosVmAthSetFileAddressToProcedureList(__memory pMem, PCHAOSVMATH_CONFIGURE pConfigure) {
	PCHAOSVMATH_PROCEDURE pProcedure = &(pConfigure->ProceduceList);
	__address addrImageBase = GetNtHeader(pMem)->OptionalHeader.ImageBase;
	__integer i = 0;
	for (i = 0; i < pConfigure->iProcedureCount; i++) {
		__memory pCurr = pMem + Rva2Raw(pMem, pProcedure[i].addrMemoryAddress - addrImageBase);
		__memory pKeyCurr = pMem + Rva2Raw(pMem, pProcedure[i].addrKeyMemoryAddress - addrImageBase);
		pProcedure[i].pAddress = pCurr;
		pProcedure[i].pKeyAddress = pKeyCurr;
	}
}

__INLINE__ __void __INTERNAL_FUNC__ FreeChaosVmAth(PCHAOSVMATH pChaosVmAth) {
	__memory pOrigChaosVm = NULL;
	__memory pChaosVm = NULL;
	PCHAOSVMATH_CONFIGURE pConfigure = NULL;
	__integer i = 0;
	__integer iCount = 0;

	if (!pChaosVmAth) return;

	pChaosVm = pChaosVmAth->pChaosVm;
	pOrigChaosVm = pChaosVmAth->pOrigChaosVm;
	pConfigure = pChaosVmAth->pConfigure;
	if (pChaosVmAth->piVmpProcedureCount) iCount = *(pChaosVmAth->piVmpProcedureCount);

	// �ͷŻ��������,����DISAth�н����ͷ�
	//if (pChaosVm) UnMapResourceData(pChaosVm);

	if (pOrigChaosVm) 
		__logic_delete__(pOrigChaosVm);

	for (i = 0; i < iCount; i++) {
		__logic_delete__(pChaosVmAth->pChaosVmpProcedure[i]);
	}

	__logic_delete__(pChaosVmAth);
	//__logic_memset__(pChaosVmAth, 0, sizeof(CHAOSVMATH));
}

/*
 * ����:
 *	pConfigure:DIS���ýṹ
 *
 * ����:
 *	��������������ӵ�Ҫ�����ĳ�����
 *
 *	Ҫ��̬�ͷŵ��ڴ���:
 *		1.Ŀ���ļ�ӳ�䱾��
 *		2.�����������ԭʼ����(ԭʼ����,δ��������չʹ��)
 *		3.����������������ṹ
 *		4.Ŀ���ļ�ĩβ���ݱ���(�������ĩβ����)
 *		5.�������������
 */
__dword __API__ ChaosVmAthAttach(PDISATH_CONFIGURE pConfigure) {
	__dword dwRet = __DIS_ERROR_SUCCESS__;

	// ���ӻ��������
	dwRet = DISAthAttach(pConfigure);
	if (!__DISATH_SUCCESS__(dwRet))
		FreeChaosVmAth(g_pChaosVmAth);

	return dwRet;
}

/*
 * ����:
 *	pChaosVmAth:����������������ṹ
 *	iCount:Ҫ���б�������л���ʱ������
 *	pOpcodeTbl1:��һ�ű����
 *	pOpcodeTbl2:�ڶ��ű����
 *
 * ����:
 *	ͨ��ת����OPCODE���ĳһ�����л���ʱ(�������ĺ���)��CPU��ǲ���������˳�򽻻�,��������Ŀ����ʹÿ���������ĺ���������CPU�Ľ���ָ��ִ�е���ǲ
 *	�������ж��в�ͬ�����˳��,������ֹ�Թ̶���ַ�ķ���
 */
__INLINE__ __void __INTERNAL_FUNC__ ChaosVmAthMixTables(PCHAOSVMATH pChaosVmAth, __integer iCount, __byte *pOpcodeTbl1, __byte *pOpcodeTbl2) {
	__bool bSwitched[0x100] = {FALSE};
	__integer i = 0;
	PCHAOSVM_RUNTIME pRuntime = &(pChaosVmAth->pChaosVmRuntimeList[iCount]);
	PCHAOSVM_CPU pCPU = &(pRuntime->Cpu);
	PONE_OPCODE_TABLE_ENTRY pOneByteOpcodeTable = &(pCPU->OneOpcodeTableEntry);
	PTWO_BYTE_OPCODE_TABLE_ENTRY pTwoByteOpcodeTable = &(pCPU->TwoByteOpcodeTableEntry);
	ONE_OPCODE_TABLE_ENTRY OneOpcodeTableEntryTmp;
	TWO_BYTE_OPCODE_TABLE_ENTRY TwoByteOpcodeTableEntryTmp;

	// ��һ�ű�
	for (i = 0; i < 0x100; i++) {
		__byte bTargetIndex = pOpcodeTbl1[i];
		if (bSwitched[i]) continue;//����Ѿ��õ��˽��������

		__logic_memcpy__((__memory)(&OneOpcodeTableEntryTmp), (__memory)(&pOneByteOpcodeTable[i]), sizeof(ONE_OPCODE_TABLE_ENTRY));
		__logic_memcpy__((__memory)(&pOneByteOpcodeTable[i]), (__memory)(&pOneByteOpcodeTable[bTargetIndex]), sizeof(ONE_OPCODE_TABLE_ENTRY));
		__logic_memcpy__((__memory)(&pOneByteOpcodeTable[bTargetIndex]), (__memory)(&OneOpcodeTableEntryTmp), sizeof(ONE_OPCODE_TABLE_ENTRY));

		// �����Խ�����־
		bSwitched[i] = TRUE;
		bSwitched[bTargetIndex] = TRUE;
	}

	// �ڶ��ű�
	__logic_memset__(bSwitched, FALSE, 0x100);
	for (i = 0; i < 0x100; i++) {
		__byte bTargetIndex = pOpcodeTbl2[i];
		if (bSwitched[i]) continue;//����Ѿ��õ��˽��������

		__logic_memcpy__((__memory)(&TwoByteOpcodeTableEntryTmp), (__memory)(&pTwoByteOpcodeTable[i]), sizeof(TWO_BYTE_OPCODE_TABLE_ENTRY));
		__logic_memcpy__((__memory)(&pTwoByteOpcodeTable[i]), (__memory)(&pTwoByteOpcodeTable[bTargetIndex]), sizeof(TWO_BYTE_OPCODE_TABLE_ENTRY));
		__logic_memcpy__((__memory)(&pTwoByteOpcodeTable[bTargetIndex]), (__memory)(&TwoByteOpcodeTableEntryTmp), sizeof(TWO_BYTE_OPCODE_TABLE_ENTRY));

		// �����Խ�����־
		bSwitched[i] = TRUE;
		bSwitched[bTargetIndex] = TRUE;
	}
}

/*
 * ����:
 *	pChaosVmAth:����������������ṹ
 *	pProcedure:Ҫ�����������ļ���ַ
 *	iProcedureSize:Ҫ���������Ĵ�С
 *	addrProcedure:Ҫ�����������ڴ��ַ
 *	iIndex:��������������
 *
 * ����:
 *	ִ�ж�ָ�����������⻯
 */
PCHAOSVMP_PROCEDURE __INTERNAL_FUNC__ ChaosVmAthProtectIt(PCHAOSVMATH pChaosVmAth, __memory pProcedure, __integer iProcedureSize, __offset ofProcedureRVA, \
															__memory pKeyProcedure, __integer iKeyProcedureSize, __offset ofKeyProcedureRVA, __integer iIndex) {
	__byte Opcode1Tbl[0x100] = {0};
	__byte Opcode2Tbl[0x100] = {0};
	__byte ModRmTbl[0x03] = {0};
	__byte SibTbl[0x03] = {0};
	__address addrWriteAddress = 0, addrTargetImageBase = 0;
	__offset ofOffset = 0, ofRVA = 0;
	__integer iWriteSize = 0;
	//__integer iCount = *(pChaosVmAth->piVmpProcedureCount);//��ȡ��ǰ��������������еĺ�������
	PCHAOSVMP_PROCEDURE pVmpProcedure = NULL;
	PCHAOSVMATH_CONFIGURE pConfigure = pChaosVmAth->pConfigure;
	PCHAOSVM_RUNTIME pRuntime = &(pChaosVmAth->pChaosVmRuntimeList[iIndex]);
	PCHAOSVM_CPU pCPU = &(pRuntime->Cpu);
	__bool bHeader = FALSE;
	__dword dwNextKey = 0;
	__memory pChaosVmInvokeThunkCode = NULL;
	__integer iChaosVmInvokeThunkCodeSize = 0;
	__memory pOrigVmpProcedure = NULL;

	// �Ƿ���ͷ����
	if (iIndex == 0)
		bHeader = TRUE;

	// ÿ��������ӵ���Լ���OPCODE���������Modrm/Sib��
	GenerateRandOpcodeTable(Opcode1Tbl, NULL);
	GenerateRandOpcodeTable(Opcode2Tbl, NULL);
	GenerateRandModRmTable(ModRmTbl, NULL);
	GenerateRandSibTable(SibTbl, NULL);

	// �趨�������������ͷ
	iChaosVmInvokeThunkCodeSize = iProcedureSize;
	if (pConfigure->pGenChaosVmInvokeThunkCode) {
		// ʹ���Զ���ĵ���ͷ
		pChaosVmInvokeThunkCode = pConfigure->pGenChaosVmInvokeThunkCode(iChaosVmInvokeThunkCodeSize);
		if (!pChaosVmInvokeThunkCode) goto _use_def_invoke_thunkcode;//�����ָ������ʧ��,�����Ĭ�����ɵĻ�ָ��
	} else {
_use_def_invoke_thunkcode:
		// ʹ��Ĭ�ϵĻ�ָ���޸�����
		pChaosVmInvokeThunkCode = (__memory)__logic_new_size__(iChaosVmInvokeThunkCodeSize);
		__logic_memset__(pChaosVmInvokeThunkCode, 0x90, iChaosVmInvokeThunkCodeSize);
	}

	// ����ԭʼ�ĺ���
	pOrigVmpProcedure = (__memory)__logic_new_size__(iProcedureSize);
	if (!pOrigVmpProcedure) {
		if (pChaosVmInvokeThunkCode) __logic_delete__(pChaosVmInvokeThunkCode);
	}
	__logic_memcpy__(pOrigVmpProcedure, pProcedure, iProcedureSize);

	// �������������
	if (bHeader) {
		pVmpProcedure = VmpThisProcedure(pProcedure, iProcedureSize, ofProcedureRVA, pChaosVmAth->pVmHash, \
										pChaosVmAth->pVmEncrypt, pChaosVmAth->pVmInstRemainEncrypt, Opcode1Tbl, \
										Opcode2Tbl, ModRmTbl, SibTbl, pChaosVmInvokeThunkCode, iChaosVmInvokeThunkCodeSize, \
										0, TRUE, &dwNextKey, \
										pKeyProcedure, iKeyProcedureSize, ofKeyProcedureRVA);
	} else {
		pVmpProcedure = VmpThisProcedure(pProcedure, iProcedureSize, ofProcedureRVA, pChaosVmAth->pVmHash, \
										pChaosVmAth->pVmEncrypt, pChaosVmAth->pVmInstRemainEncrypt, Opcode1Tbl, \
										Opcode2Tbl, ModRmTbl, SibTbl, pChaosVmInvokeThunkCode, iChaosVmInvokeThunkCodeSize, \
										pChaosVmAth->dwKey, FALSE, &dwNextKey, \
										pKeyProcedure, iKeyProcedureSize, ofKeyProcedureRVA);
	}

	// �鿴�����ͷųɹ�
	if (!pVmpProcedure)
		return NULL;

	// �ͷŵ���ͷ��ָ��
	if (pChaosVmInvokeThunkCode) __logic_delete__(pChaosVmInvokeThunkCode);

	// ���û�����Ϣ
	pChaosVmAth->dwKey = dwNextKey;//����KEY
	pChaosVmAth->pChaosVmpProcedure[iIndex] = pVmpProcedure;
	__logic_memcpy__(pVmpProcedure->ModRmTbl, &ModRmTbl, sizeof(__byte) * 3);
	__logic_memcpy__(pVmpProcedure->SibTbl, &SibTbl, sizeof(__byte) * 3);

	// ���ñ�VMP��ĺ������µ���������
	if (pConfigure->Mode == CRM_INFECT) {
		/*
		 * ��Ⱦģʽ
		 */
		__memory pProcedureKeep = NULL;//�������汻���⻯��ĺ���ָ��
		iWriteSize = pVmpProcedure->iSize;//��ȡ���������ĳ���
		__logic_memcpy__(pChaosVmAth->pProcedureWriteTo, pVmpProcedure->pVmpProcedure, iWriteSize);//д�뵽�ڴ�
		addrTargetImageBase = pChaosVmAth->addrTargetImageBase;
		ofOffset = (__offset)(pChaosVmAth->pProcedureWriteTo - pChaosVmAth->pXFileAttachInfo->pMem);//д��λ�õ�ƫ��
		ofRVA = Raw2Rva(pChaosVmAth->pXFileAttachInfo->pMem, ofOffset);
		addrWriteAddress = (__address)(addrTargetImageBase + ofRVA);//Ŀǰû����;,�����Ժ�ʹ��
		//__logic_delete__(pVmpProcedure->pVmpProcedure);//�ͷ�ԭ�ȵ��ڴ�
		pProcedureKeep = pVmpProcedure->pVmpProcedure;//����һ��
		//pVmpProcedure->pVmpProcedure = (__memory)addrWriteAddress;//��д����λ��ת�����ڴ��ַ
		pVmpProcedure->ofVmpProcedureRVA = (__offset)ofRVA;//д��RVA
		pChaosVmAth->pProcedureWriteTo += iWriteSize;
		__logic_memcpy__(&(pRuntime->Procedure), pVmpProcedure, sizeof(CHAOSVMP_PROCEDURE));

		// �趨�ص�����,�����µı����
		ChaosVmAthSetDefRunTables(pChaosVmAth, iIndex);//��ʼ����
		ChaosVmAthMixTables(pChaosVmAth, iIndex, Opcode1Tbl, Opcode2Tbl);

		// ���������ֽ����ļ�
		if (pChaosVmAth->pConfigure->bUseByteCodeFile) {
			PCHAOSVM_EMULATION_BYTECODE_PROCEDURE pByteCodeProcedure = NULL;
			// ���������ĺ���д�뵽bytecode�ļ�
			pByteCodeProcedure = ChaosVmByteCodeFileAddByteCodeProcedureToFile(&(pChaosVmAth->pByteCodeFile), pProcedureKeep, iWriteSize);
			if (!pByteCodeProcedure) {
				// д�뺯����bytecode�ļ�����
			}
		}/* end if */

		(*(pChaosVmAth->piVmpProcedureCount))++;//���ӱ�������������
		__logic_delete__(pProcedureKeep);//�ͷű����⻯��ĺ����ڴ�
	} else if (pConfigure->Mode == CRM_EMULATION) {
		/*
		 * ����ģʽ
		 * �ڷ���ģʽ��,���������ĺ������ڴ����¼һ��ԭʼ�ļ��ܱ���,���������bytecode�ļ�
		 */
		PCHAOSVM_EMULATION_BYTECODE_PROCEDURE pByteCodeProcedure = NULL;
		__dword dwOrigProcedureKey = 0xFBAC1983;

		iWriteSize = pVmpProcedure->iSize;//��ȡ���������ĳ���
		XorArray(dwOrigProcedureKey, pOrigVmpProcedure, pChaosVmAth->pProcedureWriteTo, iWriteSize);//����д�뵽�ڴ�
		pChaosVmAth->pProcedureWriteTo += iWriteSize;
		__logic_delete__(pOrigVmpProcedure);
		pChaosVmAth->pProcedureWriteTo += iWriteSize;
		__logic_memcpy__(&(pRuntime->Procedure), pVmpProcedure, sizeof(CHAOSVMP_PROCEDURE));

		// ���������ĺ���д�뵽bytecode�ļ�
		pByteCodeProcedure = ChaosVmByteCodeFileAddByteCodeProcedureToFile(&(pChaosVmAth->pByteCodeFile), pVmpProcedure->pVmpProcedure, iWriteSize);
		if (!pByteCodeProcedure) {
			// д�뺯����bytecode�ļ�����
		}

		// ���ܱ�������
		XorArray(__CHAOSVM_DECRYPT_OPCODE_TABLE_KEY_EMULATION_MODE__, Opcode1Tbl, pRuntime->Opcode1Tbl, 0x100);
		XorArray(__CHAOSVM_DECRYPT_OPCODE_TABLE_KEY_EMULATION_MODE__, Opcode2Tbl, pRuntime->Opcode2Tbl, 0x100);

	} else if (pConfigure->Mode == CRM_EMULATION_SYS) {
		;
	} else if (pConfigure->Mode == CRM_NETWORK) {
		;
	} else {
		;
	}

	// �趨���л���ʱ����
	pRuntime->iIndex = iIndex;//�趨�˺���������

	return pVmpProcedure;
}

/*
 * ����:
 *	pChaosVmAth:����������������ṹ
 *
 * ����:
 *	������ѡ���ĺ���
 */
__integer __INTERNAL_FUNC__ ChaosVmAthSelected(PCHAOSVMATH pChaosVmAth) {
	__integer i = 0;
	PCHAOSVMATH_CONFIGURE pConfigure = pChaosVmAth->pConfigure;
	__address addrTargetImageBase = pChaosVmAth->addrTargetImageBase;
	__offset ofProcedureRVA = 0;
	__offset ofKeyProcedureRVA = 0;

	for (i = 0; i < pConfigure->iProcedureCount; i++) {
		ofProcedureRVA = (__offset)(pConfigure->ProceduceList[i].addrMemoryAddress - addrTargetImageBase);
		// ���addrKeyMemoryAddress == 0��ʾ���������Կ
		if (pConfigure->ProceduceList[i].addrKeyMemoryAddress == 0) ofKeyProcedureRVA = 0;
		else ofKeyProcedureRVA = (__offset)(pConfigure->ProceduceList[i].addrKeyMemoryAddress - addrTargetImageBase);
		if (!ChaosVmAthProtectIt(
			pChaosVmAth, pConfigure->ProceduceList[i].pAddress, pConfigure->ProceduceList[i].iSize, ofProcedureRVA, \
			pConfigure->ProceduceList[i].pKeyAddress, pConfigure->ProceduceList[i].iKeySize, ofKeyProcedureRVA, i
			)) {
			return 0;
		}
	}
	return i;
}

/*
 * ����:
 *	Ŀ���ļ�·��
 *
 * ����:
 *	����Ŀ���ļ�����
 */
CHAOSVMATH_FILETYPE __API__ ChaosVmAthGetFileType(__tchar *pTargetFilePath) {
	__memory pMem = NULL;
	__integer iTargetFileSize = 0;

	pMem = MappingFile(pTargetFilePath, &iTargetFileSize, FALSE, 0, 0);
	if (!pMem)
		return CFT_NONE;
	
	if (!IsValidPE32(pMem)) {
		UnMappingFile(pMem);
		return CFT_NONE;
	}

	if (IsDriverFile(pMem)) {
		return CFT_SYS;
	} else if (IsDriverFile(pMem)) {
		UnMappingFile(pMem);
		return CFT_DLL;
	} else {
		UnMappingFile(pMem);
		return CFT_EXE;
	}
	
	return CFT_NONE;
}

//__byte __INTERNAL_FUNC__ ChaosVmpAthOpcodeExchange(__byte bOpcode) {
//	return 0;
//}
//
//__byte* __INTERNAL_FUNC__ ChaosVmpAthModRmSibExchage(__byte *bOpcode) {
//	return NULL;
//}
//

// DIS����������
#include "ChaosVmAthDISAthSteps.c"