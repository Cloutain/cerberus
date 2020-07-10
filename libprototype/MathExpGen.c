#include "MathExpGen.h"

/*
 * 1.ʹ��GenerateMathExp����һ��pMathExp���ʽ�ṹ
 * 2.ʹ��GetMathExpVariableNumber(pMathExp)��ȡ���ʽ�б����ĸ���
 * 3.����һ��pMathExpAsmGenConfigure�ı������������ջ�ռ���ջ��
 * 4.ʹ��GenerateMathExpAsmSource(pMathExp, pMathExpAsmGenConfigure)���ɻ��Դ����pAsmSource�ṹ
 * 5.ʹ��Assembler�������л��
 * 6.ʹ��ReleaseMathExp(&pMathExp)�ͷű��ʽ���ڴ�
 *
 * !ע:VarReg�Ǻ����Ĵ���,��������ֻ����ת�Ʊ�������,�������������
 */

#define __OpObjectIsVariable__()						(GetRandDword() % 2 == 0)
//#define __ConstToMathExpOptBitConst__()\
//	if (pMathExp->Bit == ASMER_8_BITS) pMathExp->dwVal = pMathExp->dwVal & 0xFF;\
//	else if (pMathExp->Bit == ASMER_16_BITS) pMathExp->dwVal = pMathExp->dwVal & 0xFFFF;\
//	else if (pMathExp->Bit == ASMER_32_BITS) pMathExp->dwVal = pMathExp->dwVal & 0xFFFFFFFF;
#define __WriteImmediateOnBit__(Bit, Immediate){\
	__dword dwImmediate = 0;\
	if ((Bit) == ASMER_8_BITS) {\
		dwImmediate = (Immediate) & 0xFF;\
		WriteImmediate((__memory)&dwImmediate, (Bit));\
	} else if ((Bit) == ASMER_16_BITS) {\
		dwImmediate = (Immediate) & 0xFFFF;\
		WriteImmediate((__memory)&dwImmediate, (Bit));\
	} else if ((Bit) == ASMER_32_BITS) {\
		dwImmediate = (Immediate) & 0xFFFFFFFF;\
		WriteImmediate((__memory)&dwImmediate, (Bit));}}

#define __WriteImmediateOnBitButTokenIs32Bits__(Bit, Immediate){\
	__dword dwImmediate = 0;\
	if ((Bit) == ASMER_8_BITS) {\
		dwImmediate = (Immediate) & 0xFF;\
		WriteImmediate((__memory)&dwImmediate, ASMER_32_BITS);\
	} else if ((Bit) == ASMER_16_BITS) {\
		dwImmediate = (Immediate) & 0xFFFF;\
		WriteImmediate((__memory)&dwImmediate, ASMER_32_BITS);\
	} else if ((Bit) == ASMER_32_BITS) {\
		dwImmediate = (Immediate) & 0xFFFFFFFF;\
		WriteImmediate((__memory)&dwImmediate, ASMER_32_BITS);}}

#define __GenerateMathOpt__()				(MATH_OPT)(GetRandDword() % __MAX_MATH_OPT__ + 1)
#define __GenerateMathOptWithOutNot__()		(MATH_OPT)(GetRandDword() % (__MAX_MATH_OPT__ - 1) + 1)
#define __HasPassMathExp__(pConfigure)		(RollIt((pConfigure)->MathExpRollArray))
#define __HasSubMathExp__(pConfigure)		(RollIt((pConfigure)->MathExpRollArray))

__INLINE__ ASMER_BIT __INTERNAL_FUNC__ GetRandValBit() {
	ASMER_BIT Bit;
	__integer iIndex = (GetRandDword() % 3) + 1;
	switch (iIndex) {
	case 1:
		Bit = ASMER_8_BITS;
	break;
	case 2:
		Bit = ASMER_16_BITS;
	break;
	case 3:
		Bit = ASMER_32_BITS;
	break;
	}
	return Bit;
}

