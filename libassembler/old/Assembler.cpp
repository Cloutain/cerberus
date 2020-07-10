// ��������
PASM_TOKEN g_pTokenList = NULL;
DWORD *g_pConstList = NULL;
DWORD g_dwConstIndex = 0;
DWORD g_dwTokenIndex = 0;
BOOL g_bCompilingError = FALSE;

ASM_TOKEN CHARM_INTERNAL_FUNC NextToken()
{
	if (g_pTokenList == NULL)
	{
		g_bCompilingError = TRUE;
		return ASMER_TK_ERROR;
	}
	else if (g_pTokenList[g_dwTokenIndex] == ASMER_TK_END)
	{
		g_pTokenList = NULL;
		g_pConstList = NULL;
		g_dwTokenIndex = 0;
		g_dwConstIndex = 0;
		g_bCompilingError = FALSE;
		return ASMER_TK_END;
	}

	return g_pTokenList[g_dwTokenIndex++];
}

DWORD CHARM_INTERNAL_FUNC NextConst()
{
	if (g_pConstList == NULL)
	{
		return 0;
	}
	return g_pConstList[g_dwConstIndex++];
}

VOID CHARM_INTERNAL_FUNC ResetConst(DWORD dwNewConst)
{
	if (g_pConstList == NULL)
	{
		return;
	}
	g_pConstList[g_dwConstIndex] = dwNewConst;
}

INLINE VOID CHARM_INTERNAL_FUNC BackConst()
{
	g_dwConstIndex--;
}

INLINE VOID CHARM_INTERNAL_FUNC BackToken()
{
	g_dwTokenIndex--;
}

INLINE DWORD CHARM_INTERNAL_FUNC GetNowConstIndex()
{
	return g_dwConstIndex;
}

INLINE DWORD CHARM_INTERNAL_FUNC GetNowTokenIndex()
{
	return g_dwTokenIndex;
}

BOOL CHARM_INTERNAL_FUNC MatchToken(ASM_TOKEN Token)
{
	if (NextToken() != Token)
	{
		g_bCompilingError = TRUE;
		return FALSE;
	}

	return TRUE;
}

/*
 * !ע:�ļ�λ�ò��ܸĶ�
 */
#include "AssemblerSymbolTable.cpp"
#include "AssemblerCompiling.cpp"
#include "AssemblerLinking.cpp"

VOID CHARM_INTERNAL_FUNC ReleaseAsmObjList(PASM_OBJ *pAsmObjListPoint)
{
	PASM_OBJ pCurr = *pAsmObjListPoint, pTmp = NULL;
	while (pCurr != NULL)
	{
		pTmp = pCurr->pNext;
		__delete__(pCurr);
		pCurr = pTmp;
	}

	*pAsmObjListPoint = NULL;
}

INLINE PASM_OBJ CHARM_INTERNAL_FUNC Compiling(PASM_SOURCE pAsmSource)
{
	g_pTokenList = pAsmSource->pAsmTokenList;
	g_pConstList = pAsmSource->pConstList;
	g_dwTokenIndex = g_dwConstIndex = g_dwSymbolIndex = 0;
	g_bCompilingError = FALSE;

	PASM_OBJ pAsmObjList = NULL, *pAsmObjPoint = &pAsmObjList;

	while (TRUE)
	{
		*pAsmObjPoint = AnalyzeAsmOneInst();

		// ����Ƿ���ִ���
		if (g_bCompilingError == TRUE)
		{
			ReleaseAsmObjList(&pAsmObjList);
			return NULL;
		}
		else if (*pAsmObjPoint == NULL)
		{
			// ����Ƿ�������һ��ָ��
			break;
		}
		pAsmObjPoint = &((*pAsmObjPoint)->pNext);
	}

	return pAsmObjList;
}

INLINE PASM_OBJ CHARM_INTERNAL_FUNC SetAddrLabelInfo(PASM_OBJ pAsmObj)
{
	// �����ζ�����
	if ((pAsmObj->Inst == ASMER_PSEUDO_ADDR_LAB) || (pAsmObj->Inst == ASMER_PSEUDO_ADDR_VALUE_LAB))
	{
		while ((pAsmObj->Inst == ASMER_PSEUDO_ADDR_LAB) || (pAsmObj->Inst == ASMER_PSEUDO_ADDR_VALUE_LAB))
		{
			pAsmObj->dwOffset = g_dwLinkingCount;
			pAsmObj->CompilingInternal.pSymbolNode->dwOffset = g_dwLinkingCount;// ����ƫ��
			pAsmObj = pAsmObj->pNext;//������һ���ڵ�
		}
		return pAsmObj;
	}

	// ���ǵ�ַ�����ֱ�ӷ���
	return pAsmObj;
}

