#include "Encoder.h"
//__byte g_ModRM[256] = {
//	"\x00","\x08","\x10","\x18","\x20","\x28","\x30","\x38",
//	"\x01","\x09","\x11","\x19","\x21","\x29","\x31","\x39",
//	"\x02","\x0A","\x12","\x1A","\x22","\x2A","\x32","\x3A",
//	"\x03","\x0B","\x13","\x1B","\x23","\x2B","\x33","\x3B",
//	"\x04","\x0C","\x14","\x1C","\x24","\x2C","\x34","\x3C",
//	"\x05","\x0D","\x15","\x1D","\x25","\x2D","\x35","\x3D",
//	"\x06","\x0E","\x16","\x1E","\x26","\x2E","\x36","\x3E",
//	"\x07","\x0F","\x17","\x1F","\x27","\x2F","\x37","\x3F",
//
//	"\x40","\x48","\x50","\x58","\x60","\x68","\x70","\x78",
//	"\x41","\x49","\x51","\x59","\x61","\x69","\x71","\x79",
//	"\x42","\x4A","\x52","\x5A","\x62","\x6A","\x72","\x7A",
//	"\x43","\x4B","\x53","\x5B","\x63","\x6B","\x73","\x7B",
//	"\x44","\x4C","\x54","\x5C","\x64","\x6C","\x74","\x7C",
//	"\x45","\x4D","\x55","\x5D","\x65","\x6D","\x75","\x7D",
//	"\x46","\x4E","\x56","\x5E","\x66","\x6E","\x76","\x7E",
//	"\x47","\x4F","\x57","\x5F","\x67","\x6F","\x77","\x7F",
//
//	"\x80","\x88","\x90","\x98","\xA0","\xA8","\xB0","\xB8",
//	"\x81","\x89","\x91","\x99","\xA1","\xA9","\xB1","\xB9",
//	"\x82","\x8A","\x92","\x9A","\xA2","\xAA","\xB2","\xBA",
//	"\x83","\x8B","\x93","\x9B","\xA3","\xAB","\xB3","\xBB",
//	"\x84","\x8C","\x94","\x9C","\xA4","\xAC","\xB4","\xBC",
//	"\x85","\x8D","\x95","\x9D","\xA5","\xAD","\xB5","\xBD",
//	"\x86","\x8E","\x96","\x9E","\xA6","\xAE","\xB6","\xBE",
//	"\x87","\x8F","\x97","\x9F","\xA7","\xAF","\xB7","\xBF",
//
//	"\xC0","\xC8","\xD0","\xD8","\xE0","\xE8","\xF0","\xF8",
//	"\xC1","\xC9","\xD1","\xD9","\xE1","\xE9","\xF1","\xF9",
//	"\xC2","\xCA","\xD2","\xDA","\xE2","\xEA","\xF2","\xFA",
//	"\xC3","\xCB","\xD3","\xDB","\xE3","\xEB","\xF3","\xFB",
//	"\xC4","\xCC","\xD4","\xDC","\xE4","\xEC","\xF4","\xFC",
//	"\xC5","\xCD","\xD5","\xDD","\xE5","\xED","\xF5","\xFD",
//	"\xC6","\xCE","\xD6","\xDE","\xE6","\xEE","\xF6","\xFE",
//	"\xC7","\xCF","\xD7","\xDF","\xE7","\xEF","\xF7","\xFF"
//};
//
//__byte g_SIB[256] = {
//	"\x00","\x01","\x02","\x03","\x04","\x05","\x06","\x07",
//	"\x08","\x09","\x0A","\x0B","\x0C","\x0D","\x0E","\x0F",
//	"\x10","\x11","\x12","\x13","\x14","\x15","\x16","\x17",
//	"\x18","\x19","\x1A","\x1B","\x1C","\x1D","\x1E","\x1F",
//	"\x20","\x21","\x22","\x23","\x24","\x25","\x26","\x27",
//	"\x28","\x29","\x2A","\x2B","\x2C","\x2D","\x2E","\x2F",
//	"\x30","\x31","\x32","\x33","\x34","\x35","\x36","\x37",
//	"\x38","\x39","\x3A","\x3B","\x3C","\x3D","\x3E","\x3F",
//
//	"\x40","\x41","\x42","\x43","\x44","\x45","\x46","\x47",
//	"\x48","\x49","\x4A","\x4B","\x4C","\x4D","\x4E","\x4F",
//	"\x50","\x51","\x52","\x53","\x54","\x55","\x56","\x57",
//	"\x58","\x59","\x5A","\x5B","\x5C","\x5D","\x5E","\x5F",
//	"\x60","\x61","\x62","\x63","\x64","\x65","\x66","\x67",
//	"\x68","\x69","\x6A","\x6B","\x6C","\x6D","\x6E","\x6F",
//	"\x70","\x71","\x72","\x73","\x74","\x75","\x76","\x77",
//	"\x78","\x79","\x7A","\x7B","\x7C","\x7D","\x7E","\x7F",
//
//	"\x80","\x81","\x82","\x83","\x84","\x85","\x86","\x87",
//	"\x88","\x89","\x8A","\x8B","\x8C","\x8D","\x8E","\x8F",
//	"\x90","\x91","\x92","\x93","\x94","\x95","\x96","\x97",
//	"\x98","\x99","\x9A","\x9B","\x9C","\x9D","\x9E","\x9F",
//	"\xA0","\xA1","\xA2","\xA3","\xA4","\xA5","\xA6","\xA7",
//	"\xA8","\xA9","\xAA","\xAB","\xAC","\xAD","\xAE","\xAF",
//	"\xB0","\xB1","\xB2","\xB3","\xB4","\xB5","\xB6","\xB7",
//	"\xB8","\xB9","\xBA","\xBB","\xBC","\xBD","\xBE","\xBF",
//
//	"\xC0","\xC1","\xC2","\xC3","\xC4","\xC5","\xC6","\xC7",
//	"\xC8","\xC9","\xCA","\xCB","\xCC","\xCD","\xCE","\xCF",
//	"\xD0","\xD1","\xD2","\xD3","\xD4","\xD5","\xD6","\xD7",
//	"\xD8","\xD9","\xDA","\xDB","\xDC","\xDD","\xDE","\xDF",
//	"\xE0","\xE1","\xE2","\xE3","\xE4","\xE5","\xE6","\xE7",
//	"\xE8","\xE9","\xEA","\xEB","\xEC","\xED","\xEE","\xEF",
//	"\xF0","\xF1","\xF2","\xF3","\xF4","\xF5","\xF6","\xF7",
//	"\xF8","\xF9","\xFA","\xFB","\xFC","\xFD","\xFE","\xFF"
//};

