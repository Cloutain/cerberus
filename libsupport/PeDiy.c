#include "PeDiy.h"
#include "xRand.h"
#include "lcrypto.h"
#include "Misc.h"

PIMAGE_NT_HEADERS __API__ GetNtHeader(__memory pMem) {
	PIMAGE_DOS_HEADER pDosH = (PIMAGE_DOS_HEADER)pMem;
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)(pDosH->e_lfanew + pMem);
	return pNtH;
}

PIMAGE_SECTION_HEADER __API__ GetFirstSectionByNtHeader(PIMAGE_NT_HEADERS pNtH) {
	return IMAGE_FIRST_SECTION(pNtH);
}

PIMAGE_SECTION_HEADER __API__ GetFirstSection(__memory pMem) {
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	return GetFirstSectionByNtHeader(pNtH);
}

PIMAGE_SECTION_HEADER __API__ GetTailSection(__memory pMem) {
	__word wNumberOfSections = GetNumberOfSections(pMem);
	return (GetFirstSection(pMem) + wNumberOfSections - 1);
}

__integer __API__ GetSectionIndex(__memory pMem, __char *pSectionName) {
	__word wNumberOfSection = GetNumberOfSections(pMem);
	PIMAGE_SECTION_HEADER pSecHdr = GetFirstSection(pMem);
	__integer i = 0;
	for (i = 0; i < wNumberOfSection; i++) {
		if (__logic_memcmp__(pSecHdr->Name, pSectionName, __logic_strlen__(pSectionName)) == 0)
			return i;
		pSecHdr++;
	}
	return -1;
}

__integer __API__ GetEntrySectionIndex(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	PIMAGE_SECTION_HEADER pSecHdr = NULL;
	__word wNumberOfSections = 0;
	__offset ofEntryRva = 0;
	__integer i = 0;

	pNtHdr = GetNtHeader(pMem);
	wNumberOfSections = pNtHdr->FileHeader.NumberOfSections;
	ofEntryRva = (__offset)(pNtHdr->OptionalHeader.AddressOfEntryPoint);
	pSecHdr = GetFirstSection(pMem);
	for (i = 0 ; i < wNumberOfSections; i++) {
		if ((pSecHdr->VirtualAddress <= ofEntryRva) && (ofEntryRva < (pSecHdr->VirtualAddress + pSecHdr->Misc.VirtualSize)))
			return i;
		pSecHdr++;
	}

	return -1;
}

__bool __API__ PredictNewSectionHeader(__memory pMem, __dword dwVirtualSize, PIMAGE_SECTION_HEADER pNewSectionHeader) {
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_SECTION_HEADER pCurrTailSectionHdr = GetTailSection(pMem);
	if (pCurrTailSectionHdr == NULL)
		return FALSE;

	pNt = GetNtHeader(pMem);
	__logic_memset__(pNewSectionHeader->Name, 0, IMAGE_SIZEOF_SHORT_NAME);
	pNewSectionHeader->SizeOfRawData = Alig(dwVirtualSize, pNt->OptionalHeader.FileAlignment, TRUE);
	pNewSectionHeader->PointerToRawData	= Alig(pCurrTailSectionHdr->PointerToRawData + pCurrTailSectionHdr->SizeOfRawData, pNt->OptionalHeader.FileAlignment, TRUE);
	pNewSectionHeader->Misc.VirtualSize	= dwVirtualSize;
	pNewSectionHeader->VirtualAddress = Alig(pCurrTailSectionHdr->VirtualAddress + pCurrTailSectionHdr->Misc.VirtualSize, pNt->OptionalHeader.SectionAlignment, TRUE);
	pNewSectionHeader->Characteristics = 0;
	pNewSectionHeader->PointerToLinenumbers	= 0;
	pNewSectionHeader->NumberOfLinenumbers	= 0;
	pNewSectionHeader->NumberOfRelocations	= 0;
	pNewSectionHeader->PointerToRelocations	= 0;
	return TRUE;
}

PIMAGE_SECTION_HEADER __API__ GetEntryPointSection(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHeader = GetNtHeader(pMem);
	__dword dwEntryRva = (pNtHeader->OptionalHeader).AddressOfEntryPoint;
	__dword dwSectionCount = GetNumberOfSections(pMem);
	PIMAGE_SECTION_HEADER pSectionTable = GetFirstSection(pMem);
	__integer i = 0;
	for (i = 0; i < dwSectionCount; i++) {
		if ((dwEntryRva >= pSectionTable->VirtualAddress) &&\
			(dwEntryRva < (pSectionTable->VirtualAddress) + (pSectionTable->SizeOfRawData)))
			break;
		pSectionTable++;
	}
	return pSectionTable;
}

__address __API__ GetEntryPointMemoryAddress(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHdr = GetNtHeader(pMem);
	__dword dwImageBase = pNtHdr->OptionalHeader.ImageBase;
	return (dwImageBase + pNtHdr->OptionalHeader.AddressOfEntryPoint);
}

PIMAGE_SECTION_HEADER __API__ GetSection(__memory pMem, __char *lpSectionName) {
	PIMAGE_SECTION_HEADER pSectH = GetFirstSection(pMem);
	__word wNumOfSects = GetNumberOfSections(pMem);
	__integer i = 0;
	for (i = 0; i < wNumOfSects; i++) {
		if (__logic_memcmp__(lpSectionName, pSectH->Name, __logic_strlen__(lpSectionName)) == 0)
			return pSectH;
		pSectH++;
	}
	return NULL;
}

PIMAGE_SECTION_HEADER __API__ GetSectionByIndex(__memory pMem, __integer iIndex) {
	PIMAGE_SECTION_HEADER pSectH = GetFirstSection(pMem);
	__word wNumOfSects = GetNumberOfSections(pMem);
	if (iIndex >= wNumOfSects) return NULL;
	return (pSectH + iIndex);
}

PIMAGE_DATA_DIRECTORY __API__ ExistDataDirectory(__memory pMem, __integer iIndex) {
	PIMAGE_NT_HEADERS pNtHeader = GetNtHeader(pMem);
	if ((pNtHeader->OptionalHeader).DataDirectory[iIndex].VirtualAddress != 0)
		return &((pNtHeader->OptionalHeader).DataDirectory[iIndex]);
	return NULL;
}

__word __API__ GetNumberOfSections(__memory pMem) {
	return GetNtHeader(pMem)->FileHeader.NumberOfSections;
}

__void __API__ RandomAllSectionName(__memory pMem) {
	__word wNumberOfSections = GetNumberOfSections(pMem);
	PIMAGE_SECTION_HEADER pSectionHeader = GetFirstSection(pMem);
	__integer i = 0, j = 0;
	for (i = 0; i < wNumberOfSections; i++, pSectionHeader++) {
		// ����һ���������
		__byte SectionName[IMAGE_SIZEOF_SHORT_NAME];			
		for (j = 0; j < IMAGE_SIZEOF_SHORT_NAME; j++) {
			__byte b = (__byte)GenerateRandomNumber() % 26;//26����ĸ�е�һ��
			b += 'A';//ȫ��Ϊ��д��ĸ
			SectionName[j] = b;
		}
		__logic_memcpy__(pSectionHeader->Name, SectionName, IMAGE_SIZEOF_SHORT_NAME);
	}/* end for */
}

__bool __API__ IsValidPE32(__memory pMem) {
	PIMAGE_DOS_HEADER pDosH = (PIMAGE_DOS_HEADER)pMem;
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	return ((pDosH->e_magic == IMAGE_DOS_SIGNATURE) && (pNtH->Signature == IMAGE_NT_SIGNATURE));
}

__bool __API__ IsDriverFile(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	// �����ж��Ƿ���һ��PE�ļ�
	if (IsValidPE32(pMem) == FALSE) return FALSE;
	pNtHeader = GetNtHeader(pMem);
	if ((pNtHeader->OptionalHeader).Subsystem == IMAGE_SUBSYSTEM_NATIVE)
		return TRUE;
	return FALSE;
}

__bool __API__ IsDllFile(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	// �����ж��Ƿ���һ��PE�ļ�
	if (IsValidPE32(pMem) == FALSE) return FALSE;
	pNtHeader = GetNtHeader(pMem);
	if ((pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DLL) == IMAGE_FILE_DLL)
		return TRUE;
	return FALSE;
}

__bool __API__ IsRawSection(PIMAGE_SECTION_HEADER pSectH) {
	return !((pSectH->SizeOfRawData == 0) || (pSectH->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA));
}

__dword __API__ GetTimeDateStamp(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHeader = NULL;
	// �����ж��Ƿ���һ��PE�ļ�
	if (IsValidPE32(pMem) == FALSE) return 0;
	pNtHeader = GetNtHeader(pMem);
	return pNtHeader->FileHeader.TimeDateStamp;
}

__bool __API__ InThisSection(PIMAGE_SECTION_HEADER pSectH, __offset ofOffset, __bool bRva) {
	return (bRva ? (ofOffset >= (__offset)(pSectH->VirtualAddress)) && (ofOffset < (__offset)(pSectH->VirtualAddress + pSectH->Misc.VirtualSize)) :
			(ofOffset >= (__offset)(pSectH->PointerToRawData)) && (ofOffset < (__offset)(pSectH->PointerToRawData + pSectH->SizeOfRawData)));
}

__bool __API__ InThisSectionByAddress(PIMAGE_SECTION_HEADER pSectH, __address addrImageBase, __address addrAddress) {
	return ((addrAddress >= (__address)(addrImageBase + pSectH->VirtualAddress)) && \
		(addrAddress < (addrImageBase + (__address)(pSectH->VirtualAddress + pSectH->Misc.VirtualSize))));
}

PIMAGE_SECTION_HEADER __API__ Rva2Section(__memory pMem, __offset ofRva) {
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	PIMAGE_SECTION_HEADER pSectH = GetFirstSectionByNtHeader(pNtH);
	__word wNumOfSects = pNtH->FileHeader.NumberOfSections;
	while (wNumOfSects > 0) {
		if (InThisSection(pSectH, ofRva, TRUE))
			break;

		--wNumOfSects;
		++pSectH;
	}

	return (0 == wNumOfSects ? NULL : pSectH);
}

PIMAGE_SECTION_HEADER __API__ Raw2Section(__memory pMem, __offset ofRaw) {
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	PIMAGE_SECTION_HEADER pSectH = GetFirstSectionByNtHeader(pNtH);
	__word wNumOfSects = pNtH->FileHeader.NumberOfSections;
	while (wNumOfSects > 0) {
		if (InThisSection(pSectH, ofRaw, FALSE))
			break;

		--wNumOfSects;
		pSectH++;
	}

	return (0 == wNumOfSects ? NULL : pSectH);
}

__offset __API__ Rva2Raw(__memory pMem, __offset ofRva) {
	PIMAGE_SECTION_HEADER pSectH = Rva2Section(pMem, ofRva);
	return ((NULL == pSectH) ? NULL : (ofRva - pSectH->VirtualAddress + pSectH->PointerToRawData));
}

__offset __API__ Raw2Rva(__CONST__ __memory pMem, __offset ofRaw) {
	PIMAGE_SECTION_HEADER pSectH = Raw2Section(pMem, ofRaw);
	return ((NULL == pSectH) ? NULL : (ofRaw - pSectH->PointerToRawData + pSectH->VirtualAddress));
}

__integer __API__ Alig(__integer n, __integer iFactor, __bool bUp) {
	__integer r = n % iFactor;
	if (0 == r)
		return n;

	n -= r;//��ȥ��������
	if (bUp)
		n += iFactor;

	return n;
}

__address __API__ AligImageBase(__address n) {
	if (n == 0) return 0;
	n = Alig(n, __IMAGE_BASE_ALIGN__, TRUE);
	return n;
}

__void __API__ CopyExportTableTo(PIMAGE_EXPORT_DIRECTORY pExportDir, __offset ofNewRva, __offset ofOldRva) {
	__char *pszNames = NULL;
	__integer i = 0;
	__offset ofOffset = ofNewRva - ofOldRva;

	(__offset)(pExportDir->Name) += ofOffset;
	(__offset)(pExportDir->AddressOfNames) += ofOffset;
	(__offset)(pExportDir->AddressOfFunctions) += ofOffset;
	(__offset)(pExportDir->AddressOfNameOrdinals) += ofOffset;

	pszNames = (__char *)((__integer)(pExportDir->AddressOfNames - ofNewRva) + (__integer)pExportDir);
	for (i = 0; i < pExportDir->NumberOfNames; ++i)
		pszNames[i] += ofOffset;
}

__void __API__ CopyImportTableTo(PIMAGE_IMPORT_DESCRIPTOR pImportDesc, __offset ofNewRva, __offset ofOldRva) {
	__offset ofOffset = ofNewRva - ofOldRva;
	PIMAGE_IMPORT_DESCRIPTOR pImportDesc2 = pImportDesc;

	while (pImportDesc->Name) {
		PIMAGE_THUNK_DATA pFirstThunk, pOrgFirstThunk = NULL;
		(__offset)(pImportDesc->Name) += ofOffset;

		pFirstThunk	 = (PIMAGE_THUNK_DATA)((__integer)(pImportDesc->FirstThunk) - (__integer)ofOldRva + (__integer)pImportDesc2);
		(__offset)(pImportDesc->FirstThunk) += ofOffset;

		if (pImportDesc->OriginalFirstThunk) {
			pOrgFirstThunk = (PIMAGE_THUNK_DATA)((__integer)(pImportDesc->OriginalFirstThunk) - (__integer)ofOldRva + (__integer)pImportDesc2);
			(__offset)(pImportDesc->OriginalFirstThunk)	+= ofOffset;
		}

		while (pFirstThunk->u1.AddressOfData) {
			if (~IMAGE_ORDINAL_FLAG & pFirstThunk->u1.Ordinal) {
				(__offset)(pFirstThunk->u1.AddressOfData) += ofOffset;
				if (pOrgFirstThunk)
					(__offset)(pOrgFirstThunk->u1.AddressOfData) += ofOffset;
			}
			
			if (pOrgFirstThunk)
				++pOrgFirstThunk;

			++pFirstThunk;
		}

		++pImportDesc;
	}
}

__INLINE__ __void __INTERNAL_FUNC__ _CopyResourceTo(PIMAGE_RESOURCE_DIRECTORY pResDir, PIMAGE_RESOURCE_DIRECTORY pRootResDir, __offset ofNewRva, __offset ofOldRva) {
	__offset ofOffset = ofNewRva - ofOldRva;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pResDirEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((__integer)pResDir + sizeof(IMAGE_RESOURCE_DIRECTORY));
	__integer j = pResDir->NumberOfIdEntries + pResDir->NumberOfNamedEntries;
	while (j--) {
		if (pResDirEntry->DataIsDirectory) {
			_CopyResourceTo((PIMAGE_RESOURCE_DIRECTORY)((__integer)pRootResDir + (__integer)(pResDirEntry->OffsetToDirectory)), pRootResDir, ofNewRva + (__offset)(pResDirEntry->OffsetToDirectory), ofOldRva + (__offset)(pResDirEntry->OffsetToDirectory));
		} else {
			PIMAGE_RESOURCE_DATA_ENTRY pResDataDir = (PIMAGE_RESOURCE_DATA_ENTRY)((__integer)pRootResDir + (__integer)(pResDirEntry->OffsetToDirectory));
			(__offset)(pResDataDir->OffsetToData) += ofOffset;
		}
	}
}
__void __API__ CopyResourceTo(PIMAGE_RESOURCE_DIRECTORY pRootResDir, __offset ofNewRva, __offset ofOldRva) {
	_CopyResourceTo(pRootResDir, pRootResDir, ofNewRva, ofOldRva);
}

