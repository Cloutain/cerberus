#include "PeDiy.h"
#include "Misc.h"
#include "xRand.h"
#include "XFileFormat.h"

// Ĭ��XFILE�ڶ�������
#define __XFILE_ALIGNMENT__					0x100
// Ĭ��X�ļ�����
#define __XFILE_SECTION_NAME__				"XFILE"
// Ĭ��X����
#define __XFILE_X_SECTION_NAME__			"XSECTION"
// ����DNA
#define __XFILE_BASE_DNA__					0xF6E5D4C3

// ���������Ϊ������µ�X��
__INLINE__ PXFILE_SECTION_HEADER __INTERNAL_FUNC__ XFileGetCurrentSectionHeader(__memory pMem, PXFILE_SECTION_HEADER pCurrSectionHeader) {
	PIMAGE_SECTION_HEADER pXFileNtSectionHeader = NULL;
	PXFILE_HEADER pXFileHeader = NULL;
	PXFILE_SECTION_HEADER pXFileSectionHeader = NULL;
	__memory pLocal = NULL;

	// �����Ƿ����XFile��
	pXFileNtSectionHeader = XFileExistXFileInPEFile(pMem);
	if (!pXFileNtSectionHeader) return NULL;

	// ��ȡX�ļ�
	pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);

	// ��ȡ��ǰ��ĩβ��
	pXFileSectionHeader = XFileGetTailSectionInPEFile(pMem);
	if (pXFileSectionHeader) {
		pCurrSectionHeader->dwPointerToRawData = pXFileSectionHeader->dwPointerToRawData + pXFileSectionHeader->dwSizeOfSection;
		pCurrSectionHeader->dwVirtualAddress = pXFileSectionHeader->dwVirtualAddress + pXFileSectionHeader->dwSizeOfSection;
		pCurrSectionHeader->dwOffsetByXFile = pXFileSectionHeader->dwOffsetByXFile + pXFileSectionHeader->dwSizeOfSection;
		pLocal = pMem + pCurrSectionHeader->dwPointerToRawData;
	} else {
		pCurrSectionHeader->dwPointerToRawData = pXFileNtSectionHeader->PointerToRawData + pXFileHeader->dwSizeOfAllHeaders;
		pCurrSectionHeader->dwVirtualAddress = pXFileNtSectionHeader->VirtualAddress + pXFileHeader->dwSizeOfAllHeaders;
		pCurrSectionHeader->dwOffsetByXFile = pXFileHeader->dwSizeOfAllHeaders;
		pLocal = (__memory)pMem + pCurrSectionHeader->dwPointerToRawData;
	}

	return pCurrSectionHeader;
}

#define __XFILE_PREDICT_DEF_SECTION_NAME__				"XTMPSEC"
__bool __API__ XFilePredictNewSectionHeader(__memory pMem, PXFILE_SECTION_HEADER pNewSectionHeader) {
	XFILE_SECTION_HEADER CurrXFileSectionHeader = {0};

	// �����Ƿ����XFile��
	if (!XFileExistXFileInPEFile(pMem)) return FALSE;

	// ��ȡ��ȷ�Ľ�
	XFileGetCurrentSectionHeader(pMem, &CurrXFileSectionHeader);

	// �����µĽ�
	__logic_strcpy__(pNewSectionHeader->szSectionName, __XFILE_PREDICT_DEF_SECTION_NAME__);
	pNewSectionHeader->dwDNA = 0;
	pNewSectionHeader->dwPointerToRawData = CurrXFileSectionHeader.dwPointerToRawData;
	pNewSectionHeader->dwVirtualAddress = CurrXFileSectionHeader.dwVirtualAddress;
	pNewSectionHeader->dwOffsetByXFile = CurrXFileSectionHeader.dwOffsetByXFile;
	pNewSectionHeader->dwSizeOfSection = 0;
	pNewSectionHeader->dwCharacteristics = __XFILE_SECTION_CHARACTERISTICS_USED__;

	return TRUE;
}

PXFILE_SECTION_HEADER __API__ XFileGetFirstSectionInPEFile(__memory pMem) {
	PXFILE_HEADER pXFileHeader = NULL;
	PXFILE_SECTION_HEADER pXFileSectionHeadr = NULL;
	PIMAGE_SECTION_HEADER pXFileNtSectionHeader = NULL;

	// �Ƿ����һ��X�ļ�
	pXFileNtSectionHeader = XFileExistXFileInPEFile(pMem);
	if (!pXFileNtSectionHeader) return NULL;

	pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);

	// �����Ƿ���ڽ�
	if (pXFileHeader->dwSectionNumber == 0)
		return NULL;

	pXFileSectionHeadr = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER));

	return pXFileSectionHeadr;
}

