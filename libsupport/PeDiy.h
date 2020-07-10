/*
 * ����PE�ļ���������
 */
#if !defined(__PEDIY__H__)
#define __PEDIY__H__

#include "Common.h"
#include <Windows.h>
#include "winternl.h"

#if defined(__cplusplus)
extern "C"
{
#endif

/*
 * �꺯��
 */
// ��ȡPEB
#if (MSVC_VER <= 8)
__inline struct _TEB * GetNtCurrentTeb( void ) { __asm mov eax, fs:[0x18] }
#else
#define GetNtCurrentTeb			NtCurrentTeb
#endif
#define __NtCurrentPeb__()		(GetNtCurrentTeb()->Peb)
// ��ȡ��ǰ�Ļ���ַ
#define __GetNowImageBase__()	(GetNtCurrentTeb()->Peb->ImageBaseAddress)
// ��ȡDOSͷ
#define __GetDosHeader__(x)		((PIMAGE_DOS_HEADER)(x))
// ��ȡNTͷ
#define __GetNtHeader__(x)		((PIMAGE_NT_HEADERS)((__dword)__GetDosHeader__(x)->e_lfanew + (__dword)(x)))
// ��ȡ�׽ڽڱ�
#define __GetSectionHeader__(x)	IMAGE_FIRST_SECTION(x)
// RVA��VA
#define __RvaToVa__(base,offset) ((__void *)((__dword)(base) + (__dword)(offset)))
// VA��RVA
#define __VaToRva__(base,offset) ((__void *)((__dword)(offset) - (__dword)(base)))
// Ĭ�ϵ��½�����
#define __DEF_NEWSECTION_CHARACTERISTICS__ IMAGE_SCN_CNT_CODE | IMAGE_SCN_CNT_INITIALIZED_DATA | IMAGE_SCN_MEM_EXECUTE | IMAGE_SCN_MEM_READ | IMAGE_SCN_MEM_WRITE

// �������궨��
#if (MSVC_VER <= 8)
#define IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE				0x0040
#define IMAGE_DLLCHARACTERISTICS_NX_COMPAT					0x0100
#endif


/*
 * ��������
 */
// ��ȡNTͷ
PIMAGE_NT_HEADERS __API__ GetNtHeader(__memory pMem);
// ��ȡͷ�ڱ���ڴ�ӳ���ļ�
PIMAGE_SECTION_HEADER __API__ GetFirstSection(__memory pMem);
// ��ȡͷ�ڱ��NTͷ
PIMAGE_SECTION_HEADER __API__ GetFirstSectionByNtHeader(PIMAGE_NT_HEADERS pNtH);
// ��ȡĩβ��
PIMAGE_SECTION_HEADER __API__ GetTailSection(__memory pMem);
// ��ȡ������
__integer __API__ GetSectionIndex(__memory pMem, __char *pSectionName);
// ��ȡ��ڵ������
__integer __API__ GetEntrySectionIndex(__memory pMem);
// Ԥ���½ڵĽڱ�
__bool __API__ PredictNewSectionHeader(__memory pMem, __dword dwVirtualSize, PIMAGE_SECTION_HEADER pNewSectionHeader);
// ��ȡ��ڵ��
PIMAGE_SECTION_HEADER __API__ GetEntryPointSection(__memory pMem);
// ��ȡ��ڵ��ڴ��ַ
__address __API__ GetEntryPointMemoryAddress(__memory pMem);
// ��ȡ�ڱ��ָ������
PIMAGE_SECTION_HEADER __API__ GetSection(__memory pMem, __char *lpSectionName);
// ��ȡ�ڱ�ӽ�����
PIMAGE_SECTION_HEADER __API__ GetSectionByIndex(__memory pMem, __integer iIndex);
// ���ĳ������Ŀ¼���Ƿ����
PIMAGE_DATA_DIRECTORY __API__ ExistDataDirectory(__memory pMem, __integer iIndex);
// ������н���
__void __API__ RandomAllSectionName(__memory pMem);
// ȷ������Ч��PE��ʽ�ļ�
__bool __API__ IsValidPE32(__memory pMem);
// ȷ���Ƿ���һ��SYS�ļ�
__bool __API__ IsDriverFile(__memory pMem);
// ȷ���Ƿ���һ��DLL�ļ�
__bool __API__ IsDllFile(__memory pMem);
// ��ȡ��ȷ��SizeOfImage
__integer __API__ CorrectImageSize(__memory pMem);
// ȷ��������Ƿ���ʵ������
__bool __API__ IsRawSection(PIMAGE_SECTION_HEADER pSectH);
// ��ȡʱ���
__dword __API__ GetTimeDateStamp(__memory pMem);
// ��ȡ�ڵ�����
__word __API__ GetNumberOfSections(__memory pMem);
// �Ƿ�������������
__bool __API__ InThisSection(PIMAGE_SECTION_HEADER pSectH, __offset ofOffset, __bool bRva);
// �Ƿ�������������,ͨ����ַ�Ƚ�
__bool __API__ InThisSectionByAddress(PIMAGE_SECTION_HEADER pSectH, __address addrImageBase, __address addrAddress);
// ͨ��RVA��ȡ�ڱ�
PIMAGE_SECTION_HEADER __API__ Rva2Section(__memory pMem, __offset ofRva);
// ͨ��RAW��ȡ�ڱ�
PIMAGE_SECTION_HEADER __API__ Raw2Section(__memory pMem, __offset ofRaw);
// RVA ת�� RAW
__offset __API__ Rva2Raw(__memory pMem, __offset ofRva);
// RAW ת�� RVA
__offset __API__ Raw2Rva(__memory pMem, __offset ofRaw);
// �������
__integer __API__ Alig(__integer n, __integer iFactor, __bool bUp);
// ���㰴�ջ���ַ����
#define __IMAGE_BASE_ALIGN__	0x1000//PAGE_SIZE = 40960
__address __API__ AligImageBase(__address n);
// ����������
__void __API__ CopyExportTableTo(PIMAGE_EXPORT_DIRECTORY pExportDir, __offset ofNewRva, __offset ofOldRva);
// ���������
__void __API__ CopyImportTableTo(PIMAGE_IMPORT_DESCRIPTOR pImportDesc, __offset ofNewRva, __offset ofOldRva);
// ������Դ��
__void __API__ CopyResourceTo(PIMAGE_RESOURCE_DIRECTORY pRootResDir, __offset ofNewRva, __offset ofOldRva);
// ���ƻ�ַ�ض�λ��
__void __API__ CopyBaseRelocationTo(PIMAGE_BASE_RELOCATION pBaseRelocation, __offset ofNewRva, __offset ofOldRva, __integer iSize);
// Ѱ����Դͨ������
PIMAGE_RESOURCE_DATA_ENTRY __API__ FindResourceInfo(__memory pMem, __integer iType);
typedef struct _RESOURCE_INFO {
	PIMAGE_RESOURCE_DATA_ENTRY pDataEntry;//���
	__byte *pPoint;//����ָ��
	__integer iPointSize;//���ݳ���
} RESOURCE_INFO, *PRESOURCE_INFO;
// ��ȡ��Դmanifest����
PRESOURCE_INFO __API__ GetManifestResourceInfo(__memory pMem);
// ��ȡ��Դ����ICON����
PRESOURCE_INFO __API__ GetIconResourceInfo(__memory pMem);
// ɾ������Ŀ¼��
__bool __API__ DeleteDataDirectoryObject(__memory pMem, __integer iObject);
__bool __API__ DeleteDataDirectoryObjectOnMemMap(__memory pMem, __integer iObject);
// ����½�
PIMAGE_SECTION_HEADER __API__ AddSection(__memory pMem, __char *Name, __dword Characteristics, __dword VirtualSize, __memory pData, __bool bForce);
// ����ĩβ��ͨ���ڴ�
PIMAGE_SECTION_HEADER __API__ CoverTailSectionFromImage(__memory pMem, __memory *pNewMemPoint, __integer iMemSize, __integer iNewSize, __memory pData, __integer iDataSize, __integer *piNewMemSize);
// ����ĩβ��
PIMAGE_SECTION_HEADER __API__ CoverTailSection(__tchar *pFilePath, __memory *pMemPoint, __integer iNewSize, __memory pData, __integer iDataSize, __integer *piNewMemSize);
// ɾ����
__bool __API__ DelSectionBySectionIndex(__memory pMem, __integer n);
__bool __API__ DelSectionBySectionName(__memory pMem, __char *pName);
__bool __API__ DelSectionBySectionHeader(__memory pMem, PIMAGE_SECTION_HEADER pSectH);
__bool __API__ DelSectionBySectionIndexOnMemMap(__memory pMem, __integer n);
__bool __API__ DelSectionBySectionNameOnMemMap(__memory pMem, __char *pName);
__bool __API__ DelSectionBySectionHeaderOnMemMap(__memory pMem, PIMAGE_SECTION_HEADER pSectH);
// ��ȡ�ļ�
__integer __API__ GetFilePhySize(__tchar *lpszFile);
// ӳ���ļ�
__memory __API__ MappingFile(__tchar *lpszFile, __integer *piFileSize, __bool bWrite, __integer iMaximumSizeHigh, __integer iMaximumSizeLow);
// ����ӳ���ļ������µĳ���
__memory __API__ MappingFilePlusNewSize(__tchar *lpszFile, __memory pMap, __integer iOldSize, __integer iNewPlusSize, __bool bWrite);
// ���ӳ���ļ�
__bool __API__ UnMappingFile(__memory pBaseAddress);
// ��ȡ��ʵ���ļ�����
__integer __API__ GetRealPeFileSize(__memory pMem);
// ��ȡĩβ����ָ��
__memory __API__ GetTailDataPoint(__memory pMem, __integer iFileSize);
// ��ȡĩβ���ݳ���
__integer __API__ GetTailDataSize(__memory pMem, __integer iFileSize);
// ���������ļ�ĩβ
__integer __API__ ResetFileTail(__tchar *lpszFile, __integer iNewTail);
// ��������ӳ�����ӵĳ���,��Ҫ����½�ʱ
__integer __API__ GetAddSectionMapSize(__memory pMem, __integer iNewMap);
// ����һ��PE�ļ�ӳ�䵽��һƬ�ڴ�,2012.2.7����
__bool __API__ CopyMemToMem(__memory pFromMemory, __memory pToMemory, __integer iSizeOfImage);
// ����һ��PE�ļ�ӳ�䵽��һƬ�ڴ沢ʹ���ڴ����
__bool __API__ CopyMemToMemBySecAlign(__memory pFromMemory, __memory pToMemory, __integer iSizeOfImage);
// �����ڴ汣��
__bool __API__ ProtectImage(__memory pImageBase, __bool bWriteAll);
// ���ս����ýڵ��ڴ�����
__bool __API__ SetSectionCharacteristics(__memory pImageBase);
// ���õ�ǰӳ�����нڵ�ĳ������
__void __API__ SetAllSectionCharacteristics(__memory pMem, __dword dwCharacteristics);
// �޸��������
__bool __API__ RefixBoundIAT(__memory pImageBase);
// �޸������
__bool __API__ RefixIAT(__memory pImageBase);
// �ӽ��������
typedef enum _CRYPTIAT_TYPE {
	LIB_NAME = 0,
	PROC_NAME = 1
} CRYPTIAT_TYPE;
typedef __void (__INTERNAL_FUNC__ *FPCryptIAT)(__integer iIndex, CRYPTIAT_TYPE Type, __memory pData, __integer iDataSize, __bool bOrd, __dword dwFirstThunk, __memory pArgs);
// ���������
__bool __API__ EncryptIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescryptor, __address addrImageBase, FPCryptIAT pEncryptIAT, __void *pEncryptFuncArgs, __bool bOnFile);
// ���������
__bool __API__ DecryptIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescryptor, __address addrImageBase, FPCryptIAT pDecryptIAT, __void *pEncryptFuncArgs);
// ���������
#define FPShowImportDetail FPCryptIAT
__bool __API__ ShowIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescryptor, __address addrImageBase, FPShowImportDetail pShowIAT, __void *pShowFuncArgs, __bool bOnFile);
// ͳ��������ĸ���
__integer __API__ CountLibraryInImportTable(__memory pMem, __bool bOnFile);
// ͳ����������ݵĳ���
__integer __API__ CountImportTableDataSize(__memory pMem, __bool bOnFile);
// ����һ�������
PIMAGE_IMPORT_DESCRIPTOR __API__ MakeImportDescriptor(__memory pMem, __integer iLibCount, __integer iHowManyProcInEachLib, __integer *iSize);
// �޸�TLS��
__bool __API__ RefixTLS(__memory pImageBase, __bool bDetach);
// �ض�λ����
__bool __API__ BaseRelocation(__memory pMem, __address addrOldImageBase, __address addrNewImageBase, __bool bIsInFile);
// �ض�λ����,�ض�λ��һ������ַ,д��ʱ��д������һ������ַ
__bool __API__ BaseRelocationAssignImageBaseWriteToOtherImageBase(__address addrWriteTo, __address addrOldImageBase, __address addrNewImageBase, __bool bIsInFile);
// ʹ���ض�λ���޸�ֱ���������ļ�
__bool __API__ BaseRelocationOnFile(__memory pMem, __address addrNewImageBase);
// �ض�λ����,�ض�λ��һ������ַ,д��ʱ��д������һ������ַ д���ַΪ�ļ���
__bool __API__ BaseRelocationAssignImageBaseWriteToOtherImageBaseOnFile(__memory pMem, __address addrNewImageBase);
// HOOK������
FARPROC __API__ HookExportProcAddress(HMODULE hModule, __char *lpProcName, FARPROC fpAddr);
// ���������л�ȡ������ַͨ���ļ�
FARPROC __API__ xGetProcAddressImmediately(HMODULE hDll, __char *pFuncName);
FARPROC __API__ xGetProcAddress(HMODULE hDll, __char *pFuncName);
FARPROC __API__ xGetAddressFromOnFile(__memory pMem, __char *pFuncName, __offset *pofFuncRva);
typedef __integer (__INTERNAL_FUNC__ *FPHashFunc)(__memory pTarget, __integer iTargetSize, __memory pHashValue);
FARPROC __API__ xGetProcAddressByHash(HMODULE hDll, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc);
FARPROC __API__ xGetProcAddressImmediatelyByHash(HMODULE hDll, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc);
FARPROC __API__ xGetProcAddressEx(HMODULE hDll, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA);
// PE������
typedef __bool (__INTERNAL_FUNC__ *FPRefixIAT)(__void *pArg);//�޸��������ԭ��
typedef __void (__INTERNAL_FUNC__ *FPEntryFunction)();//��ڵ�ԭ��
FPEntryFunction __API__ PeLoader(__memory pLoadCode, __memory pOutMemory, __integer iOutMemorySize, __bool bRefixIAT, FPRefixIAT pRefixIAT, __void *pRefixIATArg);