__INLINE__ __void __INTERNAL_FUNC__ SetSS(PASM_OBJ pAsmObj, __byte Scale) {
	pAsmObj->CompilingInternal.SIB.SS = Scale;
}

__INLINE__ __void __INTERNAL_FUNC__ SetIndex(PASM_OBJ pAsmObj, ASMER_TYPE Reg) {
	__byte Index = 0;
	__bool bIsIA32e = FALSE;
	__GetRegisterIndex__(Index, Reg);
	pAsmObj->CompilingInternal.SIB.Index = Index;
	__IsIA32Ex__(bIsIA32e, Reg);
	if (bIsIA32e) {
		pAsmObj->bUseREX = TRUE;
		pAsmObj->CompilingInternal.REX.X = 1;
	}
}

__INLINE__ __void __INTERNAL_FUNC__ SetBase(PASM_OBJ pAsmObj, ASMER_TYPE Reg) {
    // �����������,�������ֻ������Mod = 00��ʱ��,��ebp�Ĵ���,ʹ��SIB��ƫ����
	if (Reg == NONE)
		pAsmObj->CompilingInternal.SIB.Base = 5;
	else {
		__byte Base = 0;
		__bool bIsIA32e = FALSE;
		__GetRegisterIndex__(Base, Reg);
		pAsmObj->CompilingInternal.SIB.Base = Base;
		__IsIA32Ex__(bIsIA32e, Reg);
		if (bIsIA32e) {
			pAsmObj->bUseREX = TRUE;
			pAsmObj->CompilingInternal.REX.B = 1;
		}
	}
}

__INLINE__ __void __INTERNAL_FUNC__ SetRO(PASM_OBJ pAsmObj, ASMER_TYPE Reg) {
	__byte RO = 0;
	__bool bIsIA32e = FALSE;
	__GetRegisterIndex__(RO, Reg);
	pAsmObj->CompilingInternal.ModRM.RO = RO;
	__IsIA32Ex__(bIsIA32e, Reg);
	if (bIsIA32e) {
		pAsmObj->bUseREX = TRUE;	
		pAsmObj->CompilingInternal.REX.R = 1;
	}
}

__INLINE__ __bool __INTERNAL_FUNC__ IsGotoSIB(PASM_OBJ pAsmObj) {
	if ((pAsmObj->CompilingInternal.ModRM.Mod < 3) && (pAsmObj->CompilingInternal.ModRM.RM == 4)) {
		pAsmObj->CompilingInternal.bUseSIB = TRUE;
		return TRUE;
	}
	return FALSE;
}

/*
 * ������SIB��[disp32]�����
 */ 
__INLINE__ __void __INTERNAL_FUNC__ SetMod(PASM_OBJ pAsmObj, PASMER_OPERAND pModRmSib) {
	if (pModRmSib->Type == OP_REG)
		pAsmObj->CompilingInternal.ModRM.Mod = 3;
	else if (pModRmSib->OffsetBit == ASMER_0_BITS)
		pAsmObj->CompilingInternal.ModRM.Mod = 0;
	else if ((pModRmSib->OffsetBit == ASMER_32_BITS) && (pModRmSib->Base == NONE))
		pAsmObj->CompilingInternal.ModRM.Mod = 0;
	else if (pModRmSib->OffsetBit == ASMER_8_BITS)
		pAsmObj->CompilingInternal.ModRM.Mod = 1;
	else if (pModRmSib->OffsetBit == ASMER_32_BITS)
		pAsmObj->CompilingInternal.ModRM.Mod = 2;
}

