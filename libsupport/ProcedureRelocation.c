#define __ProRelocationJmpUpOffset__(Offset, InstLen) ((~(Offset) + 1) - (InstLen))
#define __ProRelocationJmpDownOffset__(Offset, InstLen) ((Offset) + (InstLen))
__address __API__ CalcTargetAddress(__byte bSize, __address addrNowAddress, __offset ofOffset, __integer iInstLen, __bool bUp) {
	__address addrTargetAddress = 0;

	if (bSize == 8) {
		__byte bOffset = 0;
		bOffset = (__byte)ofOffset;
		if (bUp) {
			bOffset = __ProRelocationJmpUpOffset__(bOffset, iInstLen);
			addrTargetAddress = addrNowAddress - bOffset;
		} else {
			bOffset = __ProRelocationJmpDownOffset__(bOffset, iInstLen);
			addrTargetAddress = addrNowAddress + bOffset;
		}
	} else if (bSize == 16) {
		__word wOffset = 0;
		wOffset = (__word)ofOffset;
		if (bUp) {
			wOffset = __ProRelocationJmpUpOffset__(wOffset, iInstLen);
			addrTargetAddress = addrNowAddress - wOffset;
		} else {
			wOffset = __ProRelocationJmpDownOffset__(wOffset, iInstLen);
			addrTargetAddress = addrNowAddress + wOffset;
		}
	} else if (bSize == 32) {
		__dword dwOffset = 0;
		dwOffset = (__dword)ofOffset;
		if (bUp) {
			dwOffset = __ProRelocationJmpUpOffset__(dwOffset, iInstLen);
			addrTargetAddress = addrNowAddress - dwOffset;
		} else {
			dwOffset = __ProRelocationJmpDownOffset__(dwOffset, iInstLen);
			addrTargetAddress = addrNowAddress + dwOffset;
		}
	}

	return addrTargetAddress;
}

__offset __API__ CalcCurrOffsetFromAddress(__byte bSize, __address addrTargetAddress, __address addrNowAddress, __integer iInstLen) {
	__offset ofOffset = 0;
	ofOffset = (__offset)(addrTargetAddress - addrNowAddress);
	if (bSize == 8) {//8λ
		__byte bOffset = (__byte)ofOffset;
		if (__IsNegative8__(bOffset)) {
			// ����
			bOffset = (~bOffset) - 1;
		} else {
			// ����
			bOffset -= iInstLen;
		}
		ofOffset = bOffset;
	} else if (bSize == 16) {
		__word wOffset = (__word)ofOffset;
		if (__IsNegative16__(wOffset)) {
			// ����
			wOffset = (~wOffset) - 1;
		} else {
			// ����
			wOffset -= iInstLen;
		}
		ofOffset = wOffset;
	} else {
		__dword dwOffset = (__dword)ofOffset;
		if (__IsNegative32__(dwOffset)) {
			// ����
			dwOffset = (~dwOffset) - 1;
		} else {
			// ����
			dwOffset -= iInstLen;
		}
		ofOffset = dwOffset;
	}

	return ofOffset;
}

/*
 * ʹ����support�����õ�hde32���������,ûʹ��udis86����,hde32����
 * ��ʹ���κ�API
 */
#define __MAX_API_PROCEDURE_LENGTH__				4096
__integer __API__ SimpleCalcThisProcedureLength(__memory pProcedure) {
	__memory pPoint = pProcedure;
	__integer iLength = 0;
	//ud_t ud_obj;
	//ud_init(&ud_obj);
	//ud_set_input_buffer(&ud_obj, pProcedure, __MAX_API_PROCEDURE_LENGTH__);//Ĭ��һ����������󳤶�Ϊ4096
	//ud_set_mode(&ud_obj, 32);
	//ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	//while (ud_disassemble(&ud_obj)) {
	//	if ((ud_obj.mnemonic == UD_Inop) || (ud_obj.mnemonic == UD_Iint3))
	//		break;
	//	iLength += ud_obj.inp_ctr;
	//}

	hde32s hs;
	do {
		hde32_disasm(pPoint, &hs);
		iLength += hs.len;
		pPoint += hs.len;
	} while ((hs.opcode != 0x90) && (hs.opcode != 0xCC));
	iLength -= hs.len;//��ȥ���һ������ĳ���
	return iLength;
}

__INLINE__ __bool __INTERNAL_FUNC__ IsJccJmpInstruction8Bits(hde32s *pHs) {
	__byte bOpcode = 0;
	bOpcode = (__byte)(pHs->opcode);
	switch (bOpcode) {
	case 0x77:
	case 0x73:
	case 0x72:
	case 0x76:
	case 0xE3:
	case 0x74:
	case 0x7F:
	case 0x7D:
	case 0x7C:
	case 0x7E:
	case 0x75:
	case 0x71:
	case 0x7B:
	case 0x70:
	case 0x7A:
	case 0x78:
	case 0xEB:
		return TRUE;
	break;
	}
	return FALSE;
}

