#if !defined(__CHAOSVM_CPU_INTERNAL_H__)
#define __CHAOSVM_CPU_INTERNAL_H__

#include "Common.h"
#include "ChaosVmCpu.h"
#include "ChaosVmCpuX86.h"
#include "ChaosVmCpuDataTypeConversion.h"
#include "ChaosVmCpuCalculateFinalAddress.h"
#include "ChaosVmCpuMemoryAccess.h"
#include "ChaosVmCpuInstructionType.h"

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(__CPU_INTERNAL_DEBUG__)
#define __CPU_UNREFERENCED_PARAMETER__(P)       (P) = (P)
#else
#define __CPU_UNREFERENCED_PARAMETER__(P)
#endif

/*
 * �Ĵ�����غ�
 */
#define __CPU_GR32__(pCPU, bGeneralRegister32Index)		((pCPU)->CurrentRegistersStatus.BasicRegister.GeneralRegister.Array[bGeneralRegister32Index].dwValue32)
#define __CPU_GR16__(pCPU, bGeneralRegister32Index)		((pCPU)->CurrentRegistersStatus.BasicRegister.GeneralRegister.Array[bGeneralRegister32Index].wValue16)
#define __CPU_GR8__(pCPU, bGeneralRegister32Index)		((pCPU)->CurrentRegistersStatus.BasicRegister.GeneralRegister.Array[(bGeneralRegister32Index) & 0x3].Value8[((bGeneralRegister32Index) >> 2) & 0x1])

#define __CPU_GR8_GET__(pCPU, bGeneralRegister8Index)				__CPU_GR8__(pCPU, bGeneralRegister8Index)
#define __CPU_GR8_SET__(pCPU, bGeneralRegister8Index, bInt8Value)	(__CPU_GR8__(pCPU, bGeneralRegister8Index) = bInt8Value)
#define __CPU_GR8_AL__(pCPU)										__CPU_GR8__(pCPU, CHAOSVM_R_AL)

#define __CPU_SR__(pCPU, bSegmentRegisterIndex)						((pCPU)->CurrentRegistersStatus.SegmentRegister.Array[bSegmentRegisterIndex].wValue16) 
#define __CPU_EFLAG_GET__(pCPU, bEFlagIndex)						((bEFlagIndex) == CHAOSVM_ER_IOPL) \
																	? \
																	(__byte)(((pCPU)->CurrentRegistersStatus.BasicRegister.EFlag.BitArray) >> (bEFlagIndex) & 0x03) \
																	: \
																	(__byte)(((pCPU)->CurrentRegistersStatus.BasicRegister.EFlag.BitArray) >> (bEFlagIndex) & 0x01)

#define __CPU_EFLAG_SET__(pCPU, bEFlagIndex, bNewValue)				((pCPU)->CurrentRegistersStatus.BasicRegister.EFlag.BitArray) = \
																	((bNewValue) == 1) \
																	? \
																	((pCPU)->CurrentRegistersStatus.BasicRegister.EFlag.BitArray) | ((__dword)1 << (bEFlagIndex)) \
																	: \
																	((pCPU)->CurrentRegistersStatus.BasicRegister.EFlag.BitArray) & (~((__dword)1 << (bEFlagIndex)))

#define __EFLAG_STATUS_TO_NUM__(EFlagStatus)						(((EFlagStatus) == __EFLAG_SET__) ? 1 : 0)
#define __NUM_TO_EFLAG_STATUS__(Number)								(((Number) == 0) ? __EFLAG_RESET__ : __EFLAG_SET__)
#define __CPU_EFLAG_GET_AS_NUM__(pCPU, bEFlagIndex)					(__EFLAG_STATUS_TO_NUM__(__CPU_EFLAG_GET__(pCPU, bEFlagIndex)))
#define __CPU_EIP__(pCPU)											((pCPU)->CurrentRegistersStatus.EIP)
#define __CPU_IP__(pCPU)											((pCPU)->CurrentRegistersStatus.EIP)
#define __CPU_IS_SINGLE_STEP__(pCPU)								(__EFLAG_SET__ == __CPU_EFLAG_GET__(pCPU, CHAOSVM_ER_TF))

/*
 * ǰ׺��ز���״̬
 */
