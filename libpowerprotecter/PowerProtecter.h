/*
 * ����һ������,�������������ʱ����һƬ�µ��ڴ��������,��������ʱ������Ƭ�ڴ�
 * �����Ľ�����������һ��δ���ܵĺ���P,Pͨ����ϣ����õ�һ��KEY,���KEY���ڽ���
 * ��ǰ�������ĺ���
 * ������������ָ��Ҳ�ܵ����ܱ���,����ָ�������ر���,һ�ر����ǽ�ָ����ȡ����,��
 * ���¼�ṹ,��ԭָ��λ�÷��ÿ��Դ����쳣��ָ��,ָ��ļӽ���ֻͨ��һ���򵥵�KEY��XOR
 * ���㡣����ָ�����ͨ������һƬδ�����ܵ��ڴ�����,��Ƭ����ͨ����ϣ����õ�һ��KEY,
 * ���KEY�����ܵ�ǰָ��
 */
#if !defined(__POWER_PROTECTER_H__)
#define __POWER_PROTECTER_H__

#include "Common.h"
#include "Support.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// �������ܺ����Ķ�������
#define __POWER_PROTECT_ENCRYPT_PROCEDURE_ALIGN__					sizeof(__integer)

// �������������ýṹ
typedef struct _POWER_PROTECTER_CONFIGURE {
	__integer iTmp;
} POWER_PROTECTER_CONFIGURE, *PPOWER_PROTECTER_CONFIGURE;

// ������������Ϣ�ṹ
typedef struct _POWER_PROTECTER_INFO {
	__address addrOrigTargetImageBase;//ԭʼ��Ŀ�����ַ
	__address addrNowTargetImageBase;//��ǰ��Ŀ�����ַ
	__address addrPowerProtecterOrigImageBase;//������������ԭʼ����ַ
	__address addrPowerProtecterNowImageBase;//�����������ĵ�ǰ����ַ
} POWER_PROTECTER_INFO, *PPOWER_PROTECTER_INFO;

/*
 * �˽ṹ����������������������ָ����߼����ڴ���ڴ��ַ,�ļ���ַ,�Ѿ�����
 */
typedef struct _POWER_PROTECTER_PROCEDURE_RECORD {
	union {
		__address addrMemAddress;//�ڴ��ַ
		__offset ofMemRVA;//�ڴ�ƫ��
	};
	__memory pFileAddress;//�ļ���ַ
	__integer iSize;//��������
} POWER_PROTECTER_PROCEDURE_RECORD, POWER_PROTECTER_WATCH_RECORD, POWER_PROTECTER_INSTRUCTION_RECORD, \
  *PPOWER_PROTECTER_PROCEDURE_RECORD, *PPOWER_PROTECTER_WATCH_RECORD, *PPOWER_PROTECTER_INSTRUCTION_RECORD;

// Ҫ������ָ��
#define __POWER_PROTECT_IS_NOT_A_FLOW_INSTRUCTION__			0x80
typedef enum _POWER_PROTECTER_FLOW_TYPE {
	PPFT_NONE = __POWER_PROTECT_IS_NOT_A_FLOW_INSTRUCTION__,
	PPFT_JO = 1,
	PPFT_JNO = 2,
	PPFT_JB = 3,
	PPFT_JAE = 4,
	PPFT_JZ = 5,
	PPFT_JNZ = 6,
	PPFT_JBE = 7,
	PPFT_JA = 8,
	PPFT_JS = 9,
	PPFT_JNS = 10,
	PPFT_JP = 11,
	PPFT_JNP = 12,
	PPFT_JL = 13,
	PPFT_JGE = 14,
	PPFT_JLE = 15,
	PPFT_JG = 16,
	PPFT_JCXZ = 17,
	PPFT_JECXZ = 18,
	PPFT_JRCXZ = 19,
	PPFT_LOOPNE = 20,
	PPFT_LOOPE = 21,
	PPFT_LOOP = 22,
	PPFT_JMP = 23,
	PPFT_CALL = 24,
	PPFT_RET = 25
} POWER_PROTECTER_FLOW_TYPE;

/*
 * �˽ṹ������Ŀ���������,������¼������ָ�����Ϣ
 */
#define __POWER_PROTECTER_MAX_INSTRUCTION_BUFFER_SIZE__				0x20
typedef struct _POWER_PROTECTER_ENCRYPT_INSTRUCTION {
	__bool bOut;//�Ƿ����������ת

	union {
		__address addMemAddress;//�ڴ��ַ
		__offset ofMemRVA;//�ڴ�ƫ��
	};

	__byte InstBuf[__POWER_PROTECTER_MAX_INSTRUCTION_BUFFER_SIZE__];//ָ�� <- ʹ��չ�����
	POWER_PROTECTER_FLOW_TYPE JmpType;//��ʲô���͵���ת <- �����0x80 ��Ϊ����ת
	__bool bIsWatched;//����ָ����������ڴ��ַ
	POWER_PROTECTER_WATCH_RECORD WatchRecord;//Ҫ���ӵ��ڴ� <- ʹ��InstBuf�����ļ���
	__dword dwKey;//���Ҫ��Ҫ���ӵ��ڴ�,��ʹ��ָ������Կ���м���

	/*
	 * Ŀ���ַ��RVAֻ�ڴ���ָ��������ָ�������
	 */
	union {
		__address addrTargetMemAddress;//Ŀ���ڴ��ַ
		__offset ofTargetMemRVA;//Ŀ���ڴ��RVA
	};
	__integer iInstLen;//ָ���
	__integer iRetSize;//��Ҫ���صĲ�������
} POWER_PROTECTER_ENCRYPT_INSTRUCTION, *PPOWER_PROTECTER_ENCRYPT_INSTRUCTION;