__INLINE__ MATH_OPT __INTERNAL_FUNC__ GenerateMathOpt() {
	MATH_OPT Opt = __GenerateMathOpt__();
	return Opt;
}

__INLINE__ MATH_OPT __INTERNAL_FUNC__ GenerateMathOptWithOutNot() {
	MATH_OPT Opt = __GenerateMathOptWithOutNot__();
	return Opt;
}

__INLINE__ MATH_OPT __INTERNAL_FUNC__ GenerateReverseMathOpt(MATH_OPT Opt) {
	MATH_OPT ReverseOpt = MATH_NONE;
	switch (Opt){
	case MATH_ADD:
		ReverseOpt = MATH_SUB;
	break;
	case MATH_SUB:
		ReverseOpt = MATH_ADD;
	break;
	case MATH_MUL:
		ReverseOpt = MATH_DIV;
	break;
	case MATH_DIV:
		ReverseOpt = MATH_MUL;
	break;
	case MATH_XOR:
		ReverseOpt = MATH_XOR;
	break;
	}
	return ReverseOpt;
}

/*
 * ֻ����һ������ʽģ��,�������ӱ��ʽ��ʲô
 * ֻ���������������������ʽ�ṹ���ڴ�
 * ���ɺ�,������ӱ��ʽ���޳���ֵ,���ӱ��ʽ��
 * һ���г���ֵ���߱���
 * �������ʽ�������б������ʽ,ֻ��ӵ���ӱ��ʽ
 */
__INLINE__ PMATHEXP __INTERNAL_FUNC__ CreateMathExp() {
	PMATHEXP pMathExp = __logic_new__(MATHEXP, 1);
	memset(pMathExp, 0, sizeof(MATHEXP));
	return pMathExp;
}

__INLINE__ ASMER_TYPE __INTERNAL_FUNC__ RandReg(ASMER_BIT Bit) {
	ASMER_TYPE CurrReg = NONE;
	if (Bit == ASMER_8_BITS) {
		CurrReg = R_AL + (GetRandDword() % 8);
	} else if (Bit == ASMER_16_BITS) {
		do {
			CurrReg = R_AX + (GetRandDword() % 8);
		} while (CurrReg == R_SP);
	} else if (Bit == ASMER_32_BITS) {
		do {
			CurrReg = R_EAX + (GetRandDword() % 8);
		} while (CurrReg == R_ESP);
	}
	return CurrReg;
}

__INLINE__ ASMER_TYPE __INTERNAL_FUNC__ SelectCurrReg(PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE CurrReg;
	do {
		CurrReg = RandReg(pMathExpAsmConfigure->Bit);
	} while ((CurrReg == pMathExpAsmConfigure->ResultReg) || 
		(CurrReg == pMathExpAsmConfigure->VarReg) || 
		(CurrReg == pMathExpAsmConfigure->TmpReg) ||
		(CurrReg == R_ESP) || (CurrReg == R_SP));
	return CurrReg;
}

PMATHEXP __INTERNAL_FUNC__ GenerateMathRootExp(PPROTOTYPE_CONFIGURE pProtoTypeConfigure) {
	MATH_OPT Opt;
	PMATHEXP pMathExp = CreateMathExp();

	// �Ƿ���ڱ������ʽ
	if (__HasPassMathExp__(pProtoTypeConfigure)) {//ӵ�б������ʽ
		/*
		 * ���ѡ��һ��������
		 * �����NOT�������趨��
		 * �������ɲ�����
		 */
		Opt = GenerateMathOpt();
		if (Opt == MATH_NOT) {
			pMathExp->bNot = TRUE;
			Opt = GenerateMathOptWithOutNot();
			pMathExp->Opt = Opt;
		} else
			pMathExp->Opt = Opt;
		// �������ɱ������ʽ
		pMathExp->pPassivityExp = CreateMathExp();
		// �������ʽ�Ƿ���ҪNOT������
		Opt = GenerateMathOpt();
		if (Opt == MATH_NOT)
			pMathExp->pPassivityExp->bNot = TRUE;

		///*
		// * �����֤�������ʽ�Ƿ���һ������,�����һ��������
		// * ��֤��ǰ�������Ƿ���һ����������,���Ϊ����������
		// * �������ɲ�����
		// */
		//if (pMathExp->pPassivityExp->bVariable) {
		//	while (pMathExp->Opt == MATH_DIV) {
		//		pMathExp->Opt = GenerateMathOptWithOutNot();
		//	}
		//}/* end if */
	} else {//û�б������ʽ
		Opt = GenerateMathOpt();
		if (Opt == MATH_NOT)
			pMathExp->bNot = TRUE;
	}
	return pMathExp;
}

