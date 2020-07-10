#if !defined(__ASSEMBLER_COMMON_H__)
#define __ASSEMBLER_COMMON_H__

#include "Common.h"
#include "y.tab.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#define ASMER_TYPE YYSTYPE

//// ��������ʽ
//typedef enum _OPT_FORM
//{
//	OF_NONE = 0,
//
//	/* op */ 
//	OF_OP, 
//	
//	/* op imm */
//	OF_I8,
//	OF_I16, 
//	OF_I32,
//
//	/* op mem */
//	OF_M8,
//	OF_M16, 
//	OF_M32,
//
//	/* op reg */
//	OF_R8,
//	OF_R16, 
//	OF_R32,
//
//	/* op reg, imm */
//	OF_R8_I8,
//	OF_R16_I8, 
//	OF_R16_I16,
//	OF_R32_I8,
//	OF_R32_I16, 
//	OF_R32_I32,
//	
//	/* op reg, mem */
//	OF_R8_M8,
//	OF_R16_M8,
//	OF_R16_M16,
//	OF_R32_M8,
//	OF_R32_M16,
//	OF_R32_M32,
//	OF_R32_M64,/* Ӧ���Ǵ���� */
//	OF_R64_M64,
//	OF_R64_M128,
//	OF_R128_M64,
//	OF_R128_M128,
//
//	/* op reg, reg */
//	OF_R8_R8,
//	OF_R16_R8,
//	OF_R16_R16,
//	OF_R32_R8,
//	OF_R32_R16, 
//	OF_R32_R32,
//	OF_R64_R64,
//	OF_R128_R128,
//
//	/* op mem, imm */
//	OF_M8_I8,
//	OF_M16_I8, 
//	OF_M16_I16,
//	OF_M32_I8,
//	OF_M32_I16, 
//	OF_M32_I32,
//
//	/* op mem, reg */
//	OF_M8_R8,
//	OF_M8_R16,
//	OF_M16_R16,
//	OF_M32_R16,
//	OF_M32_R32,
//
//	/* op imm:imm */
//	OF_I16_I8,
//	OF_I16_I16,
//	OF_I16_I32,
//
//	/* opt reg, mem, imm */
//	OF_R8_M8_I8,
//	OF_R16_M16_I8,
//	OF_R16_M16_I16, 
//	OF_R32_M32_I8,
//	OF_R32_M32_I16, 
//	OF_R32_M32_I32,
//	OF_R128_M128_8,
//
//	/* opt reg, reg, imm */
//	OF_R8_R8_I8,
//	OF_R16_R16_I8,
//	OF_R16_R16_I16, 
//	OF_R32_R32_I8,
//	OF_R32_R32_I16,
//	OF_R32_R32_I32,
//	OF_R128_R128_8
//} OPT_FORM;
	
typedef enum _ASMER_BIT {
	ASMER_0_BITS = 0,
	ASMER_8_BITS = 8,
	ASMER_16_BITS = 16,
	ASMER_32_BITS = 32,
	ASMER_64_BITS = 64,
	ASMER_80_BITS = 80,
	ASMER_128_BITS = 128
} ASMER_BIT;

#define __MAX_SYMBOL_SIZE__	128
typedef struct _ASM_SOURCE {
	__char *pText;//���Դ����
	__integer iTokenListMaxCount;//��ǵ�������
	__integer iImmediateMaxSize;//�������ڴ�������
	__integer iSymbolNameLength;//��������󳤶�
	__integer iSymbolMaxCount;//���ű���ŵĸ���
} ASM_SOURCE, *PASM_SOURCE;

typedef struct _MODRM_TYPE {
	__byte RM:3;
	__byte RO:3;
	__byte Mod:2;
} MODRM_TYPE;

typedef struct _SIB_TYPE {
	__byte Base:3;
	__byte Index:3;
	__byte SS:2;
} SIB_TYPE;

typedef enum _ASMER_SYMBOL_TYPE {
	AST_ADDR_NONE = 0,
	AST_ADDR_LAB
} ASMER_SYMBOL_TYPE;

