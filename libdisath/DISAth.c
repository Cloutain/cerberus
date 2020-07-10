#include "DISAth.h"

/*
 * �꺯��
 */
// �����躯������ֵ
#define __HandleDISAthStepReturnValue__(n, x)				{\
	if (!(__DISATH_SUCCESS__(x))) return x;\
	else if ((x & __DISATH_ERROR_SUCCESS_GOTO__) == __DISATH_ERROR_SUCCESS_GOTO__) {\
		if ((x == __DISATH_ERROR_SUCCESS_GOTO_0__) && (n != 0)) goto _call_back_0;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_1__) && (n != 1)) goto _call_back_1;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_2__) && (n != 2)) goto _call_back_2;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_3__) && (n != 3)) goto _call_back_3;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_4__) && (n != 4)) goto _call_back_4;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_5__) && (n != 5)) goto _call_back_5;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_6__) && (n != 6)) goto _call_back_6;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_7__) && (n != 7)) goto _call_back_7;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_8__) && (n != 8)) goto _call_back_8;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_9__) && (n != 9)) goto _call_back_9;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_A__) && (n != 10)) goto _call_back_10;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_B__) && (n != 11)) goto _call_back_11;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_C__) && (n != 12)) goto _call_back_12;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_D__) && (n != 13)) goto _call_back_13;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_E__) && (n != 14)) goto _call_back_14;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_F__) && (n != 15)) goto _call_back_15;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_10__) && (n != 16)) goto _call_back_16;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_11__) && (n != 17)) goto _call_back_17;\
		else if ((x == __DISATH_ERROR_SUCCESS_GOTO_12__) && (n != 18)) goto _call_back_18;\
	}\
}