PXFILE_SECTION_HEADER __API__ XFileGetTailSectionInPEFile(__memory pMem) {
	PXFILE_HEADER pXFileHeader = NULL;
	PXFILE_SECTION_HEADER pXFileSectionHeadr = NULL;
	PIMAGE_SECTION_HEADER pXFileNtSectionHeader = NULL;

	// �Ƿ����һ��X�ļ�
	pXFileNtSectionHeader = XFileExistXFileInPEFile(pMem);
	if (!pXFileNtSectionHeader) return NULL;

	pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);

	// �����Ƿ���ڽ�
	if (pXFileHeader->dwSectionNumber == 0)
		return NULL;

	pXFileSectionHeadr = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER) + (sizeof(XFILE_SECTION_HEADER) * (pXFileHeader->dwSectionNumber - 1)));

	return pXFileSectionHeadr;
}

PIMAGE_SECTION_HEADER __API__ XFileExistXFileInPEFile(__memory pMem) {
	PXFILE_SECTION_HEADER pXFileSectionHeader = NULL;
	PIMAGE_SECTION_HEADER pXFileNtSectionHeader = NULL;
	PXFILE_HEADER pXFileHeader = NULL;
	__integer i = 0;
	__integer j = 0;

	pXFileNtSectionHeader = GetFirstSection(pMem);
	j = GetNumberOfSections(pMem);
	for (i = 0; i < j; i++, pXFileNtSectionHeader++) {
		pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);
		if (__IS_XFILE__(pXFileHeader->dwSign)) 
			return pXFileNtSectionHeader;
	}

	return NULL;
}

__integer __API__ XFileCalcXFileHeaderSize(__dword dwMaxSectionNumber) {
	__integer iAllSectionSize = 0;

	iAllSectionSize = (dwMaxSectionNumber * sizeof(XFILE_SECTION_HEADER)) + sizeof(XFILE_HEADER);
	iAllSectionSize = Alig(iAllSectionSize, __XFILE_ALIGNMENT__, TRUE);

	return iAllSectionSize;
}

