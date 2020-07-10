#include "ChaosVmLdr.h"
#include "DISKernel.h"
#include "resource.h"

/*
 * ��Ҫ����ȫ�ֱ���
 */
__EXTERN__ CHAOSVM_LOADER_CONFIGURE g_ChaosVmLdrConfigure = {0};

/*
 * ˽��ȫ�ֱ���
 */
PCHAOSVM_EMULATION_CONFIGURE g_pChaosVmEmulationConfigure = NULL;
PCHAOSVM_EMULATION_BYTECODE_FILE g_pChaosVmEmulationByteCodeFile = NULL;
__address g_addrNowChaosVmLdrImageBase = 0;
__address g_addrOrigChaosVmLdrImageBase = 0;
__address g_addrNowTargetImageBase = 0;
__address g_addrOrigImageBase = 0;
__address g_iOrigSizeOfImage = 0;
__address g_iNowSizeOfImage = 0;
__tchar g_szLocalPath[MAX_PATH] = {0};

__void __INTERNAL_FUNC__ ChaosVmLdrEntry() {
	__tchar szChaosVmEmulationPath[MAX_PATH] = {0};
	__tchar szByteCodePath[MAX_PATH] = {0};
	__memory pByteCodeFileMap = NULL, pByteCode = NULL;
	__integer iByteCodeSize = 0;
	__dword dwByteCodeCrc32 = 0;
	__bool bVerifySign = FALSE;
	PCHAOSVM_RUNTIME pRuntime = NULL;
	
	// ������
	__dword hChaosVmEmulation = NULL;
	FPChaosVmEmulationEntry pChaosVmEmulationEntry = NULL;

	// �����ڲ�������
	__PrintDbgInfo_DebugerInit__(NULL);
	__PrintDbgInfo_DebugerWriteLine__("Entry ChaosVmLdrEntry");

	// ��ȡģ������Ŀ¼
	if (!GetLocalPath(NULL, g_szLocalPath)) {
		// ������
		return;
	}

	// ƴ�ӻ����������������·�����ֽ����·��
	__logic_tcscpy__(szChaosVmEmulationPath, g_szLocalPath);
	__logic_tcscat__(szChaosVmEmulationPath, g_ChaosVmLdrConfigure.szChaosVmEmulationFileName);
	__logic_tcscpy__(szByteCodePath, g_szLocalPath);
	__logic_tcscat__(szByteCodePath, g_ChaosVmLdrConfigure.szByteCodeFileName);

	// ����Ĭ�ϵ���Ϣ
	{
		if (__logic_tcslen__(g_ChaosVmLdrConfigure.szMessageBoxTitle) == 0)
			__logic_tcscpy__(g_ChaosVmLdrConfigure.szMessageBoxTitle, _T("ChaosVm"));

		if (__logic_tcslen__(g_ChaosVmLdrConfigure.szMessageBoxContextOnEmulationFileNotExist) == 0)
			__logic_tcscpy__(g_ChaosVmLdrConfigure.szMessageBoxContextOnEmulationFileNotExist, _T("The chaosvm emulation file not exist"));

		if (__logic_tcslen__(g_ChaosVmLdrConfigure.szMessageBoxContextOnByteCodeFileNotExist) == 0)
			__logic_tcscpy__(g_ChaosVmLdrConfigure.szMessageBoxContextOnByteCodeFileNotExist, _T("The chaosvm byte code file not exist"));
	}

	// ��֤�ֽ����Ƿ����
	if (!ExistFile(szByteCodePath)) {
		if (!ExistFile(szByteCodePath)) {
			MessageBox(NULL, g_ChaosVmLdrConfigure.szMessageBoxContextOnByteCodeFileNotExist, g_ChaosVmLdrConfigure.szMessageBoxTitle, g_ChaosVmLdrConfigure.dwMessageStyle);
			ExitProcess(-1);
		}
	}

//#if defined(__PRINT_DBG_INFO__)
//	{
//		__char printfByteCodePath[MAX_PATH] = {0};
//		__char printfChaosVmEmulationFileName[MAX_PATH] = {0};
//
//		UnicodeToAnsi(szChaosVmEmulationPath, __logic_tcslen__(szChaosVmEmulationPath), printfChaosVmEmulationFileName, MAX_PATH);
//		UnicodeToAnsi(szByteCodePath, __logic_tcslen__(szByteCodePath), printfByteCodePath, MAX_PATH);
//
//		__PrintDbgInfo_DebugerWriteLine__("ChaosVm emulation path:");
//		__PrintDbgInfo_DebugerWriteLine__(printfChaosVmEmulationFileName);
//		__PrintDbgInfo_DebugerWriteLine__("ChaosVm byte code file path:");
//		__PrintDbgInfo_DebugerWriteLine__(printfByteCodePath);
//	}
//#endif

	// ��ȡ�ֽ����ļ�
	pByteCodeFileMap = MappingFile(szByteCodePath, &iByteCodeSize, FALSE, 0, 0);
	if (!pByteCodeFileMap) {
		// ӳ���ֽ����ļ�����
		return;
	}
	__PrintDbgInfo_DebugerWriteLine__("Mapping bytecode file success");

	// ���䱣���ֽ���Ŀռ�
	pByteCode = (__memory)__logic_new_size__(iByteCodeSize);
	if (!pByteCode) {
		// �����ֽ����ڴ����
		UnMappingFile(pByteCodeFileMap);
		return;
	}
	__logic_memcpy__(pByteCode, pByteCodeFileMap, iByteCodeSize);
	UnMappingFile(pByteCodeFileMap);
	__PrintDbgInfo_DebugerWriteLine__("Copy bytecode to decode buffer success");

	// �����ֽ����ļ�
	g_pChaosVmEmulationByteCodeFile = (PCHAOSVM_EMULATION_BYTECODE_FILE)pByteCode;

	// ��֤��ǰ�ֽ����ļ���CRC32ֵ
	bVerifySign = ChaosVmByteCodeFileVerifySign(g_pChaosVmEmulationByteCodeFile, &dwByteCodeCrc32);
	__PrintDbgInfo_DebugerWriteFormatStringInteger__("Bytecode file curr crc32 = ", dwByteCodeCrc32);
	__PrintDbgInfo_DebugerWriteFormatStringInteger__("Bytecode file target crc32 = ", g_pChaosVmEmulationByteCodeFile->dwCrc32);

	if (!bVerifySign) {
		// �ֽ����ļ����޸�
		__logic_delete__(pByteCode);
		return;
	}

	// ���ػ��������������
#if defined(_DEBUG)
	//hChaosVmEmulation = (__dword)LoadLibrary(szChaosVmEmulationPath);
	//if (!hChaosVmEmulation) {
	//	// ���ط�����ʧ��
	//	__logic_delete__(pByteCode);
	//	return;
	//}

#else
	{
		//
		// �ڷ������״̬��
		//
		if (g_ChaosVmLdrConfigure.DebugConfigure.bDebugChaosVm != TRUE) {
			__memory pChaosVmEmulation = NULL, pChaosVmEmulationDecode = NULL;
			__integer iChaosVmEmulationFileSize = 0, iChaosVmEmulationSizeOfImage = 0;
			__address addrChaosVmEmulationImageBase = 0;
			PIMAGE_NT_HEADERS pChaosVmEmulationNtHdr = NULL;

	//////////////////////////////////////////////////////////////////////////
	// �ͷŻ��������������
	#define __ChaosVmLdrEntryFreeChaosVmEmulation__(x){\
		if (g_ChaosVmLdrConfigure.bUseChaosVmEmulationInResource) UnMapResourceData(x);\
		else UnMappingFile(x);}
	//////////////////////////////////////////////////////////////////////////

			if (g_ChaosVmLdrConfigure.bUseChaosVmEmulationInResource) {
				__PrintDbgInfo_DebugerWriteLine__("Load chaosvm emulation from resource");
				// ��ȡ��Դָ��
				pChaosVmEmulation = GetResourcePoint(g_addrNowChaosVmLdrImageBase, IDR_CHAOSVM_EMULATION, _T("BIN"), &iChaosVmEmulationFileSize);
				if (!pChaosVmEmulation) {
					__logic_delete__(pByteCode);
					return;
				}
			} else {
				__PrintDbgInfo_DebugerWriteLine__("Load chaosvm emulation from file");
				// ӳ����������������

				// ����������ļ��Ƿ����,2012.2.12 �޸�
				if (!g_ChaosVmLdrConfigure.bUseChaosVmEmulationInResource) {
					if (!ExistFile(szChaosVmEmulationPath)) {
						MessageBox(NULL, g_ChaosVmLdrConfigure.szMessageBoxContextOnEmulationFileNotExist, g_ChaosVmLdrConfigure.szMessageBoxTitle, g_ChaosVmLdrConfigure.dwMessageStyle);
						ExitProcess(-1);
					}
				}

				pChaosVmEmulation = MappingFile(szChaosVmEmulationPath, &iChaosVmEmulationFileSize, FALSE, 0, 0);
				if (!pChaosVmEmulation) {
					__logic_delete__(pByteCode);
					return;
				}/* end if */
			}

			__PrintDbgInfo_DebugerWriteLine__("Load chaosvm emulation success");

			/*
			 * ���ܻ��������������
			 * ��������ڴ��ʱ��,Ҫ�������ַ�ڴ�������Ⱥ�ĵ�ַ
			 */
			{
				__integer iTotalChaosVmEmulationBufferSize = 0;

				pChaosVmEmulationNtHdr = GetNtHeader(pChaosVmEmulation);
				iChaosVmEmulationSizeOfImage = pChaosVmEmulationNtHdr->OptionalHeader.SizeOfImage;

				iTotalChaosVmEmulationBufferSize = iChaosVmEmulationSizeOfImage + __IMAGE_BASE_ALIGN__;

				pChaosVmEmulationDecode = (__memory)__logic_new_size__(iTotalChaosVmEmulationBufferSize);
				if (!pChaosVmEmulationDecode) {
					// �����ڴ�ʧ��
					__logic_delete__(pByteCode);
					__ChaosVmLdrEntryFreeChaosVmEmulation__(pChaosVmEmulation);
					return;
				}

				// �����Ҫ�������������������ŵ��ڴ��ַ
				addrChaosVmEmulationImageBase = AligImageBase((__address)pChaosVmEmulationDecode);
				__PrintDbgInfo_DebugerWriteLine__("Decode chaosvm emulation success");
			}

			// ͨ���Զ����PE Loader���ص��ڴ�
			{
				FPDllMain pEntry = NULL;

				__PrintDbgInfo_DebugerWriteLine__("Already invoke PeLoader");
				pEntry = (FPDllMain)PeLoader(pChaosVmEmulation, addrChaosVmEmulationImageBase, iChaosVmEmulationSizeOfImage, TRUE, NULL, NULL);
				if (!pEntry) {
					// PE ����������ʧ��
					__logic_delete__(pByteCode);
					return;
				}
				__PrintDbgInfo_DebugerWriteFormatStringInteger__("PeLoader load chaosvm emulation to memory success, chaosvm emulation address = ", addrChaosVmEmulationImageBase);

				// �ͷŻ���������������ṹ
				__ChaosVmLdrEntryFreeChaosVmEmulation__(pChaosVmEmulation);

				// ִ��dllmain
				__PrintDbgInfo_DebugerWriteLine__("Already invoke chaosvm emulation dllmain");
				__PrintDbgInfo_DebugerWriteFormatStringInteger__("ChaosVm emulation dllmain address = ", pEntry);
				if (!pEntry(addrChaosVmEmulationImageBase, DLL_PROCESS_ATTACH, NULL)) {
					__logic_delete__(pByteCode);
					return;
				}/* end if */
			}

			hChaosVmEmulation = (__dword)addrChaosVmEmulationImageBase;
		} else {
			// �ڵ���״̬��
			hChaosVmEmulation = (__dword)LoadLibrary(szChaosVmEmulationPath);
			if (!hChaosVmEmulation) {
				// ���ط�����ʧ��
				__logic_delete__(pByteCode);
				return;
			}/* end if */
		}
	}/* end #else */
