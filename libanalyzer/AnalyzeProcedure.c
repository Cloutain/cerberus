__integer __INTERNAL_FUNC__ CountProcedureList(PPROCEDURE pProcedure) {
	__integer iCount = 0;
	PPROCEDURE pCurrProcCount = pProcedure;
	while (pCurrProcCount) {
		if (!(pCurrProcCount->bBlock))//���ԷǴ����
			iCount++;
		pCurrProcCount = pCurrProcCount->pNext;
	}
	return iCount;
}

__integer __INTERNAL_FUNC__ CountAllBloackList(PPROCEDURE pProcedure) {
	__integer iCount = 0;
	PPROCEDURE pCurrProcCount = pProcedure;
	while (pCurrProcCount) {
		iCount++;
		pCurrProcCount = pCurrProcCount->pNext;
	}
	return iCount;
}

#define __JmpUpOffsetByAnalyzeJmpEnd__(Offset, InstLen) ((~(Offset) + 1) - (InstLen))
__INLINE__ __memory __INTERNAL_FUNC__ AnalyzeJmpEnd(__memory pMem, __address ImageBase, __address addrCurrMemoryAddress, ud_t *pud_obj) {
	__integer iRva = 0;
	__memory pJmpTo = NULL;

	// ����Ƕ�̬��ת��ֱ���˳�
	if (pud_obj->operand[0].type == UD_OP_MEM)
		return NULL;

	if (pud_obj->operand[0].offset == 8) {
		// 8λ
		__byte bOffset = pud_obj->operand[0].lval.ubyte;
		if (__IsNegative8__(bOffset)) {
			iRva = addrCurrMemoryAddress - __JmpUpOffsetByAnalyzeJmpEnd__(bOffset, pud_obj->inp_ctr) - ImageBase;
			pJmpTo = pMem + AnalyzerRva2Raw(pMem, iRva);
		}
	} else if (pud_obj->operand[0].offset == 16) {
		// 16λ
		__word wOffset = pud_obj->operand[0].lval.uword;
		if (__IsNegative16__(wOffset)) {
			iRva = addrCurrMemoryAddress - __JmpUpOffsetByAnalyzeJmpEnd__(wOffset, pud_obj->inp_ctr) - ImageBase;
			pJmpTo = pMem + AnalyzerRva2Raw(pMem, iRva);
		}
	} else if (pud_obj->operand[0].offset == 32) {
		// 32λ
		__dword dwOffset = pud_obj->operand[0].lval.udword;
		if (__IsNegative32__(dwOffset) == TRUE)
		{
			iRva = addrCurrMemoryAddress - __JmpUpOffsetByAnalyzeJmpEnd__(dwOffset, pud_obj->inp_ctr) - ImageBase;
			pJmpTo = pMem + AnalyzerRva2Raw(pMem, iRva);
		}
	}
	return pJmpTo;
}

/*
 * Ԥ�����β�ĺ����Ľ�β���������
 * 1:��һ��JMPָ��ֱ����ת�������������һ��RETָ���֮ǰ
 * 2:�ٴ�Ѱ�ҵ�ret���
 * 3:����ĩβ����
 */
__INLINE__ __offset __INTERNAL_FUNC__ PredictProcEnd(__memory pMem, __address ImageBase, __memory pStart, \
													 __address addrStartMemoryAddress, __memory pRet, PANALYZE_CONFIGURE pAnalyzeConfigure) {
	__offset ofOffset = 0;
	__memory pCurr = pStart;
	__memory pJmpEnd = NULL;
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, pStart, __MAX_PROCEDURE_LENGTH__);//Ĭ��һ����������󳤶�Ϊ4096
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	while (ud_disassemble(&ud_obj)) {
		pCurr += ofOffset;
		ofOffset += ud_obj.inp_ctr;
		if (ud_obj.mnemonic == UD_Ijmp) {
			/*
			 * �����һ��ָ����JMP������ת��Ŀ���ַ�Ƿ��������retָ����ͬ����С��
			 * ��ô���λ�þ��Ǻ����Ľ�β
			 */
			pJmpEnd = AnalyzeJmpEnd(pMem, ImageBase, addrStartMemoryAddress, &ud_obj);
			if ((!pJmpEnd) || (pJmpEnd <= pRet))
				return ofOffset;
		} else if (ud_obj.mnemonic == UD_Iret) {
			return ofOffset;
		} else if (ud_obj.mnemonic == UD_Inop) {
			if (!(pAnalyzeConfigure->bNopIsNotEnd))
				return ofOffset - ud_obj.inp_ctr;
		} else if (ud_obj.mnemonic == UD_Iint3) {
			if (!(pAnalyzeConfigure->bInt3IsNotEnd))
				return ofOffset - ud_obj.inp_ctr;
		} else if ((ud_obj.mnemonic == UD_Iadd) && (ud_obj.inp_ctr == 2) && (*(__word *)&(ud_obj.inp_sess) == 0)) {
			if (!(pAnalyzeConfigure->bZeroIsNotEnd))
				return ofOffset - ud_obj.inp_ctr;
		} else {
			// �Զ��ĺ���ĩβ������
		}
	}
	return ofOffset;
}

__INLINE__ PINSTRUCTION_JMP __INTERNAL_FUNC__ FindMaxJccTargetMemoryAddress(__address addrCurrMemoryAddress, PX86INSTRUCTION pInstructListHeader) {
	PINSTRUCTION_JMP pFind = NULL;
	PX86INSTRUCTION pCurrent = pInstructListHeader;
	while (pCurrent) {
		// ����ֻ�ж�JCCΪƫ����ת�����
		if ((pCurrent->bJmp) && (pCurrent->InstructionJmp.JmpType == JMP_TYPE_JCC_OFFSET)) {
			// У���Ƿ���������
			if (pCurrent->InstructionJmp.JmpWay == JMP_WAY_DOWN) {
				// У��������ת��ַ�Ƿ���ڵ�ǰָ��ĵ�ַ
				if (pCurrent->InstructionJmp.addrTargetMemoryAddress > addrCurrMemoryAddress) {
					addrCurrMemoryAddress = pCurrent->InstructionJmp.addrTargetMemoryAddress;
					pFind = &(pCurrent->InstructionJmp);
				}
			}
		}
		pCurrent = pCurrent->pNext;
	}/* end while */
	return pFind;
}