__bool __API__ XFileAttachInfoInit(PXFILE_ATTACH_INFO pXFileAttachInfo) {
	__tchar *pTargetFilePath = NULL;
	__tchar *pTargetResourceType = NULL;
	__integer iTargetFileSize = 0;
	__memory pMem = NULL;
	__bool bIsResource = FALSE;
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	//__logic_memset__(pXFileAttachInfo, 0, sizeof(XFILE_ATTACH_INFO));
	pTargetFilePath = pXFileAttachInfo->szTargetFilePath;
	pTargetResourceType = pXFileAttachInfo->szResourceType;
	
	// ����һЩĬ��ֵ
	if (!pXFileAttachInfo->szResourceType) pXFileAttachInfo->szResourceType = (__tchar *)_T("BIN");
	if (!pXFileAttachInfo->szSectionName) pXFileAttachInfo->szSectionName = (__char *)__XFILE_SECTION_NAME__;
	if (!pXFileAttachInfo->szXSectionName) pXFileAttachInfo->szXSectionName = (__char *)__XFILE_X_SECTION_NAME__;
	if (pXFileAttachInfo->dwDNA == 0) pXFileAttachInfo->dwDNA = __XFILE_BASE_DNA__ + GetRandDword();

	/*
	 * �����16λΪ0����Ϊ������ԴID��
	 * ӳ���ļ�
	 */
	if (__HiWord__(pTargetFilePath) == 0) {
		__dword dwKey = 0;
		dwKey = pXFileAttachInfo->dwResourceDecodeKey;
		pMem = MapResourceDataEx(NULL, (__word)pTargetFilePath, pTargetResourceType, &iTargetFileSize, dwKey);
		bIsResource = TRUE;
	} else {
		pMem = MappingFile(pTargetFilePath, &iTargetFileSize, TRUE, 0, 0);
		bIsResource = FALSE;
	}
	if (!pMem)
		return FALSE;

	// �ж�Ŀ���Ƿ���PE�ļ�
	if (!IsValidPE32(pMem)) {
		UnMappingFile(pMem);
		return FALSE;
	}

	pXFileAttachInfo->iFileSize = iTargetFileSize;
	pXFileAttachInfo->iOrigFileSize = iTargetFileSize;
	pXFileAttachInfo->pMem = pMem;
	pXFileAttachInfo->szTargetFilePath = pTargetFilePath;
	pXFileAttachInfo->bUsed = TRUE;
	pXFileAttachInfo->bImageBaseRelocate = (IsDllFile(pMem) || IsDriverFile(pMem));

	{
		// ȡ��ĩβ����
		__integer iTargetTailDataSize = 0;
		__memory pTargetTailData = NULL;
		__memory pTailData = NULL;
		__offset ofTailDataRawOffset = 0;
		PIMAGE_DATA_DIRECTORY pSecurityDataDirectory = NULL;
		__bool bTailDataIsSecurityDataDirectory = FALSE;
		__offset ofSecurityDataDirectoryRawOffsetByTailData = 0;

		iTargetTailDataSize = GetTailDataSize(pMem, iTargetFileSize);
		if (iTargetTailDataSize != 0) {
			pTargetTailData = (__memory)__logic_new_size__(iTargetTailDataSize);
			pTailData = GetTailDataPoint(pMem, iTargetFileSize);
			ofTailDataRawOffset = (__offset)(pTailData - pMem);//ĩβ���ݵ��ļ�ƫ��
			__logic_memcpy__(pTargetTailData, pTailData, iTargetTailDataSize);

			// ȷ���Ƿ����֤���
			pSecurityDataDirectory = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_SECURITY);
			if (pSecurityDataDirectory) {
				if (pSecurityDataDirectory->VirtualAddress == ofTailDataRawOffset) {
					bTailDataIsSecurityDataDirectory = TRUE;
					ofSecurityDataDirectoryRawOffsetByTailData = 0;
				} else if (pSecurityDataDirectory->VirtualAddress > ofTailDataRawOffset) {
					bTailDataIsSecurityDataDirectory = TRUE;
					ofSecurityDataDirectoryRawOffsetByTailData = pSecurityDataDirectory->VirtualAddress - ofTailDataRawOffset;
				} else {
					bTailDataIsSecurityDataDirectory = FALSE;
					ofSecurityDataDirectoryRawOffsetByTailData = 0;
				}/* end else */
			}

			// �ر�ӳ��,��û��ĩβ���ݵĳ�������ӳ��Ŀ��
			if (bIsResource)
				UnMapResourceData(pMem);
			else
				UnMappingFile(pMem);
			iTargetFileSize -= iTargetTailDataSize;

			// ���������ļ�ĩβ
			ResetFileTail(pTargetFilePath, iTargetFileSize);

			// ���´�ӳ��
			if (bIsResource) {
				pMem = MapResourceDataSubNewSize(pMem, pXFileAttachInfo->iOrigFileSize, iTargetTailDataSize);
			} else
				pMem = MappingFile(pTargetFilePath, NULL, TRUE, 0, iTargetFileSize);

			// �����µ�X�ļ����ӽṹ
			if (!pMem) {
				__logic_memset__(pXFileAttachInfo, 0, sizeof(XFILE_ATTACH_INFO));
				return FALSE;
			} else {
				pXFileAttachInfo->pMem = pMem;
				pXFileAttachInfo->iFileSize = iTargetFileSize;
				pXFileAttachInfo->pTailData = pTargetTailData;
				pXFileAttachInfo->iTailDataSize = iTargetTailDataSize;
				pXFileAttachInfo->ofOrigTailDataRawOffset = ofTailDataRawOffset;
				pXFileAttachInfo->bTailDataIsSecurityDataDirectory = bTailDataIsSecurityDataDirectory;//ĩβ�����Ƿ���֤���
				pXFileAttachInfo->ofSecurityDataDirectoryRawOffsetByTailData = ofSecurityDataDirectoryRawOffsetByTailData;
			}
		}/* end if */
	}

	// ����ԭʼ��PEͷ
	pNtHdr = GetNtHeader(pXFileAttachInfo->pMem);
	__logic_memcpy__(&(pXFileAttachInfo->OrigTargetNtHeader), pNtHdr, sizeof(IMAGE_NT_HEADERS));

	// ����ԭʼ�������
	{
		PIMAGE_DATA_DIRECTORY pImportDataDirectory = ExistDataDirectory(pXFileAttachInfo->pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);
		if (!pImportDataDirectory) {
			// ������������
			pXFileAttachInfo->pOrigImportDescriptor = NULL;
		} else {
			pXFileAttachInfo->pOrigImportDescriptor = (__memory)__logic_new_size__(pImportDataDirectory->Size);
			if (pXFileAttachInfo->pOrigImportDescriptor) {
				__memory pImportData = pXFileAttachInfo->pMem + Rva2Raw(pXFileAttachInfo->pMem, pImportDataDirectory->VirtualAddress);
				__logic_memcpy__(pXFileAttachInfo->pOrigImportDescriptor, pImportData, pImportDataDirectory->Size);
			} else {
				pXFileAttachInfo->pOrigImportDescriptor = NULL;
			}
		}/* end else */
	}

	//// ����һ�����������
	//{
	//	__integer iLibCount = CountLibraryInImportTable(pXFileAttachInfo->pMem, TRUE);
	//	PIMAGE_IMPORT_DESCRIPTOR pDummyImportDescriptor = MakeImportDescriptor(pXFileAttachInfo->pMem, iLibCount, 1);

	//}
	

	return TRUE;
}

/*
 * ����:
 *	�����X�ļ��ĳ����ǰ���PE�ļ��Ķ������ȶ����
 */