/*
 * �����������߳���
 */
__void __INTERNAL_FUNC__ GenerateMathExpOptObject(PMATHEXP pMathExp) {
	if (__OpObjectIsVariable__())
		pMathExp->bVariable = TRUE;
	else {
		pMathExp->ValBit = GetRandValBit();
		pMathExp->dwVal = GetRandDword();//���ѡ��һ������
	}
}

/*
 * ��������ѧ������ʽ��
 * ������ÿ�������㷨,�������ӱ��ʽ
 */
PMATHEXP __INTERNAL_FUNC__ GenerateMathExp(PPROTOTYPE_CONFIGURE pProtoTypeConfigure) {
	PMATHEXP pMathExp = GenerateMathRootExp(pProtoTypeConfigure);//����һ����
	PMATHEXP pPassMathExp = pMathExp->pPassivityExp;
	/*
	 * ���������ʽ�Ƿ����ӱ��ʽ
	 * ���û���ӱ��ʽ�����ѡ��һ������
	 */
	if (__HasSubMathExp__(pProtoTypeConfigure))
		pMathExp->pSubExp = GenerateMathExp(pProtoTypeConfigure);
	else
		GenerateMathExpOptObject(pMathExp);

	// ���ɱ������ʽ���ӱ��ʽ
	if (pPassMathExp) {
		if (__HasSubMathExp__(pProtoTypeConfigure))
			pPassMathExp->pSubExp = GenerateMathExp(pProtoTypeConfigure);
		else
			GenerateMathExpOptObject(pPassMathExp);
	}/* end if */
	return pMathExp;
}

__dword __INTERNAL_FUNC__ GetMathExpVariableNumber(PMATHEXP pMathExp) {
	PMATHEXP pCurr = pMathExp, pPassMathExp = pCurr->pPassivityExp;
	__integer iVarCount = 0;
	// �����ӱ��ʽ�ı�������
	if (pCurr->pSubExp)
		iVarCount += GetMathExpVariableNumber(pCurr->pSubExp);
	
	// ���㱻�����ʽ�ı�������
	if (pPassMathExp)
		iVarCount += GetMathExpVariableNumber(pPassMathExp);

	// ���������Ƿ��Ǳ���
	if (pCurr->bVariable)
		iVarCount++;

	return iVarCount;
}

__dword __INTERNAL_FUNC__ GetMathExpImmediateNumber(PMATHEXP pMathExp) {
	PMATHEXP pCurr = pMathExp, pPassMathExp = pCurr->pPassivityExp;
	__integer iImmediateCount = 0;
	// �����ӱ��ʽ�ı�������
	if (pCurr->pSubExp)
		iImmediateCount += GetMathExpImmediateNumber(pCurr->pSubExp);

	// ���㱻�����ʽ�ı�������
	if (pPassMathExp)
		iImmediateCount += GetMathExpImmediateNumber(pPassMathExp);

	// ���������Ƿ��Ǳ���
	if (!(pCurr->bVariable))
		iImmediateCount++;

	return iImmediateCount;
}