#endif

	__PrintDbgInfo_DebugerWriteLine__("Load chaosvm emulation success");

	// ��ȡ�����������Runtime
	pRuntime = (PCHAOSVM_RUNTIME)(g_addrNowTargetImageBase + g_ChaosVmLdrConfigure.ChaosVmLoaderData.ofChaosVmRuntimeRVA);

	// ��ȡ�����������������ں�����ַ
	pChaosVmEmulationEntry = (FPChaosVmEmulationEntry)xGetProcAddress(hChaosVmEmulation, "ChaosVmEntry");
	if (!pChaosVmEmulationEntry) {
		// ��ȡ���溯�����ʧ��
		__logic_delete__(pByteCode);

		// �ͷ��ڴ�.2012.2.9 �޸�
		if (g_ChaosVmLdrConfigure.DebugConfigure.bDebugChaosVm != TRUE) {
			__logic_delete__(hChaosVmEmulation);
		} else {
			FreeLibrary(hChaosVmEmulation);
		}

		return;
	}

	// ������������������
	__PrintDbgInfo_DebugerWriteLine__("Already invoke ChaosVmEmulationEntry");
	pChaosVmEmulationEntry(g_pChaosVmEmulationConfigure, g_pChaosVmEmulationByteCodeFile, pRuntime);
	__PrintDbgInfo_DebugerRelease__();//�ͷ��ڲ�������

	// ������Ϊֹ,�������������������Ҫ�����Ѿ����,�Ժ������һЩ������,�Ѿ��ӽ��ܲ���
}