PIMAGE_SECTION_HEADER __API__ XFileAddXFileToPEFile(PXFILE_ATTACH_INFO pXFileAttachInfo, __integer iSectionSize, __dword dwMaxSectionNumber) {
	__memory pMem = NULL;
	__tchar *szTargetFilePath = NULL;
	PXFILE_HEADER pXFileHeader = NULL;
	PIMAGE_SECTION_HEADER pXFileNtSectionHeader = NULL;
	__integer iTargetFileSize = 0;
	__integer iXFileNtSectionSize = 0, iAllSectionSize = 0, iNewTargetFileSize = 0;
	__char *pSectionName = pXFileAttachInfo->szSectionName;

	// ������
	if (dwMaxSectionNumber <= 0) return NULL;
	if (!pXFileAttachInfo->bUsed) return NULL;

	// ��ȡ����ֵ
	szTargetFilePath = pXFileAttachInfo->szTargetFilePath;
	pMem = pXFileAttachInfo->pMem;
	iTargetFileSize = pXFileAttachInfo->iFileSize;
	
	// �������PE�ļ��Ľ�,�鿴�Ƿ��Ѿ�����X�ļ�
	if (XFileExistXFileInPEFile(pMem))
		return NULL;

	// ����XFile���ܹ��Ĵ�С
	iAllSectionSize = (dwMaxSectionNumber * sizeof(XFILE_SECTION_HEADER)) + sizeof(XFILE_HEADER);
	iAllSectionSize = Alig(iAllSectionSize, __XFILE_ALIGNMENT__, TRUE);
	iXFileNtSectionSize = iAllSectionSize + iSectionSize;
	iXFileNtSectionSize = GetAddSectionMapSize(pMem, iXFileNtSectionSize);

	// �µ�ӳ���С
	iNewTargetFileSize = iTargetFileSize + iXFileNtSectionSize;
	pXFileAttachInfo->iFileSize = iNewTargetFileSize;

	// ����ӳ��
	UnMappingFile(pMem);
	pMem = (LPBYTE)MappingFile(szTargetFilePath, NULL, TRUE, 0, iNewTargetFileSize);
	if (!pMem) {
		pXFileAttachInfo->bUsed = FALSE;//�趨Ϊû��ʹ��
		return NULL;
	}
	pXFileAttachInfo->pMem = pMem;

	// �������½�
	pXFileNtSectionHeader = AddSection(pMem, pSectionName, __DEF_NEWSECTION_CHARACTERISTICS__, iXFileNtSectionSize, NULL, FALSE);
	if (!pXFileNtSectionHeader)
		return NULL;
	
	// ��ʼ��XFILE��ʽͷ
	pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);
	__logic_memset__(pXFileHeader, 0, iXFileNtSectionSize);
	pXFileHeader->dwSign = __XFILE_SIGN__;
	pXFileHeader->dwVersion = __XFILE_VERSION__;
	pXFileHeader->dwSectionNumber = 0;
	pXFileHeader->dwMaxSectionNumber = dwMaxSectionNumber;
	pXFileHeader->dwXFileSize = iXFileNtSectionSize;
	pXFileHeader->dwAlignment = __XFILE_ALIGNMENT__;
	pXFileHeader->dwXFileNtSectionHdrFileOffset = pXFileNtSectionHeader->PointerToRawData;
	pXFileHeader->dwXFileNtSectionHdrMemoryOffset = pXFileNtSectionHeader->VirtualAddress;
	pXFileHeader->dwSizeOfAllHeaders = iAllSectionSize;

	return pXFileNtSectionHeader;
}

/*
 * ����:
 *	�ڵ���AddXFileSectionByXFileHeader֮��,ʹ�����ķ���ֵ���е���
 *	�����X�ļ��ĳ����ǰ���X�ļ��ڲ��������Ƚ��ж����
 */
PXFILE_HEADER __API__ XFileAddXFileToXFileSection(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_SECTION_HEADER pXFileSectionHeader, __integer iSectionSize, __dword dwMaxSectionNumber) {
	PXFILE_HEADER pXFileHeader = NULL;
	__integer iXFileSectionSize = 0, iAllSectionSize = 0;
	__memory pMem = pXFileAttachInfo->pMem;
	__char *pSectionName = pXFileAttachInfo->szSectionName;

	// ����XFile���ܹ��Ĵ�С
	iAllSectionSize = (dwMaxSectionNumber * sizeof(XFILE_SECTION_HEADER)) + sizeof(XFILE_HEADER);
	iAllSectionSize = Alig(iAllSectionSize, __XFILE_ALIGNMENT__, TRUE);
	iXFileSectionSize = iAllSectionSize + iSectionSize;
	iXFileSectionSize = Alig(iXFileSectionSize, __XFILE_ALIGNMENT__, TRUE);

	// ��ʼ��XFILE��ʽͷ
	pXFileHeader = (PXFILE_HEADER)(pMem + pXFileSectionHeader->dwPointerToRawData);
	pXFileHeader->dwSign = __XFILE_SUB_SIGN__;
	pXFileHeader->dwVersion = __XFILE_VERSION__;
	pXFileHeader->dwSectionNumber = 0;
	pXFileHeader->dwMaxSectionNumber = dwMaxSectionNumber;
	pXFileHeader->dwXFileSize = iXFileSectionSize;
	pXFileHeader->dwAlignment = __XFILE_ALIGNMENT__;
	pXFileHeader->dwXFileSectionHdrFileOffset = pXFileSectionHeader->dwPointerToRawData;
	pXFileHeader->dwXFileSectionHdrMemoryOffset = pXFileSectionHeader->dwVirtualAddress;
	pXFileHeader->dwSizeOfAllHeaders = iAllSectionSize;

	return pXFileHeader;
}

