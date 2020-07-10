#include "AssemblerEncoder.h"

__integer __INTERNAL_FUNC__ Encoder(__memory pInstBuf, PASM_OBJ pAsmObj, __offset ofCurrentOffset) {
	/*
	 * [Prefixes] [Opcode] [ModR/M] [SIB] [Displacement] [Immediate]
	 * 1.д��ǰ׺(����еĻ�)
	 * 2.д�������
	 * 3.д��ModRM | SIB(����еĻ�)
	 * 4.д��ƫ��(����еĻ�)
	 * 5.д��������(����еĻ�)
	 * 6.д���ַ���ƫ������ƫ�Ƶ�ַ(����еĻ�)
	 * 7.����ָ���
	 */
	__integer iInstLength = 0, i = 0;
	PASMER_OPERAND pOperand = (PASMER_OPERAND)&(pAsmObj->Operand);
	__logic_memcpy__(pInstBuf, pAsmObj->bPrefix, pAsmObj->bPrefixCount);
	iInstLength += pAsmObj->bPrefixCount;
	__logic_memcpy__(pInstBuf + iInstLength, pAsmObj->bOpcodes, pAsmObj->bOpcodesCount);
	iInstLength += pAsmObj->bOpcodesCount;
	if (pAsmObj->bUseModRM) {
		__logic_memcpy__(pInstBuf + iInstLength, (__memory)&(pAsmObj->ModRM), 1);
		iInstLength++;
	}
	if (pAsmObj->bUseSIB) {
		__logic_memcpy__(pInstBuf + iInstLength, (__memory)&(pAsmObj->bUseSIB), 1);
		iInstLength++;
	}
	// ����������
	for (i = 0; i < pAsmObj->bOperandCount; i++) {
		// �޶���ַ���,������ǵ�ַ���,����������
		if (pOperand[i].bAddrLabel)
			pOperand[i].ofRelocationFixOffset = ofCurrentOffset + iInstLength;

		// д��ֵ
		switch (pOperand[i].ValBit) {
		case ASMER_8_BITS:{
			__logic_memcpy__(pInstBuf + iInstLength, (__memory)&(pOperand[i].Val.uByte), sizeof(__byte));
			iInstLength += sizeof(__byte);
		}break;
		case ASMER_16_BITS:{
			__logic_memcpy__(pInstBuf + iInstLength, (__memory)&(pOperand[i].Val.uWord), sizeof(__word));
			iInstLength += sizeof(__word);
		}break;
		case ASMER_32_BITS:{
			__logic_memcpy__(pInstBuf + iInstLength, (__memory)&(pOperand[i].Val.uDword), sizeof(__dword));
			iInstLength += sizeof(__dword);
		}break;
		}/* end switch */
	}/* end for */
	pAsmObj->bInstLength = (__byte)iInstLength;
	return iInstLength;
}