// ��������ӳ��
__bool __API__ PrepareImage(__memory pMemoryImage, __integer iImageSize, __bool bRefFixTls, __bool bRefixIAT, FPRefixIAT pRefixIAT, __void *pRefixIATArg);
// ����DLL
__void __API__ InitDllManager();
__void __API__ DestoryDllManager();
typedef __bool (__API__ *FPDllMain)(HMODULE hinstDLL, __dword fdwReason, __void *lpvReserved);
HMODULE __API__ xLoadLibraryFromMemoryEx(__memory pDllCode, __memory pOutMemory, __dword dwFlags);
HMODULE __API__ xLoadLibraryFromMemory(__memory pDllCode, __tchar *svMappingName, __dword dwFlags);
HMODULE __API__ xLoadLibrary(__tchar *pLibName);
HMODULE __API__ xGetModuleHandle(__tchar *pLibName);
// �ͷ�DLL
__bool __API__ xFreeLibrary(HMODULE hLibModule);
// �滻Ӧ�ó���ICON
__void __API__ ChangedExeIcon(__tchar *lpExeName, __tchar *lpIconFile);
// ��ȡkernel32�Ļ���ַ
HMODULE __API__ GetKernel32BaseAddress();
// ��ȡntdll�Ļ���ַ
HMODULE __API__ GetNtdllBaseAddress();
// ��ȡLoadLibraryA�ĵ�ַ
FARPROC __API__ GetLoadLibraryAAddress();