PXFILE_SECTION_HEADER __API__ XFileAddXFileSectionByNtSectionHeader(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_SECTION_HEADER pXFileNtSectionHeader, __integer iSectionSize, __dword dwCharacteristics, __dword dwEx1, __dword dwEx2) {
	PXFILE_SECTION_HEADER pXFileSectionHeader = NULL, pXFileLastSectionHeader = NULL;
	PXFILE_HEADER pXFileHeader = NULL;
	__memory pLocal = NULL;
	__dword dwSectionSize = 0;
	__memory pMem = NULL;
	__tchar *szTargetFilePath = NULL;
	__integer iTargetFileSize = 0;
	__offset ofRvaLocal = 0;
	XFILE_SECTION_HEADER CurrXFileSectionHeader = {0};
	__char *pSectionName = NULL;
	__dword dwDNA = 0;

	// Ĭ���趨Ϊ�ڴ��㹻
	__bool bEnoughMemory = TRUE;

	// ���
	if (!pXFileAttachInfo->bUsed)
		return NULL;
	pMem = pXFileAttachInfo->pMem;
	szTargetFilePath = pXFileAttachInfo->szTargetFilePath;
	iTargetFileSize = pXFileAttachInfo->iFileSize;
	pSectionName = pXFileAttachInfo->szXSectionName;
	dwDNA = pXFileAttachInfo->dwDNA;

	// ��ȡX�ļ�
	pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);

	// �ж��Ƿ���X��־
	if (!__IS_XFILE__(pXFileHeader->dwSign))
		return NULL;

	// ����Ƿ��Ѿ��������Ľ���Ŀ
	if (pXFileHeader->dwSectionNumber >= pXFileHeader->dwMaxSectionNumber)
		return NULL;

	// ������Զ������ȵĳ���
	dwSectionSize = Alig(iSectionSize, pXFileHeader->dwAlignment, TRUE);

	// ��ȡ�½�ͷλ��
	pXFileSectionHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER) + (sizeof(XFILE_SECTION_HEADER) * pXFileHeader->dwSectionNumber));

	// ���㵱ǰ��
	XFileGetCurrentSectionHeader(pMem, &CurrXFileSectionHeader);

	if (pXFileHeader->dwSectionNumber != 0) {
		pXFileLastSectionHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER) + (sizeof(XFILE_SECTION_HEADER) * (pXFileHeader->dwSectionNumber - 1)));
	} else {
		pXFileLastSectionHeader = NULL;
	}

	// �жϵ�ǰӳ���Ƿ����㹻�ĳ��������½�
	//bEnoughMemory = TRUE;//Ĭ��ΪTRUE
	if (pXFileLastSectionHeader) {
		if ((pXFileLastSectionHeader->dwOffsetByXFile + pXFileLastSectionHeader->dwSizeOfSection + dwSectionSize)  > pXFileHeader->dwXFileSize)
			bEnoughMemory = FALSE;
	} else if ((pXFileHeader->dwSizeOfAllHeaders + dwSectionSize) > pXFileHeader->dwXFileSize) {
		bEnoughMemory = FALSE;
	}

	// ���û���㹻���ڴ�
	if (!bEnoughMemory) {
		__integer iNewSectionSize = 0;

		// ������ĩβ�ڵĳ���
		//iNewSectionSize = pXFileLastSectionHeader->dwOffsetByXFile + pXFileLastSectionHeader->dwSizeOfSection + dwSectionSize;
		iNewSectionSize = CurrXFileSectionHeader.dwOffsetByXFile + dwSectionSize;

		// ��չĩβ��
		pXFileNtSectionHeader = CoverTailSection(szTargetFilePath, &pMem, iNewSectionSize, NULL, 0, &iTargetFileSize);
		if (!pXFileNtSectionHeader)
			return NULL;

		// �����趨������Ϣ
		pXFileAttachInfo->bUsed = TRUE;
		pXFileAttachInfo->pMem = pMem;
		pXFileAttachInfo->iFileSize = iTargetFileSize;
		pXFileAttachInfo->szTargetFilePath = szTargetFilePath;

		// ���¶�λ��X�ļ�,�������趨�ߴ�
		pXFileHeader = (PXFILE_HEADER)(pMem + pXFileNtSectionHeader->PointerToRawData);
		pXFileHeader->dwXFileSize = pXFileNtSectionHeader->SizeOfRawData;

		// �����µ�X��ǰҪ��ӵĽ��Լ��µ�Xĩβ��λ��
		pXFileSectionHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER) + (sizeof(XFILE_SECTION_HEADER) * pXFileHeader->dwSectionNumber));
		pXFileLastSectionHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER) + (sizeof(XFILE_SECTION_HEADER) * (pXFileHeader->dwSectionNumber - 1)));
	}/* end if */

	// ����ͷ��Ϣ
	__logic_strcpy__(pXFileSectionHeader->szSectionName, pSectionName);
	pXFileSectionHeader->dwDNA = dwDNA;
	pXFileSectionHeader->dwPointerToRawData = CurrXFileSectionHeader.dwPointerToRawData;
	pXFileSectionHeader->dwVirtualAddress = CurrXFileSectionHeader.dwVirtualAddress;
	pXFileSectionHeader->dwOffsetByXFile = CurrXFileSectionHeader.dwOffsetByXFile;
	pXFileSectionHeader->dwSizeOfSection = dwSectionSize;
	pXFileSectionHeader->dwCharacteristics = dwCharacteristics | __XFILE_SECTION_CHARACTERISTICS_USED__;

	// �����PE�ļ�����ͨ���ڴ�����
	if ((dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_PE__) && (!(dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_PE_FILE_ALIGN__)))
	{
		pXFileSectionHeader->ofMemImageBaseOffset = (__offset)dwEx1;
		// ������Զ�������������Ŀ���ļ���RVA
		{
			__dword delta = 0;
			// �õ�����PEӳ��Ļ���ַ
			delta = pXFileSectionHeader->dwVirtualAddress + pXFileSectionHeader->ofMemImageBaseOffset;
			pXFileSectionHeader->dwCustomDataRVA = dwEx2 + delta;
		}
	}

	// ����XFILE������
	(pXFileHeader->dwSectionNumber)++;

	return pXFileSectionHeader;
}