// ���ܺ�����Ҫ��Ϣ�ṹ
typedef struct _POWER_PROTECTER_DECRYPT_PROCEDURE {
	POWER_PROTECTER_PROCEDURE_RECORD KeyProcedure;//�����¼�˽������������Ӧ�ĺ���,����ṹʹ�ü��ܺ�����Ϊ��Կ���н���
	//union {
	//	__address addrProcAddress;//����ԭ���ĵ�ַ
	//	__offset ofProcRVA;//����ԭ����RVA
	//};
} POWER_PROTECTER_DECRYPT_PROCEDURE, *PPOWER_PROTECTER_DECRYPT_PROCEDURE;

// �Ĵ�������
typedef struct _POWER_PROTECTER_RUN_CONTEXT {
	__dword dwEFlag;//��־�Ĵ���
	__dword EDI;
	__dword ESI;
	__dword EBP;
	__dword ESP;
	__dword EBX;
	__dword EDX;
	__dword ECX;
	__dword EAX;
} POWER_PROTECTER_RUN_CONTEXT, *PPOWER_PROTECTER_RUN_CONTEXT;

/*
 * �˽ṹ�����ڱ���������Ŀ������,������¼���ܺ�������Ϣ
 */
typedef struct _POWER_PROTECTER_ENCRYPT_PROCEDURE {
	POWER_PROTECTER_PROCEDURE_RECORD KeyProcedure;//�����¼�˽������������Ӧ�ĺ���,����ṹʹ�ü��ܺ�����Ϊ��Կ���н���
	union {
		__address addrProcAddress;//����ԭ���ĵ�ַ
		__offset ofProcRVA;//������RVA
	};
	__integer iTotalSize;//�˽ṹ + �������� �����ĳ���
	__integer iProcSize;//�����������ĳ���,����ĳ���
	__integer iEncryptInstCount;//����ָ�����
	__bool bInit;//�Ƿ��Ѿ���ʼ��
	CRITICAL_SECTION LocalCriticalSection;//��ǰ�̵߳�ͬ������
	__integer iHandleCount;//�������,��Ϊ0ʱ�Ϳ������ٵ����ٵ��ڴ���
	__memory pRunSpace;//���пռ�
	PPOWER_PROTECTER_DECRYPT_PROCEDURE pDecryptProcedure;//���ܽṹ
	//POWER_PROTECTER_RUN_CONTEXT Context;//��¼������ʼʱ��������
	// ���������ܺ�ĺ���
	// �������iEncryptInstCount��POWER_PROTECTER_ENCRYPT_INSTRUCTION�ṹ <- ʹ�ú��������Ľ��м���
} POWER_PROTECTER_ENCRYPT_PROCEDURE, *PPOWER_PROTECTER_ENCRYPT_PROCEDURE;

//#define __MAX_POWER_PROTECT_PROCEDURE_ARGC__				0x20
//#define __MAX_POWER_PROTECTER_STACK_SIZE__					0x2000//8KB
//#define __POWER_PROTECTER_ESP_POINT__						0x1000//4KB
//typedef struct _POWER_PROTECTER_PROCEDURE_RUN_FRAME {
//	__dword dwRetAddress;//���ص�ַ
//	__dword dwArgs[__MAX_POWER_PROTECT_PROCEDURE_ARGC__];//��������
//} POWER_PROTECTER_PROCEDURE_RUN_FRAME, *PPOWER_PROTECTER_PROCEDURE_RUN_FRAME;

//#define __POWER_PROTECTER_DATA_SIGN_1__						0xFB11CA42
//#define __POWER_PROTECTER_DATA_SIGN_2__						0xED45AB33
//typedef struct _POWER_PROTECTER_PROCEDURE_DATA {
//	__dword dwSign[2];//����
//	__dword dwOrigEsp;//ԭESP��ֵ
//	__dword dwNewStackAddress;//�¶�ջ���׵�ַ,�����ͷ�����ṹ
//	PPOWER_PROTECTER_ENCRYPT_PROCEDURE pCurrEncryptProcedure;//ָ��һ�����ܺ����ṹ
//	__dword dwStartEsp;//�¶�ջ��ջ��ESP�ĵ�ַ,��ֵ����Ҫ��Ŀ���Ǽ���Ŀ�꺯���Ĳ�������,��retָ��͵��÷�ʽ�й�ϵ
//} POWER_PROTECTER_PROCEDURE_DATA, *PPOWER_PROTECTER_PROCEDURE_DATA;
//
//typedef struct _POWER_PROTECTER_RUNTIME {					// <- �¶�ջ���
//	POWER_PROTECTER_PROCEDURE_RUN_FRAME RunFrame;//���ж�ջ
//	POWER_PROTECTER_PROCEDURE_DATA Data;//��Ҫ������
//} POWER_PROTECTER_RUNTIME, *PPOWER_PROTECTER_RUNTIME;

// ��ʼ����������,���������
__void __API__ PowerProtecterInit(PPOWER_PROTECTER_CONFIGURE pConfigure, PPOWER_PROTECTER_INFO pInfo);

#if defined(__cplusplus)
}
#endif

#endif