__INLINE__ PX86INSTRUCTION __INTERNAL_FUNC__ FindLastRet(PX86INSTRUCTION pInstructListHeader) {
	PX86INSTRUCTION pFind = NULL;
	PX86INSTRUCTION pCurrent = pInstructListHeader;

	while (pCurrent) {
		if (pCurrent->ud_obj.mnemonic == UD_Iret) {
			pFind = pCurrent;
		}
		pCurrent = pCurrent->pNext;
	}/* end while */
	return pFind;
}

__INLINE__ __bool __INTERNAL_FUNC__ IsProcEnd(ud_t *pud_obj, PX86INSTRUCTION pInstructListHeader, PX86INSTRUCTION pInstruction, \
											  __bool *pPredictProcEnd, __integer *piEndSize) {
	__address ImageBase = __GetProgramFromInstruction__(pInstruction)->ImageBase;
	// �鿴��ǰָ���Ƿ���ret, retn�����,������תָ���,�鿴�Ƿ���JCCָ����ת�����
	PANALYZE_CONFIGURE pAnalyzeConfigure = &(__GetProgramFromInstruction__(pInstruction)->AnalyzeConfigure);
	if (pud_obj->mnemonic == UD_Iret) {
		// ������ת��,�ҵ�������ת��ַ,�����������Ԥ���������
		PINSTRUCTION_JMP pMaxJmpEnd = FindMaxJccTargetMemoryAddress(pInstruction->addrCurrMemoryAddress, pInstructListHeader);
		if (pMaxJmpEnd == NULL)
			return TRUE;//û��JCC��ת
		
		if (pMaxJmpEnd->addrTargetMemoryAddress <= pInstruction->addrCurrMemoryAddress)
			return TRUE;//����JCC��ת�����ת����ǰָ��
		else {//Ŀ���ַ�ڵ�ǰret֮��
			__memory pDeepStart = pMaxJmpEnd->pTargetFileAddress;
			__address addrDeepStartMemoryAddress = pMaxJmpEnd->addrTargetMemoryAddress;
			__memory pRet = pInstruction->pCurrFileAddress;
			__memory pMem = __GetProgramFromInstruction__(pInstruction)->pMem;
			__integer iRet = PredictProcEnd(pMem, ImageBase, pDeepStart, addrDeepStartMemoryAddress, pRet, pAnalyzeConfigure);
			/*
			 * jcc lab1
			 * -> ret
			 * ...
			 * ...
			 * lab1:
			 * ...
			 * ...
			 * -> ret <- ��ε�ĩβ
			 * iRet = ��lab1�����ĩβ�ĳ���
			 * �������Ҫ���ϵ�һ��ret��lab1�ľ���
			 */
			*piEndSize = iRet + (__integer)(pDeepStart - pRet);
			*pPredictProcEnd = TRUE;
		}
	} else if (pud_obj->mnemonic == UD_Ijmp) {
		PX86INSTRUCTION pLastRet = NULL;
		__address addrTargetAddress = 0;

		// �����������ת
		if (pInstruction->InstructionJmp.JmpWay == JMP_WAY_UP) {
			/*
			 * lab1:
			 * ...
			 * ...
			 * ret
			 * ...
			 * ...
			 * jmp lab1 <- ������ĩβ
			*/
			pLastRet = FindLastRet(pInstructListHeader);
			if (pLastRet) {
				addrTargetAddress = pInstruction->InstructionJmp.addrTargetMemoryAddress;
				if (addrTargetAddress <= pLastRet->addrCurrMemoryAddress)
					return TRUE;
			}
		}
	} else if (pud_obj->mnemonic == UD_Inop) {
		if (!(pAnalyzeConfigure->bNopIsNotEnd))
			return TRUE;
	} else if (pud_obj->mnemonic == UD_Iint3) {
		if (!(pAnalyzeConfigure->bInt3IsNotEnd))
			return TRUE;
	} else if ((pud_obj->mnemonic == UD_Iadd) && (pud_obj->inp_ctr == 2) && (*(__word *)&(pud_obj->inp_sess) == 0)) {
		if (!(pAnalyzeConfigure->bZeroIsNotEnd))
			return TRUE;
	} else {
		// �Զ��ĺ���ĩβ������
	}

	return FALSE;
}

PPROCEDURE __INTERNAL_FUNC__ InitProcedure(LPVOID pParents)
{
	PPROCEDURE pProcedure = __logic_new__(PROCEDURE, 1);
	if (!(pProcedure)) return NULL;
	ZeroMemory(pProcedure, sizeof(PROCEDURE));
	pProcedure->pParents = pParents;
	return pProcedure;
}

__void __INTERNAL_FUNC__ ReleaseProcedureRefFromList(PPROCEDURE_REFFROM *pRefFormListPoint) {
	PPROCEDURE_REFFROM pCurr = *pRefFormListPoint, pTmp = NULL;
	while (pCurr) {
		pTmp = pCurr->pNext;
		__logic_delete__(pCurr);
		pCurr = pTmp;
	}
	*pRefFormListPoint = NULL;
}

__INLINE__ __void __INTERNAL_FUNC__ ReleaseProcedure(PPROCEDURE *pProcedurePoint) {
	if ((*pProcedurePoint)->pInstruction)
		ReleaseInstructionList(&((*pProcedurePoint)->pInstruction));
	if ((*pProcedurePoint)->pProcedureRefForm)
		ReleaseProcedureRefFromList(&((*pProcedurePoint)->pProcedureRefForm));
	__logic_delete__(*pProcedurePoint);
	*pProcedurePoint = NULL;
}

__void __INTERNAL_FUNC__ ReleaseProcedureList(PPROCEDURE *pProcedureHeaderPoint) {
	PPROCEDURE pCurr = *pProcedureHeaderPoint, pTmp = NULL;
	while (pCurr) {
		pTmp = pCurr->pNext;
		ReleaseProcedure(&pCurr);
		pCurr = pTmp;
	}
	*pProcedureHeaderPoint = NULL;
}