/*
 * ����:
 *	ͨ��һ��X�ļ�����һ��X��,��ͨ��NT��ͷ���X�ڲ�ͬ,�����������X��,����ʹ����չĩβ�ڵķ�ʽ�Զ���չ�ڵĴ�С
 *	�������������,��ΪX�ļ������ǹ̶���
 */
PXFILE_SECTION_HEADER __API__ XFileAddXFileSectionByXFileHeader(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_HEADER pXFileHeader, __integer iSectionSize, __dword dwCharacteristics, __dword dwEx1, __dword dwEx2) {
	PXFILE_SECTION_HEADER pXFileSectionHeader = NULL;
	__memory pLocal = NULL;
	__dword dwSectionSize = 0;
	__offset ofRvaLocal = 0;
	XFILE_SECTION_HEADER CurrXFileSectionHeader = {0};
	__memory pMem = pXFileAttachInfo->pMem;
	__char *pSectionName = pXFileAttachInfo->szXSectionName;
	__dword dwDNA = pXFileAttachInfo->dwDNA;

	// �ж��Ƿ���X��־
	if (!__IS_XFILE__(pXFileHeader->dwSign))
		return NULL;

	// ����Ƿ��Ѿ��������Ľ���Ŀ
	if (pXFileHeader->dwSectionNumber >= pXFileHeader->dwMaxSectionNumber)
		return NULL;

	// ������Զ������ȵĳ���
	dwSectionSize = Alig(iSectionSize, pXFileHeader->dwAlignment, TRUE);

	// ���㵱ǰ��
	XFileGetCurrentSectionHeader(pMem, &CurrXFileSectionHeader);

	// �жϵ�ǰӳ���Ƿ����㹻�ĳ��������½�
	//bEnoughMemory = TRUE;//Ĭ��ΪTRUE
	if (CurrXFileSectionHeader.dwOffsetByXFile + dwSectionSize > pXFileHeader->dwXFileSize)
		return NULL;

	// ��ȡ�½�ͷλ��
	pXFileSectionHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER) + (sizeof(XFILE_SECTION_HEADER) * pXFileHeader->dwSectionNumber));

	// ����ͷ��Ϣ
	__logic_strcpy__(pXFileSectionHeader->szSectionName, pSectionName);
	pXFileSectionHeader->dwDNA = dwDNA;
	pXFileSectionHeader->dwPointerToRawData = CurrXFileSectionHeader.dwPointerToRawData;
	pXFileSectionHeader->dwVirtualAddress = CurrXFileSectionHeader.dwVirtualAddress;
	pXFileSectionHeader->dwOffsetByXFile = CurrXFileSectionHeader.dwOffsetByXFile;
	pXFileSectionHeader->dwSizeOfSection = dwSectionSize;
	pXFileSectionHeader->dwCharacteristics = dwCharacteristics | __XFILE_SECTION_CHARACTERISTICS_USED__;

	// �����PE�ļ�����ͨ���ڴ�����
	if ((dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_PE__) && (!(dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_PE_FILE_ALIGN__)))
	{
		pXFileSectionHeader->ofMemImageBaseOffset = (__offset)dwEx1;
		// ������Զ�������������Ŀ���ļ���RVA
		{
			__dword delta = 0;
			// �õ�����PEӳ��Ļ���ַ
			delta = pXFileSectionHeader->dwVirtualAddress + pXFileSectionHeader->ofMemImageBaseOffset;
			pXFileSectionHeader->dwCustomDataRVA = dwEx2 + delta;
		}
	}

	// ����XFILE������
	(pXFileHeader->dwSectionNumber)++;

	return pXFileSectionHeader;
}