__INLINE__ ASM_BLOCK_TYPE __INTERNAL_FUNC__ ReturnAsmBlockType(PMATHEXP pMathExp) {
	if (pMathExp->bNot) {//����NOT������
		if (pMathExp->pPassivityExp) {//���ڱ������ʽ
			if (pMathExp->pSubExp) {//�����ӱ��ʽ
				return ASM_BLOCK_TYPE_NOT_SUB_PASSIV;
			} else {//�������ӱ��ʽ
				if (pMathExp->bVariable)//Ϊ����
					return ASM_BLOCK_TYPE_NOT_VAR_PASSIV;
				else//Ϊ����
					return ASM_BLOCK_TYPE_NOT_CONST_PASSIV;
			}
		} else {//�����ڱ������ʽ
			if (pMathExp->pSubExp) {//�����ӱ��ʽ
				return ASM_BLOCK_TYPE_NOT_SUB;
			} else {//�������ӱ��ʽ
				if (pMathExp->bVariable)//Ϊ����
					return ASM_BLOCK_TYPE_NOT_VAR;
				else//Ϊ����
					return ASM_BLOCK_TYPE_NOT_CONST;
			}
		}
	} else {//������NOT������
		if (pMathExp->pPassivityExp) {//���ڱ������ʽ
			if (pMathExp->pSubExp) {//�����ӱ��ʽ
				return ASM_BLOCK_TYPE_SUB_PASSIV;
			} else {//�������ӱ��ʽ
				if (pMathExp->bVariable)//Ϊ����
					return ASM_BLOCK_TYPE_VAR_PASSIV;
				else//Ϊ����
					return ASM_BLOCK_TYPE_CONST_PASSIV;
			}
		} else {//�����ڱ������ʽ
			if (pMathExp->pSubExp) {//�����ӱ��ʽ
				return ASM_BLOCK_TYPE_SUB;
			} else {//�������ӱ��ʽ
				if (pMathExp->bVariable)//Ϊ����
					return ASM_BLOCK_TYPE_VAR;
				else//Ϊ����
					return ASM_BLOCK_TYPE_CONST;
			}
		}	
	}/* end else */

	return ASM_BLOCK_TYPE_NONE;
}