__INLINE__ PINSTRUCTION_JMP __INTERNAL_FUNC__ GetInstructionJmpListHeader(PPROCEDURE pProcedure) {
	PX86INSTRUCTION pInstruction = pProcedure->pInstruction;
	PINSTRUCTION_JMP pInstructionJmpListHeader = NULL, *pCurrInstructionJmpPoint = &pInstructionJmpListHeader;
	while (pInstruction) {
		if (pInstruction->bJmp) {
			*pCurrInstructionJmpPoint = &(pInstruction->InstructionJmp);
			pCurrInstructionJmpPoint = &((*pCurrInstructionJmpPoint)->pNext);
		}
		pInstruction = pInstruction->pNext;
	}

	return pInstructionJmpListHeader;
}

/*
 * ��ȡ��������֡
 * ʹ��IsProcEnd��������ĩβ,�������м�������CALLָ��
 */
__INLINE__ PPROCEDURE __INTERNAL_FUNC__ GetProcFrame(__memory pMem, __memory pStart, __integer iBlockMaxSize, PPROGRAM pParents) {
	PPROCEDURE pProcedure = InitProcedure(pParents);
	PX86INSTRUCTION pInstructionPrev = NULL, *pInstructionPoint = &(pProcedure->pInstruction);
	__address ImageBase = pParents->ImageBase;
	__offset ofOffset = 0;
	__integer iContinueSize = 0, iInstCount = 0;
	__bool bPredictProcEnd = FALSE;//�Ƿ�����Ԥ�⺯����β
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_input_buffer(&ud_obj, pStart, iBlockMaxSize);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);

	/*
	 * Ԥ���β����
	 * ������Ϊ������ָ����ʱ,Ԥ��֪��������ĩβ
	 */
	// ��ȡ�����������
	while (ud_disassemble(&ud_obj)) {
		__memory pCurrent = pStart + ofOffset;

		// �����δָ֪��,���¼��������
		if (ud_obj.mnemonic == UD_Iinvalid)
			(pProcedure->iInvalidOpcodeCount)++;

		*pInstructionPoint = InitInstruction(pProcedure, pInstructionPrev);
		AnalyzeInstructionPass1(&ud_obj, pMem, ImageBase, pCurrent, ofOffset, *pInstructionPoint);
		pInstructionPrev = *pInstructionPoint;

		// �ж��Ƿ񵽴ﺯ��ĩβ,�������
		if (IsProcEnd(&ud_obj, pProcedure->pInstruction, *pInstructionPoint, &bPredictProcEnd, &iContinueSize)) {
			ofOffset += ud_obj.inp_ctr;
			iInstCount++;
			goto _end_while;
		}

		/*
		 * �������������˵����ǰָ��ض���retָ��
		 */
		if (bPredictProcEnd) {
			// �������Ļ�������������Ϊ
			// �ӵ�ǰָ�Ԥ����ĺ�����β
			__memory pRetNextInstruction = (*pInstructionPoint)->pCurrFileAddress + ud_obj.inp_ctr;//�õ�retָ����һ��ָ���λ��
			ofOffset += ud_obj.inp_ctr;//����ǰָ��ĳ��ȼ���ƫ��ֵ��
			ud_set_input_buffer(&ud_obj, pRetNextInstruction, (size_t)iContinueSize);//ud_obj�����³�ʼ��
			pInstructionPoint = &((*pInstructionPoint)->pNext);
			iInstCount++;
			bPredictProcEnd = FALSE;
			continue;//ֱ�ӽ�����һ��
		}

		// �ƶ�ָ��
		pInstructionPoint = &((*pInstructionPoint)->pNext);
		ofOffset += ud_obj.inp_ctr;
		iInstCount++;
	}/* end while */
_end_while:

	// ��亯��������Ϣ
	pProcedure->addrMemoryStartAddress = pParents->ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pStart - pMem));
	pProcedure->pFileStartAddress = pStart;
	pProcedure->iSize = (__integer)ofOffset;
	pProcedure->iInstCount = iInstCount;
	pProcedure->pInstructionJmp = GetInstructionJmpListHeader(pProcedure);
	return pProcedure;
}

///*
// * ����һ�����������ݵ���һ������
// * ����pTo
// */
//__INLINE__ PPROCEDURE __INTERNAL_FUNC__ CopyProcedureData(PPROCEDURE pTo, PPROCEDURE pFrom) {
//	pTo->addrAllocStackMemoryAddress = pFrom->addrAllocStackMemoryAddress;
//	pTo->addrMemoryStartAddress = pFrom->addrMemoryStartAddress;
//	pTo->AllocStackType = pFrom->AllocStackType;
//	pTo->bBlock = pFrom->bBlock;
//	pTo->bEntry = pFrom->bEntry;
//	pTo->iInstCount = pFrom->iInstCount;
//	pTo->iInvalidOpcodeCount = pFrom->iInvalidOpcodeCount;
//	pTo->iSize = pFrom->iSize;
//	pTo->iStackSize = pFrom->iStackSize;
//	pTo->pAllocStackFileAddress = pFrom->pAllocStackFileAddress;
//	pTo->pFileStartAddress = pFrom->pFileStartAddress;
//	pTo->pInstruction = pFrom->pInstruction;
//	pTo->pInstructionJmp = pFrom->pInstructionJmp;
//	pTo->pParents = pFrom->pParents;
//	pTo->pProcedureRefForm = pFrom->pProcedureRefForm;
//	pTo->pNext = pFrom->pNext;
//	return pTo;
//}

/*
 * ��������
 * �ӵ͵�ַ���ߵ�ַ��˳��
 * ʹ����ĭ����
 */