__INLINE__ PIMAGE_BASE_RELOCATION __INTERNAL_FUNC__ GetNextBaseRelocation(PIMAGE_BASE_RELOCATION pBr) {
	return (PIMAGE_BASE_RELOCATION)((__integer)(pBr + 1) + ((pBr->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) >> 1));
}

__void __API__ CopyBaseRelocationTo(PIMAGE_BASE_RELOCATION pBaseRelocation, __offset ofNewRva, __offset ofOldRva, __integer iSize) {
	__offset ofOffset = ofNewRva - ofOldRva;
	__integer iCurrSize = 0;
	for (iCurrSize = 0; iCurrSize <= iSize; iCurrSize += pBaseRelocation->SizeOfBlock) {
		(__offset)(pBaseRelocation->VirtualAddress) += ofOffset;
		pBaseRelocation = GetNextBaseRelocation(pBaseRelocation);
	}
}

__INLINE__ PIMAGE_RESOURCE_DATA_ENTRY __INTERNAL_FUNC__ _FindResourceInfo(PIMAGE_RESOURCE_DIRECTORY pDir, PIMAGE_RESOURCE_DIRECTORY pRoot, __integer iType) {
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((__integer)pDir + sizeof(IMAGE_RESOURCE_DIRECTORY));
	__integer iNumberEntries = (__integer)(pDir->NumberOfNamedEntries + pDir->NumberOfIdEntries);
	__integer i = 0;
	for (i = 0; i < iNumberEntries; i++, pEntry++) {
		if (pEntry->DataIsDirectory) {//�����Ŀ¼
			PIMAGE_RESOURCE_DIRECTORY pDir = (PIMAGE_RESOURCE_DIRECTORY)((__integer)pRoot + pEntry->OffsetToDirectory);
			return _FindResourceInfo(pDir, pRoot, iType);
		} else {
			PIMAGE_RESOURCE_DATA_ENTRY pDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)((__integer)pRoot + pEntry->OffsetToDirectory);
			return pDataEntry;
		}/* end else */
	}/* end for */
	return NULL;
}

PIMAGE_RESOURCE_DATA_ENTRY __API__ FindResourceInfo(__memory pMem, __integer iType) {
	PIMAGE_RESOURCE_DIRECTORY pResRoot = NULL;
	PIMAGE_RESOURCE_DIRECTORY_ENTRY pEntry = NULL;
	__integer iNumberEntries = 0, i = 0;
	PIMAGE_NT_HEADERS pNtHdr = GetNtHeader(pMem);//������Դ���Ƿ����
	if (pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress == 0)
		return NULL;

	// ��ȡROOT�ڵ�
	pResRoot = (PIMAGE_RESOURCE_DIRECTORY)((__integer)pMem + Rva2Raw(pMem, pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE].VirtualAddress));
	pEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((__integer)pResRoot + sizeof(IMAGE_RESOURCE_DIRECTORY));
	iNumberEntries = pResRoot->NumberOfNamedEntries + pResRoot->NumberOfIdEntries;
	for (i = 0; i < iNumberEntries; i++, pEntry++) {
		if (pEntry->Id == iType) {
			if (pEntry->DataIsDirectory) {//�����Ŀ¼
				PIMAGE_RESOURCE_DIRECTORY pDir = (PIMAGE_RESOURCE_DIRECTORY)((__integer)pResRoot + pEntry->OffsetToDirectory);
				// �ж��Ƿ��Ǽ��������������ֱ������
				PIMAGE_RESOURCE_DATA_ENTRY pRet = _FindResourceInfo(pDir, pResRoot, iType);
				if (pRet) return pRet;
			}
		}/* end if */
	}
	return NULL;
}

PRESOURCE_INFO __API__ GetManifestResourceInfo(__memory pMem) {
	PRESOURCE_INFO pInfo = NULL;
	PIMAGE_RESOURCE_DATA_ENTRY pManifestData = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_RESOURCE);
	if (pDataDirectory == NULL)
		return NULL;

	// Ѱ��manifest��Դ
	pManifestData = FindResourceInfo(pMem, 0x18);//manifest == 24
	if (pManifestData == NULL)
		return NULL;

	pInfo = (PRESOURCE_INFO)__logic_new__(RESOURCE_INFO, 1);
	if (pInfo == NULL)
		return NULL;

	pInfo->pDataEntry = pManifestData;
	pInfo->pPoint = (__memory)pMem + Rva2Raw((__memory)pMem, pManifestData->OffsetToData);
	pInfo->iPointSize = pManifestData->Size;
	return pInfo;
}

PRESOURCE_INFO __API__ GetIconResourceInfo(__memory pMem) {
	PRESOURCE_INFO pInfo = NULL;
	PIMAGE_RESOURCE_DATA_ENTRY pIconData = NULL;
	PIMAGE_DATA_DIRECTORY pDataDirectory = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_RESOURCE);
	if (pDataDirectory == NULL)
		return NULL;

	// ѰICON��Դ
	pIconData = FindResourceInfo(pMem, 0x03);//3 == ICON
	if (pIconData == NULL)
		return NULL;

	pInfo = (PRESOURCE_INFO)__logic_new__(RESOURCE_INFO, 1);
	if (pInfo == NULL)
		return NULL;

	pInfo->pDataEntry = pIconData;
	pInfo->pPoint = (__memory)pMem + Rva2Raw((LPVOID)pMem, pIconData->OffsetToData);
	pInfo->iPointSize = pIconData->Size;
	return pInfo;
}