__void __INTERNAL_FUNC__ AsmBlock(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure);//��������
__INLINE__ __void __INTERNAL_FUNC__ AsmAdd(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(ADD);WriteInst(DestReg);WriteInst(COMMA);WriteInst(SourceReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmSub(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(SUB);WriteInst(DestReg);WriteInst(COMMA);WriteInst(SourceReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmMul(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(MUL);WriteInst(DestReg);WriteInst(COMMA);WriteInst(SourceReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmDiv(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {

	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->VarReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(R_EAX);WriteInst(COMMA);WriteInst(DestReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(DIV);WriteInst(SourceReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(DestReg);WriteInst(COMMA);WriteInst(R_EAX);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(pMathExpAsmConfigure->VarReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmMod(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->VarReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(R_EAX);WriteInst(COMMA);WriteInst(DestReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(DIV);WriteInst(SourceReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(DestReg);WriteInst(COMMA);WriteInst(R_EDX);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(pMathExpAsmConfigure->VarReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmOr(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(OR);WriteInst(DestReg);WriteInst(COMMA);WriteInst(SourceReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmAnd(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(ADD);WriteInst(DestReg);WriteInst(COMMA);WriteInst(SourceReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmXor(ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(DestReg);WriteInst(COMMA);WriteInst(SourceReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmOpt(MATH_OPT Opt, ASMER_TYPE DestReg, ASMER_TYPE SourceReg, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	switch (Opt) {
	case MATH_ADD:{
		AsmAdd(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_SUB:{
		AsmSub(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_MUL:{
		AsmMul(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_DIV:{
		AsmDiv(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_MOD:{
		AsmMod(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_OR:{
		AsmOr(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_AND:{
		AsmAnd(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	case MATH_XOR:{
		AsmXor(DestReg, SourceReg, pMathExpAsmConfigure);
	}break;
	}
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnVar(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(COMMA);WriteInst(pMathExpAsmConfigure->VarReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnConst(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(COMMA);
	__WriteImmediateOnBitButTokenIs32Bits__(pMathExp->ValBit, pMathExp->dwVal);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnNotVar(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(COMMA);WriteInst(pMathExpAsmConfigure->VarReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(NOT);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnNotConst(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	WriteInst(LINESTART);WriteInst(MOV);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(COMMA);
	__WriteImmediateOnBitButTokenIs32Bits__(pMathExp->ValBit, pMathExp->dwVal);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(NOT);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnVarPassiv(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE TmpReg = pMathExpAsmConfigure->TmpReg;
	AsmBlock(pMathExp->pPassivityExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	AsmBlockOnVar(pMathExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(TmpReg);WriteInst(LINEND);
	AsmOpt(pMathExp->Opt, pMathExpAsmConfigure->ResultReg, TmpReg, pMathExpAsmConfigure);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnNotVarPassiv(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE TmpReg = pMathExpAsmConfigure->TmpReg;
	AsmBlock(pMathExp->pPassivityExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	AsmBlockOnNotVar(pMathExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(TmpReg);WriteInst(LINEND);
	AsmOpt(pMathExp->Opt, pMathExpAsmConfigure->ResultReg, TmpReg, pMathExpAsmConfigure);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnConstPassiv(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE TmpReg = pMathExpAsmConfigure->TmpReg;
	AsmBlock(pMathExp->pPassivityExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	AsmBlockOnConst(pMathExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(TmpReg);WriteInst(LINEND);
	AsmOpt(pMathExp->Opt, pMathExpAsmConfigure->ResultReg, TmpReg, pMathExpAsmConfigure);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnNotConstPassiv(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE TmpReg = pMathExpAsmConfigure->TmpReg;
	AsmBlock(pMathExp->pPassivityExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	AsmBlockOnNotConst(pMathExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(TmpReg);WriteInst(LINEND);
	AsmOpt(pMathExp->Opt, pMathExpAsmConfigure->ResultReg, TmpReg, pMathExpAsmConfigure);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnSub(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	AsmBlock(pMathExp->pSubExp, pMathExpAsmConfigure);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnNotSub(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	AsmBlock(pMathExp->pSubExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(NOT);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnSubPassiv(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE TmpReg = pMathExpAsmConfigure->TmpReg;
	AsmBlock(pMathExp->pPassivityExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	AsmBlock(pMathExp->pSubExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(TmpReg);WriteInst(LINEND);
	AsmOpt(pMathExp->Opt, pMathExpAsmConfigure->ResultReg, TmpReg, pMathExpAsmConfigure);
}

__INLINE__ __void __INTERNAL_FUNC__ AsmBlockOnNotSubPassiv(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASMER_TYPE TmpReg = pMathExpAsmConfigure->TmpReg;
	AsmBlock(pMathExp->pPassivityExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(PUSH);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	AsmBlock(pMathExp->pSubExp, pMathExpAsmConfigure);
	WriteInst(LINESTART);WriteInst(NOT);WriteInst(pMathExpAsmConfigure->ResultReg);WriteInst(LINEND);
	WriteInst(LINESTART);WriteInst(POP);WriteInst(TmpReg);WriteInst(LINEND);
	AsmOpt(pMathExp->Opt, pMathExpAsmConfigure->ResultReg, TmpReg, pMathExpAsmConfigure);
}

/*
 * ������ȼ�
 * �������ʽ > �ӱ��ʽ > �������ʽ
 * ����:���ҽڵ���������
 */
__void __INTERNAL_FUNC__ AsmBlock(PMATHEXP pMathExp, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	ASM_BLOCK_TYPE AsmBlockType = ReturnAsmBlockType(pMathExp);//���ر��ʽ����

	switch (AsmBlockType) {
	case ASM_BLOCK_TYPE_NOT_SUB_PASSIV:
		AsmBlockOnNotSubPassiv(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_SUB_PASSIV:
		AsmBlockOnSubPassiv(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_NOT_SUB:
		AsmBlockOnNotSub(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_SUB:
		AsmBlockOnSub(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_NOT_CONST_PASSIV:
		AsmBlockOnNotConstPassiv(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_CONST_PASSIV:
		AsmBlockOnConstPassiv(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_NOT_VAR_PASSIV:
		AsmBlockOnNotVarPassiv(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_VAR_PASSIV:
		AsmBlockOnVarPassiv(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_NOT_CONST:
		AsmBlockOnNotConst(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_NOT_VAR:
		AsmBlockOnNotVar(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_CONST:
		AsmBlockOnConst(pMathExp, pMathExpAsmConfigure);
	break;
	case ASM_BLOCK_TYPE_VAR:
		AsmBlockOnVar(pMathExp, pMathExpAsmConfigure);
	break;
	}/* end switch */
}

/*
 * ������ѧ���ʽ
 */
PMATHEXP __INTERNAL_FUNC__ GenerateMathExpression(PPROTOTYPE_CONFIGURE pProtoTypeConfigure) {
	PMATHEXP pMathExp = GenerateMathExp(pProtoTypeConfigure);
	return pMathExp;
}

__INLINE__ __void __INTERNAL_FUNC__ InitMathExpReg(PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure) {
	pMathExpAsmConfigure->VarReg = RandReg(pMathExpAsmConfigure->Bit);
	do {
		pMathExpAsmConfigure->TmpReg = RandReg(pMathExpAsmConfigure->Bit);
	} while (pMathExpAsmConfigure->TmpReg == pMathExpAsmConfigure->VarReg);
	do {
		pMathExpAsmConfigure->ResultReg = RandReg(pMathExpAsmConfigure->Bit);
	} while ((pMathExpAsmConfigure->ResultReg == pMathExpAsmConfigure->VarReg) || (pMathExpAsmConfigure->ResultReg == pMathExpAsmConfigure->TmpReg));
}

/*
 * �γ����������޹صĻ�����
 */
__memory __INTERNAL_FUNC__ GenerateMathExpCode(PMATHEXP pMathExp, __integer *piOutBinSize, PPROTOTYPE_MATHEXP_ASM_CONFIGURE pMathExpAsmConfigure, __integer iImageBase) {
	__memory pMathExpCode = NULL;
	__integer iBinSize = 0;
	ASM_SOURCE AsmSource;

	// ����Ĭ�ϵĻ�����ṹ
	GenerateDefAsmSource(&AsmSource);
	InitAssembler(&AsmSource, NULL);

	// ����ѡ��Ĵ���
	InitMathExpReg(pMathExpAsmConfigure);

	// ���л��
	AsmBlock(pMathExp, pMathExpAsmConfigure);
	WriteInst(END);//д��Դ����������
	pMathExpCode = Assembler(&AsmSource, &iBinSize, iImageBase);
	*piOutBinSize = iBinSize;
	return pMathExpCode;
}

/*
 * ɾ���ڵ������������㷨
 */
__void __INTERNAL_FUNC__ ReleaseMathExp(PMATHEXP *pMathExpPoint) {
	PMATHEXP pCurr = *pMathExpPoint;

	if (pCurr->pSubExp)
		ReleaseMathExp(&(pCurr->pSubExp));

	if (pCurr->pPassivityExp)
		ReleaseMathExp(&(pCurr->pPassivityExp));

	__logic_delete__(pCurr);
	*pMathExpPoint = NULL;
	return;
}

#include "CipherExpGen.c"