//#define __CopyProcedureData__(pTo, pFrom){\
//	PPROCEDURE __pToLink__ = (pTo)->pNext;\
//	memcpy((pTo), (pFrom), sizeof(PROCEDURE));\
//	(pTo)->pNext = __pToLink__;}
__INLINE__ PPROCEDURE __INTERNAL_FUNC__ SortProcedureList(PPROCEDURE pProcedureHeader) {
	PPROCEDURE p = NULL, q = NULL, t = NULL;
	__bool b = FALSE;

	// ���ֻ��һ���ڵ�,ֱ���˳�
	if (!(pProcedureHeader->pNext))
		return pProcedureHeader;
	
	q = p = pProcedureHeader;
	while (1) {

		// ÿ���µ�ѭ�����������ñ�־
		b = FALSE;

		// ��������p��ֵ�����
		p = q;

		// ÿ����һ�εĽڵ㶼����ΪNULL
		t = NULL;

		while (p) {
			// ��һ���ڵ㲻Ϊ��
			if (p->pNext) {

				// ǰһ���ڵ�ĵ�ַ ���� ��һ���ڵ�ĵ�ַ
				if (p->addrMemoryStartAddress > p->pNext->addrMemoryStartAddress) {
					PPROCEDURE m = NULL, n = NULL;

					m = p;
					n = p->pNext;

					if (t) t->pNext = n;//�����һ���ڵ㲻ΪNULL

					m->pNext = n->pNext;
					n->pNext = m;

					p = n;//����һ���ڵ�,����������p = p->pNextִ�к�p�������һ���ڵ�

					// ���ñ�־,��ʾ��һ������δ���
					b = TRUE;
				}
			}

			// ���t��NULL,���ʾ��¼ͷ�ڵ�
			if (!t) q = p;

			t = p;//��¼��һ���ڵ�
			p = p->pNext;//�ƶ�����һ���ڵ�
		}/* end while */
		
		// ÿ��ѭ����϶���������Ƿ����,�˳�ѭ��
		if (!b) break;
	}

	return q;
}

__INLINE__ __integer __INTERNAL_FUNC__ FindInlineProcedure(PPROCEDURE pProcedureListHeader) {
	__integer iCount = 0;
	PPROCEDURE pCheckProcedure = pProcedureListHeader;
	while (pCheckProcedure) {
		if (pCheckProcedure->pNext) {
			if (pCheckProcedure->addrMemoryStartAddress + pCheckProcedure->iSize > pCheckProcedure->pNext->addrMemoryStartAddress) {
				pCheckProcedure->pNext->bInline = TRUE;
				pCheckProcedure->pNext->pFatherProcedure = pCheckProcedure;
			}
		}
		pCheckProcedure = pCheckProcedure->pNext;
	}
}

__INLINE__ __bool __INTERNAL_FUNC__ SetProcToReferenceFromList(PPROCEDURE pProcedure, PPROCEDURE pRefFromProc) {
	PPROCEDURE_REFFROM *pCurr = &(pProcedure->pProcedureRefForm);
	if (!(*pCurr)) {
		pProcedure->pProcedureRefForm = __logic_new__(PROCEDURE_REFFROM, 1);
		pProcedure->pProcedureRefForm->pProcedure = (__void *)pRefFromProc;
		pProcedure->pProcedureRefForm->pNext = NULL;
		return FALSE;
	} else {
		// Ѱ�ҿ��Ƿ�������
		while (*pCurr) {
			// ������ֱ�ӷ���
			if ((*pCurr)->pProcedure == pRefFromProc)
				return TRUE;
			pCurr = &((*pCurr)->pNext);
		}
	}
	
	// ����
	*pCurr = __logic_new__(PROCEDURE_REFFROM, 1);
	(*pCurr)->pProcedure = pRefFromProc;
	(*pCurr)->pNext = NULL;
	return FALSE;
}

__INLINE__ PPROCEDURE __INTERNAL_FUNC__ IsInProcedureList(PPROCEDURE pProcedureList, __address addrGotoAddress) {
	PPROCEDURE pCurr = pProcedureList;
	while (pCurr) {
		__address addrEndAddress = pCurr->addrMemoryStartAddress + pCurr->iSize;
		if ((addrGotoAddress >= pCurr->addrMemoryStartAddress) && (addrGotoAddress < addrEndAddress))
			return pCurr;
		pCurr = pCurr->pNext;
	}
	return NULL;
}

/*
 * ͨ���Ѿ���������ĺ���,�������Ե�ǰ����������
 * ͨ�������е���CALL��ת������ʶ��
 * �������������ļ���
 */
#define __GotoOutProcedure__(addrGotoMemoryAddress)	(((addrGotoMemoryAddress) < addrMemoryStart) || (addrGotoMemoryAddress >= addrMemoryEnd))
__void __INTERNAL_FUNC__ Procedure2Procedure(__memory pMem, PPROCEDURE pProcedureListHeader, PPROCEDURE pProcedure) {
	PINSTRUCTION_JMP pInstructionJmp = pProcedure->pInstructionJmp;
	__address addrMemoryStart = pProcedure->addrMemoryStartAddress;
	__address addrMemoryEnd = addrMemoryStart + pProcedure->iSize;
	PPROGRAM pParents = __GetProcedureParents__(pProcedure);
	PPROCEDURE *pMainProcedurePointEnd = &pProcedureListHeader;//������ĩβָ��
	while (*pMainProcedurePointEnd) pMainProcedurePointEnd = &((*pMainProcedurePointEnd)->pNext);//�ƶ�������ĩβ
	// ���Ҫɨ��ĺ����Ƿ�������������,����������������������ӵ�ĩβ
	if (!IsInProcedureList(pProcedureListHeader, addrMemoryStart)) {
		*pMainProcedurePointEnd = pProcedure;
		pMainProcedurePointEnd = &((*pMainProcedurePointEnd)->pNext);
	}
	
	// ����ָ����,�������CALLָ��,������µĺ�������
	while (pInstructionJmp) {
		if ((pInstructionJmp->JmpType == JMP_TYPE_CALL_OFFSET) || (pInstructionJmp->JmpType == JMP_TYPE_JMP_OFFSET)) {
			__address addrTargetMemoryAddress = 0;
			// ������תĿ���Ƿ���Ч
			if (pInstructionJmp->bTargetAddressIsInvalid)
				goto _continue;
			addrTargetMemoryAddress = pInstructionJmp->addrTargetMemoryAddress;//��ȡĿ����ڴ��ַ
			// ���������������
			if (__GotoOutProcedure__(addrTargetMemoryAddress)) {
				__memory pProcStart = pInstructionJmp->pTargetFileAddress;
				PPROCEDURE pNewProcedure = NULL;
				PPROCEDURE pTargetProcedure = IsInProcedureList(pProcedureListHeader, addrTargetMemoryAddress);
				pInstructionJmp->bJmpOut = TRUE;
				// ���Ŀ�꺯���Ѿ��õ��������Թ�
				if (pTargetProcedure) {
					// �����ת������������
					SetProcToReferenceFromList(pTargetProcedure, pProcedure);
					goto _continue;
				}

				pNewProcedure = GetProcFrame(pMem, pProcStart, __MAX_PROCEDURE_LENGTH__, pParents);
				if (!pNewProcedure)
					goto _continue;
				
				// �趨�µĺ�����ת����
				SetProcToReferenceFromList(pNewProcedure, pProcedure);
				Procedure2Procedure(pMem, pProcedureListHeader, pNewProcedure);
			}
		}/* end if */
_continue:
		pInstructionJmp = pInstructionJmp->pNext;
	}
	return;
}