__void __API__ XFileReleaseXFileAttach(PXFILE_ATTACH_INFO pData) {
	__bool bResource = FALSE;
	PIMAGE_NT_HEADERS pNtHdr = NULL;

	if (!pData->bUsed)
		return;

	if (HIWORD(pData->szTargetFilePath) == 0)
		bResource = TRUE;
	else
		bResource = FALSE;

	// ����ĩβ����
	if (pData->bKeepTailData) {
		if (pData->iTailDataSize != 0) {
			// �ر�ӳ��,�����µĳ��ȴ�ӳ��
			if (bResource)
				UnMapResourceData(pData->pMem);
			else
				UnMappingFile(pData->pMem);

			// ���´�ӳ��
			if (bResource) {
				pData->pMem = MapResourceDataPlusNewSize(pData->pMem, pData->iFileSize, pData->iTailDataSize);
			} else {
				pData->pMem = MappingFile(pData->szTargetFilePath, NULL, TRUE, 0, pData->iFileSize + pData->iTailDataSize);
			}
			if (!pData->pMem) {
				return;
			}
			__logic_memcpy__(pData->pMem + pData->iFileSize, pData->pTailData, pData->iTailDataSize);
			pData->iFileSize += pData->iTailDataSize;

			// ����֤������
			if (pData->bTailDataIsSecurityDataDirectory) {
				PIMAGE_DATA_DIRECTORY pSecurityDataDirectory = NULL;

				// ��ȫ֤����Ƿ����
				pSecurityDataDirectory = ExistDataDirectory(pData->pMem, IMAGE_DIRECTORY_ENTRY_SECURITY);
				if (pSecurityDataDirectory) {
					__memory pTailData = NULL;

					pTailData = GetTailDataPoint(pData->pMem, pData->iFileSize);
					pSecurityDataDirectory->VirtualAddress = pTailData - pData->pMem + pData->ofSecurityDataDirectoryRawOffsetByTailData;//ƫ�Ƶ��޶�
				}
			}/* end if */
		}/* end if */
	}

	// ��ȡNTͷ
	pNtHdr = GetNtHeader(pData->pMem);

	// ����Ƿ���Ҫ�ر�DEP����
	if (pData->bCloseNX)
		pNtHdr->OptionalHeader.DllCharacteristics &= ~IMAGE_DLLCHARACTERISTICS_NX_COMPAT;

	// ����Ƿ���Ҫ�ر������ַ��ӳ��,�����DLL����Դ�����
	if ((pData->bCloseRandAddressMap) && (!pData->bImageBaseRelocate))
		pNtHdr->OptionalHeader.DllCharacteristics &= ~IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE;

	// �������н�Ϊ��д
	if (pData->bSetAllSectionCanWrite)
		SetAllSectionCanWrite(pData->pMem);

	// ������н���
	if (pData->bRandomAllSectionName)
		RandomAllSectionName(pData->pMem);

	// ɾ��ָ��������Ŀ¼
	{
		__integer n = 0;
		for (n = 0; n < 0x10; n++)
			if (pData->bWillDelDataDirectoryIndexs[n]) DeleteDataDirectoryObject(pData->pMem, n);
	}

	// �Ƿ����¼���Ŀ���У���
	if (pData->bResetCheckSum)
		RefixCheckSum(pData->pMem, pData->iFileSize);
	else 
		RefixCheckSum(pData->pMem, 0);

	// ��߼����Ը���
	if (pData->bCompatibility) {
		// ɾ���������
		DeleteDataDirectoryObject(pData->pMem, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
		// ɾ�������ַ��
		DeleteDataDirectoryObject(pData->pMem, IMAGE_DIRECTORY_ENTRY_IAT);
	}

	// ����������е�һЩ������߼�����
	if (pData->bKeepSameOrigImpLib) {
		if (pData->pOrigImportDescriptor) {

		}/* end if */
	}

	// �ر�ӳ��
	if (bResource) {
		UnMapResourceData(pData->pMem);
	} else {
		UnMappingFile(pData->pMem);
	}

	// �ͷ�ĩβ���ݵ��ڴ�
	if (pData->pTailData) __logic_delete__(pData->pTailData);
	
	// �ͷű��ݵ����������
	if (pData->pOrigImportDescriptor) __logic_delete__(pData->pOrigImportDescriptor);

	// ���X�ļ����ӽṹ
	__logic_memset__(pData, 0, sizeof(XFILE_ATTACH_INFO));
}

__bool __API__ XFileInThisXFileSection(PXFILE_SECTION_HEADER pSectH, __offset ofOffset, __bool bRva) {
	return (bRva ? (ofOffset >= (__offset)(pSectH->dwVirtualAddress)) && (ofOffset < (__offset)(pSectH->dwVirtualAddress + pSectH->dwSizeOfSection)) :
		(ofOffset >= (__offset)(pSectH->dwPointerToRawData)) && (ofOffset < (__offset)(pSectH->dwPointerToRawData + pSectH->dwSizeOfSection)));

}

PXFILE_SECTION_HEADER __API__ XFileGetXFileSectionHeaderByOffset(PXFILE_HEADER pXFileHeader, __offset ofOffset, __bool bRva) {
	PXFILE_SECTION_HEADER pHeader = NULL, pRet = NULL;
	__integer i = 0;

	pHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER));
	for (i = 0; i < pXFileHeader->dwSectionNumber; pHeader++, i++) {
		if (XFileInThisXFileSection(pHeader, ofOffset, bRva)) {
			pRet = pHeader;
			break;
		}
	}

	return pRet;
}