__dword __API__ DISAthAttach(PDISATH_CONFIGURE pConfigure) {
	PXFILE_ATTACH_INFO pXFileAttachInfo = NULL;
	PXFILE_HEADER pXFileHeader = NULL;
	PIMAGE_SECTION_HEADER pXFileNtSectionHeader = NULL;
	__integer iXSectionSize = 0;
	PIMAGE_NT_HEADERS pTargetNtHeader = NULL;
	PIMAGE_NT_HEADERS pDISNtHeader = NULL;
	__memory pDIS = NULL;
	__integer iDISSize = 0;
	PXFILE_SECTION_HEADER pXSection = NULL;
	PDIS_CONFIGURE pDISConfigure = NULL;
	__bool bDISFromResource = FALSE;
	__dword dwStepReturn = __DISATH_ERROR_SUCCESS__;

	// ��ʼ������
	pXFileAttachInfo = &(pConfigure->XFileAttachInfo);

	// ִ��0�Ų���
_call_back_0:
	dwStepReturn = DISAthStep0(pConfigure);
	__HandleDISAthStepReturnValue__(0, dwStepReturn);

	// ��ӳ��
	if (!XFileAttachInfoInit(pXFileAttachInfo)) {
		return __DISATH_ERROR_MAPPING_FILE_FAILED__;
	}

	// ִ��1�Ų���
_call_back_1:
	dwStepReturn = DISAthStep1(pXFileAttachInfo, pConfigure);
	__HandleDISAthStepReturnValue__(1, dwStepReturn);
	
	// ���X�ļ�
	pXFileNtSectionHeader = XFileAddXFileToPEFile(pXFileAttachInfo, 0x1000, 5);
	if (!pXFileNtSectionHeader) {
		if (pConfigure->bIfXFileExistFailed)
			return __DISATH_ERROR_ADD_XFILE_FAILED__;
		else {
			// ��ȡX�ļ�
			pXFileNtSectionHeader = XFileExistXFileInPEFile(pXFileAttachInfo->pMem);
			if (!pXFileNtSectionHeader)
				return __DISATH_ERROR_ADD_XFILE_FAILED__;
		}/* end else */
	}

	// ִ��2�Ų���
_call_back_2:
	dwStepReturn = DISAthStep2(pXFileAttachInfo, pXFileNtSectionHeader);
	__HandleDISAthStepReturnValue__(2, dwStepReturn);

	// ��ȡĿ���ļ��Ļ�����Ϣ
	pTargetNtHeader = GetNtHeader(pXFileAttachInfo->pMem);

	// ִ��3�Ų���
_call_back_3:
	dwStepReturn = DISAthStep3(pXFileAttachInfo, pTargetNtHeader);
	__HandleDISAthStepReturnValue__(3, dwStepReturn);

	// ��ʼ����ؼ�������
	{
		XFILE_SECTION_HEADER NewXFileSection = {0};
		__address addrDISImageBaseInTarget = 0;
		__offset ofDISOffsetByXSectionBody = 0;
		__integer iDISSizeOfImage = 0;
		__memory pDISInTarget = NULL;
		__offset ofDISConfigureRVA = 0;

		// Ԥ�ȼ����µ�X��
		if (!XFilePredictNewSectionHeader(pXFileAttachInfo->pMem, &NewXFileSection)) {
			return __DISATH_ERROR_PREDICT_NEW_XSECTION_FAILED__;
		}

		// ִ��4�Ų���
_call_back_4:
		dwStepReturn = DISAthStep4(pXFileAttachInfo, &NewXFileSection);
		__HandleDISAthStepReturnValue__(4, dwStepReturn);

		// ��ȡDIS��Ŀ���ļ��еĻ���ַ
		addrDISImageBaseInTarget = pTargetNtHeader->OptionalHeader.ImageBase + NewXFileSection.dwVirtualAddress;
		addrDISImageBaseInTarget = AligImageBase(addrDISImageBaseInTarget);
		ofDISOffsetByXSectionBody = addrDISImageBaseInTarget - (pTargetNtHeader->OptionalHeader.ImageBase + NewXFileSection.dwVirtualAddress);

		// ִ��5�Ų���
_call_back_5:
		dwStepReturn = DISAthStep5(pXFileAttachInfo, addrDISImageBaseInTarget, ofDISOffsetByXSectionBody);
		__HandleDISAthStepReturnValue__(5, dwStepReturn);

		// ӳ��DIS
		bDISFromResource = (__HiWord__(pConfigure->szDISFilePath) == 0);
		if (bDISFromResource) {
			pDIS = MapResourceDataEx(NULL, pConfigure->wResourceID, pConfigure->szResourceType, &iDISSize, pConfigure->dwDISResourceKey);
			if (!pDIS) {
				return __DISATH_ERROR_MAP_DIS_FROM_RESOURCE_FAILED__;
			}
		} else {
			__memory t = NULL;
			pDIS = MappingFile(pConfigure->szDISFilePath, &iDISSize, FALSE, 0, 0);
			if (!pDIS) {
				return __DISATH_ERROR_MAP_DIS_FROM_FILE_FAILED__;
			}
			t = (__memory)__logic_new_size__(iDISSize);
			__logic_memcpy__(t, pDIS, iDISSize);
			UnMappingFile(pDIS);
			pDIS = t;
		}

		// ִ��6�Ų���
_call_back_6:
		dwStepReturn = DISAthStep6(pXFileAttachInfo, pDIS, iDISSize);
		__HandleDISAthStepReturnValue__(6, dwStepReturn);

		// �������X�ڵĳ���
		pDISNtHeader = GetNtHeader(pDIS);
		iDISSizeOfImage = pDISNtHeader->OptionalHeader.SizeOfImage;
		iXSectionSize = iDISSizeOfImage + ofDISOffsetByXSectionBody;

		// ִ��7�Ų���
_call_back_7:
		dwStepReturn = DISAthStep7(pXFileAttachInfo, pDIS, iDISSize, iDISSizeOfImage, ofDISOffsetByXSectionBody, &iXSectionSize);
		__HandleDISAthStepReturnValue__(7, dwStepReturn);

		// ����DIS�������������ڴ�ƫ��(RVA)
		{
			__memory x = NULL;
			__memory y = NULL;

			x = (__memory)__logic_new_size__(iDISSizeOfImage);
			CopyMemToMemBySecAlign(pDIS, x, iDISSizeOfImage);
			y = (__memory)xGetProcAddressImmediately((HMODULE)x, "g_DISConfigure");
			if (!y) {
				return __DISATH_ERROR_GET_DISCONFIGURE_FAILED__;
			}
			ofDISConfigureRVA = (__offset)(y - x);
			__logic_delete__(x);
		}

		// ���һ����X��
		pXSection = XFileAddXFileSectionByNtSectionHeader(pXFileAttachInfo, pXFileNtSectionHeader, iXSectionSize, __XFILE_SECTION_CHARACTERISTICS_PE__, (__dword)ofDISOffsetByXSectionBody, ofDISConfigureRVA);
		if (!pXSection) {
			return __DISATH_ERROR_ADD_XSECTION_FAILED__;
		}

		// ִ��8�Ų���
_call_back_8:
		dwStepReturn = DISAthStep8(pXFileAttachInfo, pDIS, iDISSize, pXFileNtSectionHeader, pXSection, pXSection->dwDNA, ofDISConfigureRVA);
		__HandleDISAthStepReturnValue__(8, dwStepReturn);

		// ���»�ȡĿ���ļ���NTͷ
		pTargetNtHeader = GetNtHeader(pXFileAttachInfo->pMem);

		// ִ��9�Ų���
_call_back_9:
		dwStepReturn = DISAthStep9(pXFileAttachInfo, pTargetNtHeader);
		__HandleDISAthStepReturnValue__(9, dwStepReturn);

		// DIS��Ŀ���ļ��е�λ��
		pDISInTarget = pXFileAttachInfo->pMem + pXSection->dwPointerToRawData + ofDISOffsetByXSectionBody;

		// ���ڴ�ӳ�临��yImportTable
		CopyMemToMemBySecAlign(pDIS, pDISInTarget, iDISSizeOfImage);

		// ִ��10�Ų���
_call_back_10:
		dwStepReturn = DISAthStep10(pXFileAttachInfo, pDIS, pDISInTarget, iDISSizeOfImage);
		__HandleDISAthStepReturnValue__(10, dwStepReturn);

		// �ͷ�DIS
		if (bDISFromResource)
			UnMapResourceData(pDIS);
		else
			__logic_delete__(pDIS);
		
		// ��ȡDIS�����ýṹ
		pDISConfigure = (PDIS_CONFIGURE)xGetProcAddressImmediately((HMODULE)pDISInTarget, "g_DISConfigure");
		if (!pDISConfigure)
			return __DISATH_ERROR_GET_DISCONFIGURE_FAILED__;

		// ����DIS
		__logic_memset__(pDISConfigure, 0, sizeof(DIS_CONFIGURE));
		pDISConfigure->ofOrigEntryRVA = pXFileAttachInfo->OrigTargetNtHeader.OptionalHeader.AddressOfEntryPoint;
		pDISConfigure->addrOrigImageBase = pXFileAttachInfo->OrigTargetNtHeader.OptionalHeader.ImageBase;
		pDISConfigure->iOrigSizeOfImage = pXFileAttachInfo->OrigTargetNtHeader.OptionalHeader.SizeOfImage;
		pDISConfigure->iNowSizeOfImage = pTargetNtHeader->OptionalHeader.SizeOfImage;
		pDISConfigure->bFirstRun = TRUE;
		pDISConfigure->bProtectMyIAT = pConfigure->bProtectMyIAT;
		pDISConfigure->bProtectTargetIAT = pConfigure->bProtectTargetIAT;

		// ����DIS�Լ�����������ṹ
		pDISConfigure->DISImportProtect.addrImageBase = addrDISImageBaseInTarget;//����DIS��Ŀ���еĻ���ַ,�����DLL��ô�������ַ�Ǳ䶯��

		pDISConfigure->DISImportProtect.iProtectDllCrc32Count = pConfigure->iDISProtectDllCrc32Count;
		__logic_memcpy__(&(pDISConfigure->DISImportProtect.dwProtectDllCrc32List), &(pConfigure->dwDISProtectDllCrc32List), sizeof(__dword) * pDISConfigure->DISImportProtect.iProtectDllCrc32Count);

		pDISConfigure->DISImportProtect.iProtectApiCrc32Count = pConfigure->iDISProtectApiCrc32Count;
		__logic_memcpy__(&(pDISConfigure->DISImportProtect.dwProtectApiCrc32List), &(pConfigure->dwDISProtectApiCrc32List), sizeof(__dword) * pDISConfigure->DISImportProtect.iProtectApiCrc32Count);

		pDISConfigure->DISImportProtect.iProtectDllCallDllMainCrc32Count = pConfigure->iDISProtectDllCallDllMainCrc32Count;
		__logic_memcpy__(&(pDISConfigure->DISImportProtect.dwProtectDllCallDllMainCrc32List), &(pConfigure->dwDISProtectDllCallDllMainCrc32List), sizeof(__dword) * pDISConfigure->DISImportProtect.iProtectDllCallDllMainCrc32Count);

		// ����Ŀ���ļ�����������ṹ
		pDISConfigure->TargetImportProtect.addrImageBase = pTargetNtHeader->OptionalHeader.ImageBase;//����Ŀ���ļ��Ļ���ַ,���Ŀ���ļ���DLL���߿��������ַ��ӳ��,��ô���ֵ�Ǳ䶯��

		pDISConfigure->TargetImportProtect.iProtectDllCrc32Count = pConfigure->iTargetProtectDllCrc32Count;
		__logic_memcpy__(&(pDISConfigure->TargetImportProtect.dwProtectDllCrc32List), &(pConfigure->dwTargetProtectDllCrc32List), sizeof(__dword) * pDISConfigure->TargetImportProtect.iProtectDllCrc32Count);

		pDISConfigure->TargetImportProtect.iProtectApiCrc32Count = pConfigure->iTargetProtectApiCrc32Count;
		__logic_memcpy__(&(pDISConfigure->TargetImportProtect.dwProtectApiCrc32List), &(pConfigure->dwTargetProtectApiCrc32List), sizeof(__dword) * pDISConfigure->TargetImportProtect.iProtectApiCrc32Count);

		pDISConfigure->TargetImportProtect.iProtectDllCallDllMainCrc32Count = pConfigure->iTargetProtectDllCallDllMainCrc32Count;
		__logic_memcpy__(&(pDISConfigure->TargetImportProtect.dwProtectDllCallDllMainCrc32List), &(pConfigure->dwTargetProtectDllCallDllMainCrc32List), sizeof(__dword) * pDISConfigure->TargetImportProtect.iProtectDllCallDllMainCrc32Count);

		// ����DIS�������
		{
			PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceDISImportTableAddress = NULL;
			__offset ofDISNewAddressTableRva = 0;
			__offset ofDISNewAddressTableRvaByDIS = 0;
			__memory pDISNewImportTable = NULL;

			// ��ȡDIS�������ַ����Ŀ���е�λ��
			pDISNewImportTable = (__memory)&(pDISConfigure->addrDISNewImportTable);
			// ������������ַ��������ֵ
			{
				__integer i = 0;
				__address *pAddrDISNewImportTable = NULL;

				for (i = 0; i < __MAX_JMP_ADDRESS_TABLE_COUNT__ / sizeof(__address); i++) {
					pAddrDISNewImportTable = (__address *)((__memory)pDISNewImportTable + i * sizeof(__address));
					*pAddrDISNewImportTable = (__address)GenerateRandomNumber();
				}
			}

			// ��ȡDIS�µ������ַ����Ŀ���ַ�е�ƫ��

			/*
			 * ���¼�����������µ������ַ����Ŀ��������ַ��RVA,������������ʱ��������DIS��RVAƫ�Ƽ����,��������������
			 * ��������˰���,�ش˱��,�������Ǽ�������ṩ���ص�����ʹ��
			 * ofDISNewAddressTableRva = pXSection->dwVirtualAddress + ofDISOffsetByXSectionBody + (pDISNewImportTable - pDISInTarget);
			 */
			ofDISNewAddressTableRva = pXSection->dwVirtualAddress + ofDISOffsetByXSectionBody + (pDISNewImportTable - pDISInTarget);
			ofDISNewAddressTableRvaByDIS = (__offset)(pDISNewImportTable - pDISInTarget);

			// ִ��11�Ų���
_call_back_11:
			dwStepReturn = DISAthStep11(pXFileAttachInfo, pDISInTarget, pDISConfigure, ofDISNewAddressTableRva);
			__HandleDISAthStepReturnValue__(11, dwStepReturn);

			if (pDISConfigure->bProtectMyIAT) {
				// ���»�ȡDIS�Ļ���ַ
				pDISNtHeader = GetNtHeader(pDISInTarget);
				// ��ʹ��__USE_DEF_IMAGEBASE_AS_ORIG__,ֱ��ʹ��ָ���Ļ���ַ
				pReferenceDISImportTableAddress = PowerEncryptImportTable(pDISInTarget, pDISNtHeader->OptionalHeader.ImageBase, &(pDISConfigure->DISImportProtect), FALSE, ofDISNewAddressTableRvaByDIS);
				if (!pReferenceDISImportTableAddress) {
					return __DISATH_ERROR_ENCRYPT_DIS_IMPORT_TABLE_FAILED__;
				}/* end if */
			}

			// ִ��12�Ų���
_call_back_12:
			dwStepReturn = DISAthStep12(pXFileAttachInfo, pDISInTarget, pDISConfigure, pReferenceDISImportTableAddress);
			__HandleDISAthStepReturnValue__(12, dwStepReturn);

			ReleaseReferenceImportTableAddress(&pReferenceDISImportTableAddress);
		}

		// ����Ŀ���ļ��������
		{
			PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceTargetImportTableAddress = NULL;
			__offset ofTargetNewAddressTableRva = 0;
			__memory pTargetNewImportTable = NULL;

			// ��ȡ�������ַ����Ŀ���е�λ��
			pTargetNewImportTable = (__memory)&(pDISConfigure->addrTargetNewImportTable);
			// ������������ַ��������ֵ
			{
				__integer i = 0;
				__address *pAddrTargetNewImportTable = NULL;

				for (i = 0; i < __MAX_JMP_ADDRESS_TABLE_COUNT__ / sizeof(__address); i++) {
					pAddrTargetNewImportTable = (__address *)((__memory)pTargetNewImportTable + i * sizeof(__address));
					*pAddrTargetNewImportTable = (__address)GenerateRandomNumber();
				}
			}

			// ��ȡ�µ������ַ����Ŀ���ַ�е�ƫ��
			/*
			 * ��Ϊ�µ������ַ������DIS��������,����������������ȷ��
			 */
			ofTargetNewAddressTableRva = pXSection->dwVirtualAddress + ofDISOffsetByXSectionBody + (pTargetNewImportTable - pDISInTarget);

			// ִ��13�Ų���
_call_back_13:
			dwStepReturn = DISAthStep13(pXFileAttachInfo, pDISInTarget, pDISConfigure, ofTargetNewAddressTableRva);
			__HandleDISAthStepReturnValue__(13, dwStepReturn);

			if (pDISConfigure->bProtectTargetIAT) {
				pReferenceTargetImportTableAddress = PowerEncryptImportTable(pXFileAttachInfo->pMem, __USE_DEF_IMAGEBASE_AS_ORIG__, &(pDISConfigure->TargetImportProtect), TRUE, ofTargetNewAddressTableRva);
				if (!pReferenceTargetImportTableAddress) {
					return __DISATH_ERROR_ENCRYPT_TARGET_IMPORT_TABLE_FAILED__;
				}
			}

			// ִ��14�Ų���
_call_back_14:
			dwStepReturn = DISAthStep14(pXFileAttachInfo, pDISInTarget, pDISConfigure, pReferenceTargetImportTableAddress);
			__HandleDISAthStepReturnValue__(14, dwStepReturn);

			ReleaseReferenceImportTableAddress(&pReferenceTargetImportTableAddress);
		}

		// ִ��15�Ų���
_call_back_15:
		dwStepReturn = DISAthStep15(pXFileAttachInfo, pDISInTarget, pDISConfigure);
		__HandleDISAthStepReturnValue__(15, dwStepReturn);

		// �޸�Ŀ�꺯������ڵ�
		{
			__memory pDISEntryProc = NULL;

			if (pXFileAttachInfo->bImageBaseRelocate)
				pDISEntryProc = (__memory)xGetProcAddressImmediately((HMODULE)pDISInTarget, "DISEntryInDll");
			else
				pDISEntryProc = (__memory)xGetProcAddressImmediately((HMODULE)pDISInTarget, "DISEntry");

			// �޸�
			pTargetNtHeader->OptionalHeader.AddressOfEntryPoint = pXSection->dwVirtualAddress + ofDISOffsetByXSectionBody + (pDISEntryProc - pDISInTarget);

			// ִ��16�Ų���
_call_back_16:
			dwStepReturn = DISAthStep16(pXFileAttachInfo, pTargetNtHeader);
			__HandleDISAthStepReturnValue__(16, dwStepReturn);
		}

		// ִ��17�Ų���
_call_back_17:
		dwStepReturn = DISAthStep17(pXFileAttachInfo, pDISInTarget, pDISConfigure);
		__HandleDISAthStepReturnValue__(17, dwStepReturn);

	}

	// �ر��ļ�ӳ��
	XFileReleaseXFileAttach(pXFileAttachInfo);

	// ִ��18�Ų���
_call_back_18:
	dwStepReturn = DISAthStep18(pConfigure);
	__HandleDISAthStepReturnValue__(18, dwStepReturn);

	return __DISATH_ERROR_SUCCESS__;
}