__bool __API__ DeleteDataDirectoryObject(__memory pMem, __integer iObject) {
	__bool b = TRUE;
	PIMAGE_NT_HEADERS pNtH = NULL;
	PIMAGE_DATA_DIRECTORY pDir = NULL;
	PIMAGE_SECTION_HEADER pSectH = NULL;
	if (iObject >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return FALSE;
	pNtH = GetNtHeader(pMem);
	pDir = &(pNtH->OptionalHeader.DataDirectory[iObject]);
	pSectH = Rva2Section(pMem, pDir->VirtualAddress);

	if ((0 == pDir->VirtualAddress) || (0 == pDir->Size))
		return FALSE;

	//	�����Ŀ¼���ռһ��, ��ôֱ��ɾ�������,����ʹ�� 0 ���
	if ((pDir->VirtualAddress == pSectH->VirtualAddress) && (pDir->Size == pSectH->SizeOfRawData) || (pDir->Size == pSectH->Misc.VirtualSize))
		b = DelSectionBySectionHeader(pMem, pSectH) != 0;
	else {
		__logic_memset__(((__memory)pMem + Rva2Raw(pMem, pDir->VirtualAddress)), 0, pDir->Size);
	}

	pDir->VirtualAddress = 0;
	pDir->Size = 0;

	return b;
}

__bool __API__ DeleteDataDirectoryObjectOnMemMap(__memory pImageBase, __integer iObject) {
	__bool b = TRUE;
	PIMAGE_NT_HEADERS pNtH = NULL;
	PIMAGE_DATA_DIRECTORY pDir = NULL;
	PIMAGE_SECTION_HEADER pSectH = NULL;
	if (iObject >= IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return FALSE;
	pNtH = GetNtHeader(pImageBase);
	pDir = &(pNtH->OptionalHeader.DataDirectory[iObject]);
	pSectH = Rva2Section(pImageBase, pDir->VirtualAddress);

	if ((0 == pDir->VirtualAddress) || (0 == pDir->Size))
		return FALSE;

	//	�����Ŀ¼���ռһ��, ��ôֱ��ɾ�������,����ʹ�� 0 ���
	if ((pDir->VirtualAddress == pSectH->VirtualAddress) && (pDir->Size == pSectH->SizeOfRawData) || (pDir->Size == pSectH->Misc.VirtualSize))
		b = DelSectionBySectionHeader(pImageBase, pSectH) != 0;
	else {
		__logic_memset__(__RvaToVa__(pImageBase, pDir->VirtualAddress), 0, pDir->Size);
	}

	pDir->VirtualAddress = 0;
	pDir->Size = 0;
	return b;
}

PIMAGE_SECTION_HEADER __API__ AddSection(__memory pMem, __char *Name, __dword Characteristics, __dword VirtualSize, __memory pData, __bool bForce) {
	PIMAGE_NT_HEADERS pNtH = NULL;
	PIMAGE_SECTION_HEADER pFirstSectH = NULL;
	__word wNumOfSects = 0;
	PIMAGE_SECTION_HEADER pSectH1 = NULL;
	PIMAGE_SECTION_HEADER pSectH2 = NULL;
	PIMAGE_SECTION_HEADER pSectH = NULL;
	__integer iFirstPointer = 0;
	__integer x = 0;

	// �����ж�Ҫ���ӵĽڵĳߴ��Ƿ�Ϊ0, ���Ϊ0ֱ���˳�
	if (!VirtualSize) return NULL;

	pNtH = GetNtHeader(pMem);
	pFirstSectH = GetFirstSectionByNtHeader(pNtH);
	wNumOfSects = pNtH->FileHeader.NumberOfSections++; //������
	pSectH1 = pFirstSectH + wNumOfSects; //�µĽ�ͷλ��
	pSectH2 = pSectH1 - 1; //ԭ�����һ���ڵĽ�ͷ

	iFirstPointer = (__integer)(pMem + pFirstSectH->PointerToRawData);

	if ((__integer)pSectH1 - iFirstPointer < sizeof (IMAGE_SECTION_HEADER)) {//û�пռ乻���һ����ͷ 
		if (!bForce)
			return NULL;

		x = sizeof (IMAGE_SECTION_HEADER) - (__integer)pSectH1 + (__integer)(pFirstSectH->PointerToRawData + pMem); //�������˶����Լ�
		pSectH = pSectH2; //�Ժ�һ��һ��Ų
		while (wNumOfSects > 0) {
			__memory Src = pSectH->PointerToRawData + pMem;
			__memory Dst = Alig((__integer)(Src + x), pNtH->OptionalHeader.FileAlignment, TRUE);
			if (IsRawSection(pSectH)) {
				__logic_memmove__(Src, Dst, pSectH->SizeOfRawData); //����Ų
			}

			pSectH->PointerToRawData = (__integer)(Dst - pMem);
			--wNumOfSects;
			--pSectH;
		}
	}
	
	if (!Name) Name = "\0";

	__logic_memcpy__(pSectH1->Name, Name, IMAGE_SIZEOF_SHORT_NAME);
	pSectH1->Characteristics = Characteristics;
	pSectH1->SizeOfRawData = Alig(VirtualSize, pNtH->OptionalHeader.FileAlignment, TRUE);
	pSectH1->PointerToRawData = Alig(pSectH2->PointerToRawData + pSectH2->SizeOfRawData, pNtH->OptionalHeader.FileAlignment, TRUE);
	pSectH1->Misc.VirtualSize = VirtualSize;
	pSectH1->VirtualAddress = Alig(pSectH2->VirtualAddress + pSectH2->Misc.VirtualSize, pNtH->OptionalHeader.SectionAlignment, TRUE);
	pSectH1->PointerToLinenumbers = 0;
	pSectH1->NumberOfLinenumbers = 0;
	pSectH1->NumberOfRelocations = 0;
	pSectH1->PointerToRelocations = 0;

	if (pData)
		__logic_memcpy__((pMem + pSectH1->PointerToRawData), pData, VirtualSize);
	CorrectImageSize(pMem);
	
	return pSectH1;
}

PIMAGE_SECTION_HEADER __API__ CoverTailSectionFromImage(__memory pMem, __memory *pNewMemPoint, __integer iMemSize, __integer iNewSize, __memory pData, __integer iDataSize, __integer *piNewMemSize) {
	PIMAGE_SECTION_HEADER pTailSectionHdr = NULL;
	__integer iNewTailSectionSize = 0;
	__word wNumberOfSections = 0;
	__integer iMapSize = 0;
	__integer iTailDataSize = 0, iNewRealSize = 0;
	__memory pNewMem = NULL, pTailSection = NULL;
	__memory pTailData = (__memory)GetTailDataPoint(pMem, iMemSize);//����ĩβ����
	if (pTailData) {
		__memory pTmp = pTailData;
		iTailDataSize = iMemSize - (__integer)(pTailData - pMem);
		pTailData = (__memory)__logic_new_size__(iTailDataSize);
		__logic_memcpy__(pTailData, pTmp, iTailDataSize);
	}

	// �����µ�ӳ�䳤��
	wNumberOfSections = GetNumberOfSections(pMem);
	pTailSectionHdr = GetFirstSection(pMem) + (wNumberOfSections - 1);
	if (iNewSize < pTailSectionHdr->SizeOfRawData)
		iNewTailSectionSize = pTailSectionHdr->SizeOfRawData;
	else {
		__integer iFileAlign = GetNtHeader(pMem)->OptionalHeader.FileAlignment;
		iNewTailSectionSize = Alig(iNewSize, iFileAlign, TRUE);
	}
	iMapSize = pTailSectionHdr->PointerToRawData;
	iMapSize = iMapSize + iNewTailSectionSize + iTailDataSize;

	// ��������
	pNewMem = (__memory)__logic_new_size__(iMapSize);
	if (!pNewMem) {
		__logic_delete__((__void *)pTailData);
		return NULL;
	}
	__logic_memcpy__(pNewMem, pMem, iMemSize);
	__logic_delete__((__void *)pMem);//�ͷ�ԭ���ڴ�
	pMem = pNewMem;//��������ָ��

	// ��ȡ��ĩβ��
	pTailSectionHdr = GetFirstSection(pMem) + (wNumberOfSections - 1);
	if (!pTailSectionHdr) {
		if (pTailData)
			__logic_delete__((__void *)pTailData);
		__logic_delete__((__void *)pNewMem);
		return NULL;
	}

	// �����޶�ĩβ��
	pTailSectionHdr->Misc.VirtualSize = iNewSize;
	pTailSectionHdr->SizeOfRawData = iNewTailSectionSize;
	pTailSection = pMem + pTailSectionHdr->PointerToRawData;
	if ((pData) && (iDataSize))
		__logic_memcpy__(pTailSection, pData, iDataSize);

	// �����趨SizeOfImage
	CorrectImageSize(pMem);

	// ��������ĩβ����
	iNewRealSize = GetRealPeFileSize(pMem);//��ȡ��ǰ��ʵ���ļ�����
	if (iTailDataSize) {
		__memory pNewTailData = GetTailDataPoint(pMem, iMapSize);
		__logic_memcpy__(pNewTailData, pTailData, iTailDataSize);
	}

	*pNewMemPoint = pMem;
	*piNewMemSize = iMapSize;
	return pTailSectionHdr;
}

PIMAGE_SECTION_HEADER __API__ CoverTailSection(__tchar *pFilePath, __memory *pMemPoint, __integer iNewSize, __memory pData, __integer iDataSize, __integer *piNewMemSize) {
	__integer iFileSize = 0, iTailDataSize = 0, iNewTailSectionSize = 0, iMapSize = 0, iNewRealSize = 0;
	__word wNumberOfSections = 0;
	__memory pMem = NULL, pTailData = NULL;
	PIMAGE_SECTION_HEADER pTailSectionHdr = NULL;
	__memory pTailSection = NULL;

	// �Ƿ�رվɵ�ӳ��
	if (*pMemPoint) {
		UnMappingFile(*pMemPoint);
		*pMemPoint = NULL;
	}

	// ���ļ�ӳ��
	pMem = MappingFile(pFilePath, &iFileSize, FALSE, 0, 0);
	if (pMem == NULL)
		return NULL;

	// ����ĩβ����
	pTailData = GetTailDataPoint(pMem, iFileSize);
	if (pTailData) {
		__memory pTmp = pTailData;
		iTailDataSize = iFileSize - (__integer)(pTailData - pMem);
		pTailData = (__memory)__logic_new_size__(iTailDataSize);
		__logic_memcpy__(pTailData, pTmp, iTailDataSize);
	}

	// �����µ�ӳ�䳤��
	wNumberOfSections = GetNumberOfSections(pMem);
	pTailSectionHdr = GetFirstSection(pMem) + (wNumberOfSections - 1);
	if (iNewSize < pTailSectionHdr->SizeOfRawData)
		iNewTailSectionSize = pTailSectionHdr->SizeOfRawData;
	else {
		__integer iFileAlign = GetNtHeader(pMem)->OptionalHeader.FileAlignment;
		iNewTailSectionSize = Alig(iNewSize, iFileAlign, TRUE);
	}
	iMapSize = pTailSectionHdr->PointerToRawData;
	iMapSize = iMapSize + iNewTailSectionSize + iTailDataSize;

	// ���ӳ��
	UnMappingFile(pMem);
	
	// �ٴδ�ӳ�䰴���µ�ӳ�䳤��
	pMem = MappingFile(pFilePath, &iFileSize, TRUE, 0, iMapSize);
	if (!pMem) {
		if (pTailData)
			__logic_delete__((__void *)pTailData);
		return NULL;
	}

	// ��ȡ��ĩβ��
	pTailSectionHdr = GetFirstSection(pMem) + (wNumberOfSections - 1);
	if (!pTailSectionHdr) {
		if (pTailData)
			__logic_delete__((__void *)pTailData);
		UnMappingFile(pMem);
		return NULL;
	}

	// �����޶�ĩβ��
	pTailSectionHdr->Misc.VirtualSize = iNewSize;
	pTailSectionHdr->SizeOfRawData = iNewTailSectionSize;
	pTailSection = pMem + pTailSectionHdr->PointerToRawData;
	if ((pData) && (iDataSize))
		__logic_memcpy__(pTailSection, pData, iDataSize);

	// �����趨SizeOfImage
	CorrectImageSize(pMem);

	// ��������ĩβ����
	iNewRealSize = GetRealPeFileSize(pMem);//��ȡ��ǰ��ʵ���ļ�����
	if (iTailDataSize) {
		__memory pNewTailData = GetTailDataPoint(pMem, iNewRealSize + iTailDataSize);//iNewRealSize + iTailDataSize = �ļ���ʵ�ĳ���
		__logic_memcpy__(pNewTailData, pTailData, iTailDataSize);
	}

	*pMemPoint = pMem;
	*piNewMemSize = iNewRealSize + iTailDataSize;
	return pTailSectionHdr;
}

__integer __API__ CorrectImageSize(__memory pMem) {
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	PIMAGE_SECTION_HEADER pSectH = GetFirstSectionByNtHeader(pNtH) + pNtH->FileHeader.NumberOfSections - 1;
	
	pNtH->OptionalHeader.SizeOfImage = Alig(pSectH->VirtualAddress + pSectH->Misc.VirtualSize, pNtH->OptionalHeader.SectionAlignment, TRUE);
	return pNtH->OptionalHeader.SizeOfImage;
}

__bool __API__ DelSectionBySectionIndex(__memory pMem, __integer n) {
	return DelSectionBySectionHeader(pMem, GetFirstSection(pMem) + (n - 1));
}

__bool __API__ DelSectionBySectionName(__memory pMem, __char *pName) {
	return DelSectionBySectionHeader(pMem, GetSection(pMem, pName));
}

__bool __API__ DelSectionBySectionHeader(__memory pMem, PIMAGE_SECTION_HEADER pSectH) {
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	PIMAGE_SECTION_HEADER pCurrSecHdr = GetFirstSectionByNtHeader(pNtH);
	__integer i = 0, iNumberOfSections = 0;

	if ((!pMem) || (!pSectH))
		return FALSE;
	
	iNumberOfSections = (__integer)(pNtH->FileHeader.NumberOfSections);
	for (i = 0; i < iNumberOfSections; i++) {
		if (__logic_memcmp__(pCurrSecHdr, pSectH, sizeof (IMAGE_SECTION_HEADER)) == 0) {//�Ƿ���Ҫɾ���Ľ�
			__memory pPoint = pMem + pCurrSecHdr->PointerToRawData;
			__logic_memset__(pPoint, 0, pCurrSecHdr->SizeOfRawData);
			__logic_memset__(pCurrSecHdr, 0, sizeof(IMAGE_SECTION_HEADER));
			return TRUE;
		}
		pCurrSecHdr++;
	}
	return FALSE;
}

__bool __API__ DelSectionBySectionIndexOnMemMap(__memory pMem, __integer n) {
	return DelSectionBySectionHeaderOnMemMap(pMem, GetFirstSection(pMem) + (n - 1));
}

__bool __API__ DelSectionBySectionNameOnMemMap(__memory pMem, __char *pName) {
	return DelSectionBySectionHeaderOnMemMap(pMem, GetSection(pMem, pName));
}

__bool __API__ DelSectionBySectionHeaderOnMemMap(__memory pMem, PIMAGE_SECTION_HEADER pSectH) {
	PIMAGE_NT_HEADERS pNtH = GetNtHeader(pMem);
	PIMAGE_SECTION_HEADER pCurrSecHdr = GetFirstSectionByNtHeader(pNtH);
	__integer i = 0, iNumberOfSections = 0;

	if ((!pMem) || (!pSectH))
		return FALSE;

	iNumberOfSections = (__integer)(pNtH->FileHeader.NumberOfSections);
	for (i = 0; i < iNumberOfSections; i++) {
		if (__logic_memcmp__(pCurrSecHdr, pSectH, sizeof (IMAGE_SECTION_HEADER)) == 0) {//�Ƿ���Ҫɾ���Ľ�
			__memory pPoint = __RvaToVa__(pMem, pCurrSecHdr->VirtualAddress);
			__logic_memset__(pPoint, 0, pCurrSecHdr->Misc.VirtualSize);
			__logic_memset__(pCurrSecHdr, 0, sizeof(IMAGE_SECTION_HEADER));
			return TRUE;
		}
		pCurrSecHdr++;
	}
	return FALSE;
}

__integer __API__ GetFilePhySize(__tchar *lpszFile) {
	__integer iHighFileSize = 0, iRet = 0;
	HANDLE hFile = CreateFile(lpszFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return 0;

	iRet = GetFileSize(hFile, &iHighFileSize);
	CloseHandle(hFile);
	return iRet;
}

__memory __API__ MappingFile(__tchar *lpszFile, __integer *piFileSize, __bool bWrite, __integer iMaximumSizeHigh, __integer iMaximumSizeLow) {
	__memory pRet = NULL;
	__integer iFileSize = 0, iMapSize;
	HANDLE hFileMapping;
	HANDLE hFile = CreateFile(lpszFile, (bWrite ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ), bWrite ? 0 : FILE_SHARE_READ, NULL, OPEN_ALWAYS,
							FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (INVALID_HANDLE_VALUE == hFile)
		return NULL;
	
	iFileSize = GetFileSize(hFile, NULL);
 
	if (piFileSize)
		*piFileSize = iFileSize;

	iMapSize = iMaximumSizeLow;

	// �����λΪ0��ʹ�õ�ǰ�ļ���С����ӳ��
	if (iMapSize == 0)
		iMapSize = iFileSize;

	hFileMapping = CreateFileMapping(hFile, NULL, (bWrite ? PAGE_READWRITE : PAGE_READONLY), iMaximumSizeHigh, iMapSize, NULL);
	CloseHandle(hFile);
	if (!hFileMapping)
		return NULL;

	pRet = MapViewOfFile(hFileMapping, (bWrite ? (FILE_MAP_WRITE | FILE_MAP_READ) : FILE_MAP_READ), 0, 0, 0);
	CloseHandle(hFileMapping);
	return pRet;
}

__memory __API__ MappingFilePlusNewSize(__tchar *lpszFile, __memory pMap, __integer iOldSize, __integer iNewPlusSize, __bool bWrite) {
	__integer iFileSize = 0;
	__memory pNewMap = NULL;
	UnMappingFile(pMap);
	pNewMap = (LPBYTE)MappingFile(lpszFile, &iFileSize, bWrite, 0, iOldSize + iNewPlusSize);
	if (!pNewMap)
		return NULL;
	return pNewMap;
}

__bool __API__ UnMappingFile(__memory pBaseAddress) {
	FlushViewOfFile(pBaseAddress, 0);
	return UnmapViewOfFile(pBaseAddress);
}

__integer __API__ GetRealPeFileSize(__memory pMem) {
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	PIMAGE_SECTION_HEADER pFirstSectionHeader = NULL, pTailSectionHeader = NULL;
	__integer iRet = 0;
	__word wSectionNumber = 0;
	__integer i = 0;

	pNtHdr = __GetNtHeader__(pMem);
	wSectionNumber = pNtHdr->FileHeader.NumberOfSections;
	pFirstSectionHeader = __GetSectionHeader__(pNtHdr);
	pTailSectionHeader = pFirstSectionHeader + (wSectionNumber - 1);//��ȡĩβ��
	iRet = pTailSectionHeader->PointerToRawData + pTailSectionHeader->SizeOfRawData;
	
	return iRet;
}

__memory __API__ GetTailDataPoint(__memory pMem, __integer iFileSize) {
	__word wSectionNumber = GetNumberOfSections(pMem);
	PIMAGE_SECTION_HEADER pSectionHeader = GetFirstSection(pMem);
	PIMAGE_SECTION_HEADER pLastSectionTbl = &pSectionHeader[wSectionNumber - 1];//��ĩβ�ڱ�
	__offset ofOffset = (__offset)(pLastSectionTbl->PointerToRawData + pLastSectionTbl->SizeOfRawData);//�ļ�ʵ�ʳ���
	if (ofOffset == iFileSize) return NULL;
	return (pMem + ofOffset);
}

__integer __API__ GetTailDataSize(__memory pMem, __integer iFileSize) {
	__integer iTailDataSize = 0;
	__memory pTailData = GetTailDataPoint(pMem, iFileSize);
	if (!pTailData)
		return 0;
	
	iTailDataSize = iFileSize - ((__integer)pTailData - (__integer)pMem);
	return iTailDataSize;
}

__integer __API__ ResetFileTail(__tchar *lpszFile, __integer iNewTail) {
	__integer iLen = 0;
	HANDLE hFile = CreateFile(lpszFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_ARCHIVE, NULL);
	if (hFile == NULL)
		return 0;

	iLen = SetFilePointer(hFile, iNewTail, NULL, FILE_BEGIN);
	if (SetEndOfFile(hFile) == FALSE) {
		CloseHandle(hFile);
		return 0;
	}
	CloseHandle(hFile);
	return iLen;
}

__integer __API__ GetAddSectionMapSize(__memory pMem, __integer iNewMap) {
	__word wSectionNumber = GetNumberOfSections(pMem);
	PIMAGE_SECTION_HEADER pSectionHeader = GetFirstSection(pMem);
	PIMAGE_SECTION_HEADER pLastSectionTbl = &pSectionHeader[wSectionNumber - 1];//��ĩβ�ڱ�
	__integer iOrigEnd = pLastSectionTbl->PointerToRawData + pLastSectionTbl->SizeOfRawData;
	PIMAGE_NT_HEADERS pNt = GetNtHeader(pMem);
	__integer iFileAlig = pNt->OptionalHeader.FileAlignment;
	__integer iAlig = 0, iRemain = iOrigEnd % iFileAlig;//ʹ���ļ����Ƚ��ж���
	__integer iRet = 0;
	iAlig = iOrigEnd;
	if (iRemain) {
		iAlig -= iRemain;//��ȥ��������
		iAlig += iFileAlig;//��һ����������
	}
	iRet = iAlig - iOrigEnd;//�����ĳ��ȼ�ȥԭ�ȵĳ���,ΪҪ���ӵ�ӳ�䳤��
	iRet += Alig(iNewMap, iFileAlig, TRUE);//��ָ����ӳ�䳤��Ҳ����һ�ζ���
	return iRet;
}

__bool __API__ CopyMemToMem(__memory pFromMemory, __memory pToMemory, __integer iSizeOfImage) {
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	__word wNumberOfSection = 0;
	__integer iHdrLen = 0;
	__integer i = 0;

	pNt = GetNtHeader(pFromMemory);
	pSectionHeader = GetFirstSectionByNtHeader(pNt);
	wNumberOfSection = pNt->FileHeader.NumberOfSections;

	// �����Ϊbbs�ڵ����
	while (pSectionHeader->PointerToRawData == 0) {
		pSectionHeader++;
	}

	iHdrLen = pSectionHeader->PointerToRawData;//�ӵ�һ���ڴ�������ȫ����PEͷ����,��������ȫ

	// ���»�ȡ�׽�ͷ
	pSectionHeader = GetFirstSectionByNtHeader(pNt);

	__logic_memset__(pToMemory, 0, iSizeOfImage);//���Ƚ�Ŀ��ӳ������ɾ�
	__logic_memcpy__(pToMemory, pFromMemory, iHdrLen);//����PEͷ+��ͷ

	for (i = 0; i < wNumberOfSection; i++) {
		__memory pSecMem1Addr = NULL, pSecMem2Addr = NULL;
		__integer iSecSize = 0;
		// ���ƽ�����
		pSecMem1Addr = pToMemory + pSectionHeader->VirtualAddress;
		pSecMem2Addr = pFromMemory + pSectionHeader->VirtualAddress;

		// WIN7����ϵͳ,ϵͳ������kernel32.dll ��Щ���ܻ������Щ��Ѫ������
		if (pSectionHeader->SizeOfRawData > pSectionHeader->Misc.VirtualSize)
			iSecSize = pSectionHeader->Misc.VirtualSize;
		else
			iSecSize = pSectionHeader->SizeOfRawData;

		if (iSecSize != 0)
			__logic_memcpy__(pSecMem1Addr, pSecMem2Addr, iSecSize);
		pSectionHeader++;
	}

	return TRUE;
}

__bool __API__ CopyMemToMemBySecAlign(__memory pFromMemory, __memory pToMemory, __integer iSizeOfImage) {
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;
	__word wNumberOfSection = 0;
	__integer iHdrLen = 0;
	__integer i = 0;

	pNt = GetNtHeader(pFromMemory);
	pSectionHeader = GetFirstSectionByNtHeader(pNt);
	wNumberOfSection = pNt->FileHeader.NumberOfSections;

	// �����Ϊbbs�ڵ����
	while (pSectionHeader->PointerToRawData == 0) {
		pSectionHeader++;
	}

	iHdrLen = pSectionHeader->PointerToRawData;//�ӵ�һ���ڴ�������ȫ����PEͷ����,��������ȫ

	// ���»�ȡ�׽�ͷ
	pSectionHeader = GetFirstSectionByNtHeader(pNt);

	__logic_memset__(pToMemory, 0, iSizeOfImage);//���Ƚ�Ŀ��ӳ������ɾ�
	__logic_memcpy__(pToMemory, pFromMemory, iHdrLen);//����PEͷ+��ͷ
	
	for (i = 0; i < wNumberOfSection; i++) {
		__memory pSecMemAddr, pSecFileAddr;
		__integer iSecSize;
		// ���ƽ�����
		pSecMemAddr = pToMemory + pSectionHeader->VirtualAddress;
		pSecFileAddr = pFromMemory + Rva2Raw(pFromMemory, pSectionHeader->VirtualAddress);
		iSecSize = pSectionHeader->SizeOfRawData;
		if (iSecSize != 0)
			__logic_memcpy__(pSecMemAddr, pSecFileAddr, iSecSize);
		pSectionHeader++;
	}

	return TRUE;
}

__bool __API__ ProtectImage(__memory pImageBase, __bool bWriteAll) {
	PIMAGE_NT_HEADERS pNt = GetNtHeader(pImageBase);
	__word wSectionNumber = pNt->FileHeader.NumberOfSections;
	__integer iHdrLen = pNt->OptionalHeader.SizeOfHeaders;
	__integer iSecAlign = pNt->OptionalHeader.SectionAlignment;
	__integer i = 0;
	PIMAGE_SECTION_HEADER pSectionHeadr = (PIMAGE_SECTION_HEADER)GetFirstSection(pImageBase);//������
	for (i = 0; i < wSectionNumber; i++) {
		__integer iNewProtect = 0, dwOldProtect;
		__bool bWrite, bRead, bExec, bShared;
		__memory pSecMemAddr;
		__integer iSecLen;
		pSecMemAddr = (__memory)__RvaToVa__(pImageBase, pSectionHeadr->VirtualAddress);
		iSecLen = Alig(pSectionHeadr->Misc.VirtualSize, iSecAlign, TRUE);
		
		// ��������
		bWrite = (pSectionHeadr->Characteristics & IMAGE_SCN_MEM_WRITE) ? TRUE : FALSE;
		bRead = (pSectionHeadr->Characteristics & IMAGE_SCN_MEM_READ) ? TRUE : FALSE;
		bExec = (pSectionHeadr->Characteristics & IMAGE_SCN_MEM_EXECUTE) ? TRUE : FALSE;
		bShared = (pSectionHeadr->Characteristics & IMAGE_SCN_MEM_SHARED) ? TRUE : FALSE;

		if (bWrite && bRead && bExec && bShared) iNewProtect = PAGE_EXECUTE_READWRITE;
		else if (bWrite && bRead && bExec) iNewProtect = PAGE_EXECUTE_WRITECOPY;
		else if (bRead && bExec) iNewProtect = PAGE_EXECUTE_READ;
		else if (bExec) iNewProtect = PAGE_EXECUTE;
		else if (bWrite && bRead && bShared) iNewProtect = PAGE_READWRITE;
		else if (bWrite && bRead) iNewProtect = PAGE_WRITECOPY;
		else if (bRead) iNewProtect = PAGE_READONLY;
		if (bWriteAll) iNewProtect |= PAGE_WRITECOPY;
		if (pSectionHeadr->Characteristics & IMAGE_SCN_MEM_NOT_CACHED) iNewProtect |= PAGE_NOCACHE;
		if (!iNewProtect) return FALSE;
		VirtualProtect(pSecMemAddr, iSecLen, iNewProtect, &dwOldProtect);
		pSectionHeadr++;
	}
	return TRUE;
}

__bool __API__ SetSectionCharacteristics(__memory pImageBase) {
	return ProtectImage(pImageBase, FALSE);
}

__void __API__ SetAllSectionCharacteristics(__memory pMem, __dword dwCharacteristics) {
	PIMAGE_NT_HEADERS pNtHdr = GetNtHeader(pMem);
	PIMAGE_SECTION_HEADER pSectionHdr = GetFirstSectionByNtHeader(pNtHdr);
	__word wNumberOfSection = pNtHdr->FileHeader.NumberOfSections;
	__integer i = 0;
	for (i = 0; i < wNumberOfSection; i++, pSectionHdr++){
		pSectionHdr->Characteristics |= dwCharacteristics;
	}
}

__bool __API__ RefixBoundIAT(__memory pImageBase) {
	/*
	 * 
	 * Ĭ����������Ϊ��Ϊ�ӳټ��ص� DLL �����ɰ󶨵ĵ����ַ��
	 * ������� DLL���� Helper ��������ͼʹ�ð���Ϣ�������Ƕ�ÿ�����õĵ������ GetProcAddress��
	 * ���ʱ�������ѡ��ַ����ص� DLL ��ʱ�������ѡ��ַ��ƥ�䣬�� Helper ����������󶨵����ַ��
	 * ���ڲ�������ִ�У��ͺ���õ�ַ�������ڣ���
	 * �����δ����� DLL ���ӳټ��ص��룬��������������������ָ�� /delay:nobind ����ֹ���ɰ󶨵���
	 * ��ַ����ֹ�õ�ַ��ʹ��ͼ���ļ��еĿռ䡣
	 */
	//PCHAR ImportedName = NULL;
	//PIMAGE_BOUND_IMPORT_DESCRIPTOR BoundImportDescriptor = ExistDataDirectory(pImageBase, IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT);
	//if (BoundImportDescriptor == NULL)
	//	return FALSE;
	return TRUE;
}

__bool __API__ RefixIAT(__memory pImageBase) {
	__integer iOrigIatSize = 0;
	__integer iOldProtect = 0;
	__integer iImportAddressTableSize = 0;
	__integer iOldImportAddressTableProtect = 0;
	__integer iSizeOfImage = GetNtHeader(pImageBase)->OptionalHeader.SizeOfImage;
	__memory pImportAddressTable = NULL;
	PIMAGE_DATA_DIRECTORY pImportAddressDataDir = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pOrigIatAddress = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImportDes = NULL;
	PIMAGE_DATA_DIRECTORY pImportDataDir = ExistDataDirectory(pImageBase, IMAGE_DIRECTORY_ENTRY_IMPORT);//����Ƿ�ӵ��ӳ���
	if (pImportDataDir == NULL)
		return FALSE;

	pOrigIatAddress = (PIMAGE_IMPORT_DESCRIPTOR)(__RvaToVa__(pImageBase, pImportDataDir->VirtualAddress));
	iOrigIatSize = pImportDataDir->Size;

	VirtualProtect((__void *)pOrigIatAddress, iOrigIatSize, PAGE_EXECUTE_WRITECOPY, &iOldProtect);
	// ���ｫλ������Ŀ¼12�����������ַ��ĵ�ַҲ����Ϊ��д,��������������ô��ַ�������ڴ˴�
	pImportAddressDataDir = ExistDataDirectory(pImageBase, IMAGE_DIRECTORY_ENTRY_IAT);
	if (pImportAddressDataDir) {
		pImportAddressTable = (__memory)__RvaToVa__(pImageBase, pImportAddressDataDir->VirtualAddress);
		iImportAddressTableSize = pImportAddressDataDir->Size;
		VirtualProtect((__void *)pImportAddressTable, iImportAddressTableSize, PAGE_EXECUTE_WRITECOPY, &iOldImportAddressTableProtect);
	}

	pImportDes = (PIMAGE_IMPORT_DESCRIPTOR)__RvaToVa__(pImageBase, pImportDataDir->VirtualAddress);

	// ��Щ����ֻ����FirstThunk
	while ((pImportDes->OriginalFirstThunk) || (pImportDes->FirstThunk)) {
		__char *svDllName = (__char *)__RvaToVa__(pImageBase, pImportDes->Name);
		HMODULE hDll = GetModuleHandleA((__char *)svDllName);//ӳ��⵽��ַ�ռ�
		if(!hDll) hDll = LoadLibraryA(svDllName);
		if(!hDll) {
			return FALSE;
		}

		if (pImportDes->TimeDateStamp == 0 || TRUE) {
			PIMAGE_THUNK_DATA pTdIn, pTdOut;
			pImportDes->ForwarderChain = (__integer)hDll;
			pImportDes->TimeDateStamp = 0xCDC31337; // This is bullshit cuz I don't want to call libc.

			// ���������ַ
			if (pImportDes->OriginalFirstThunk == 0)//������ֶ�Ϊ0,��ʹ��FirstThunk
				pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(pImageBase, pImportDes->FirstThunk);
			else
				pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(pImageBase, pImportDes->OriginalFirstThunk);
			pTdOut = (PIMAGE_THUNK_DATA)__RvaToVa__(pImageBase, pImportDes->FirstThunk);

			while(pTdIn->u1.Function) {
				FARPROC pFunc;

				// ��������������Ժ���������
				if (pTdIn->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
					pFunc = GetProcAddress(hDll, (__char *)MAKEINTRESOURCE(pTdIn->u1.Ordinal));//�������
				} else {
					// ����������
					PIMAGE_IMPORT_BY_NAME pIbn;
					pIbn = (PIMAGE_IMPORT_BY_NAME)__RvaToVa__(pImageBase, pTdIn->u1.AddressOfData);
					pFunc = GetProcAddress(hDll, (__char *)pIbn->Name);
				}

				// ����ȡ������ַ�Ƿ�ʧ��
				if (!pFunc) {
					return FALSE;
				}

				pTdOut->u1.Function = (__dword)pFunc;
				pTdIn++;
				pTdOut++;
			}/* end while */
		}/* end if */
		// ��һ��DLL
		pImportDes++;
	}/* end while */

	// �趨����ڴ�Ϊԭ����
	VirtualProtect((__void *)pOrigIatAddress, iOrigIatSize, iOldProtect, NULL);
	if (pImportAddressTable)//��ַ�����
		VirtualProtect((__void *)pImportAddressTable, iImportAddressTableSize, iOldImportAddressTableProtect, NULL);
	return TRUE;
}

__bool __API__ EncryptIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescryptor, __address addrImageBase, FPCryptIAT pEncryptIAT, __void *pEncryptFuncArgs, __bool bOnFile) {
	__memory pMem = NULL;
	__integer iLibIndex = 0, iProcIndex = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDes = pImportDescryptor;

	if (!pEncryptIAT) return FALSE;
	if (bOnFile) pMem = (__memory)addrImageBase;

	// ��Щ����ֻ����FirstThunk
	while ((pImportDes->OriginalFirstThunk) || (pImportDes->FirstThunk)) {
		__char *svDllName = NULL;
		PIMAGE_THUNK_DATA pTdIn = NULL;
		PIMAGE_THUNK_DATA pTdOut = NULL;
		__dword dwFirstThunk = 0;
		if (bOnFile)
			svDllName = (__char *)(pMem + Rva2Raw((__void *)pMem, pImportDes->Name));
		else
			svDllName = (__char *)__RvaToVa__(addrImageBase, pImportDes->Name);

		// ���ܿ���
		pEncryptIAT(iLibIndex, LIB_NAME, (__byte *)svDllName, __logic_strlen__(svDllName), FALSE, (__address)svDllName, pEncryptFuncArgs);
		iLibIndex++;

		// ���������ַ
		if (pImportDes->OriginalFirstThunk == 0) {//������ֶ�Ϊ0,��ʹ��FirstThunk
			if (bOnFile) {
				pTdIn = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw((__void *)pMem, pImportDes->FirstThunk));
			} else {
				pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(addrImageBase, pImportDes->FirstThunk);
			}
			pTdOut = pTdIn;
		} else {
			if (bOnFile) {
				pTdIn = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw((__void *)pMem, pImportDes->OriginalFirstThunk));
				pTdOut = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw((__void *)pMem, pImportDes->FirstThunk));
			} else {
				pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(addrImageBase, pImportDes->OriginalFirstThunk);
				pTdOut = (PIMAGE_THUNK_DATA)__RvaToVa__(addrImageBase, pImportDes->FirstThunk);
			}
		}
		
		iProcIndex = 0;
		// ���ܺ���
		while (pTdIn->u1.Function) {
			dwFirstThunk = (__dword)((__address)pTdOut - addrImageBase);//��¼FirstThunk

			// ��������������Ժ���������
			if (pTdIn->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
				__word wOrd = pTdIn->u1.Ordinal;
				pEncryptIAT(iProcIndex, PROC_NAME, (__memory)&wOrd, 2, TRUE, dwFirstThunk, pEncryptFuncArgs);
			} else {
				__char *szProcName = NULL;
				PIMAGE_IMPORT_BY_NAME pIbn = NULL;
				if (bOnFile)
					pIbn = (PIMAGE_IMPORT_BY_NAME)(pMem + Rva2Raw((__memory)pMem, pTdIn->u1.AddressOfData));
				else
					pIbn = (PIMAGE_IMPORT_BY_NAME)__RvaToVa__(addrImageBase, pTdIn->u1.AddressOfData);

				// ���ܺ�����
				szProcName = (__char *)(pIbn->Name);
				pEncryptIAT(iProcIndex, PROC_NAME, (__memory)szProcName, __logic_strlen__(szProcName), FALSE, dwFirstThunk, pEncryptFuncArgs);
			}
			// ��һ������
			iProcIndex++;
			pTdIn++;
			pTdOut++;
		}/* end while */
		// ��һ��DLL
		pImportDes++;
	}/* end while */
	return TRUE;
}