__INLINE__ __bool __INTERNAL_FUNC__ IsJccJmpInstruction16Bits(hde32s *pHs) {
	__byte bOpcode1 = 0;
	__byte bOpcode2 = 0;
	if (!(pHs->p_66)) return FALSE;
	bOpcode1 = (__byte)(pHs->opcode);

	if (bOpcode1 == 0xE9) return TRUE;

	if (bOpcode1 != 0x0F) return FALSE;
	bOpcode2 = (__byte)(pHs->opcode2);
	switch (bOpcode2) {
	case 0x77:
	case 0x73:
	case 0x72:
	case 0x76:
	case 0xE3:
	case 0x74:
	case 0x7F:
	case 0x7D:
	case 0x7C:
	case 0x7E:
	case 0x75:
	case 0x71:
	case 0x7B:
	case 0x70:
	case 0x7A:
	case 0x78:
		return TRUE;
		break;
	}
	return FALSE;
}

__INLINE__ __bool __INTERNAL_FUNC__ IsJccJmpInstruction32Bits(hde32s *pHs) {
	__byte bOpcode1 = 0;
	__byte bOpcode2 = 0;
	if (pHs->p_66) return FALSE;
	bOpcode1 = (__byte)(pHs->opcode);

	if (bOpcode1 == 0xE9) return TRUE;

	if (bOpcode1 != 0x0F) return FALSE;
	bOpcode2 = (__byte)(pHs->opcode2);
	switch (bOpcode2) {
	case 0x77:
	case 0x73:
	case 0x72:
	case 0x76:
	case 0xE3:
	case 0x74:
	case 0x7F:
	case 0x7D:
	case 0x7C:
	case 0x7E:
	case 0x75:
	case 0x71:
	case 0x7B:
	case 0x70:
	case 0x7A:
	case 0x78:
		return TRUE;
		break;
	}
	return FALSE;
}

__memory __API__ RelocationThisProcedure(__memory pProcedure, __memory pOrigProcedureAddress, __integer iProcedureSize) {
	/*
	 * �����޸�CALL����, ����Ƕ�̬�����򲻽��д���
	 */
	__memory pCurrAddress = pProcedure;
	__offset ofOffsetByProcHdr = 0;
	hde32s hs;
	while (ofOffsetByProcHdr <= iProcedureSize) {
		hde32_disasm(pCurrAddress, &hs);

		// �����8λ��JCC JMP
		if (IsJccJmpInstruction8Bits(&hs)) {

		} else if (IsJccJmpInstruction16Bits(&hs)) {//�����16λ��JCC JMP

		} else if ((hs.opcode == 0xE8) || (IsJccJmpInstruction32Bits(&hs))) {// ��CALLָ�� �� JCC JMP 32λ��ָ��
			if (!hs.p_66) {//32λ
				__address addrTargetAddress = 0;
				__offset ofOffset = 0;
				__memory pFixAddress = NULL;
				ofOffset = (__offset)(hs.imm.imm32);
				pFixAddress = pCurrAddress + 1;//opcode
				// �鿴������������������
				if (__IsNegative32__(ofOffset)) {
					// ������
					addrTargetAddress = CalcTargetAddress(32, (__address)(pOrigProcedureAddress + ofOffsetByProcHdr), ofOffset, hs.len, TRUE);
				} else {
					// ������
					addrTargetAddress = CalcTargetAddress(32, (__address)(pOrigProcedureAddress + ofOffsetByProcHdr), ofOffset, hs.len, FALSE);
				}

				// ͨ��Ŀ���ַ�뵱ǰ�ĵ�ַ,���¼��������ƫ��
				ofOffset = CalcCurrOffsetFromAddress(32, addrTargetAddress, (__address)pCurrAddress, hs.len);
				*(__offset *)pFixAddress = ofOffset;//��������ƫ��
			}
		}
		ofOffsetByProcHdr += hs.len;
		pCurrAddress += hs.len;
	}

	//ud_t ud_obj;
	//ud_init(&ud_obj);
	//ud_set_input_buffer(&ud_obj, pProcedure, iProcedureSize);//Ĭ��һ����������󳤶�Ϊ4096
	//ud_set_mode(&ud_obj, 32);
	//ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	//while (ud_disassemble(&ud_obj)) {
	//	if (ud_obj.mnemonic == UD_Icall) {
	//		if (ud_obj.operand[0].type == UD_OP_JIMM) {
	//			if (ud_obj.operand[0].size == 32) {
	//				// 32λ
	//				__address addrTargetAddress = 0;
	//				__offset ofOffset = 0;
	//				__memory pFixAddress = NULL;
	//				ofOffset = (__offset)(ud_obj.operand[0].lval.udword);
	//				pCurrAddress += ofOffsetByProcHdr;
	//				pFixAddress = pCurrAddress + 1;//opcode
	//				// �鿴������������������
	//				if (__IsNegative32__(ofOffset)) {
	//					// ������
	//					addrTargetAddress = CalcTargetAddress((__address)(pOrigProcedureAddress + ofOffsetByProcHdr), ofOffset, ud_obj.inp_ctr, TRUE);
	//				} else {
	//					// ������
	//					addrTargetAddress = CalcTargetAddress((__address)(pOrigProcedureAddress + ofOffsetByProcHdr), ofOffset, ud_obj.inp_ctr, FALSE);
	//				}

	//				// ͨ��Ŀ���ַ�뵱ǰ�ĵ�ַ,���¼��������ƫ��
	//				ofOffset = CalcCurrOffsetFromAddress(32, addrTargetAddress, (__address)pCurrAddress, ud_obj.inp_ctr);
	//				*(__offset *)pFixAddress = ofOffset;//��������ƫ��
	//			}/* end else */
	//		}/* end else */
	//	}
	//	ofOffsetByProcHdr += ud_obj.inp_ctr;
	//}/* end while */
}