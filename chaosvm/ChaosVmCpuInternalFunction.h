#if !defined(__CHAOSVM_CPU_INTERNAL_FUCTION_H__)
#define __CHAOSVM_CPU_INTERNAL_FUCTION_H__

/*
 * ���ļ���������ָ��ʵ�ֵĺ���ߺ���
 *
 * ע:
 * 1) ��������λָ�����غ�.
 * 2) ���� ((LONGLONG)0-0x80000000)��
 * VC���ƺ���BUG��ֱ����(LONGLONG)(-0x80000000)�޷���-0x80000000ת��Ϊ64λ�з��Ÿ���
 * (����4146: unary minus operator applied to unsigned type, result still unsigned )
 * 3) �޸ı�־λʱ��AFҪ��CF���޸�ǰ���㣬��Ϊ����AF��Ҫָ��ִ��ʱ��CF
 */
#define __MAKE_UINT16__(uInt8_Low, uInt8_High)		(UINT8_TO_UINT16((uInt8_Low) & 0xFF) | (UINT8_TO_UINT16((uInt8_High) & 0xFF) << 8))
#define __MAKE_UINT32__(uInt16_Low, uInt16_High)    (UINT16_TO_UINT32((uInt16_Low) & 0xFFFF) | (UINT16_TO_UINT32((uInt16_High) & 0xFFFF) << 16))
#define __MAKE_UINT64__(uInt32_Low, uInt32_High)    (UINT32_TO_UINT64((uInt32_Low) & 0xFFFFFFFF) | (UINT32_TO_UINT64((uInt32_High) & 0xFFFFFFFF) << 16))

#define __Low32__(uInt64Value)						(UINT64_TO_UINT32(uInt64Value))
#define __High32__(uInt64Value)						(UINT64_TO_UINT32((uInt64Value) >> 32))

#define __Low16__(uInt32Value)						(UINT32_TO_UINT16(uInt32Value))
#define __High16__(uInt32Value)						(UINT32_TO_UINT16((uInt32Value) >> 16))

#define __Low8__(uInt16Value)						(UINT16_TO_UINT8(uInt16Value))
#define __High8__(uInt16Value)						(UINT16_TO_UINT8((uInt16Value) >> 8))

/*
 * ������λ
 */
#define __UINT8_MSB__(uInt8Value)					((uInt8Value >> 7) & 0x1)
#define __UINT16_MSB__(uInt16Value)					UINT16_TO_UINT8((uInt16Value >> 15) & 0x1)
#define __UINT32_MSB__(uInt32Value)					UINT32_TO_UINT8((uInt32Value >> 31) & 0x1)

/*
 * �����λ
 */
#define __UINT8_MSB_1__(uInt8Value)					((uInt8Value >> 6) & 0x1)
#define __UINT16_MSB_1__(uInt16Value)				UINT16_TO_UINT8((uInt16Value >> 14) & 0x1)
#define __UINT32_MSB_1__(uInt32Value)				UINT32_TO_UINT8((uInt32Value >> 30) & 0x1)

#define __UINT8_LSB__(uInt8Value) 					(uInt8Value & 0x1)
#define __UINT16_LSB__(uInt16Value)					UINT16_TO_UINT8(uInt16Value & 0x1)
#define __UINT32_LSB__(uInt32Value) 				UINT32_TO_UINT8(uInt32Value & 0x1)

/*
 * ָ����HOOK����
 */
#define __ExecuteInstBefore__(pChaosVmCpuArg)\
	if ((pChaosVmCpuArg)->HookRoutine.pExecuteEachInstruction)\
		(pChaosVmCpuArg)->HookRoutine.pExecuteEachInstruction((pChaosVmCpuArg), (pChaosVmCpuArg)->pUserContext);
#define __ExecuteInstAfter__(pChaosVmCpuArg)\
	if ((pChaosVmCpuArg)->HookRoutine.pExecuteEachInstructionCompleted)\
		(pChaosVmCpuArg)->HookRoutine.pExecuteEachInstructionCompleted((pChaosVmCpuArg), (pChaosVmCpuArg)->pUserContext);

#endif