//#define __CHAOSVM_LOADER_DEBUG_STEP__				1
__void __INTERNAL_FUNC__ DISStep0(__dword hModule, __dword ul_reason_for_call, __void *lpReserved) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
		mov eax, 0
		pop eax
		int 3
	}
#endif
	return;
}

__dword __INTERNAL_FUNC__ DISStep1(__dword hModule, __dword ul_reason_for_call, __void *lpReserved) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 1
			pop eax
			int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep2(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 2
			pop eax
			int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep3(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, __dword *pOutMyDNA) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 3
			pop eax
			int 3
	}
#endif

	if (pOutMyDNA)
		*pOutMyDNA = __CHAOSVM_LOADER_XFILE_DNA__;

	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep4(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, PXFILE_SECTION_HEADER pXFileSectionHeader, PXFILE_SECTION_HEADER pXFileFatherSectionHeader) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 4
			pop eax
			int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep5(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, PXFILE_SECTION_HEADER pXFileSectionHeader, PXFILE_SECTION_HEADER pXFileFatherSectionHeader, PDIS_CONFIGURE pDISConfigure) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 5
			pop eax
			int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep6(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, PXFILE_SECTION_HEADER pXFileSectionHeader, PXFILE_SECTION_HEADER pXFileFatherSectionHeader, PDIS_CONFIGURE pDISConfigure, __memory pDISImageBase, __address addrDISOrigImageBase) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 6
			pop eax
			int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep7(PDIS_CONFIGURE pDISConfigure, __memory pDISImageBase, __address addrDISOrigImageBase) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 7
			pop eax
			int 3
	}