__INLINE__ __void __INTERNAL_FUNC__ SetRM(PASM_OBJ pAsmObj, ASMER_TYPE Reg) {
	__byte RM = 0;
	__bool bIsIA32e = FALSE;
	__GetRegisterIndex__(RM, Reg);
	pAsmObj->CompilingInternal.ModRM.RM = RM;
	__IsIA32Ex__(bIsIA32e, Reg);
	if (bIsIA32e) {
		pAsmObj->bUseREX = TRUE;
		pAsmObj->CompilingInternal.REX.B = 1;
	}
}

__INLINE__ __integer __INTERNAL_FUNC__ SetModrmSib(PASM_OBJ pAsmObj, PASMER_OPERAND pRO, PASMER_OPERAND pModRmSib) {
	SetMod(pAsmObj, pModRmSib);//����MOD
	SetRO(pAsmObj, pRO->Base);//����RO
	SetRM(pAsmObj, pModRmSib->Base);//����RM
	// ȷ���Ƿ���SIBλ
	if (IsGotoSIB(pAsmObj)) {
		SetSS(pAsmObj, pModRmSib->Scale);//����SS
		SetIndex(pAsmObj, pModRmSib->Index);//����Index
		SetBase(pAsmObj, pModRmSib->Base);//����Base
		return 2;
	}
	return 1;
}

/*
 * ����ֻ�����ö�Ӧ�ļĴ���, �����ڵ������Ƿ���Ϊ��Ӧ���ֽ�, ����ʹ������Ĵ���
 */ 
__INLINE__ ASMER_TYPE __INTERNAL_FUNC__ MapOpExToRegType(__byte bOpEx) {
	if (bOpEx == 0)
		return R_EAX;
	else if (bOpEx == 1)
		return R_ECX;
	else if (bOpEx == 2)
		return R_EDX;
	else if (bOpEx == 3)
		return R_EBX;
	else if (bOpEx == 4)
		return R_ESP;
	else if (bOpEx == 5)
		return R_EBP;
	else if (bOpEx == 6)
		return R_ESI;
	else if (bOpEx == 7)
		return R_EDI;
	return R_EAX;
}

/*
 * �����޿̶�ʱ��SIB����,���������ֻʹ��ModRm
 * ���ǻ��Ĵ���Ϊesp, ���Ĵ���Ϊesp, �϶�����SIBλ
 */
__INLINE__ __integer __INTERNAL_FUNC__ MakeCode(PASM_OBJ pAsmObj, __byte bOpEx) {
	PASMER_OPERAND pCurrRO = NULL, pCurrOpt0 = &(pAsmObj->Operand[0]);
	PASMER_OPERAND pCurrModrmSib = NULL, pCurrOpt1 = &(pAsmObj->Operand[1]);
	ASMER_OPERAND TmpOp;
	memset(&TmpOp, 0, sizeof(ASMER_OPERAND));
	pAsmObj->CompilingInternal.bUseModRM = TRUE;
	if (bOpEx != OPEX_NONE) {
		/*
		 * ������� <<IA-32 Architecture Instruction Set Reference A-M ��¼A TableA-6��>> 
		 */
		TmpOp.Base = MapOpExToRegType(bOpEx);
		pCurrRO = &TmpOp;
		pCurrModrmSib = pCurrOpt0;
	} else {
		if ((pCurrOpt0->Type == OP_REG) && (pCurrOpt1->Type == OP_MEM)) {
			pCurrRO = pCurrOpt0;
			pCurrModrmSib = pCurrOpt1;
		} else if ((pCurrOpt0->Type == OP_MEM) && (pCurrOpt1->Type == OP_REG)) {
			pCurrRO = pCurrOpt1;
			pCurrModrmSib = pCurrOpt0;
		} else if ((pCurrOpt0->Type == OP_REG) && (pCurrOpt1->Type == OP_REG)) {
			pCurrRO = pCurrOpt0;
			pCurrModrmSib = pCurrOpt1;
		}
	}/* end else */ 
	return SetModrmSib(pAsmObj, pCurrRO, pCurrModrmSib);
}

__byte __INTERNAL_FUNC__ GetModRM(PASM_OBJ pAsmObj) {
	return *(__byte *)&(pAsmObj->ModRM);
}

__byte __INTERNAL_FUNC__ GetSIB(PASM_OBJ pAsmObj) {
	return *(__byte *)&(pAsmObj->SIB);
}

__byte __INTERNAL_FUNC__ GetREX(PASM_OBJ pAsmObj) {
	pAsmObj->REX.Rel = 4;
	return *(__byte *)&(pAsmObj->REX);
}
