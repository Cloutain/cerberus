#if !defined(__ANALYZEBIN_H__)
#define __ANALYZEBIN_H__

/*
 *
 * !ע:���·�����ʹ�û����ֱ�ӱ���ĳ���,ʶ���ȶ��ϵ�
 * ���ļ�Ϊ��������֡�Ĵ���
 * ���𵥸�����֡����:
 * 1)�ӵ�ǰλ�ÿ�ʼ���з����
 * 2)��¼������ת����ָ��
 * 3)����ret����retnָ���,����������ת��������
 * 4)����ҵ�jcc���ж��Ƿ���ת��ĵ�ַ�Ƿ���ڵ�ǰ��ָ���ַ
 * 5-1)����,����ת������1,����Ԥ������β����
 * 5-2)�޴��ڵ����,�˺����������,����
 * �����Ϻ������ͳ�ΪA����
 * Ԥ������β:
 * ���������֡Ҳ������ɲ��ȶ�,��Щ������ret֮����һЩָ��
 * ����Щָ��֮��ض���һ��JMP��ת�����һ��ret����֮ǰ
 * ���ô�������Ԥ������β
 * �������3�����
 * 1)�ҵ������Ǹ�����JMP�򽫴���Ϊ��β
 * 2)����ҵ�nop��Ϊ��β(�û�����ָ����ѡ��,Ĭ�ϴ���)
 * 3)����ҵ�int3��Ϊ��β(�û�����ָ����ѡ��,Ĭ�ϴ���)
 * 4)����ҵ�0��Ϊ��β(�û�����ָ����ѡ��,Ĭ�ϴ���)
 *
 * 
 * �����������ļ��ĺ���֡����:
 * 1)��PE�ļ���ȡ����ڵ�
 * 2)�����λ�ÿ�ʼ�������з���,ֱ���������ں���
 * 3)����ں�������ת����������,����Ѱ��CALL��������������תָ��
 * 4)�Ҵ�һ��������ڲ���ʼ�ݹ��������
 * 5)ֱ��������̼������
 *
 * ���˴�һ��������������к�����,�����������л�������û��ͨ����תָ��
 * ������ĺ���,�󲿷ֵ�ԭ������Ϊ��̬��ת
 * ���ֺ�������ΪB,B��������,(��׼����ȡ)
 * ʶ��B����������
 * 1)���������ڱ�,��ӵ��ִ��Ȩ�޵Ľ�ȫ���ҳ���
 * 2)��A����������ʹ�ú���ͷ�ڴ��ַ���дӵ͵��ߵ�����
 * 3)ͨ��A��������Щɸѡ���Ĵ�������Ա�,�ص�����A������û���漰���ĵط�
 * 4)����Щ�ڴ����Ѱ��,Ѱ�ҷ���������
 * 5-1)ͨ����������ͷ������(�ռ����ֱ����������ѡ����ɵĺ���ͷ),�ҵ�����뵥������֡��������
 *     �������������ɸѡ��ͨ��,ͨ��Ԥ�жϴ˳�����ʲô���������ɶ��趨��,���û�м�����Ǻ��ֱ�����
 *     �����α���������
 * 5-2)ͨ��Ѱ��NOP,CC�����Եı�������ɵĶ����������ֺ�����
 * 6)���ֳ��Ĵ������������֡����,����ڷ����Ĺ�����,���������ʧ��(��Ч��IA32����),�򽫴���Ϊ
 *   ������д���
 *
 *
 * ����������
 * ��һ�����
 * 1)���ζ�����ָ����з���,�趨ָ���Ƿ���Խ���,�����г���HOOK������̽,��ͨ��ƫ�Ƶ�
 *   ����תָ�����ʶ��,������HOOK�ɹ���,���������ָ��Ա�־�Ĵ�����Ӱ���ͨ�üĴ�����Ӱ��
 * 2)ͨ��IsProcEnd(�����Ѿ����������㷨)����ȷ����ں����ĳ���
 * 3)�ٴα�����ں���,ͨ��������ں�����CALLָ��,��������������
 * 4)ѭ��1,2����ֱ����ӳ�������еĺ������������
 * 5)������������(��ַ�ɵ͵���)
 * 6)��������ӳ���ڴ��ɸߵ������ν��жԱ�,�ҳ�δ�ҵ��Ĵ����,��ʹ��,1,2�������м���
 *   ���޷������Ĵ���ʶΪ����
 *
 * �ڶ������
 * 1)����������
 * 2)��ʶ�������,�����һ���������ٵľֲ���������Ŀռ�,�������sub esp, XXXָ��
 * 3)�ڱ������������������ָ��ֱ�Ӷ�ȫ�ֱ�����������,��¼����ʵ�ַ
 * 4)���������תָ��,��������Ŀ���ַָ�����ת��������
 * 
 * ���������
 * 1)����������
 * 2)�ҵ����ж�̬��תָ��,���������з�������,ֱ���ҳ���̬��ת�Ĵ���ֵ�ĺϳɽ��,����������
 *   ���ﷴ���Ľ����㷨��
 *   �鿴��ǰ��תָ��,�Ƿ�������ָ��
 *   �������������������
 *   ���û�������Ϸ���1��ָ��,���û�д��ڿ���Ӱ��Ĵ�����mov,leaָ��,��ֱ���Ӵ�ָ��Ϊ����
 *   HOOK��ָ��,�����̬��ת����ʹ�������Ĵ������ڴ���ת��ֱ����Ϊ����HOOK��ָ��
 *
 * B���ͺ�����������
 * ֱ�ӽ�B����Ϊ��ʹ�ò���(������϶��ʹ��),�п��ܻ��IAT���������ƻ�,������IAT������ת�������
 * ����IAT����ת�ṹ
 * ��Щ��������ִ��IAT���ʱ��,�Ὣ���뵽IAT�Ĳ��ֵ���ʹ��һ������������
 * ���ڱ�������ʹ��call dword ptr [stub_iat_address]�����
 * ��ת�����Ϊjmp dword ptr [iat_address]
 * iat_addressָ��������API��ַ
 *
 */