__bool __API__ DecryptIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescryptor, __address addrImageBase, FPCryptIAT pDecryptIAT, __void *pEncryptFuncArgs) {
	PIMAGE_IMPORT_DESCRIPTOR pImportDes = pImportDescryptor;
	__integer iLibIndex = 0, iProcIndex = 0;
	__integer iFirstThunk = 0;
	if (!pDecryptIAT) return FALSE;

	// ��Щ����ֻ����FirstThunk
	while ((pImportDes->OriginalFirstThunk) || (pImportDes->FirstThunk)) {
		PIMAGE_THUNK_DATA pTdIn = NULL;
		__char *svDllName = (__char *)__RvaToVa__(addrImageBase, pImportDes->Name);
		pDecryptIAT(iLibIndex, LIB_NAME, (__memory)svDllName, 0, FALSE, iFirstThunk, pEncryptFuncArgs);//���ܿ���
		iLibIndex++;

		// ���������ַ,ֱ��ʹ��FirstThunk.
		pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(addrImageBase, pImportDes->FirstThunk);

		// ���ܺ���
		iProcIndex = 0;
		while (pTdIn->u1.Function) {
			// ��ǰ��FirstThunk
			iFirstThunk = (__integer)pTdIn - (__integer)addrImageBase;

			// ��������������Ժ���������
			if (pTdIn->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
				__word wOrd = pTdIn->u1.Ordinal;
				pDecryptIAT(iProcIndex, PROC_NAME, (__memory)&wOrd, 0, TRUE, iFirstThunk, pEncryptFuncArgs);
			} else {
				// ����������
				PIMAGE_IMPORT_BY_NAME pIbn = (PIMAGE_IMPORT_BY_NAME)__RvaToVa__(addrImageBase, pTdIn->u1.AddressOfData);
				__char *szProcName = (__char *)(pIbn->Name);
				pDecryptIAT(iProcIndex, PROC_NAME, (__memory)szProcName, 0, FALSE, iFirstThunk, pEncryptFuncArgs);//���ܺ�����
			}
			// ��һ������
			iProcIndex++;
			pTdIn++;
		}/* end while */
		// ��һ��DLL
		pImportDes++;
	}/* end while */
	return TRUE;
}