PPROCEDURE __INTERNAL_FUNC__ FindEntryProcedure(PPROCEDURE pProcedureList) {
	PPROCEDURE pCurr = pProcedureList;
	while (pCurr) {
		if (pCurr->bEntry)
			return pCurr;
		pCurr = pCurr->pNext;
	}
	return NULL;
}

/*
 * ʹ��JMPָ����м���
 * ����Ǵ�����������TRUE,��֮����FALSE
 */
__INLINE__ __bool __INTERNAL_FUNC__ AbyssAnalyze(__memory pMem, __address ImageBase, __memory pCurr, __integer iSize) {
	__address addrStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pCurr - pMem));
	__offset ofOffset = 0;
	PX86INSTRUCTION pInstruction = NULL, pPrevInstruction = NULL, *pCurrInstructionPoint = &pInstruction;
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_input_buffer(&ud_obj, pCurr, iSize); 

	// ʹ�õ�һ��ָ����ǲ�������з���
	while (ud_disassemble(&ud_obj)) {
		__address addrCurrentAddress = addrStartAddress + ofOffset;
		*pCurrInstructionPoint = __logic_new__(X86INSTRUCTION, 1);
		__logic_memset__((*pCurrInstructionPoint), 0, sizeof(X86INSTRUCTION));
		(*pCurrInstructionPoint)->addrCurrMemoryAddress = addrCurrentAddress;
		(*pCurrInstructionPoint)->addrMemoryBlockStart = addrStartAddress;
		(*pCurrInstructionPoint)->iBlockSize = iSize;
		(*pCurrInstructionPoint)->pMem = pMem;
		__logic_memcpy__(&((*pCurrInstructionPoint)->ud_obj), &ud_obj, sizeof(ud_t));
		(*pCurrInstructionPoint)->pPrev = pPrevInstruction;//�����ϲ�ָ��
		(*pCurrInstructionPoint)->pNext = NULL;

		// ִ�з���
		InstructionDispatcher(g_pAnalyzeDataDispatcher, *pCurrInstructionPoint);
		if ((*pCurrInstructionPoint)->bErrorLogicInstruction) {//��������߼�����ָ����ֱ���˳�
			ReleaseInstructionList(&pInstruction);
			return TRUE;
		}

		// ��������
		if (pPrevInstruction)
			pPrevInstruction->pNext = (*pCurrInstructionPoint);

		pPrevInstruction = (*pCurrInstructionPoint);
		pCurrInstructionPoint = &((*pCurrInstructionPoint)->pNext);//��һ���ڵ�
		ofOffset += (__integer)ud_insn_len(&ud_obj);
	}/* end while */

	/*
	 * ���û�д���ָ��,������������ķ�������
	 */

	// �ͷ�ָ������
	ReleaseInstructionList(&pInstruction);
	return FALSE;
}

/*
 * ����Ǵ���������ֱ�ӷ���TRUE,��֮����FALSE
 */
__INLINE__ __bool __INTERNAL_FUNC__ DeepAnalyzeBlock(__memory pMem, __address ImageBase, __memory pCurr, __integer iSize, PANALYZE_CONFIGURE pAnalyzeConfigure) {
	__bool bBlock = FALSE;
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_input_buffer(&ud_obj, pCurr, iSize);

	while (ud_disassemble(&ud_obj)) {
		enum ud_mnemonic_code mnemonic = ud_obj.mnemonic;
		if ((mnemonic == UD_Iinvalid) && (!(pAnalyzeConfigure->bDisInvalidOpcodeIsData))) {
			bBlock = TRUE;
			return bBlock;
		}
	}

	/*
	 * ������ȷ���
	 */
	bBlock = AbyssAnalyze(pMem, ImageBase, pCurr, iSize);

	return bBlock;
}

/*
 * Ԥ�������׶�,�ڷ���������ʱ����,�������������
 * ��Χ�Ľ綨,���������򷵻�TRUE,��֮ΪFALSE
 */
__INLINE__ __bool __INTERNAL_FUNC__ PredictBlockEnd(__memory pMem, __address ImageBase, __memory pCurr, __integer iSize, \
													__integer *piOutSize, PANALYZE_CONFIGURE pAnalyzeConfigure) {
	__bool bBlock = FALSE;
	__offset ofOffset = 0;
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_input_buffer(&ud_obj, pCurr, iSize);
	while (ud_disassemble(&ud_obj)) {
		enum ud_mnemonic_code mnemonic = ud_obj.mnemonic;
		if ((mnemonic == UD_Inop) || \
			(mnemonic == UD_Iint3) || \
			((mnemonic == UD_Iadd) && (ud_obj.inp_ctr == 2) && (*(__word *)&(ud_obj.inp_sess) == 0))) {
			/*
			 * �����������
			 * ����Ƿ񵽴����û�����������С��Χ,���û��ֱ����Ϊ����
			 * ������ڵ���������������
			 */
			if (ofOffset < pAnalyzeConfigure->bCodeMixSize)
				bBlock = TRUE;
			else
				// ������ȷ���
				bBlock = DeepAnalyzeBlock(pMem, ImageBase, pCurr, ofOffset, pAnalyzeConfigure);
			*piOutSize = (__integer)ofOffset;
			return bBlock;
		}/* end if */
		ofOffset += (__integer)ud_insn_len(&ud_obj);
	}

	// ��������ȼ���
	bBlock = DeepAnalyzeBlock(pMem, ImageBase, pCurr, iSize, pAnalyzeConfigure);
	*piOutSize = (__integer)ofOffset;
	return bBlock;
}