// ��ǰָ���Ƿ���REPǰ׺
#define __IS_INSTRUCTION_REPEAT__(pCPU)								(pCPU->bRepPrefixEnable == __REP_PREFIX_REP__)
// ��ǰָ���Ƿ���REPEǰ׺
#define __IS_INSTRUCTION_REPE__(pCPU)								(pCPU->bRepPrefixEnable == __REP_PREFIX_REPE__)
// ��ǰָ���Ƿ���REPNEǰ׺
#define __IS_INSTRUCTION_REPNE__(pCPU)								(pCPU->bRepPrefixEnable == __REP_PREFIX_REPNE__)
// ��õ�ǰָ��REP��ǰ׺�Ļ�����ֵ
#define __IS_INSTRUCTION_PREFIX_REP_ENABLE__(pCPU)					(pCPU->bRepPrefixEnable != __REP_PREFIX_NULL__)

#define __INSTRUCTION_REPEAT_PREFIX__(pCPU)							(pCPU->bRepPrefixEnable)

// ��ǰָ���Ƿ���16λ������ģʽ
#define __IS_INSTRUCTION_OPERAND_SIZE_16__(pCPU)					(pCPU->bOperandType == __CPU_OPERAND_TYPE_16__)
// ��ǰָ���Ƿ���16λѰַģʽ
#define __IS_INSTRUCTION_ADDRESS_MODE_16__(pCPU)					(pCPU->bAddressingMode == __CPU_ADDRESSING_MODE_16__)

#define __INSTRUCTION_DATASEG_INDEX__(pCPU)							(pCPU->bDataSegmentRegister)
#define __INSTRUCTION_DATASEG__(pCPU)								(__CPU_SR__(pCPU, pCPU->bDataSegmentRegister))

/*
 * Last Status
 */
#define __SET_LAST_STATUS__(pCPU, Status)							((pCPU)->LastStatus = Status)
#define __GET_LAST_STATUS__(pCPU)									((pCPU)->LastStatus)

/*
 * Modrm����������
 */
#define __CPU_ANALYSE_MODRM__(pCPU, IndexOfModRMByte, pArgs)		((PMODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY)((pCPU)->ModrmByteAnalyseRoutineTableEntry))[(pCPU)->bAddressingMode].AnalyseRoutine((pCPU), (pCPU)->OpcodeBuffer + (IndexOfModRMByte), (pCPU)->bOpcodeMaxLength - (IndexOfModRMByte), pArgs)

/*
 * �����ڵ���ʱʹ�õĺ�
 */
extern __void __INTERNAL_FUNC__ ChaosVmCpuErrorOutput(PCHAOSVM_CPU pCPU, __char *pFormatStr, __char *Info);//ChaosVmKernel.c
extern __void __INTERNAL_FUNC__ ChaosVmCpuAssert(int Result,const char *SourceFile, const char * FunctionName, int LineNo, char * Exp);

#if defined(__CPU_INTERNAL_DEBUG__)
// ���ָ��ָ��Դ�ĳ��־��Ӱ������Ϊ"Undefined",�Ա���ͺ˶Ա�־�Ĵ���ʱ���Դ˱�־�ĶԱ�
#define __CPU_ASSERT__(exp)								ChaosVmCpuAssert(((exp) ? 0 : 1),__FILE__, __FUNCTION__, __LINE__, #exp)
#define __CPU_EFLAG_AFFECT_UNDEFINED__(pCPU, Index)		((pCPU)->UnCheckFlag[EFLAG_INDEX_##Index] = 1)
#define __CPU_EFLAG_NOT_AFFECT__(pCPU, Index)			((pCPU)->UnCheckFlag[EFLAG_INDEX_##Index] = 1)
#else
#define __CPU_ASSERT__(exp)
#define __CPU_EFLAG_AFFECT_UNDEFINED__(pCPU, Index)
#define __CPU_EFLAG_NOT_AFFECT__(pCPU, Index)
#endif

#include "ChaosVmCpuReadDataPart.h"
#include "ChaosVmCpuReadDispPart.h"
#include "ChaosVmCpuReadImm.h"
#include "ChaosVmCpuInternalFunction.h"

/*
 * ������Ϣ�������,�����ڵ��԰汾ʱ�����Ϣ,
 * ��,����δʵ�ֵ�ָ��ʱ,�˺�����������ʾ���������ϸ��Ϣ
 */
#define __CPU_DBG_OUTPUT__ ChaosVmCpuErrorOutput

#if defined(__cplusplus)
}
#endif

#endif