PXFILE_SECTION_HEADER __API__ XFileGetXFileSectionHeaderByDNA(__memory pMem, PXFILE_HEADER pXFileHeader, __dword dwDNA, __bool bFile, PXFILE_SECTION_HEADER *pXFatherSectionHdrPoint) {
	PXFILE_SECTION_HEADER pHeader = NULL, pRet = NULL;
	__integer i = 0;

	// ������ǰӳ���XFILE��
	// ���pRet��Ϊ�����ʾ�Ѿ��ҵ�
	pHeader = (PXFILE_SECTION_HEADER)((__memory)pXFileHeader + sizeof(XFILE_HEADER));
	for (i = 0; i < (pXFileHeader->dwSectionNumber && !pRet); pHeader++, i++) {
		if (pHeader->dwDNA == dwDNA) {
			// ���ƥ�䵽DNAֱ�ӷ���
			return pHeader;
		} else if (pHeader->dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_X__ == __XFILE_SECTION_CHARACTERISTICS_X__) {
			// �����XFILE��,��ֱ�ӽ��еݹ�
			PXFILE_HEADER pSubXFileHeader = NULL;

			// ��ȡ��X�ļ�ͷ��
			pSubXFileHeader = (PXFILE_HEADER)((bFile) ? pMem + pHeader->dwPointerToRawData : pMem + pHeader->dwVirtualAddress);
			
			// �ݹ�������
			pRet = XFileGetXFileSectionHeaderByDNA(pMem, pSubXFileHeader, dwDNA, bFile, pXFatherSectionHdrPoint);
			if (pRet) return pRet;
		} else if (pHeader->dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_PE__ == __XFILE_SECTION_CHARACTERISTICS_PE__) {
			// �����XFILE����PE�ļ����ȥ��PE�ļ��ڲ����б����ڲ���
			__memory pImage = NULL;
			PIMAGE_NT_HEADERS pNtHdr = NULL;
			PIMAGE_SECTION_HEADER pSecHdr = NULL;
			__integer j = 0;

			// ��ȡ��ǰPE�ļ���ӳ��,�����PE�ļ���ʹ���ڴ����,��������л���ַ�������
			pImage = (bFile) ? pMem + pHeader->dwPointerToRawData : pMem + pHeader->dwVirtualAddress + pHeader->ofMemImageBaseOffset;

			// ��ȡNTͷ
			pNtHdr = GetNtHeader(pImage);

			// �ҵ����PE�ļ���XFILE��
			pSecHdr = GetFirstSection(pImage);
			for (j = 0; (j < (pNtHdr->FileHeader).NumberOfSections && !pRet) ; pSecHdr++, j++) {
				__memory pSec = NULL;
				PXFILE_HEADER pXSec  = NULL;
				__bool bOnFileAlign = FALSE;

				// �жϴ�PE�Ƿ����ļ����뻹���ڴ����
				if (pHeader->dwCharacteristics & __XFILE_SECTION_CHARACTERISTICS_PE_FILE_ALIGN__ == __XFILE_SECTION_CHARACTERISTICS_PE_FILE_ALIGN__) {
					pSec = pImage + pSecHdr->PointerToRawData + pHeader->ofMemImageBaseOffset;
					bOnFileAlign = TRUE;
				} else {
					pSec = pImage + pSecHdr->VirtualAddress + pHeader->ofMemImageBaseOffset;
					bOnFileAlign = FALSE;
				}

				pXSec = (PXFILE_HEADER)pSec;
				// �жϴ˽��Ƿ���XFILE��
				if (__IS_XFILE__(pXSec->dwSign)) {
					// ���еݹ����
					*pXFatherSectionHdrPoint = pHeader;//���ø�X��
					pRet = XFileGetXFileSectionHeaderByDNA(pImage, pXSec, dwDNA, bOnFileAlign, pXFatherSectionHdrPoint);
					if (pRet) return pRet;
				}/* end if */
			}/* end for */
		}/* end else if */

		// ...
	}

	return NULL;
}