/*
 * ���з�������
 * ��NOP,INT3,0,��Чָ������ַ����з�����־
 * ����NOP����INT3,0,��Чָ�������ɨ���������
 * ��Ϊһ������ժ��,������ѯ,������������кܳ�
 * һ��Ϊ������ NOP,INT3,0�ֽ��򽫴�����������Ч����,
 * ��ɨ�������,���������Чָ��ֱ�Ӻ��Ե����������
 * ��һ�������������Чָ��,�����Ԥ�����׶�,������ǰ
 * �������������ָ���С����С���볤����ֱ�Ӻ���.���
 * �����򽫴������������,������������.ֱ�����
 */
__INLINE__ PPROCEDURE __INTERNAL_FUNC__ FormatBlock(__memory pMem, __address ImageBase, __memory pStart, __integer iSize, PANALYZE_CONFIGURE pAnalyzeConfigure) {
	__integer iBlockEndSignCount = 0;
	__offset ofOffset = 0;
	PPROCEDURE pBlock = NULL;
	PPROCEDURE *pCurrBlockPoint = &pBlock;
	ud_t ud_obj;
	ud_init(&ud_obj);
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	ud_set_input_buffer(&ud_obj, pStart, iSize);

	while (ud_disassemble(&ud_obj)) {
		enum ud_mnemonic_code mnemonic = ud_obj.mnemonic;
		if ((mnemonic == UD_Inop) || \
			(mnemonic == UD_Iint3) || \
			((mnemonic == UD_Iadd) && (ud_obj.inp_ctr == 2) && (*(__word *)&(ud_obj.inp_sess) == 0)))
			iBlockEndSignCount += (__integer)ud_insn_len(&ud_obj);
		else {
			/*
			 * ������Чָ����
			 * ���ڸý���Ԥ�������׶�,���ﻹ��һ���ж�
			 * ��־��ʣ�೤��,������ĳ���С����С����
			 * ����ֱ�ӽ���δ�������Ϊ����
			 */
			__integer iRemainSize = iSize - (__integer)ofOffset;
			__memory pCurr = pStart + (__integer)ofOffset;
			if (iRemainSize >= pAnalyzeConfigure->bCodeMixSize) {
				/*
				 * ����Ԥ�������׶�
				 * �����￪ʼ�ж�,���Ԥ����������Ľ��Ϊ���������ݿ���
				 * ����ǰ������������кϲ�
				 * ���򵥶�Ϊǰ������Ŀ�����ڴ�
				 */
				__integer iOutSize = 0;
				if (PredictBlockEnd(pMem, ImageBase, pCurr, iRemainSize, &iOutSize, pAnalyzeConfigure))
					iBlockEndSignCount += iOutSize;//���кϲ�
				else {
					/*
					 * �ж���һ���������Ƿ�Ϊ0�����Ϊ�����ռ�洢��
					 */
					if (iBlockEndSignCount) {
						(*pCurrBlockPoint) = __logic_new__(PROCEDURE, 1);
						__logic_memset__((*pCurrBlockPoint), 0, sizeof(PROCEDURE));
						(*pCurrBlockPoint)->bBlock = TRUE;//��������
						(*pCurrBlockPoint)->pFileStartAddress = pCurr - iBlockEndSignCount;
						(*pCurrBlockPoint)->addrMemoryStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)((*pCurrBlockPoint)->pFileStartAddress - pMem));
						(*pCurrBlockPoint)->iSize = iBlockEndSignCount;
						pCurrBlockPoint = &((*pCurrBlockPoint)->pNext);
						iBlockEndSignCount = 0;//��ձ�־��¼����
					}
					// �趨�˶�Ϊ��������
					(*pCurrBlockPoint) = __logic_new__(PROCEDURE, 1);
					__logic_memset__((*pCurrBlockPoint), 0, sizeof(PROCEDURE));
					(*pCurrBlockPoint)->bBlock = FALSE;//��������
					(*pCurrBlockPoint)->pFileStartAddress = pCurr;
					(*pCurrBlockPoint)->addrMemoryStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pCurr - pMem));
					(*pCurrBlockPoint)->iSize = iOutSize;
					pCurrBlockPoint = &((*pCurrBlockPoint)->pNext);
				}

				// �����趨�����ָ��
				pCurr += iOutSize;
				(__integer)ofOffset += iOutSize;
				iRemainSize -= iOutSize;//ʣ�೤��
				ud_set_input_buffer(&ud_obj, pCurr, iRemainSize);
				continue;
			} else {
				// ���뵽������ǽ��뵽������ĩβ
				(*pCurrBlockPoint) = __logic_new__(PROCEDURE, 1);
				__logic_memset__((*pCurrBlockPoint), 0, sizeof(PROCEDURE));
				(*pCurrBlockPoint)->bBlock = TRUE;//��ʾ�п���������
				iBlockEndSignCount += iRemainSize;
				(*pCurrBlockPoint)->iSize = iBlockEndSignCount;
				(*pCurrBlockPoint)->pFileStartAddress = pCurr;
				(*pCurrBlockPoint)->addrMemoryStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pCurr - pMem));
				pCurrBlockPoint = &((*pCurrBlockPoint)->pNext);
				iBlockEndSignCount = 0;//��ձ�־��¼����
			}
		}
		(__integer)ofOffset += ud_insn_len(&ud_obj);
	}

	/*
	 * �������һ���������
	 * ������������������
	 * 1) �������������ݶ��Ǵ�������
	 * 2) ���������������һ�������Ǵ�������
	 */
	if (iBlockEndSignCount) {
		__memory pBlockZoon = NULL;
		// ��һ�����
		if (iBlockEndSignCount == iSize)
			pBlockZoon = pStart;
		else
			pBlockZoon = pStart + iSize - iBlockEndSignCount;
		(*pCurrBlockPoint) = __logic_new__(PROCEDURE, 1);
		__logic_memset__((*pCurrBlockPoint), 0, sizeof(PROCEDURE));
		(*pCurrBlockPoint)->bBlock = TRUE;//��ʾ�п���������
		(*pCurrBlockPoint)->iSize = iBlockEndSignCount;
		(*pCurrBlockPoint)->pFileStartAddress = pBlockZoon;
		(*pCurrBlockPoint)->addrMemoryStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pBlockZoon - pMem));
		pCurrBlockPoint = &((*pCurrBlockPoint)->pNext);
		iBlockEndSignCount = 0;//��ձ�־��¼����
	}

	return pBlock;
}