__bool __API__ ShowIAT(PIMAGE_IMPORT_DESCRIPTOR pImportDescryptor, __address addrImageBase, FPShowImportDetail pShowIAT, __void *pShowFuncArgs, __bool bOnFile) {
	return EncryptIAT(pImportDescryptor, addrImageBase, pShowIAT, pShowFuncArgs, bOnFile);
}

__void __INTERNAL_FUNC__ CountLibraryPlus(__integer iIndex, CRYPTIAT_TYPE Type, __memory pData, __integer iDataSize, __bool bOrd, __dword dwFirstThunk, __memory pArgs) {
	__integer *iCount = (__integer *)pArgs;
	if (Type == LIB_NAME) (*iCount)++;
}
__integer __API__ CountLibraryInImportTable(__memory pMem, __bool bOnFile) {
	__integer iCount = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!pImportDescriptor) return 0;
	if (!EncryptIAT(pImportDescriptor, GetNtHeader(pMem)->OptionalHeader.ImageBase, CountLibraryPlus, (__void *)&iCount, bOnFile)) return 0;
	return iCount;
}

__void __INTERNAL_FUNC__ CountImportTableDataSizePlus(__integer iIndex, CRYPTIAT_TYPE Type, __memory pData, __integer iDataSize, __bool bOrd, __dword dwFirstThunk, __memory pArgs) {
	__integer *iSize = (__integer *)pArgs;
	*iSize += Alig(iDataSize, 0x10, TRUE);
}
__integer __API__ CountImportTableDataSize(__memory pMem, __bool bOnFile) {
	__integer iSize = 0;
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!pImportDescriptor) return 0;
	if (!EncryptIAT(pImportDescriptor, GetNtHeader(pMem)->OptionalHeader.ImageBase, CountImportTableDataSizePlus, (__void *)&iSize, bOnFile)) return 0;
	return iSize;
}

PIMAGE_IMPORT_DESCRIPTOR __API__ MakeImportDescriptor(__memory pMem, __integer iLibCount, __integer iHowManyProcInEachLib, __integer *iSize) {
	PIMAGE_IMPORT_DESCRIPTOR pImportDescriptor = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pDummyImportDescriptor = NULL;
	__memory pDummyImportTable = NULL;
	__integer iDummyImportTableSize = 0;

	pImportDescriptor = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!pImportDescriptor) return NULL;

	iDummyImportTableSize = sizeof(IMAGE_IMPORT_DESCRIPTOR);
	iDummyImportTableSize += (iLibCount * 0x20) + (iHowManyProcInEachLib * iLibCount * 0x20);
	iDummyImportTableSize = Alig(iDummyImportTableSize, 0x04, TRUE);
	pDummyImportTable = __logic_new_size__(iDummyImportTableSize);
	if (!pDummyImportTable) return NULL;
	
	__logic_memcpy__(pDummyImportTable, pImportDescriptor, sizeof(IMAGE_IMPORT_DESCRIPTOR));
	// �������������

	pDummyImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)pDummyImportTable;
	return pDummyImportDescriptor;
}

#include "RefixTlsPrivate.c"
__bool __API__ RefixTLS(__memory pImageBase, __bool bDetach) {
	if (ResetTlsTable() == 0) return TRUE;//���������Լ��ؿ�,��ͳ��TLS�ĳ���
	if (InitializeTlsForProccess() == FALSE) return FALSE;//��ʼ��TLS��
	if (InitializeTlsForThread() == FALSE) return FALSE;//��ʼ����һ���߳�
	if (bDetach == TRUE)
		TlsCallback(DLL_PROCESS_DETACH);//����TLS�ص�����
	else
		TlsCallback(DLL_PROCESS_ATTACH);//����TLS�ص�����
	return TRUE;
}