#include "Common.h"
#include "logic.h"
#include "udis86.h"
#include "ValidAddress.h"
#include "CpuContext.h"
#include "ImportTableReference.h"
#include "x86Instruction.h"
#include "Proecdure.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// �������ýṹ
#define __DEF_VALID_ADDR_LEVEL__				CHK_IN_USER_MEM
#define __DEF_CODE_MIX_SIZE__					6
typedef struct _ANALYZE_CONFIGURE {
	__bool bEnableDiscoveryUnknowZoon;//�Ƿ�����δ֪�����Ե�̽��
	__bool bNopIsNotEnd;//����ʱ����NOPָ����Ϊ��������
	__bool bZeroIsNotEnd;//����ʱ����0�ַ���Ϊ��������
	__bool bInt3IsNotEnd;//����ʱ����CCָ����Ϊ��������
	CHECK_VALID_ADDR_LEVEL ValidAddressLevel;//�û�ָ������Ч��ַ����
	__byte bCodeMixSize;//��С�Ĵ��볤��,���ڴ˳���,ֱ����Ϊ����
	__bool bDisInvalidOpcodeIsData;//�ر������Ч�����������ֱ����Ϊ����
	__bool bEnableImportTableReference;//������������÷���
	__address addrFromHereStartAnalyze;//ָ����ڵ���з���
} ANALYZE_CONFIGURE, *PANALYZE_CONFIGURE;

/*
 * ����ָ��
 */
typedef __integer (__API__ *FPRva2Raw)(__memory pMem, __integer iRva);
typedef __integer (__API__ *FPRaw2Rva)(__memory pMem, __integer iRaw);
typedef __void * (__API__ *FPGetCodeSectionHeader)(__memory pMem);
typedef __void * (__API__ *FPGetNtHeader)(__memory pMem);

// ������Ϣ�ṹ
typedef struct _ANALYZE_BASIC_INFO {
	__memory pMem;//�ļ�ͷָ��
	__memory pCodeSeg;//ָ����������
	__memory pTailData;//ָ��ĩβ��������

	__address ImageBase;//���ػ���ַ
	__address addrCodeSegMemoryAddress;//������ڴ��ַ

	__integer iBinSize;//�ļ�����
	__integer iCodeSegSize;//����������
	__integer iTailDataSize;//ĩβ���ݳ���
	__integer iEntryRva;//ĩβ���ݳ���

	FPRva2Raw pRva2Raw;
	FPRaw2Rva pRaw2Rva;
	FPGetCodeSectionHeader pGetCodeSectionHeader;
	FPGetNtHeader pGetNtHeader;
} ANALYZE_BASIC_INFO, *PANALYZE_BASIC_INFO;

// ����ṹ
typedef struct _PROGRAM {
	__memory pMem;//�ļ�ͷָ��
	__memory pCodeSeg;//ָ����������
	__memory pTailData;//ָ��ĩβ��������

	__address ImageBase;//���ػ���ַ
	__address addrCodeSegMemoryAddress;//������ڴ��ַ

	__integer iBinSize;//����Ī�����ݺ�ĳ���
	__integer iCodeSegSize;//����������
	__integer iTailDataSize;//ĩβ���ݳ���
	__integer iEntryRva;//ĩβ���ݳ���

	PPROCEDURE pEntryProcedure;//��ں���ָ��
	PPROCEDURE pProcedureList;//��������,������Ҫ�ͷ��ڴ�

	__integer iProcCount;//��������
	__integer iAllBlockCount;//���еĿ����
	__integer iInlineProcCount;//������������

	PIMPORT_TABLE_REFERENCE pImportTableReference;//���������

	ANALYZE_CONFIGURE AnalyzeConfigure;//��������
} PROGRAM, *PPROGRAM;

#define __GetInstructionJmpParents__(pInstJmp)						((PX86INSTRUCTION)((pInstJmp)->pParents))
#define __GetInstructionParents__(pInstruction)						((PPROCEDURE)((pInstruction)->pParents))
#define __GetProcedureParents__(pProcedure)							((PPROGRAM)((pProcedure)->pParents))
#define __GetProgramFromInstruction__(pInstruction)					(__GetProcedureParents__(__GetInstructionParents__((pInstruction))))
#define __GetValidAddressLevelFromInstruction__(pInstruction)		(__GetProgramFromInstruction__((pInstruction))->AnalyzeConfigure.ValidAddressLevel)
#define __GetValidAddressLevelFromInstructionJmp__(pInstJmp)		(__GetValidAddressLevelFromInstruction__(__GetInstructionJmpParents__((pInstJmp))))

__void __API__ ReleaseProgram(PPROGRAM *pProgramPoint);
PPROGRAM __API__ AnalyzeProgram(PANALYZE_CONFIGURE pAnalyzeConfigure, PANALYZE_BASIC_INFO pAnalyzeBasicInfo);
PX86INSTRUCTION __API__ FindInstructionByAddress(PPROGRAM pProgram, __address addrInstruction);
PPROCEDURE __API__ FindProcedureByAddress(PPROGRAM pProgram, __address addrProcedure);
PPROCEDURE __API__ FindProcedureByInsideAddress(PPROGRAM pProgram, __address addrAddress);

#if defined(__cplusplus)
}
#endif

#endif