INLINE BOOL CHARM_INTERNAL_FUNC IsFlowInstruction(PASM_OBJ pAsmObj)
{
	switch (pAsmObj->Inst)
	{
	case ASMER_JA:
	case ASMER_JB:
	case ASMER_JAE:
	case ASMER_JBE:
	case ASMER_JZ:
	case ASMER_JNZ:
	case ASMER_JMP:
	case ASMER_CALL:
		{
			return TRUE;
		}break;
	}
	return FALSE;
}

INLINE DWORD CHARM_INTERNAL_FUNC CalcFlowOffset(DWORD dwSourceAddress, DWORD dwDestAddress, BYTE bInstLength)
{
	// ����ƫ��
	DWORD dwOffset = 0;
	if (dwDestAddress >= dwSourceAddress)
	{
		dwOffset = dwDestAddress - dwSourceAddress - bInstLength;
	}
	else
	{
		dwOffset = ~(dwSourceAddress - dwDestAddress + bInstLength - 1);
	}

	return dwOffset;
}

INLINE VOID CHARM_INTERNAL_FUNC RelocationFlow(LPBYTE pBin, PASM_OBJ pAsmObj, DWORD dwLoadBase)
{
	PASMER_OPERAND pCurrOpt = &(pAsmObj->Operand[0]);
	// �޸���תָ��ƫ����
	switch (pAsmObj->Inst)
	{
	case ASMER_JA:
	case ASMER_JB:
	case ASMER_JAE:
	case ASMER_JBE:
	case ASMER_JZ:
	case ASMER_JNZ:
		{
			// ����Ҳֻ������һ�����
			if ((pCurrOpt->Type == ASMER_OP_IMM) && (pCurrOpt->CompilingInternal.pSymbolNode != NULL))
			{
				// ���ֵ�ַ���
				if (pCurrOpt->CompilingInternal.pSymbolNode->Type == AST_ADDR_LAB)
				{
					DWORD dwOffset = pCurrOpt->CompilingInternal.dwRelocationPoint;
					DWORD dwSourceAddress = pAsmObj->dwOffset + dwLoadBase;
					DWORD dwDestAddress = (pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase;

					if (pCurrOpt->OffsetBit == ASMER_16_BITS)
					{
						WORD wFlowOffset = (WORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(WORD *)(pBin + dwOffset) = wFlowOffset;
					}
					else if (pCurrOpt->OffsetBit == ASMER_32_BITS)
					{
						DWORD dwFlowOffset = (DWORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(DWORD *)(pBin + dwOffset) = dwFlowOffset;
					}
				}
				else if (pCurrOpt->CompilingInternal.pSymbolNode->Type == AST_ADDR_VALUE_LAB)
				{
					DWORD dwOffset = pCurrOpt->CompilingInternal.dwRelocationPoint;
					DWORD *pAddress = (DWORD *)((pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);

					DWORD dwSourceAddress = pAsmObj->dwOffset + dwLoadBase;
					DWORD dwDestAddress = *(DWORD *)pAddress;

					if (pCurrOpt->OffsetBit == ASMER_16_BITS)
					{
						WORD wFlowOffset = (WORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(WORD *)(pBin + dwOffset) = wFlowOffset;
					}
					else if (pCurrOpt->OffsetBit == ASMER_32_BITS)
					{
						DWORD dwFlowOffset = (DWORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(DWORD *)(pBin + dwOffset) = dwFlowOffset;
					}
				}
			}
		}break;
	case ASMER_JMP:
	case ASMER_CALL:
		{
			if ((pCurrOpt->Type == ASMER_OP_IMM) && (pCurrOpt->CompilingInternal.pSymbolNode != NULL))
			{
				// ���ֵ�ַ���
				if (pCurrOpt->CompilingInternal.pSymbolNode->Type == AST_ADDR_LAB)
				{
					DWORD dwOffset = pCurrOpt->CompilingInternal.dwRelocationPoint;
					DWORD dwSourceAddress = pAsmObj->dwOffset + dwLoadBase;
					DWORD dwDestAddress = (pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase;

					if (pCurrOpt->OffsetBit == ASMER_16_BITS)
					{
						WORD wFlowOffset = (WORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(WORD *)(pBin + dwOffset) = wFlowOffset;
					}
					else if (pCurrOpt->OffsetBit == ASMER_32_BITS)
					{
						DWORD dwFlowOffset = (DWORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(DWORD *)(pBin + dwOffset) = dwFlowOffset;
					}
				}
				else if (pCurrOpt->CompilingInternal.pSymbolNode->Type == AST_ADDR_VALUE_LAB)
				{
					DWORD dwOffset = pCurrOpt->CompilingInternal.dwRelocationPoint;
					DWORD *pAddress = (DWORD *)((pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);

					DWORD dwSourceAddress = pAsmObj->dwOffset + dwLoadBase;
					DWORD dwDestAddress = *(DWORD *)pAddress;

					if (pCurrOpt->OffsetBit == ASMER_16_BITS)
					{
						WORD wFlowOffset = (WORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(WORD *)(pBin + dwOffset) = wFlowOffset;
					}
					else if (pCurrOpt->OffsetBit == ASMER_32_BITS)
					{
						DWORD dwFlowOffset = (DWORD)CalcFlowOffset(dwSourceAddress, dwDestAddress, pAsmObj->bInstLength);
						*(DWORD *)(pBin + dwOffset) = dwFlowOffset;
					}
				}
			}
			else if ((pCurrOpt->Type == ASMER_OP_MEM) && (pCurrOpt->CompilingInternal.pSymbolNode != NULL))
			{
				// ���ֵ�ַ���
				if (pCurrOpt->CompilingInternal.pSymbolNode->Type == AST_ADDR_LAB)
				{
					DWORD dwOffset = pCurrOpt->CompilingInternal.dwRelocationPoint;
					if (pCurrOpt->OffsetBit == ASMER_16_BITS)
					{
						*(WORD *)(pBin + dwOffset) = (WORD)((pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);
					}
					else if (pCurrOpt->OffsetBit == ASMER_32_BITS)
					{
						*(DWORD *)(pBin + dwOffset) = (DWORD)((pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);
					}
				}
				else if (pCurrOpt->CompilingInternal.pSymbolNode->Type == AST_ADDR_VALUE_LAB)
				{
					DWORD dwOffset = pCurrOpt->CompilingInternal.dwRelocationPoint;
					DWORD *pAddress = (DWORD *)((pCurrOpt->CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);
					if (pCurrOpt->OffsetBit == ASMER_16_BITS)
					{
						*(WORD *)(pBin + dwOffset) = *(WORD *)pAddress;
					}
					else if (pCurrOpt->OffsetBit == ASMER_32_BITS)
					{
						*(DWORD *)(pBin + dwOffset) = *(DWORD *)pAddress;
					}
				}
			}/* end else */
		}break;
	}
}

INLINE LPBYTE CHARM_INTERNAL_FUNC Relocation(LPBYTE pBin, PASM_OBJ pAsmObjList, DWORD dwLoadBase)
{
	PASM_OBJ pCurr = pAsmObjList;
	while (pCurr != NULL)
	{
		if (pCurr->CompilingInternal.bRefAddrLabel == TRUE)
		{
			// �ض�λ���
			for (int i = 0; i < pCurr->bOperandCount; i++)
			{
				if (pCurr->Operand[i].CompilingInternal.pSymbolNode != NULL)
				{
					// ���������ָ��,���������ָ���޸�����
					if (IsFlowInstruction(pCurr) == TRUE)
					{
						RelocationFlow(pBin, pCurr, dwLoadBase);
					}
					else
					{
						// ���ֵ�ַ���
						if (pCurr->Operand[i].CompilingInternal.pSymbolNode->Type == AST_ADDR_LAB)
						{
							DWORD dwOffset = pCurr->Operand[i].CompilingInternal.dwRelocationPoint;
							if (pCurr->Operand[i].OffsetBit == ASMER_16_BITS)
							{
								*(WORD *)(pBin + dwOffset) = (WORD)((pCurr->Operand[i].CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);
							}
							else if (pCurr->Operand[i].OffsetBit == ASMER_32_BITS)
							{
								*(DWORD *)(pBin + dwOffset) = (DWORD)((pCurr->Operand[i].CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);
							}
						}
						else if (pCurr->Operand[i].CompilingInternal.pSymbolNode->Type == AST_ADDR_VALUE_LAB)
						{
							DWORD dwOffset = pCurr->Operand[i].CompilingInternal.dwRelocationPoint;
							DWORD *pAddress = (DWORD *)((pCurr->Operand[i].CompilingInternal.pSymbolNode->dwOffset) + dwLoadBase);
							if (pCurr->Operand[i].OffsetBit == ASMER_16_BITS)
							{
								*(WORD *)(pBin + dwOffset) = *(WORD *)pAddress;
							}
							else if (pCurr->Operand[i].OffsetBit == ASMER_32_BITS)
							{
								*(DWORD *)(pBin + dwOffset) = *(DWORD *)pAddress;
							}
						}
					}/* end else */
				}/* end if */
			}/* end for */
		}/* end if */
		pCurr = pCurr->pNext;
	}
	return pBin;
}

#define MAX_TMP_LINKING_BUFFER_SIZE					0x1000
INLINE LPBYTE CHARM_INTERNAL_FUNC Linking(DWORD *pdwOutSize, PASM_OBJ pAsmObjList, DWORD dwLoadBase)
{
	LPBYTE pNowBin = g_LinkingBuffer, pBin = NULL;
	DWORD dwBinSize = 0, dwInstLength = 0;
	BYTE pTmp[MAX_TMP_LINKING_BUFFER_SIZE];//4KB����ʱ����
	DWORD dwCount = 0;
	PASM_OBJ pCurr = pAsmObjList;
	g_dwLinkingCount = 0;//������0
	while (pCurr != NULL)
	{
		// ����ڵ��ǵ�ַ������¼��λ�ò��Թ�
		pCurr = SetAddrLabelInfo(pCurr);//����Ҳ������ƶ�ָ�����
		if (pCurr == NULL) break;
		pCurr->dwOffset = g_dwLinkingCount;//��ǰָ��ƫ��
		dwInstLength = LinkingOneInst(pCurr);
		if (dwInstLength == 0)//����һ��ָ��ʧ��
		{
			// ���pBin��Ϊ�����ͷſռ�
			if (pBin != NULL) __delete__(pBin);
			// �ͷ�OBJ��
			ReleaseAsmObjList(&pAsmObjList);
			*pdwOutSize = 0;
			return NULL;
		}
		// ����ָ���
		pCurr->bInstLength = (BYTE)dwInstLength;
		g_dwLinkingCount += dwInstLength;//����

		if (dwCount + dwInstLength <= MAX_TMP_LINKING_BUFFER_SIZE)
		{
			memcpy(pTmp + dwCount, pNowBin, dwInstLength);
			dwCount += dwInstLength;
		}
		else
		{
			if (pBin == NULL)//��һ����4KB
			{
				dwBinSize = dwCount + dwInstLength;
				pBin = __new__(BYTE, dwBinSize);
				memcpy(pBin, pTmp, dwCount);
				memcpy(pBin + dwCount, pNowBin, dwInstLength);
			}
			else
			{
				DWORD dwExchangeBinSize = dwBinSize;
				dwBinSize = dwBinSize + dwCount + dwInstLength;
				LPBYTE pExchange = __new__(BYTE, dwBinSize);
				memcpy(pExchange, pBin, dwExchangeBinSize);
				memcpy(pExchange + dwExchangeBinSize, pTmp, dwCount);
				memcpy(pExchange + dwExchangeBinSize + dwCount, pNowBin, dwInstLength);
				__delete__(pBin);
				pBin = pExchange;
			}
			memset(pTmp, 0, MAX_TMP_LINKING_BUFFER_SIZE);
			dwCount = 0;
		}
		pCurr = pCurr->pNext;
	}

	// �����ļ�⻺����
	if (pBin == NULL)
	{
		dwBinSize = dwCount;
		pBin = __new__(BYTE, dwBinSize);
		memcpy(pBin, pTmp, dwBinSize);
	}
	else if (dwCount != 0)
	{
		DWORD dwExchangeBinSize = dwBinSize;
		dwBinSize = dwBinSize + dwCount;
		LPBYTE pExchange = __new__(BYTE, dwBinSize);
		memcpy(pExchange, pBin, dwExchangeBinSize);
		memcpy(pExchange + dwExchangeBinSize, pTmp, dwCount);
		__delete__(pBin);
		pBin = pExchange;
	}

	*pdwOutSize = dwBinSize;
	return Relocation(pBin, pAsmObjList, dwLoadBase);
}

#define __AssemblerRunFailed__()\
	DestoryAsmerSymbolTable();\
	*pdwOutSize = 0;
#define __AssemblerRunExit__				__AssemblerRunFailed__
LPBYTE CHARM_INTERNAL_FUNC Assembler(DWORD *pdwOutSize, PASM_SOURCE pAsmSource, DWORD dwLoadBase)
{
	DWORD dwOutSize = 0;
	
	// ��ʼ�����ű�
	InitAsmerSymbolTable(pAsmSource);
	PASM_OBJ pAsmObjList = Compiling(pAsmSource);//����
	if (pAsmObjList == NULL)
	{
		// �������
		__AssemblerRunFailed__();
		return NULL;
	}

	LPBYTE pBin = Linking(&dwOutSize, pAsmObjList, dwLoadBase);//����
	if (pBin == NULL)
	{
		// ���ӳ���
		__AssemblerRunFailed__();
		return NULL;
	}

	__AssemblerRunExit__();
	*pdwOutSize = dwOutSize;
	return pBin;
}