// �ض�λ����ṹ
#pragma pack(push,1)
// �޸����
typedef struct _IMAGE_FIXUP_ENTRY {
	__word offset:12;
	__word type:4;
} IMAGE_FIXUP_ENTRY, *PIMAGE_FIXUP_ENTRY;
// �ض�λ��
typedef struct _IMAGE_FIXUP_BLOCK {
	__dword dwPageRVA;
	__dword dwBlockSize;
} IMAGE_FIXUP_BLOCK, *PIMAGE_FIXUP_BLOCK;
#pragma pack(pop)
__bool __API__ BaseRelocation(__memory pMem, __address addrOldImageBase, __address addrNewImageBase, __bool bIsInFile) {
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	__offset delta = (__offset)(addrNewImageBase - addrOldImageBase);
	__integer *pFixAddRhi = NULL;
	__bool bHaveFixAddRhi = FALSE;
	__integer iRelocSize = 0;

	pNtHdr = GetNtHeader(pMem);
	iRelocSize = pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

	if ((delta) && (iRelocSize)) {
		PIMAGE_FIXUP_BLOCK pStartFB = NULL;
		PIMAGE_FIXUP_BLOCK pIBR = NULL;
		if (bIsInFile) {
			__integer iRelocRaw = Rva2Raw(pMem, pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			pIBR = (PIMAGE_FIXUP_BLOCK)(pMem + iRelocRaw);
		} else {
			pIBR = (PIMAGE_FIXUP_BLOCK)__RvaToVa__(addrNewImageBase, pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		}
		pStartFB = pIBR;

		// ����ÿ���ض�λ��
		while ((__integer)(pIBR - pStartFB) < iRelocSize) {
			PIMAGE_FIXUP_ENTRY pFE;
			__integer i, iCount = 0;
			if (pIBR->dwBlockSize > 0) {
				iCount=(pIBR->dwBlockSize - sizeof(IMAGE_FIXUP_BLOCK)) / sizeof(IMAGE_FIXUP_ENTRY);
				pFE = (PIMAGE_FIXUP_ENTRY)(((__memory)pIBR) + sizeof(IMAGE_FIXUP_BLOCK));
			} else {
				//pIBR = (PIMAGE_FIXUP_BLOCK)(((__memory)pIBR) + sizeof(IMAGE_FIXUP_BLOCK));		
				//continue;
				break;
			}

			// �޸�ÿ�����
			for (i = 0; i < iCount; i++) {
				__memory pFixAddr = NULL;
				if (bIsInFile) {//������ļ���
					__offset ofRva = pIBR->dwPageRVA + pFE->offset;
					__offset ofRaw = Rva2Raw(pMem, ofRva);
					pFixAddr = pMem + ofRaw;
				} else {//������ڴ���
					pFixAddr = __RvaToVa__(addrNewImageBase, pIBR->dwPageRVA + pFE->offset);
				}

				// �����ض�λ
#if defined(_X86_)
				if (pFE->type == IMAGE_REL_BASED_ABSOLUTE) {
				} else if (pFE->type == IMAGE_REL_BASED_HIGH) {
					*((__sword *)pFixAddr) += (__sword)HIWORD(delta);
				} else if (pFE->type == IMAGE_REL_BASED_LOW) {
					*((__sword *)pFixAddr) += (__sword)LOWORD(delta);
				} else if (pFE->type == IMAGE_REL_BASED_HIGHLOW) {
					*((__sdword *)pFixAddr) += (__sdword)delta;
				} else if (pFE->type == IMAGE_REL_BASED_HIGHADJ) {// This one's really fucked up.
					__dword dwAdjust = 0;
					dwAdjust = ((*((__word *)pFixAddr)) << 16) | (*(__word *)(pFE + 1));
					(__sdword)dwAdjust += (__sdword)delta;
					dwAdjust += 0x00008000;
					*((__word *)pFixAddr) = HIWORD(dwAdjust);
					pFE++;
				} else {
					return FALSE;
				}/* end if */
#endif
				// ��һ��Ҫ�޸��Ŀ�
				pFE++;
			}/* end for */

			pIBR = (PIMAGE_FIXUP_BLOCK)((__memory)pIBR + pIBR->dwBlockSize);
		}/* end while */
	}
	return TRUE;
}

__bool __API__ BaseRelocationAssignImageBaseWriteToOtherImageBase(__address addrWriteTo, __address addrOldImageBase, __address addrNewImageBase, __bool bIsInFile) {
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	__memory pMem = NULL;
	__offset delta = 0;
	__word *pFixAddRhi = NULL;
	__bool bHaveFixAddRhi = FALSE;
	__integer iRelocSize = 0;

	pNtHdr = GetNtHeader((__memory)addrWriteTo);
	pMem = (__memory)addrWriteTo;
	delta = addrNewImageBase - addrOldImageBase;
	iRelocSize = pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

	if ((delta) && (iRelocSize)) {
		PIMAGE_FIXUP_BLOCK pStartFB = NULL;
		PIMAGE_FIXUP_BLOCK pIBR = NULL;
		if (bIsInFile) {
			__offset ofRelocRaw = Rva2Raw(pMem, pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
			pIBR = (PIMAGE_FIXUP_BLOCK)(pMem + ofRelocRaw);
		} else {
			pIBR = (PIMAGE_FIXUP_BLOCK)__RvaToVa__(addrWriteTo, pNtHdr->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
		}
		pStartFB = pIBR;

		// ����ÿ���ض�λ��
		while ((__integer)(pIBR - pStartFB) < iRelocSize) {
			PIMAGE_FIXUP_ENTRY pFE;
			__integer i, iCount = 0;
			if (pIBR->dwBlockSize > 0) {
				iCount = (pIBR->dwBlockSize - sizeof(IMAGE_FIXUP_BLOCK)) / sizeof(IMAGE_FIXUP_ENTRY);
				pFE = (PIMAGE_FIXUP_ENTRY)(((__memory)pIBR) + sizeof(IMAGE_FIXUP_BLOCK));
			} else {
				//pIBR = (PIMAGE_FIXUP_BLOCK)(((__memory)pIBR) + sizeof(IMAGE_FIXUP_BLOCK));
				//continue;
				break;
			}

			// �޸�ÿ�����
			for (i = 0; i < iCount; i++) {
				__memory pFixAddr = NULL;
				if (bIsInFile) {//������ļ���
					__offset ofRva = pIBR->dwPageRVA + pFE->offset;
					__offset ofRaw = Rva2Raw(pMem, ofRva);
					pFixAddr = pMem + ofRaw;
				} else {//������ڴ���
					pFixAddr = __RvaToVa__(addrWriteTo, pIBR->dwPageRVA + pFE->offset);
				}

#if defined(_X86_)
				if (pFE->type == IMAGE_REL_BASED_ABSOLUTE) {
				} else if (pFE->type == IMAGE_REL_BASED_HIGH) {
					*((__sword *)pFixAddr) += (__sword)HIWORD(delta);
				} else if (pFE->type == IMAGE_REL_BASED_LOW) {
					*((__sword *)pFixAddr) += (__sword)LOWORD(delta);
				} else if (pFE->type == IMAGE_REL_BASED_HIGHLOW) {
					*((__sdword *)pFixAddr) += (__sdword)delta;
				} else if (pFE->type == IMAGE_REL_BASED_HIGHADJ) {
					__dword dwAdjust;					
					dwAdjust=((*((__word *)pFixAddr)) << 16) | (*(__word *)(pFE + 1));
					(__sdword)dwAdjust += (__sdword)delta;
					dwAdjust += 0x00008000;
					*((__word *)pFixAddr) = HIWORD(dwAdjust);
					pFE++;
				} else {
					return FALSE;
				}
#endif
				pFE++;
			}/* end for */

			pIBR = (PIMAGE_FIXUP_BLOCK)((__memory)pIBR + pIBR->dwBlockSize);
		}/* end while */
	}
	return TRUE;
}

__bool __API__ BaseRelocationOnFile(__memory pMem, __address addrNewImageBase) {
	PIMAGE_NT_HEADERS pNtHdr = GetNtHeader(pMem);
	PIMAGE_DATA_DIRECTORY pRelocDataDir = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_BASERELOC);//����Ƿ�ӵ���ض�λ��
	__address addrOldImageBase = pNtHdr->OptionalHeader.ImageBase;
	if (pRelocDataDir == NULL)
		return FALSE;

	if (BaseRelocation(pMem, addrOldImageBase, addrNewImageBase, TRUE) == FALSE)//�ض�λ��ֱ��д���ļ���
		return FALSE;

	// �滻Ϊ�µĻ���ַ
	pNtHdr->OptionalHeader.ImageBase = addrNewImageBase;

	return TRUE;
}

__bool __API__ BaseRelocationAssignImageBaseWriteToOtherImageBaseOnFile(__memory pMem, __address addrNewImageBase) {
	PIMAGE_NT_HEADERS pNtHdr = GetNtHeader(pMem);
	PIMAGE_DATA_DIRECTORY pRelocDataDir = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	__address addrOldImageBase = pNtHdr->OptionalHeader.ImageBase;
	if (pRelocDataDir == NULL)
		return FALSE;

	// �ض�λ��ֱ��д���ļ���
	if (BaseRelocationAssignImageBaseWriteToOtherImageBase(pMem, addrOldImageBase, addrNewImageBase, TRUE) == FALSE)
		return FALSE;

	// �滻Ϊ�µĻ���ַ
	pNtHdr->OptionalHeader.ImageBase = addrNewImageBase;

	return TRUE;
}

__bool __API__ PrepareImage(__memory pMemoryImage, __integer iImageSize, __bool bRefFixTls, __bool bRefixIAT, FPRefixIAT pRefixIAT, __void *pRefixIATArg) {
	__address addrImageBase = 0;

	// �޸��ض�λ��
	addrImageBase = GetNtHeader(pMemoryImage)->OptionalHeader.ImageBase;
	if (!BaseRelocation(pMemoryImage, addrImageBase, (__address)pMemoryImage, FALSE))
		return FALSE;

	// �޸��������,��ʱ�޴���
	if (!RefixBoundIAT(pMemoryImage))
		return FALSE;

	// �޸������
	if (bRefixIAT) {
		if (pRefixIAT) {
			if (!pRefixIAT(pRefixIATArg))
				return FALSE;
		} else {
			if (!RefixIAT(pMemoryImage))
				return FALSE;
		}
	}/* end if */

	if (bRefFixTls) {
		// �޸���̬TLS��,FALSE = ATTACH
		if (!RefixTLS(pMemoryImage, FALSE))
			return FALSE;
	}

	return TRUE;
}

FARPROC __API__ HookExportProcAddress(HMODULE hModule, __char *lpProcName, FARPROC fpAddr) {
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_EXPORT_DIRECTORY pEd = NULL;
	__integer iDirCount = 0;
	__word wOrdinal = 0;
	__dword *pAddrTable = NULL;
	__dword dwOldAddr = 0;

	if (hModule==NULL) return FALSE;
	pNt = GetNtHeader((LPVOID)hModule);
	iDirCount = pNt->OptionalHeader.NumberOfRvaAndSizes;
	if (iDirCount < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return FALSE;
	if (pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size == 0) return NULL;//ȷ��������

	// ����������
	pEd = (PIMAGE_EXPORT_DIRECTORY)__RvaToVa__(hModule, pNt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress);
	if (HIWORD((__dword)lpProcName) == 0)  {
		wOrdinal = (__word)(LOWORD((__dword)lpProcName)) - pEd->Base;
	} else {
		__integer i, iCount;
		__dword *pdwNamePtr;
		__word *pwOrdinalPtr;

		iCount = (__integer)(pEd->NumberOfNames);
		pdwNamePtr = (__dword *)__RvaToVa__(hModule, pEd->AddressOfNames);
		pwOrdinalPtr =(__word *)__RvaToVa__(hModule, pEd->AddressOfNameOrdinals);

		for(i = 0; i < iCount; i++) {
			__char *svName;
			svName=(__char *)__RvaToVa__(hModule, *pdwNamePtr);
			if (__logic_strcmp__(svName, lpProcName) == 0) {
				wOrdinal = *pwOrdinalPtr;
				break;
			}

			pdwNamePtr++;
			pwOrdinalPtr++;
		}
		if(i == iCount) return FALSE;
	}
	pAddrTable = (__dword *)__RvaToVa__(hModule, pEd->AddressOfFunctions);
	dwOldAddr = (__dword) __RvaToVa__(hModule,( pAddrTable[wOrdinal]));
	pAddrTable[wOrdinal] = (__dword)__VaToRva__(hModule,((__dword)fpAddr));

	return (FARPROC) dwOldAddr;
}

FARPROC __API__ xGetAddressFromOnFile(__memory pMem, __char *pFuncName, __offset *pofFuncRva) {
	__address *pAddrTable = NULL;
	__address addrAddr = 0;
	__offset ofRVA = 0;
	__word wOrdinal = 0;
	PIMAGE_EXPORT_DIRECTORY pExportDir = NULL;
	PIMAGE_DATA_DIRECTORY pExpDataDir = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if (pExpDataDir == NULL)
		return 0;

	pExportDir = (PIMAGE_EXPORT_DIRECTORY)((__memory)pMem + Rva2Raw(pMem, pExpDataDir->VirtualAddress));
	
	// ���������
	if (HIWORD((__dword)pFuncName) == 0)  {
		wOrdinal = (__word)(LOWORD((__dword)pFuncName)) - pExportDir->Base;
	} else {
		__integer i, iCount;
		__dword *pdwNamePtr;
		__word *pwOrdinalPtr;
		
		iCount = (__integer)(pExportDir->NumberOfNames);
		pdwNamePtr = (__dword *)((__memory)pMem + Rva2Raw(pMem, pExportDir->AddressOfNames));
		pwOrdinalPtr = (__word *)((__memory)pMem + Rva2Raw(pMem, pExportDir->AddressOfNameOrdinals));

		for (i = 0; i < iCount; i++) {
			__char *svName;
			svName = (__char *)((__memory)pMem + Rva2Raw(pMem, *pdwNamePtr));
			if (__logic_strcmp__(svName, pFuncName) == 0) {
				wOrdinal = *pwOrdinalPtr;
				break;
			}
			pdwNamePtr++;
			pwOrdinalPtr++;
		}
		if(i == iCount) return NULL;
	}

	pAddrTable=(__dword *)((__memory)pMem + Rva2Raw(pMem, pExportDir->AddressOfFunctions));
	ofRVA = pAddrTable[wOrdinal];
	if (pofFuncRva) *pofFuncRva = ofRVA;
	addrAddr = (__address)((__memory)pMem + Rva2Raw(pMem, ofRVA));
	return (FARPROC)addrAddr;
}

FARPROC __API__ xGetProcAddressImmediately(HMODULE hDll, __char *pFuncName) {
	__word wOrdinal = 0;
	__integer iDirCount = 0;
	__address *pAddrTable = NULL;
	__address addrAddr = 0;
	__offset ofRVA = 0;
	__integer iExpDataSize = 0;
	PIMAGE_EXPORT_DIRECTORY pEd = NULL;
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_DATA_DIRECTORY pExportDataDirectory = NULL;
	if (hDll == NULL) return NULL;
	pNt = GetNtHeader((__memory)hDll);
	iDirCount = pNt->OptionalHeader.NumberOfRvaAndSizes;
	if (iDirCount < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return FALSE;
	pExportDataDirectory = ExistDataDirectory((__memory)hDll, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if(!pExportDataDirectory) 
		return NULL;//ȷ��������
	iExpDataSize = pExportDataDirectory->Size;
	// ���������ȡ������ַ
	pEd = (PIMAGE_EXPORT_DIRECTORY)__RvaToVa__(hDll, pExportDataDirectory->VirtualAddress);	
	/* 
	 * ��ȡ���������ĺ���
	 */
	if (HIWORD((__dword)pFuncName)==0) {//���������
		wOrdinal = (__word)(LOWORD((__dword)pFuncName)) - pEd->Base;
	} else {
		__integer i, iCount;
		__dword *pdwNamePtr;
		__word *pwOrdinalPtr;

		iCount = (__integer)(pEd->NumberOfNames);
		pdwNamePtr = (__dword *)__RvaToVa__(hDll, pEd->AddressOfNames);
		pwOrdinalPtr = (__word *)__RvaToVa__(hDll, pEd->AddressOfNameOrdinals);

		for(i = 0; i < iCount; i++) {
			__char *svName = NULL;
			svName = (__char *)__RvaToVa__(hDll, *pdwNamePtr);
			if (__logic_strcmp__(svName, pFuncName) == 0) {
				wOrdinal = *pwOrdinalPtr;
				break;
			}
			pdwNamePtr++;
			pwOrdinalPtr++;
		}
		if (i == iCount) return NULL;
	}

	pAddrTable=(__address *)__RvaToVa__(hDll, pEd->AddressOfFunctions);
	ofRVA = pAddrTable[wOrdinal];
	addrAddr = (__address)__RvaToVa__(hDll, ofRVA);
	/*
	 * �����ж��Ƿ����м���ת
	 */
	if (((__address)addrAddr >= (__address)pEd) &&
		((__address)addrAddr < (__address)pEd + (__address)iExpDataSize))
		return NULL;
	return (FARPROC)addrAddr;
}

FARPROC __API__ xGetProcAddressImmediatelyByHash(HMODULE hDll, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc) {
	__word wOrdinal = 0;
	__integer iDirCount = 0;
	__address *pAddrTable = NULL;
	__address addrAddr = 0;
	__offset ofRVA = 0;
	__integer iExpDataSize = 0;
	PIMAGE_EXPORT_DIRECTORY pEd = NULL;
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_DATA_DIRECTORY pExportDataDirectory = NULL;
	if (hDll == NULL) return NULL;
	pNt = GetNtHeader((__memory)hDll);
	iDirCount = pNt->OptionalHeader.NumberOfRvaAndSizes;
	if (iDirCount < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return FALSE;
	pExportDataDirectory = ExistDataDirectory((__memory)hDll, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if(!pExportDataDirectory) 
		return NULL;//ȷ��������
	iExpDataSize = pExportDataDirectory->Size;
	// ���������ȡ������ַ
	pEd = (PIMAGE_EXPORT_DIRECTORY)__RvaToVa__(hDll, pExportDataDirectory->VirtualAddress);	
	/* 
	 * ��ȡ���������ĺ���
	 */
	if (HIWORD((__dword)pHashPoint)==0) {//���������
		wOrdinal = (__word)(LOWORD((__dword)pHashPoint)) - pEd->Base;
	} else {
		__integer i, iCount;
		__dword *pdwNamePtr;
		__word *pwOrdinalPtr;

		iCount = (__integer)(pEd->NumberOfNames);
		pdwNamePtr = (__dword *)__RvaToVa__(hDll, pEd->AddressOfNames);
		pwOrdinalPtr = (__word *)__RvaToVa__(hDll, pEd->AddressOfNameOrdinals);

		for(i = 0; i < iCount; i++) {
			__byte HashValue[1024];
			__char *svName = NULL;
			__integer iHashValueSize = 0;
			svName = (__char *)__RvaToVa__(hDll, *pdwNamePtr);
			iHashValueSize = pHashFunc(svName, __logic_strlen__(svName), HashValue);//���й�ϣ����
			if (iHashValueSize == iHashSize) {
				if (__logic_memcmp__(HashValue, pHashPoint, iHashSize) == 0) {
					wOrdinal = *pwOrdinalPtr;
					break;
				}
			}
			pdwNamePtr++;
			pwOrdinalPtr++;
		}
		if (i == iCount) return NULL;
	}

	pAddrTable=(__address *)__RvaToVa__(hDll, pEd->AddressOfFunctions);
	ofRVA = pAddrTable[wOrdinal];
	addrAddr = (__address)__RvaToVa__(hDll, ofRVA);
	/*
	* �����ж��Ƿ����м���ת
	*/
	if (((__address)addrAddr >= (__address)pEd) &&
		((__address)addrAddr < (__address)pEd + (__address)iExpDataSize))
		return NULL;
	return (FARPROC)addrAddr;
}

// ˽�к�����������һ��,���º�����Ҫ��
FARPROC __INTERNAL_FUNC__ xLdrGetExportByName(__memory pBaseAddress, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA);
FARPROC __INTERNAL_FUNC__ xLdrGetExportByOrdinal(__memory pBaseAddress, __word wOrdinal, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA);
__INLINE__ FARPROC __INTERNAL_FUNC__ xLdrFixupForward(__memory pBaseAddress, __memory pForwardName, __word wOrdinal, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA) {
	__char NameBuffer[128] = {0};
	__wchar NameOfLib[128] = {0};
	__memory pPoint = NULL;
	__memory hModule = NULL;
	__byte HashValue[1024] = {0};
	__integer iHashValueSize = 0;
	FARPROC pFunction = NULL;

	// ��ȡ���ض�̬��ĵ�ַ
	FPLoadLibraryA pLoadLibraryA = (FPLoadLibraryA)fpLoadLibraryA;

	// ȡ��DLL�������ĺ�����
	__logic_strcpy__(NameBuffer, pForwardName);
	pPoint = __logic_strchr__(NameBuffer, '.');
	if (pPoint) {
		__char *pProcName = NULL;
		__integer iProcNameSize = 0;

		*pPoint = 0;//ʹ��0�ַ���DLL�뺯��������,���'.'

		// ����Ƿ���ApiSetMap�ĺ���
		__logic_str2lower__(NameBuffer);//ת��ΪСд
		if (__logic_strncmp__(NameBuffer, "api-", 4) == 0) {
			// Ѱ��API SET MAP
			PAPI_SET_MAP_HEADER pApiSetMapHeader = NULL;
			PAPI_SET_MAP_ENTRY pApiSetMapEntry = NULL;
			__dword dwNumberOfApiSetMapEntry = 0;
			__dword dwNumberOfHostEntry = 0;
			__wchar *pApiSetMapEntryName = NULL;
			__integer iSizeOfApiSetMapEntryName = 0;
			__wchar *pHostMapName = NULL;
			__integer iSizeOfHostMapName = 0;
			__wchar *pImportMapName = NULL;
			__integer iSizeOfImportMapName = 0;
			PAPI_SET_MAP_HOST_HEADER pApiSetMapHostHeader = NULL;
			PAPI_SET_MAP_HOST_ENTRY pApiSetMapHostEntry = NULL;
			__integer i = 0, j = 0;
			__char *pNameBuffer = NULL;

			// ��ȡҪƥ��Ŀ���
			pNameBuffer = (__char *)(NameBuffer + 4);

			pApiSetMapHeader = GetApiSetMapHeader();
			if (!pApiSetMapHeader) return NULL;

			pApiSetMapEntry = (PAPI_SET_MAP_ENTRY)((__memory)pApiSetMapHeader + sizeof(API_SET_MAP_HEADER));
			dwNumberOfApiSetMapEntry = pApiSetMapHeader->dwNumberOfApiSetModules;
			for (i = 0; i < dwNumberOfApiSetMapEntry; i++, pApiSetMapEntry++) {
				__char AnsiNameOfLib[128] = {0};

				pApiSetMapEntryName = (__wchar *)((__memory)pApiSetMapHeader + pApiSetMapEntry->dwNameOfApiSetModuleRVA);
				iSizeOfApiSetMapEntryName = pApiSetMapEntry->dwSizeOfName;

				// ת����Сд
				__logic_memset__(NameOfLib, 0, sizeof(__wchar) * 128);
				__logic_memset__(AnsiNameOfLib, 0, 128);
				__logic_tcsncpy__(NameOfLib, pApiSetMapEntryName, iSizeOfApiSetMapEntryName);
				__logic_tcs2lower_n_(NameOfLib, iSizeOfApiSetMapEntryName / sizeof(__tchar));
				__logic_tcs2str_n__(NameOfLib, AnsiNameOfLib, iSizeOfApiSetMapEntryName / sizeof(__tchar));

				// �Ա��Ƿ���Ҫ���ҵ�Lib
				if (__logic_strcmp__(AnsiNameOfLib, pNameBuffer) == 0) {
					__integer iCount = 0;
					PLDR_MODULE pLdrImportModule = NULL;
					__char szImportModuleName[128] = {0};
					PUNICODE_STRING pCurrImportName = NULL;

					pApiSetMapHostHeader = (PAPI_SET_MAP_HOST_HEADER)((__memory)pApiSetMapHeader + pApiSetMapEntry->dwHostModulesRVA);
					dwNumberOfHostEntry = pApiSetMapHostHeader->dwNumberOfHosts;
					pApiSetMapHostEntry = (PAPI_SET_MAP_HOST_ENTRY)((__memory)pApiSetMapHostHeader + sizeof(API_SET_MAP_HOST_HEADER));

					// �ҵ���ǰģ�������ģ��
					pLdrImportModule = GetExistModuleInLoadModuleList(pBaseAddress);
					pCurrImportName = &(pLdrImportModule->BaseDllName);
					
					// ���dwNumberOfHostEntry == 1
					if (dwNumberOfHostEntry == 1) {
						// Host
						pHostMapName = (__wchar *)((__memory)pApiSetMapHeader + pApiSetMapHostEntry->dwNameOfHostRVA);
						iSizeOfHostMapName = pApiSetMapHostEntry->dwSizeOfHostName;
						iCount = iSizeOfHostMapName / sizeof(__tchar);

						// ת��
						__logic_tcs2str_n__(pHostMapName, NameBuffer, iCount);
						NameBuffer[iCount] = '\0';

						// ��ת��
						goto _load_library_get_address;
					} else {
						// ����Hostӳ��
						for (j = 0; j < dwNumberOfHostEntry; j++, pApiSetMapHostEntry++) {
							__wchar ImportModule[128] = {0};
							__wchar ImportModule2[128] = {0};

							// Import
							pImportMapName = (__wchar *)((__memory)pApiSetMapHeader + pApiSetMapHostEntry->dwNameOfImportModuleRVA);
							iSizeOfImportMapName = pApiSetMapHostEntry->dwSizeOfImportModuleName;
							iCount = iSizeOfImportMapName / sizeof(__tchar);

							// �������ģ��������Ϊ0
							if (iSizeOfImportMapName == 0)
								continue;

							// Host
							pHostMapName = (__wchar *)((__memory)pApiSetMapHeader + pApiSetMapHostEntry->dwNameOfHostRVA);
							iSizeOfHostMapName = pApiSetMapHostEntry->dwSizeOfHostName;

							__logic_tcsncpy__(ImportModule, pImportMapName, iCount);
							__logic_tcs2lower_n_(ImportModule, iCount);
							ImportModule[iCount] = _T('\0');

							__logic_tcsncpy__(ImportModule2, pCurrImportName->Buffer, iCount);
							__logic_tcs2lower_n_(ImportModule2, iCount);
							ImportModule2[iCount] = _T('\0');

							// �ȶ�Importģ�������, ���������ȡ
							if (__logic_tcsncmp__(pImportMapName, ImportModule2, iCount) == 0) {
								iCount = iSizeOfHostMapName / sizeof(__tchar);
								__logic_tcs2str_n__(pHostMapName, NameBuffer, iCount);
								NameBuffer[iCount] = '\0';
								goto _load_library_get_address;
							}/* end if */
						}/* end for */
					}/* end else */
				}/* end if */
			}/* end for */
		} else {
_load_library_get_address:
			// ���ض�̬��
			hModule = (__memory)pLoadLibraryA(NameBuffer);
			if (!hModule) return NULL;

			// ������ϣֵ
			pProcName = (__char *)(pPoint + 1);
			iProcNameSize = __logic_strlen__(pPoint + 1);
			iHashValueSize = pHashFunc((__memory)pProcName, iProcNameSize, HashValue);
			return xLdrGetExportByName((__memory)hModule, (__memory)HashValue, iHashValueSize, pHashFunc, fpLoadLibraryA);
		}/* end else */
	}/* end if */

	return NULL;
}

FARPROC __INTERNAL_FUNC__ xLdrGetExportByOrdinal(__memory pBaseAddress, __word wOrdinal, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA) {
	PIMAGE_EXPORT_DIRECTORY pExportDir;
	__integer iExportDirSize;
	__dword **pExFunctionsPoint;
	FARPROC pFunction;
	PIMAGE_DATA_DIRECTORY pExportDataDirectory = ExistDataDirectory(pBaseAddress, IMAGE_DIRECTORY_ENTRY_EXPORT);
	pExportDir = (PIMAGE_EXPORT_DIRECTORY)(pBaseAddress + pExportDataDirectory->VirtualAddress);
	if (!pExportDir) 
		return NULL;
	iExportDirSize = pExportDataDirectory->Size;

	pExFunctionsPoint = (__dword **)__RvaToVa__(pBaseAddress, pExportDir->AddressOfFunctions);
	pFunction = (FARPROC)(0 != pExFunctionsPoint[wOrdinal - pExportDir->Base]
				? __RvaToVa__(pBaseAddress, pExFunctionsPoint[wOrdinal - pExportDir->Base])
				: NULL);

	if (((__address)pFunction >= (__address)pExportDir) &&
		((__address)pFunction < (__address)pExportDir + (__address)iExportDirSize))
		pFunction = xLdrFixupForward(pBaseAddress, (__memory)pFunction, wOrdinal, pHashFunc, fpLoadLibraryA);

	return pFunction;
}

FARPROC __INTERNAL_FUNC__ xLdrGetExportByName(__memory pBaseAddress, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA) {
	__word wOrdinal = 0;
	__integer iDirCount = 0;
	__address *pAddrTable = NULL;
	__address addrAddr = 0;
	__offset ofRVA = 0;
	__integer iExpDataSize = 0;
	__integer i = 0;
	PIMAGE_EXPORT_DIRECTORY pEd = NULL;
	PIMAGE_NT_HEADERS pNt = NULL;
	PIMAGE_DATA_DIRECTORY pExportDataDirectory = NULL;
	if (pBaseAddress == NULL) return NULL;
	pNt = GetNtHeader(pBaseAddress);
	iDirCount = pNt->OptionalHeader.NumberOfRvaAndSizes;
	if (iDirCount < IMAGE_NUMBEROF_DIRECTORY_ENTRIES) return FALSE;
	pExportDataDirectory = ExistDataDirectory(pBaseAddress, IMAGE_DIRECTORY_ENTRY_EXPORT);
	if(!pExportDataDirectory) 
		return NULL;//ȷ��������
	iExpDataSize = pExportDataDirectory->Size;
	// ���������ȡ������ַ
	pEd = (PIMAGE_EXPORT_DIRECTORY)__RvaToVa__(pBaseAddress, pExportDataDirectory->VirtualAddress);	
	/* 
	* ��ȡ���������ĺ���
	*/
	if (HIWORD((__dword)pHashPoint)==0) {//���������
		wOrdinal = (__word)(LOWORD((__dword)pHashPoint)) - pEd->Base;
	} else {
		__integer iCount = 0;
		__dword *pdwNamePtr = NULL;
		__word *pwOrdinalPtr = NULL;

		iCount = (__integer)(pEd->NumberOfNames);
		pdwNamePtr = (__dword *)__RvaToVa__(pBaseAddress, pEd->AddressOfNames);
		pwOrdinalPtr = (__word *)__RvaToVa__(pBaseAddress, pEd->AddressOfNameOrdinals);

		for(i = 0; i < iCount; i++) {
			__byte HashValue[1024];
			__char *svName = NULL;
			__integer iHashValueSize = 0;

			svName = (__char *)__RvaToVa__(pBaseAddress, *pdwNamePtr);
			iHashValueSize = pHashFunc(svName, __logic_strlen__(svName), HashValue);//���й�ϣ����
			if (iHashValueSize == iHashSize) {
				if (__logic_memcmp__(HashValue, pHashPoint, iHashSize) == 0) {
					wOrdinal = *pwOrdinalPtr;
					break;
				}
			}
			pdwNamePtr++;
			pwOrdinalPtr++;
		}
		if (i == iCount) return NULL;
	}

	pAddrTable=(__address *)__RvaToVa__(pBaseAddress, pEd->AddressOfFunctions);
	ofRVA = pAddrTable[wOrdinal];
	addrAddr = (__address)__RvaToVa__(pBaseAddress, ofRVA);
	/*
	 * �����ж��Ƿ����м���ת
	 */
	if (((__address)addrAddr >= (__address)pEd) &&
		((__address)addrAddr < (__address)pEd + (__address)iExpDataSize))
		return xLdrFixupForward(pBaseAddress, (__memory)addrAddr, wOrdinal, pHashFunc, fpLoadLibraryA);
	return (FARPROC)addrAddr;
}

__INLINE__ FARPROC __INTERNAL_FUNC__ xLdrGetProcedureAddress(__memory pBaseAddress, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA) {
	FARPROC pProcedureAddress = NULL;
	__dword dwOrdinal = (__dword)pHashPoint;
	if (HIWORD((__dword)pHashPoint)) {
		// ͨ����������
		pProcedureAddress = xLdrGetExportByName(pBaseAddress, pHashPoint, iHashSize, pHashFunc, fpLoadLibraryA);
	} else {
		// ͨ������
		dwOrdinal &= 0x0000FFFF;
		pProcedureAddress = xLdrGetExportByOrdinal(pBaseAddress, (__word)dwOrdinal, pHashFunc, fpLoadLibraryA);
	}
	return pProcedureAddress;
}

FARPROC __API__ xGetProcAddressEx(HMODULE hDll, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc, FARPROC fpLoadLibraryA) {
	FARPROC fnExp = NULL;
	fnExp = xLdrGetProcedureAddress((__memory)hDll, pHashPoint, iHashSize, pHashFunc, fpLoadLibraryA);
	return fnExp;
}

__integer __INTERNAL_FUNC__ xGetProcAddressHashFunc(__memory pTarget, __integer iTargetSize, __memory pHashValue) {
	__logic_memcpy__(pHashValue, pTarget, iTargetSize);
	return iTargetSize;
}
FARPROC __API__ xGetProcAddress(HMODULE hDll, __char *pFuncName) {
	FARPROC pLoadLibraryA = GetLoadLibraryAAddress();
	return xGetProcAddressEx(hDll, pFuncName, __logic_strlen__(pFuncName), xGetProcAddressHashFunc, pLoadLibraryA);
}

FARPROC __API__ xGetProcAddressByHash(HMODULE hDll, __memory pHashPoint, __integer iHashSize, FPHashFunc pHashFunc) {
	FARPROC pLoadLibraryA = GetLoadLibraryAAddress();
	return xGetProcAddressEx(hDll, pHashPoint, iHashSize, pHashFunc, pLoadLibraryA);
}

FPEntryFunction __API__ PeLoader(__memory pLoadCode, __memory pOutMemory, __integer iOutMemorySize, __bool bRefixIAT, FPRefixIAT pRefixIAT, __void *pRefixIATArg) {
	FPEntryFunction pEntryFunction = NULL;
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	__dword dwOldProtected = 0;
	__dword dwSizeOfImage = GetNtHeader(pLoadCode)->OptionalHeader.SizeOfImage;
	if (dwSizeOfImage > iOutMemorySize)//�ߴ粻����
		return NULL;

	VirtualProtect(pOutMemory, dwSizeOfImage, PAGE_WRITECOPY, &dwOldProtected);//�޸��ڴ�Ȩ��
	if (CopyMemToMemBySecAlign(pLoadCode, pOutMemory, dwSizeOfImage) == FALSE) return FALSE;//���Ƶ��ڴ�
	VirtualProtect(pOutMemory, dwSizeOfImage, dwOldProtected, NULL);//�޸Ļ�Ȩ��

	// ���½��������ڱ�
	if (PrepareImage(pOutMemory, dwSizeOfImage, TRUE, bRefixIAT, pRefixIAT, pRefixIATArg) == FALSE) return FALSE;
	ProtectImage(pOutMemory, FALSE);//�����趨�ڱ���
	pNtHdr = GetNtHeader(pOutMemory);
	pEntryFunction = (FPEntryFunction)(pOutMemory + pNtHdr->OptionalHeader.AddressOfEntryPoint);
	return pEntryFunction;
}

#include "DllLoaderPrivate.c"
__void __API__ InitDllManager() {
	InitializeCriticalSection(&g_DLLCrit);
}

__void __API__ DestoryDllManager() {
	PDLL_LIST pCur, pNext;
	pCur = g_pDLLHead;
	while (pCur) {
		pNext = pCur->pNext;
		__logic_delete__(pCur);
		pCur = pNext;
	}
	DeleteCriticalSection(&g_DLLCrit);
}

HMODULE __API__ xLoadLibraryFromMemoryEx(__memory pDllCode, __memory pOutMemory, __dword dwFlags) {
	PIMAGE_NT_HEADERS pNt = GetNtHeader(pDllCode);
	__memory pPreferredImageBase = (__memory)(pNt->OptionalHeader.ImageBase);
	__dword dwImageSize = pNt->OptionalHeader.SizeOfImage;
	__memory pImageBase = pOutMemory;
	__dword dwOldProtected = 0;

	VirtualProtect(pImageBase, dwImageSize, PAGE_WRITECOPY, &dwOldProtected);//�޸��ڴ�Ȩ��
	if (CopyMemToMemBySecAlign(pDllCode, pImageBase, dwImageSize) == FALSE)//�����ڴ�
		return NULL;
	VirtualProtect(pImageBase, dwImageSize, dwOldProtected, NULL);//�޸Ļ�Ȩ��

	// �ؽ������,�ض�λ��
	if (!(dwFlags & LOAD_LIBRARY_AS_DATAFILE)) {
		__bool bRet = FALSE;
		// �������������
		if (!PrepareImage(pImageBase, dwImageSize, FALSE, TRUE, NULL, NULL))
			return NULL;

		// �����ڱ���
		if (!ProtectImage(pImageBase, FALSE))
			return NULL;
		
		// ����DLLMAIN,FALSE = DLL����
		bRet = RunDLLMain(pImageBase, dwImageSize, FALSE);
		if (bRet == FALSE)
			return NULL;
	}/* end if */

	return (HMODULE)pImageBase;
}

HMODULE __API__ xLoadLibraryFromMemory(__memory pDllCode, __tchar *svMappingName, __dword dwFlags) {
	PIMAGE_NT_HEADERS pNt = GetNtHeader(pDllCode);
	__memory pPreferredImageBase = (__memory)(pNt->OptionalHeader.ImageBase);//��ȡImageBase
	__word wSectionNumber = pNt->FileHeader.NumberOfSections;//��ȡ����Ŀ
	__dword dwImageSize = pNt->OptionalHeader.SizeOfImage;
	__memory pImageBase = xGetModuleHandle(svMappingName);
	__bool IsWinNT = FALSE;
	__dword dwVersion = GetVersion();//�жϲ���ϵͳ
	HANDLE hMapping = NULL;
	__integer iDllRef = 0;

	if (pNt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return NULL;//�ж�Magic
	if (dwVersion < 0x80000000) IsWinNT = TRUE;
	else IsWinNT = FALSE;

	// û���ҵ���DLL��ҽ�
	if (pImageBase == NULL) {
		__bool bCreated = FALSE;
		__bool bRebased = FALSE;
		if (IsWinNT == TRUE) {
			// �Ƿ������������д�
			if (svMappingName != NULL) {
				hMapping = OpenFileMapping(FILE_MAP_WRITE, TRUE, svMappingName);
				bCreated = FALSE;
			}

			if (hMapping == NULL) {
				hMapping = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, dwImageSize + __SIZE_OF_PARAMETER_BLOCK__, svMappingName);
				if(hMapping == NULL) return NULL;
				bCreated = TRUE;
			}

			pImageBase = MapViewOfFileEx(hMapping, FILE_MAP_WRITE, 0, 0, 0, pPreferredImageBase);
			if (pImageBase == NULL) {
				pImageBase = MapViewOfFileEx(hMapping, FILE_MAP_WRITE, 0, 0, 0, NULL);
			}
			CloseHandle(hMapping);					
			if(pImageBase == NULL) return NULL;
		} else {
			// 9X��
			pImageBase = VirtualAlloc(pPreferredImageBase, dwImageSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
			if(pImageBase == NULL) {
				pImageBase = VirtualAlloc(NULL, dwImageSize, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
				if(pImageBase == NULL) return NULL;
			}
			bCreated = TRUE;
		}

		if (bCreated) {
			if (xLoadLibraryFromMemoryEx(pDllCode, (__memory)pImageBase, dwFlags) == NULL) {
				if (IsWinNT) UnmapViewOfFile(pImageBase);
				else VirtualFree(pImageBase, 0, MEM_RELEASE);
				return NULL;
			}
		}
		else
			return NULL;
	}

	// ��������
	iDllRef = AddDLLReference(pImageBase, svMappingName, dwFlags);
	if (iDllRef == -1) {
		if (pImageBase) {
			if (IsWinNT) UnmapViewOfFile(pImageBase);
			else VirtualFree(pImageBase, 0, MEM_RELEASE);
		}
		return NULL;
	}
	return (HMODULE)pImageBase;
}

HMODULE __API__ xLoadLibraryEx(__tchar *pLibName, __dword dwFlags) {
	__tchar svPath[MAX_PATH+1];
	__tchar *svFilePart = NULL;
	__integer iPathLen = 0;
	__memory pBaseAddr = NULL;
	HANDLE hLoadFile, hMap;
	HMODULE hRet;

	// ��ȡDLL
	if (dwFlags & LOAD_WITH_ALTERED_SEARCH_PATH)
		return NULL;
	else {
		iPathLen = SearchPath(NULL, pLibName, _T(".dll"), MAX_PATH, svPath, &svFilePart);
		if(iPathLen == 0) return NULL;
	}

	// ӳ���ļ�
	hLoadFile = CreateFile(svPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hLoadFile == INVALID_HANDLE_VALUE) return NULL;
	hMap = CreateFileMapping(hLoadFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hLoadFile);
	pBaseAddr = MapViewOfFile(hMap, FILE_MAP_READ, 0, 0, 0);
	if (pBaseAddr == NULL) {
		CloseHandle(hMap);
		return NULL;
	}
	hRet = xLoadLibraryFromMemory((__memory)pBaseAddr, svFilePart, dwFlags);//����
	// �ر�
	UnmapViewOfFile(pBaseAddr);
	CloseHandle(hMap);
	return (HMODULE)hRet;
}

HMODULE __API__ xLoadLibrary(__tchar *pLibName) {
	return xLoadLibraryEx(pLibName, DLL_PROCESS_ATTACH);
}

HMODULE __API__ xGetModuleHandle(__tchar *pLibName) {
	PDLL_LIST pCurr = NULL;
	if (pLibName == NULL) return NULL;
	EnterCriticalSection(&g_DLLCrit);

	// ����DLL
	for (pCurr = g_pDLLHead; pCurr != NULL; pCurr = pCurr->pNext) {
		if(__logic_tcscmp__(pCurr->svName, pLibName) == 0) break;//�йش�Сд��ƥ��
	}

	if(pCurr) {
		LeaveCriticalSection(&g_DLLCrit);
		return (HMODULE)(pCurr->pImageBase);
	} 

	LeaveCriticalSection(&g_DLLCrit);
	return NULL;
}

__bool __API__ xFreeLibrary(HMODULE hLibModule) {
	__dword dwFlags = 0;
	__integer iDllLock = 0;
	__tchar svName[MAX_PATH];
	__bool bRet = TRUE;
	PIMAGE_NT_HEADERS pNt = GetNtHeader((LPVOID)hLibModule);//�ж�image����
	__dword dwImageSize = pNt->OptionalHeader.SizeOfImage;//��ȡӳ�䳤��
	if (pNt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR32_MAGIC) return FALSE;

	iDllLock = RemoveDLLReference(hLibModule, svName, &dwFlags);//��������
	if (iDllLock == -1) return NULL;

	if (!(dwFlags & (LOAD_LIBRARY_AS_DATAFILE | DONT_RESOLVE_DLL_REFERENCES))) {
		if (iDllLock != 0) {
			__dword dwVersion = GetVersion();//��ȡ����ϵͳ�汾
			RunDLLMain(hLibModule, dwImageSize, DLL_PROCESS_DETACH);
			if(dwVersion < 0x80000000) bRet = UnmapViewOfFile(hLibModule);
			else bRet = VirtualFree(hLibModule, 0, MEM_RELEASE);//Win95|98
		}
	}
	return bRet;
}

#include "IconEx.h"
__void __API__ ChangedExeIcon(__tchar *lpExeName, __tchar *lpIconFile) {
	LPICONDIRENTRY pIconDirEntry = NULL;
	LPGRPICONDIR pGrpIconDir = NULL;
	HEADER Header;  
	HANDLE hIconFile = NULL, hUpdate = NULL; 
	__memory pIconBytes = NULL;
	__integer iRet = 0, iSize = 0, iGSize = 0;
	__dword dwReserved = 0;
	__bool bRet = FALSE;
	__dword dwRet = 0;
	__word i = 0;
  
	// ��ͼ���ļ�  
	hIconFile = CreateFile(lpIconFile, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);  
	if (hIconFile == INVALID_HANDLE_VALUE)
		return;

	// ��ȡ�ļ�ͷ����Ϣ  
	bRet = ReadFile(hIconFile, &Header, sizeof(HEADER), &dwReserved, NULL);
	if (!bRet) { 
		CloseHandle(hIconFile);  
		return;  
	}
	
	// ����ÿһ��ͼ���Ŀ¼��Ϣ�������  
	pIconDirEntry = (LPICONDIRENTRY)__logic_new__(ICONDIRENTRY, Header.idCount);
	if (!pIconDirEntry) { 
		CloseHandle(hIconFile);  
		return;  
	}

	// ��Icon�ļ��ж�ȡÿһ��ͼ���Ŀ¼��Ϣ  
	bRet = ReadFile(hIconFile, pIconDirEntry, Header.idCount * sizeof(ICONDIRENTRY), &dwReserved, NULL);  
	if (!bRet) {
		__logic_delete__((__void *)pIconDirEntry);
		CloseHandle(hIconFile); 
		return;
	}  
	iGSize = sizeof(GRPICONDIR) + Header.idCount * sizeof(ICONDIRENTRY);//����EXE�ļ���RT_GROUP_ICON��������ݽṹ�������
	pGrpIconDir = (LPGRPICONDIR)__logic_new_size__(iGSize);  
	pGrpIconDir->idReserved = Header.idReserved;//�����Ϣ,�����൱��һ��ת���Ĺ���
	pGrpIconDir->idType = Header.idType;  
	pGrpIconDir->idCount = Header.idCount;  
	// ������Ϣ������ÿһ��ͼ���Ӧ��ID��IDΪλ��������  
	for (i = 0; i < Header.idCount; i++) {
		pGrpIconDir->idEntries[i].bWidth = pIconDirEntry[i].bWidth;  
		pGrpIconDir->idEntries[i].bHeight = pIconDirEntry[i].bHeight;  
		pGrpIconDir->idEntries[i].bColorCount = pIconDirEntry[i].bColorCount;  
		pGrpIconDir->idEntries[i].bReserved = pIconDirEntry[i].bReserved;  
		pGrpIconDir->idEntries[i].wPlanes = pIconDirEntry[i].wPlanes;  
		pGrpIconDir->idEntries[i].wBitCount = pIconDirEntry[i].wBitCount;  
		pGrpIconDir->idEntries[i].dwBytesInRes = pIconDirEntry[i].dwBytesInRes;  
		pGrpIconDir->idEntries[i].nID = i;
	}  
	// ��ʼ����EXE�е�ͼ����Դ��ID��Ϊ��С�������ԭ�����ڣ�ID��ͼ����Ϣ���滻Ϊ�µġ�  
	hUpdate = BeginUpdateResource(lpExeName, FALSE);
	if (hUpdate) {
		// ���ȸ���RT_GROUP_ICON��Ϣ  
		bRet = UpdateResource(hUpdate, RT_GROUP_ICON, MAKEINTRESOURCE(0), MAKELANGID(LANG_CHINESE, SUBLANG_SYS_DEFAULT), (__void *)pGrpIconDir, iGSize);
		if (!bRet) {  
			__logic_delete__((__void *)pIconDirEntry);
			__logic_delete__((__void *)pGrpIconDir);
			CloseHandle(hIconFile);  
			return;  
		}  
		// ���ŵ���ÿһ��Icon����Ϣ��� 
		for (i = 0; i < Header.idCount; i++) {
			iSize = pIconDirEntry[i].dwBytesInRes;//Icon���ֽ���
			dwRet = SetFilePointer(hIconFile, pIconDirEntry[i].dwImageOffset, NULL, FILE_BEGIN);//ƫ���ļ���ָ�뵽��ǰͼ��Ŀ�ʼ��  
			if (dwRet == INVALID_SET_FILE_POINTER)
				break;
			//׼��pIconBytes������ļ����Byte��Ϣ���ڸ��µ�EXE�С�  
			__logic_delete__((__void *)pIconBytes);
			pIconBytes = (__memory)__logic_new_size__(iSize);
			bRet = ReadFile(hIconFile, (__void *)pIconBytes, iSize, &dwReserved, NULL);  
			if (!bRet) break;
			//����ÿһ��ID��Ӧ��RT_ICON��Ϣ  
			bRet = UpdateResource(hUpdate, RT_ICON, MAKEINTRESOURCE(pGrpIconDir->idEntries[i].nID), MAKELANGID(LANG_CHINESE, SUBLANG_SYS_DEFAULT), (__void *)pIconBytes, iSize);  
			if (!bRet) break;
		}  
		// ����EXE��Դ�ĸ��²���  
		if (pIconBytes)
			__logic_delete__((__void *)pIconBytes);
		EndUpdateResource(hUpdate, FALSE);  
	}  
	// ������Դ���ر�Icon�ļ������˸��²���������  
	__logic_delete__((__void *)pGrpIconDir);
	__logic_delete__((__void *)pIconDirEntry);
	CloseHandle(hIconFile); 	
}

#define __GetLdrModuleHeaderByIIOM__(IIOM) (PLDR_MODULE)((__memory)IIOM - 0x10)
HMODULE __API__ GetNtdllBaseAddress() {
	PPEB pPeb = __NtCurrentPeb__();
	PPEB_LDR_DATA pLdrData = pPeb->LdrData;
	PLIST_ENTRY pListEntry = pLdrData->InInitializationOrderModuleList.Flink;
	PLDR_MODULE pLdrModule = __GetLdrModuleHeaderByIIOM__(pListEntry);
	HMODULE hNtDllBase = (HMODULE)(pLdrModule->BaseAddress);
	return hNtDllBase;
}

HMODULE __API__ GetKernel32BaseAddress() {
	PPEB pPeb = __NtCurrentPeb__();
	PPEB_LDR_DATA pLdrData = pPeb->LdrData;
	PLIST_ENTRY pListEntry = pLdrData->InInitializationOrderModuleList.Flink;
	PLDR_MODULE pLdrModule = __GetLdrModuleHeaderByIIOM__(pListEntry);
	HMODULE hNtDllBase = (HMODULE)(pLdrModule->BaseAddress);
	HMODULE hKerenlBase;
	HMODULE hKernel32;
	WINDOWS_VERSION Version;
	pListEntry = pListEntry->Flink;
	pLdrModule = __GetLdrModuleHeaderByIIOM__(pListEntry);
	hKerenlBase = (HMODULE)pLdrModule->BaseAddress;
	Version = GetWindowsVersion();//�����жϲ���ϵͳ�汾�����WIN VISTA 7�����,��VISTA���°汾�򷵻ص�ַ
	if (Version <= WIN_2K3)
		return hKerenlBase;
	
	// ������WIN VISTA 7ʱ�ų��ֵ����
	pListEntry = pListEntry->Flink;
	pLdrModule = __GetLdrModuleHeaderByIIOM__(pListEntry);
	hKernel32 = (HMODULE)pLdrModule->BaseAddress;
	return hKernel32;
}

// ��ȡAPI��ַ���õĹ�ϣ����
__dword __INTERNAL_FUNC__ GetApiAddress_Crc32HashFunc(__memory pTarget, __integer iTargetSize, __memory pHashValue) {
	__dword dwCrc32 = crc32(pTarget, iTargetSize);
	__logic_memcpy__(pHashValue, &dwCrc32, sizeof(__dword));
	return sizeof(__dword);
}

FARPROC __API__ GetLoadLibraryAAddress() {
	FARPROC pLoadLibraryA = NULL;
	__dword dwCrc32 = 0;
	HMODULE hKernel32 = GetKernel32BaseAddress();
	if (hKernel32 == NULL)
		return NULL;
	dwCrc32 = __CRC32_LOADLIBRARYA_STR__;
	pLoadLibraryA = xGetProcAddressImmediatelyByHash(hKernel32, (__memory)(&dwCrc32), sizeof(__dword), GetApiAddress_Crc32HashFunc);
	return pLoadLibraryA;
}

// ������Դ
#include "OptResource.c"

/*
 * ȫ�ֱ���
 */
FPLoadLibraryA g_pLoadLibraryA;
FPVirtualAlloc g_pVirtualAlloc;
FPSetUnhandledExceptionFilter g_pSetUnhandleExceptionFilter;

// ��ӳ��DLL
/*
 * 2012.2.7���޶�,ʹ��CopyMemToMem����
 */
__memory __API__ RemapDll(__memory pOrigMap, FPVirtualAlloc pVirtualAlloc, __bool bRunDllMain) {
	__memory pNewMap = NULL;
	__integer iSizeOfImage = 0;
	FPDllMain pDllMain = NULL;
	PIMAGE_NT_HEADERS pOrigMapNtHdr = NULL;
	
	pOrigMapNtHdr = GetNtHeader(pOrigMap);
	iSizeOfImage = pOrigMapNtHdr->OptionalHeader.SizeOfImage;
	pNewMap = (__memory)pVirtualAlloc(NULL, iSizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!pNewMap) return NULL;

	//__logic_memset__(pNewMap, 0, iSizeOfImage);
	//__logic_memcpy__(pNewMap, pOrigMap, iSizeOfImage);

	if (!CopyMemToMem(pOrigMap, pNewMap, iSizeOfImage)) {
		// ��������
	}

	// �����ض�λ
	BaseRelocation(pNewMap, (__address)pOrigMap, (__address)pNewMap, FALSE);
	// ��ȡ����ڵ��ַ,������
	pDllMain = (FPDllMain)(pNewMap + (pOrigMapNtHdr->OptionalHeader.AddressOfEntryPoint));
	if (bRunDllMain) pDllMain((HMODULE)pNewMap, DLL_PROCESS_ATTACH, NULL);
	return pNewMap;
}

// �������ض�λ���
#include "ProcedureRelocation.c"
// ���������
#include "ImpTblProtecter.c"

//////////////////////////////////////////////////////////////////////////
// 2011��3��30���Ժ�׷��
// �������н�Ϊ��д
__void __API__ SetAllSectionCanWrite(__memory pMem) {
	__integer i = 0, iNumberOfSections = 0;
	PIMAGE_SECTION_HEADER pSectionHeader = NULL;

	iNumberOfSections = GetNumberOfSections(pMem);
	pSectionHeader = GetFirstSection(pMem);
	for (i = 0; i < iNumberOfSections; i++, pSectionHeader++) {
		pSectionHeader->Characteristics |= IMAGE_SCN_MEM_WRITE;
	}
}

// ��ȡApiSetMap
PAPI_SET_MAP_HEADER __API__ GetApiSetMapHeader() {
	PAPI_SET_MAP_HEADER pApiSetMap = NULL;
	__memory pPeb = NULL;

	// ����Ƿ���Vista����ϵͳ
	if (GetWindowsVersion() != WIN_VISTA)
		return NULL;

	pPeb = (__memory)__NtCurrentPeb__();
	pApiSetMap = (PAPI_SET_MAP_HEADER)*((__address *)(pPeb + 0x38));
	return pApiSetMap;
}

PLDR_MODULE __API__ GetExistModuleInLoadModuleList(__memory pBaseAddress) {
	PPEB pPeb = __NtCurrentPeb__();
	PPEB_LDR_DATA pLdrData = pPeb->LdrData;
	PLIST_ENTRY pHeaderListEntry = pLdrData->InInitializationOrderModuleList.Flink;
	PLIST_ENTRY pListEntry = NULL;
	PLDR_MODULE pLdrModule = NULL;

	pListEntry = pHeaderListEntry;
	do {
		pLdrModule = __GetLdrModuleHeaderByIIOM__(pListEntry);
		// �Ա�
		if ((__dword)(pLdrModule->BaseAddress) == (__dword)pBaseAddress)
			return pLdrModule;
		pListEntry = pListEntry->Flink;
	} while (pListEntry != pHeaderListEntry);

	return NULL;
}

__integer __API__ GetLoadModuleCount() {
	PPEB pPeb = __NtCurrentPeb__();
	PPEB_LDR_DATA pLdrData = pPeb->LdrData;
	PLIST_ENTRY pHeaderListEntry = pLdrData->InInitializationOrderModuleList.Flink;
	PLIST_ENTRY pListEntry = NULL;
	__integer iCount = 0;

	pListEntry = pHeaderListEntry;

	do {
		iCount++;
		pListEntry = pListEntry->Flink;
	} while (pListEntry != pHeaderListEntry);

	return iCount;
}