// ������Դ
#include "OptResource.h"

// ��������
#define __CRC32_LOADLIBRARYA_STR__						0x3FC1BD8D
#define __CRC32_SETUNHANDLEDEXCEPTIONFILTER_STR__		0xCB841A54
#define __CRC32_VIRTUALALLOC_STR__						0x09CE0D4A
typedef HMODULE (WINAPI *FPLoadLibraryA)(__char *lpFileName);
typedef LPVOID (WINAPI *FPVirtualAlloc)(__void *lpAddress, __integer iSize, __dword flAllocationType, __dword flProtect);
typedef LPTOP_LEVEL_EXCEPTION_FILTER (WINAPI *FPSetUnhandledExceptionFilter)(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

/*
 * ��Ƕ����
 */
#include "udis86.h"
#include "hde32/hde32.h"
#include "logic.h"
// ��ӳ��DLL
__memory __API__ RemapDll(__memory pOrigMap, FPVirtualAlloc pVirtualAlloc, __bool bRunDllMain);
// �����ض�λ�޸����
#include "ProcedureRelocation.h"
// ���������
#include "ImpTblProtecter.h"


//////////////////////////////////////////////////////////////////////////
// 2011��3��30���Ժ�׷��
// �������нڿ�д
__void __API__ SetAllSectionCanWrite(__memory pMem);

// ApiSetMap�ṹ
typedef struct _API_SET_MAP_HEADER {
	__dword dwVersionNumber;
	__dword dwNumberOfApiSetModules;
} API_SET_MAP_HEADER, *PAPI_SET_MAP_HEADER;

typedef struct _API_SET_MAP_ENTRY {
	__dword dwNameOfApiSetModuleRVA;
	__dword dwSizeOfName;
	__dword dwHostModulesRVA;
} API_SET_MAP_ENTRY, *PAPI_SET_MAP_ENTRY;

typedef struct _API_SET_MAP_HOST_HEADER {
	__dword dwNumberOfHosts;
} API_SET_MAP_HOST_HEADER, *PAPI_SET_MAP_HOST_HEADER;

typedef struct _API_SET_MAP_HOST_ENTRY {
	__dword dwNameOfImportModuleRVA;
	__dword dwSizeOfImportModuleName;
	__dword dwNameOfHostRVA;
	__dword dwSizeOfHostName;
} API_SET_MAP_HOST_ENTRY, *PAPI_SET_MAP_HOST_ENTRY;
// ��ȡAPISETMAP��ͷ�ṹ
PAPI_SET_MAP_HEADER __API__ GetApiSetMapHeader();
// �ӵ�ǰ��ģ�������ҳ�ָ����ģ��
PLDR_MODULE __API__ GetExistModuleInLoadModuleList(__memory pBaseAddress);
// ���㵱ǰ����ģ�����д��ڵ�ģ������
__integer __API__ GetLoadModuleCount();

#if defined(__cplusplus)
}
#endif

#endif