// ���ת��������
#define __AsmerTokenToSymbolType__(AsmerToken, AsmerSymbolType)\
	switch ((AsmerToken){\
	case ADDR_LAB:(AsmerSymbolType) = AST_ADDR_LAB;break;\
	default: (AsmerSymbolType) = AST_ADDR_NONE;}

// OBJ�еķ��ű�
typedef struct _ASMER_SYMBOL_NODE {
	ASMER_SYMBOL_TYPE Type;//����
	__offset ofOffset;//������鿪ʼ��ƫ��
} ASMER_SYMBOL_NODE, *PASMER_SYMBOL_NODE;

#define __CreateSymbolNode__(SymbolNode, SymbolType, OffsetFromStart)\
	(SymbolNode) = (PASMER_SYMBOL_NODE)__logic_new__(ASMER_SYMBOL_NODE, 1);\
	__logic_memset__((SymbolNode), 0, sizeof(ASMER_SYMBOL_NODE));\
	(SymbolNode)->Type = (SymbolType);(SymbolNode)->ofOffset = OffsetFromStart;

#define __DestorySymbolNode__(SymbolNode)			__logic_delete__((SymbolNode));(SymbolNode) = NULL;

// ��������
typedef struct _ASMER_OPERAND {
	__bool bAddrLabel;//����ڵ�ʹ���˵�ַ���
	__bool bIsImmediate;//��������������
	__bool bIsRegister;//�������ǼĴ���
	__bool bIsMem;//���������ڴ�
	ASMER_TYPE Type;//�Ĵ���, �ڴ�, ������
	ASMER_BIT Bit;//��ǰ������λ��
	ASMER_TYPE Base;//��
	ASMER_TYPE Index;//����
	__byte Scale;//�̶�	
	ASMER_BIT ValBit;//ֵ��ռ��λ
	union {
		__char sByte;
		__byte uByte;
		__sword sWord;
		__word uWord;
		__sdword sDword;
		__dword uDword;
	} Val;//ֵ
	
	__offset ofRelocationFixOffset;//�ض�λ�޶�ƫ��
	//__char *pAddrLabelSymbolName;//��ַ�������ָ��
	//PASMER_SYMBOL_NODE pSymbolNode;//���Žڵ�,����������Ҫ����ɾ������
} ASMER_OPERAND, *PASMER_OPERAND;

// ָ������
typedef enum _INST_CLASS {
	IC_NONE = 0,
	IC_X86,
	IC_FPU,
	IC_SIMD,
	IC_VMX
} INST_CLASS;

// ���OBJ�ṹ
typedef struct _ASM_OBJ {
	struct _ASM_OBJ *pNext;//��һ���ڵ�
	__bool bAddrLabel;//����һ����Žڵ�
	INST_CLASS InstClass;//ָ������
	ASMER_TYPE Inst;//ָ��
	__byte bOpcodes[4];//���������
	__byte bOpcodesCount;//���������
	ASMER_OPERAND Operand[3];//��������
	__byte bOperandCount;//����������
	__bool bUseOpEx;//ʹ��OP��չ
	__byte bOpEx;//��������չ
	__offset ofOffset;//ƫ��
	__byte bInstLength;//��ǰָ��ĳ���
	__bool bUsePrefix;//ʹ����ǰ׺
	__byte bPrefixCount;//ǰ׺����
	__byte bPrefix[4];//ǰ׺
	__bool bUseModRM;//ʹ����ModRM
	__bool bUseSIB;//�Ƿ���SIBλ
	__bool bUseDisplacement;//����ָ��ӵ��ƫ��
	__byte bDisplacementOwn;//ƫ�������ĸ���������
	MODRM_TYPE ModRM;
	SIB_TYPE SIB;
	__char *pAddrLabelSymbolName;//��ַ��ŵ�����ָ��
	PASMER_SYMBOL_NODE pSymbolNode;//���Žڵ�
} ASM_OBJ, *PASM_OBJ;

// ��ȡ�Ĵ�������
__integer __INTERNAL_FUNC__ GetRegisterIndex(ASMER_TYPE Reg);
__void __INTERNAL_FUNC__ SetOperand(PASMER_OPERAND pOperand, __bool bAddressLabel, ASMER_TYPE OptType, ASMER_BIT OptBit, ASMER_TYPE RegBase, \
									ASMER_TYPE RegIndex, __byte MemScale, ASMER_BIT ValueBit, __dword OptValue, __char *pAddressLabelSymbolName, \
									__offset RelocationFixOffset, PASMER_SYMBOL_NODE pCurrSymbolNode);
__void __INTERNAL_FUNC__ SetOperandWithoutVal(PASMER_OPERAND pOperand, ASMER_TYPE OptType, ASMER_BIT OptBit, ASMER_TYPE RegBase, ASMER_TYPE RegIndex);
__void __INTERNAL_FUNC__ SetAsmObj(PASM_OBJ pAsmObj, INST_CLASS InstructionClass, ASMER_TYPE Instruction, __void *pOpcodes, __byte bNumberOfOpcodes, __byte bOpcodeEx);

#if defined(_DEBUG)
#define __PRINT_DBG_INFO__
#endif

__void __INTERNAL_FUNC__ AssemblerPrintAsmObj(PASM_OBJ pAsmObj);

#if defined(__cplusplus)
}
#endif

#endif