/*
 * ����Ԥ����,ʶ�������
 * �����������������,����д���ʶ��
 */	
__INLINE__ PPROCEDURE __INTERNAL_FUNC__ AnalyzeData(__memory pMem, __address ImageBase, __memory pCurr, __integer iSize, PPROCEDURE pProcedureList, PPROGRAM pParents) {
	PPROCEDURE pFormatBlockList = NULL, pCurrBlock = NULL;
	PANALYZE_CONFIGURE pAnalyzeConfigure = &(pParents->AnalyzeConfigure);
	// ����˿�ĳ���С���û�ָ������
	if (iSize < pAnalyzeConfigure->bCodeMixSize) {
		PPROCEDURE *pCurrMainPoint = &pProcedureList;
		PPROCEDURE pBlock = __logic_new__(PROCEDURE, 1);
		__logic_memset__(pBlock, 0, sizeof(PROCEDURE));
		pBlock->bBlock = TRUE;
		pBlock->pFileStartAddress = pCurr;
		pBlock->addrMemoryStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pCurr - pMem));
		pBlock->iSize = iSize;
		pBlock->pNext = NULL;

		while (*pCurrMainPoint) pCurrMainPoint = &((*pCurrMainPoint)->pNext);
		*pCurrMainPoint = pBlock;
		return pProcedureList;
	}

	/*
	 * ��ʼ������������Ч�ķ���
	 * ������Ϻ�����еĴ������к���֡����
	 * ���η���
	 */
	pFormatBlockList = FormatBlock(pMem, ImageBase, pCurr, iSize, pAnalyzeConfigure);

	/*
	 * ������������з���
	 * ����������������벢��ʼ�������
	 */
	pCurrBlock = pFormatBlockList;
	while (pCurrBlock) {
		if (!(pCurrBlock->bBlock)) {//Ϊ�����
			// ���з���
			PPROCEDURE pProcedure = NULL;

			// ��ȡ����֡
			pProcedure = GetProcFrame(pMem, pCurrBlock->pFileStartAddress, pCurrBlock->iSize, pParents);

			/*
			 * ����Procedure2Procedure�����γɵĺ������д������ֺ���,һ���Ǻ������������е�
			 * δ֪������,һ����������һ��ɨ��ʱ�������ĺ���
			 */
			Procedure2Procedure(pMem, pProcedureList, pProcedure);
		} else {
			// ֱ�����ӵ�����
			PPROCEDURE *pCurrMainPoint = &pProcedureList;
			while (*pCurrMainPoint) pCurrMainPoint = &((*pCurrMainPoint)->pNext);
			*pCurrMainPoint = __logic_new__(PROCEDURE, 1);
			__logic_memcpy__(*pCurrMainPoint, pCurrBlock, sizeof(PROCEDURE));
			(*pCurrMainPoint)->pNext = NULL;
		}
		pCurrBlock = pCurrBlock->pNext;
	}

	// ���ٷ�����
	ReleaseProcedureList(&pFormatBlockList);
	return pProcedureList;
}

/*
 * �ڴ�����
 */
typedef struct _CODE_BLOCK {
	__memory pPoint;//ָ��
	__integer iSize;//����
} CODE_BLOCK, *PCODE_BLOCK;

/*
 * ��Ѱ���в������ϵ��ڴ�����,����ᵽ����
 */
__INLINE__ PPROCEDURE __INTERNAL_FUNC__ AnalyzeProcedurePass1DiscoveryUnknowZoon(__memory pMem, PPROCEDURE pProcedureList, PPROGRAM pParents) {
	__integer iCodeSize = pParents->iCodeSegSize;
	__integer iCurrSize = 0, iBlockSize = 0, iRemainSize = iCodeSize;
	__memory pNow = NULL;
	__memory pStart = pParents->pCodeSeg;
	__memory pCurr = pStart;
	__address ImageBase = pParents->ImageBase;
	PPROCEDURE pCurrProcedure = pProcedureList, pUnknowProcedureList = NULL;

	// ����ָ��������ɺ���
	g_pAnalyzeDataDispatcher = MakeAnalyzeDataFromInstructionDispatcher();

_new_analyze:
	while (pCurrProcedure) {
		pNow = pCurrProcedure->pFileStartAddress;
		iCurrSize = pCurrProcedure->iSize;

		if (pCurr == pNow) {//����൱��ֱ���Թ�
			pCurr += iCurrSize;
			iRemainSize -= iCurrSize;
		} else if (pCurr < pNow) {//������������
			iBlockSize = (__integer)(pNow - pCurr);
			iRemainSize -= iBlockSize;
			goto _handler;
		} else {//pCurr > pNow
			// ��������
		}
		pCurrProcedure = pCurrProcedure->pNext;
	}

	// �ж��Ƿ񵽴�ĩβ
	if (iRemainSize) {
		iBlockSize = iRemainSize;
		goto _handler;
	}

	// �ͷ���ǲ��
	DestroyDispatcherList(&g_pAnalyzeDataDispatcher);
	return pProcedureList;

// ����Ϊ����������
_handler:
	/*
	 * ����Ĵ����ǰһ�δ����и�������ǲ�֪����ǰ���������
	 * ���ݻ��Ǵ���,��������Ҫ�����ݼ���
	 */
	pUnknowProcedureList = AnalyzeData(pMem, ImageBase, pCurr, iBlockSize, pProcedureList, pParents);
	pProcedureList = SortProcedureList(pProcedureList);//����

	// �����趨���г�ʼֵ,Ȼ����������
	pCurr = pStart;
	iRemainSize = iCodeSize;//�����趨����
	pUnknowProcedureList = NULL;
	pCurrProcedure = pProcedureList;
	__PrintProcedureListCount__(pProcedureList);
	goto _new_analyze;//���½����µ�һ�η���
}