#endif

	// ������Ϳ�������ȫ�ֱ���
	g_addrNowChaosVmLdrImageBase = (__address)pDISImageBase;
	g_addrOrigChaosVmLdrImageBase = addrDISOrigImageBase;
	g_addrOrigImageBase = pDISConfigure->addrOrigImageBase;
	g_iOrigSizeOfImage = pDISConfigure->iOrigSizeOfImage;
	g_iNowSizeOfImage = pDISConfigure->iNowSizeOfImage;

	g_pChaosVmEmulationConfigure = &(g_ChaosVmLdrConfigure.ChaosVmEmulationConfigure);
	g_pChaosVmEmulationConfigure->dwTargetNowSizeOfImage = g_iNowSizeOfImage;
	g_pChaosVmEmulationConfigure->dwTargetOrigSizeOfImage = g_iOrigSizeOfImage;
	g_pChaosVmEmulationConfigure->dwTargetOrigImageBase = g_addrOrigImageBase;

	/*
	 * ���������ѡ�� 2012.2.9 ����
	 */
	__logic_memcpy__(&(g_pChaosVmEmulationConfigure->DebugConfigure), &(g_ChaosVmLdrConfigure.DebugConfigure), sizeof(CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE));
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep8() {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 8
			pop eax
			int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep9(__memory pTargetImageBase) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 9
			pop eax
			int 3
	}
#endif
	g_addrNowTargetImageBase = (__address)pTargetImageBase;

	g_pChaosVmEmulationConfigure->dwTargetNowImageBase = g_addrNowTargetImageBase;
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep10() {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
			mov eax, 10
			pop eax
			int 3
	}
#endif
	// ������������������
	ChaosVmLdrEntry();
	return __DIS_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISStep11(__address addrOrigEntryAddress) {
#if defined(__CHAOSVM_LOADER_DEBUG_STEP__)
	__asm {
		push eax
		mov eax, 11
		pop eax
		int 3
	}
#endif
	return __DIS_ERROR_SUCCESS__;
}

__memory __INTERNAL_FUNC__ DISAddThunkCodeStub(__address addrNowApiAddress) {
	return NULL;
}