/*
 * �����ں������ϵ�����ֱ����Ϊ���ݲ����ӵ�����
 */
__INLINE__ PPROCEDURE __INTERNAL_FUNC__ LinkUnknowZoonToProcedureListAsBlock(__memory pMem, PPROCEDURE pProcedureList, PPROGRAM pParents) {
	__integer iCurrSize = 0, iBlockSize = 0;
	__address ImageBase = pParents->ImageBase;
	PPROCEDURE pCurrProcedure = pProcedureList, pPrevProcedure = NULL;
	__memory pCurr = pParents->pCodeSeg;
	__memory pNow = pCurrProcedure->pFileStartAddress;

	while (pCurrProcedure) {
		pNow = pCurrProcedure->pFileStartAddress;
		iCurrSize = pCurrProcedure->iSize;

		if (pCurr == pNow) {//����൱��ֱ���Թ�
			pCurr += iCurrSize;
		} else if (pCurr < pNow) {
			PPROCEDURE pCurrBlockPoint = NULL;

			pCurrBlockPoint = __logic_new__(PROCEDURE, 1);
			__logic_memset__(pCurrBlockPoint, 0, sizeof(PROCEDURE));
			pCurrBlockPoint->bBlock = TRUE;//��������
			pCurrBlockPoint->pFileStartAddress = pCurr;
			pCurrBlockPoint->addrMemoryStartAddress = ImageBase + AnalyzerRaw2Rva(pMem, (__integer)(pCurr - pMem));
			iBlockSize = (__integer)(pNow - pCurr);//��Ĵ�С
			pCurrBlockPoint->iSize = iBlockSize;
			pCurrBlockPoint->pNext = NULL;

			// ���뵽���ʵ�λ��
			if (pPrevProcedure) pPrevProcedure->pNext = pCurrBlockPoint;
			else pProcedureList = pCurrBlockPoint;
			pCurrBlockPoint->pNext = pCurrProcedure;

			// pCurrָ���ƶ�
			pCurr = pNow + iCurrSize;
		} else {//pCurr > pNow
			// ����INLINE����,ֱ������
			pCurr = pPrevProcedure->pFileStartAddress + pPrevProcedure->iSize;
		}

		pPrevProcedure = pCurrProcedure;//��¼��һ������
		pCurrProcedure = pCurrProcedure->pNext;
	}/* end while */

	pCurrProcedure = SortProcedureList(pProcedureList);
	return pCurrProcedure;
}

/*
 * ��һ�α�������
 */
PPROCEDURE __INTERNAL_FUNC__ AnalyzeProcedurePass1(PPROGRAM pParents) {
	__memory pMem = pParents->pMem;
	__integer iSize = 0;
	PPROCEDURE pProcedureHeader = NULL, pEnteryProcedure = NULL;
	__integer iEntryRva = pParents->iEntryRva;
	__memory pStart = pMem + AnalyzerRva2Raw(pMem, iEntryRva);
	__integer iInlineProcCount = 0;
	__integer iCodeSize = pParents->iCodeSegSize;
	__integer iCodeSegRaw = pParents->pCodeSeg - pMem;
	__integer iBlockMaxSize = iCodeSize - (iEntryRva - AnalyzerRaw2Rva(pMem, iCodeSegRaw));

	// ������ǲ��
	g_pPass1Dispatcher = MakeAnalyzeInstructionPass1Dispatcher();

	// ���ȼ������ں���
	pEnteryProcedure = GetProcFrame(pMem, pStart, iBlockMaxSize, pParents);
	if (!pEnteryProcedure)
		return NULL;
	pEnteryProcedure->bEntry = TRUE;

	/*
	 * ͨ����ں����������������
	 * ���ڴ˺���Ҫ���еݹ�, �״ε�����ͷΪ�Լ�
	 */
	Procedure2Procedure(pMem, pEnteryProcedure, pEnteryProcedure);
	__PrintProcedureListCount__(pEnteryProcedure);

	pProcedureHeader = SortProcedureList(pEnteryProcedure);//�Ժ���֡�����������
	__PrintProcedureListCount__(pProcedureHeader);
	__CheckProcedureList1__(pProcedureHeader);
	__CheckProcedureList2__(pProcedureHeader);

	/*
	 * Ѱ����������
	 */
	iInlineProcCount = FindInlineProcedure(pProcedureHeader);
	pParents->iInlineProcCount = iInlineProcCount;

	/*
	 * ���������δ֪����̽��,���δ�����ϵ����������ݴ���
	 */
	if (pParents->AnalyzeConfigure.bEnableDiscoveryUnknowZoon) {
		// ����δ֪����̽��
		pProcedureHeader = AnalyzeProcedurePass1DiscoveryUnknowZoon(pMem, pProcedureHeader, pParents);
		__PrintProcedureListCount__(pProcedureHeader);
	} else {
		// ������δ֪����̽��
		pProcedureHeader = LinkUnknowZoonToProcedureListAsBlock(pMem, pProcedureHeader, pParents);
		__PrintProcedureListCount__(pProcedureHeader);
	}

	// ���������������ͼ��,�ڵ���ģʽ��ʹ��
	__CheckSameAddressProcedure__(pProcedureHeader);
	__CheckInstructionsParents__(pProcedureHeader);
	__CheckProcedureList1__(pProcedureHeader);
	__CheckProcedureList2__(pProcedureHeader);

	DestroyDispatcherList(&g_pPass1Dispatcher);//�ͷ���ǲ��
	return pProcedureHeader;
}

