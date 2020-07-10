/*
 * ����:
 *	ȷ��һ��CRC32�Ƿ����б���
 *
 * ����:
 *	dwTargetCrc32:Ŀ���CRC32ֵ
 *	pCrc32List:CRC32��ֵ�б�
 *	iCrc32ListCount:CRC32�ļ���
 */
__INLINE__ __bool __INTERNAL_FUNC__ ThisValueIsInList(__dword dwTargetCrc32, __dword *pCrc32List, __integer iCrc32ListCount) {
	__integer i;
	for (i = 0; i < iCrc32ListCount; i++) {
		if (dwTargetCrc32 == pCrc32List[i])
			return TRUE;
	}
	return FALSE;
}

/*
 * ����:
 *	��ʼ�����������
 */
__void __API__ InitImportProtecter() {
	__dword dwCrc32 = 0;
	HMODULE hKernel32 = NULL;

	__PrintDbgInfo_OutputDbgString__("Entry InitImportProtecter");

	// ��ȡkernel32�Ļ���ַ
	__PrintDbgInfo_OutputDbgString__("Get kernel32 base address");
	hKernel32 = GetKernel32BaseAddress();

	// ��ȡpVirtualAlloc�ĵ�ַ
	__PrintDbgInfo_OutputDbgString__("Get VirtualAlloc address");
	dwCrc32 = __CRC32_VIRTUALALLOC_STR__;
	g_pVirtualAlloc = (FPVirtualAlloc)xGetProcAddressImmediatelyByHash(hKernel32, (__memory)(&dwCrc32), sizeof(__dword), GetApiAddress_Crc32HashFunc);

	// ��ӳ��kernel32.dll
	__PrintDbgInfo_OutputDbgString__("Remap kernel32 map");
	hKernel32 = (HMODULE)RemapDll((__memory)hKernel32, g_pVirtualAlloc, FALSE);//����Ҳ��Ψһ���Ա��ϵ�����ĵط�

	//////////////////////////////////////////////////////////////////////////
	// ��ȡ�����֧��API��ַ
	__PrintDbgInfo_OutputDbgString__("Reget VirtualAlloc address");
	dwCrc32 = __CRC32_VIRTUALALLOC_STR__;
	g_pVirtualAlloc = (FPVirtualAlloc)xGetProcAddressImmediatelyByHash(hKernel32, (__memory)(&dwCrc32), sizeof(__dword), GetApiAddress_Crc32HashFunc);

	__PrintDbgInfo_OutputDbgString__("Get LoadLibraryA address");
	dwCrc32 = __CRC32_LOADLIBRARYA_STR__;
	g_pLoadLibraryA = (FPLoadLibraryA)xGetProcAddressImmediatelyByHash(hKernel32, (__memory)(&dwCrc32), sizeof(__dword), GetApiAddress_Crc32HashFunc);


	// �˳�
	__PrintDbgInfo_OutputDbgString__("Exit InitImportProtecter");
}

//////////////////////////////////////////////////////////////////////////
// ����Ϊ�ӽ�����������������ʵ��
// ������Կ,ʹ�ü򵥵�����㷨
__INLINE__ __void __INTERNAL_FUNC__ XorKey4Bytes(__memory pKey, __byte bXor) {
	__integer i = 0;
	for (i = 0; i < 4; i++)
		pKey[i] = pKey[i] ^ bXor;
}

/*
 * ����:
 *	���������Ļص�����
 *
 * ����:
 *	iIndex:Ҫ���ܺ����ĵ�ID��
 *	Type:���ܵ�����,������ǰ�Ƕ�̬��,����API
 *	pData:Ҫ���ܵ�����,����Type��ͬ����ͬ
 *	iDataSize:���ݵĳ���
 *	bOrd:���Type = PROC_NAME,������ֶ�ΪFALSE,���ʾ��һ�������������ĺ���
 *	dwFirstThunk:������е�FirstThunkֵ
 *	pArgs:�Զ��������ָ��
 */
__void __INTERNAL_FUNC__ EncryptIATCallBack(__integer iIndex, CRYPTIAT_TYPE Type, __memory pData, __integer iDataSize, __bool bOrd, __dword dwFirstThunk, __void *pArgs) {
	PIMPORT_PROTECT pImportProtect = (PIMPORT_PROTECT)pArgs;

	if (Type == LIB_NAME) {
		__char szLibName[128] = {0};
		__memory pKey = NULL;
		__dword dwKey = GenerateRandomNumber();//�����ȡһ��KEY
		pImportProtect->iLibraryNameLengthList[iIndex] = iDataSize;

		// ת������ΪСд
		__logic_strcpy__(szLibName, pData);
		__logic_str2lower__(szLibName);
		XorArray(dwKey, (__memory)szLibName, (__memory)pImportProtect->LibraryNameList[iIndex], iDataSize);//����
		pKey = (__memory)&dwKey;//������Կ
		XorKey4Bytes(pKey, pImportProtect->bXor);
		pImportProtect->dwLibraryKeyList[iIndex] = dwKey;
		(pImportProtect->iLibraryKeyCount)++;
	} else if (Type == PROC_NAME) {
		pImportProtect->bIsOrdList[iIndex] = bOrd;
		if (bOrd == FALSE) {
			__dword dwCrc32 = crc32(pData, iDataSize);
			__memory pCrc32 = (__memory)&dwCrc32;
			pImportProtect->dwApiNameCrc32List[iIndex] = dwCrc32;
		} else {
			pImportProtect->dwApiNameCrc32List[iIndex] = *(__dword *)pData;
			pImportProtect->dwApiNameCrc32List[iIndex] &= 0xFFFF;// ���ø�2�ֽ�Ϊ0
		}
		pImportProtect->dwFirstThunkList[iIndex] = dwFirstThunk;//��¼FirstThunk��ֵ
		(pImportProtect->iApiNameCrc32Count)++;
	}/* end else */
}

// ����������Ĺ�ϣ����
__dword __INTERNAL_FUNC__ DecrypterHashFunc(__memory pTarget, __integer iTargetSize, __memory pHashValue) {
	__memory pDecryptData = NULL;
	__dword dwDecryptSize = 0;
	__dword dwCrc32 = crc32(pTarget, iTargetSize);
	__logic_memcpy__(pHashValue, &dwCrc32, sizeof(__dword));
	return sizeof(__dword);
}

// ���������
__void __INTERNAL_FUNC__ DecryptIATCallBack(__integer iIndex, CRYPTIAT_TYPE Type, __memory pData, __integer iDataSize, __bool bOrd, __dword dwFirstThunk, __void *pArgs) {
	PIMPORT_PROTECT pImportProtect = (PIMPORT_PROTECT)pArgs;
	__dword dwLibNameCrc32 = 0;
	__bool bDllIsProtect = FALSE;

	if (Type == LIB_NAME) {
		__integer iLength = 0;
		__char pLibName[64] = {0};
		__dword dwKey = pImportProtect->dwLibraryKeyList[iIndex];//������Կ
		__memory pKey = (__memory)&dwKey;
		HMODULE hDll;
		XorKey4Bytes(pKey, pImportProtect->bXor);
		iLength = pImportProtect->iLibraryNameLengthList[iIndex];
		__logic_memcpy__(pLibName, pData, iLength);
		XorArray(dwKey, (__memory)pLibName, (__memory)pLibName, iLength);//����
		hDll = g_pLoadLibraryA(pLibName);

		dwLibNameCrc32 = crc32((__memory)pLibName, iLength);

		// ��ӳ��DLL
		if (ThisValueIsInList(dwLibNameCrc32, pImportProtect->dwProtectDllCrc32List, pImportProtect->iProtectDllCrc32Count)) {
			// �鿴�Ƿ����ų���ִ��DllMain�Ŀ�
			if (ThisValueIsInList(dwLibNameCrc32, pImportProtect->dwProtectDllCallDllMainCrc32List, pImportProtect->iProtectDllCallDllMainCrc32Count))
				hDll = (HMODULE)RemapDll((__memory)hDll, g_pVirtualAlloc, TRUE);
			else
				hDll = (HMODULE)RemapDll((__memory)hDll, g_pVirtualAlloc, FALSE);

			// ���ô�DLL��������
			bDllIsProtect = TRUE;
		} else {
			// ���ô�DLLδ��������
			bDllIsProtect = FALSE;
		}

		// ����Ϊ��ǰDLL
		pImportProtect->hDll = hDll;
	} else if (Type == PROC_NAME) {
		__memory pHashData = (__memory)&(pImportProtect->dwApiNameCrc32List[iIndex]);
		__address addrImageBase = pImportProtect->addrImageBase;
		__address *paddrOut = (__address *)(addrImageBase + (__address)dwFirstThunk);
		if (pImportProtect->bIsOrdList[iIndex]) {
			__dword dwOrd = pImportProtect->dwApiNameCrc32List[iIndex];
			*paddrOut = (__address)xGetProcAddressByHash(pImportProtect->hDll, (__memory)&dwOrd, 2, DecrypterHashFunc);
		} else
			*paddrOut = (__address)xGetProcAddressByHash(pImportProtect->hDll, pHashData, sizeof(__address), DecrypterHashFunc);
	}/* end else */
}

/*
 * ����:
 *	���������
 *
 * ����:
 *	pImageBase:����ַ
 *	pImportProtect:��������ṹָ��
 *	bOnFile:�Ƿ���һ���ļ������������
 *	pOutImportTable:����µ������
 */
__bool __API__ EncryptImportTable(__memory pImageBase, PIMPORT_PROTECT pImportProtect, __bool bOnFile, __memory pOutImportTable) {
	__byte bXor = 0;
	__integer iProcCount = 0, iLibCount = 0, iIndex = 0;
	__address addrImageBase = 0;
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	PIMAGE_DATA_DIRECTORY pImageImportDataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor = NULL;
	__integer iImportTableSize = 0;
	__dword dwFirstThunk = 0;
	__memory pMem = pImageBase;//�����ڴ����ַ

	// ��ʼ������
	pImageImportDataDirectory = ExistDataDirectory(pImageBase, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!pImageImportDataDirectory)
		return NULL;

	pNtHdr = GetNtHeader(pImageBase);
	if (bOnFile)
		addrImageBase = (__address)(pNtHdr->OptionalHeader.ImageBase);
	else
		addrImageBase = (__address)pMem;

	if (bOnFile)
		pImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(pMem + Rva2Raw(pMem, pImageImportDataDirectory->VirtualAddress));
	else
		pImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)__RvaToVa__(addrImageBase, pImageImportDataDirectory->VirtualAddress);

	// ����IMAGE_PROTECT
	bXor = (__byte)GenerateRandomNumber();//������ɳ�һ��KEY
	pImportProtect->bXor = bXor;

	// ���������Ϣ����IMPORT_PROTECT�ṹ
	// ��Щ����ֻ����FirstThunk
	while (pImageImportDescriptor->FirstThunk) {
		__address addrCurrAddress = 0;
		__dword dwCurrRva = 0;
		__char *svDllName = NULL;
		PIMAGE_THUNK_DATA pTdIn = NULL;
		PIMAGE_THUNK_DATA pTdOut = NULL;
		if (bOnFile)
			svDllName = (__char *)(pMem + Rva2Raw((__void *)pMem, pImageImportDescriptor->Name));
		else
			svDllName = (__char *)__RvaToVa__(addrImageBase, pImageImportDescriptor->Name);

		EncryptIATCallBack(iLibCount, LIB_NAME, (__memory)svDllName, __logic_strlen__(svDllName), FALSE, 0, (__void *)pImportProtect);

		// ���������ַ
		if (bOnFile) {
			pTdIn = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw((__void *)pMem, pImageImportDescriptor->OriginalFirstThunk));
			pTdOut = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw((__void *)pMem, pImageImportDescriptor->FirstThunk));
			// û��OriginalFirstThunk�����
			if ((__memory)pTdIn == pMem)
				pTdIn = pTdOut;
		} else {
			pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(addrImageBase, pImageImportDescriptor->OriginalFirstThunk);
			pTdOut = (PIMAGE_THUNK_DATA)__RvaToVa__(addrImageBase, pImageImportDescriptor->FirstThunk);
			// û��OriginalFirstThunk�����
			if ((__address)pTdIn == addrImageBase)
				pTdIn = pTdOut;
		}

		iProcCount = 0;
		// ���ܺ���
		while (pTdIn->u1.Function) {

			dwFirstThunk = (__dword)((__address)pTdOut - addrImageBase);

			// ��������������Ժ���������
			if (pTdIn->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
				__word wOrd = pTdIn->u1.Ordinal;
				EncryptIATCallBack(iIndex, PROC_NAME, (__memory)&wOrd, 2, TRUE, dwFirstThunk, (__void *)pImportProtect);
			} else {
				__char *szProcName = NULL;
				PIMAGE_IMPORT_BY_NAME pIbn = NULL;
				if (bOnFile)
					pIbn = (PIMAGE_IMPORT_BY_NAME)(pMem + Rva2Raw((__memory)pMem, pTdIn->u1.AddressOfData));
				else
					pIbn = (PIMAGE_IMPORT_BY_NAME)__RvaToVa__(addrImageBase, pTdIn->u1.AddressOfData);
				szProcName = (__char *)(pIbn->Name);
				EncryptIATCallBack(iIndex, PROC_NAME, (__memory)szProcName, __logic_strlen__(szProcName), FALSE, dwFirstThunk, (__void *)pImportProtect);
			}
			// ��һ������
			addrCurrAddress += sizeof(__address);
			iProcCount++;//������������
			iIndex++;//API������������
			pTdIn++;
			pTdOut++;
		}/* end while */

		// ����һ��DLL֮ǰ,��¼��ǰ�����е�API����
		pImportProtect->iApiNumberInThisLibrary[iLibCount] = iProcCount;
		// ��һ��DLL
		pImageImportDescriptor++;
		iLibCount++;
	}/* end while */

	// �����ܺ����ĸ����Ƿ�ͳһ
	if (iIndex != pImportProtect->iApiNameCrc32Count)
		return FALSE;

	// ���������
	iImportTableSize = pImageImportDataDirectory->Size;//�������
	if (pOutImportTable) {
		__logic_memcpy__(pOutImportTable, (__memory)pImageImportDescriptor, iImportTableSize);
	}

	__logic_memset__((__memory)pImageImportDescriptor, 0, iImportTableSize);//���ԭ�ȵ������
	return TRUE;
}

/*
 * ����:
 *	�����ӳ��DLL��Ĳ���ȫ����Ϣ
 */
__INLINE__ __void __INTERNAL_FUNC__ ClearRemapDllUnSafeInfo(__memory pDll) {
	PIMAGE_DOS_HEADER pDosHdr = NULL;
	PIMAGE_NT_HEADERS pNtHdr = NULL;

	// ��ȡPEͷ
	pDosHdr = (PIMAGE_DOS_HEADER)pDll;
	pNtHdr = GetNtHeader(pDll);
	
	// ���ӳ���������, �����, ���Ա�, �ض�λ��, ��PEͷ
	DeleteDataDirectoryObjectOnMemMap(pDll, IMAGE_DIRECTORY_ENTRY_EXPORT);
	DeleteDataDirectoryObjectOnMemMap(pDll, IMAGE_DIRECTORY_ENTRY_IMPORT);
	//DeleteDataDirectoryObjectOnMemMap(pDll, IMAGE_DIRECTORY_ENTRY_DEBUG);
	//DeleteDataDirectoryObjectOnMemMap(pDll, IMAGE_DIRECTORY_ENTRY_BASERELOC);
	// ���DOSͷ,PEͷ
	__logic_memset__((__memory)pDosHdr, 0, sizeof(IMAGE_DOS_HEADER));
	__logic_memset__((__memory)pNtHdr, 0, sizeof(IMAGE_NT_HEADERS));
}

/*
 * ����:
 *	���������
 *
 * ����:
 *	pImportProtect:��������ṹ
 */
__bool __API__ DecryptImportTable(PIMPORT_PROTECT pImportProtect) {
	__byte bXor = 0;
	HMODULE hDll = 0, hCurrDll = 0;
	__integer i = 0, j = 0, iIndex = 0;
	__integer iLength = 0;
	__char *pLibName = NULL;
	__char LibName[64] = {0};
	__dword dwLibNameCrc32 = 0;
	__bool bDllIsProtect = FALSE;

	__PrintDbgInfo_OutputDbgString__("Entry DecryptImportTable");

	bXor = pImportProtect->bXor;//��ȡ��Կ�Ľ�������
	for (i = 0; i < pImportProtect->iLibraryKeyCount; i++) {
		__dword dwKey = pImportProtect->dwLibraryKeyList[i];//������Կ
		__memory pKey = (__memory)&dwKey;
		XorKey4Bytes(pKey, bXor);
		iLength = pImportProtect->iLibraryNameLengthList[i];
		__logic_memset__(LibName, 0, 64);
		pLibName = pImportProtect->LibraryNameList[i];
		XorArray(dwKey, (__memory)pLibName, (__memory)LibName, iLength);//����
		hCurrDll = g_pLoadLibraryA(LibName);

		// ��ӳ��DLL
		dwLibNameCrc32 = crc32((__memory)pLibName, iLength);

		// ��ӳ��DLL
		if (ThisValueIsInList(dwLibNameCrc32, pImportProtect->dwProtectDllCrc32List, pImportProtect->iProtectDllCrc32Count)) {
			// �鿴�Ƿ����ų���ִ��DllMain�Ŀ�
			if (ThisValueIsInList(dwLibNameCrc32, pImportProtect->dwProtectDllCallDllMainCrc32List, pImportProtect->iProtectDllCallDllMainCrc32Count))
				hDll = (HMODULE)RemapDll((__memory)hCurrDll, g_pVirtualAlloc, TRUE);
			else
				hDll = (HMODULE)RemapDll((__memory)hCurrDll, g_pVirtualAlloc, FALSE);

			// ���ô�DLL��������
			bDllIsProtect = TRUE;
		} else {
			hDll = hCurrDll;

			// ���ô�DLLδ��������
			bDllIsProtect = FALSE;
		}

		// ��ȡAPI��ַ
		for (j = 0; j < pImportProtect->iApiNumberInThisLibrary[i]; j++) {
			__memory pHashData = (__memory)&(pImportProtect->dwApiNameCrc32List[iIndex]);
			__address addrImageBase = pImportProtect->addrImageBase;
			__dword dwFirstThunk = pImportProtect->dwFirstThunkList[iIndex];
			__address *paddrOut = (__address *)(addrImageBase + (__address)dwFirstThunk);
			if (pImportProtect->bIsOrdList[iIndex]) {
				__dword dwOrd = pImportProtect->dwApiNameCrc32List[iIndex];
				*paddrOut = (__address)xGetProcAddressByHash(hDll, (__memory)&dwOrd, 2, DecrypterHashFunc);
			} else
				*paddrOut = (__address)xGetProcAddressByHash(hDll, pHashData, sizeof(__address), DecrypterHashFunc);

			// ������������
			iIndex++;
		}/* end for */

		// ����
		if (iIndex != pImportProtect->iApiNameCrc32Count)
			return FALSE;

		// ����Ǳ���������������DLL��������Ϣ
		if (bDllIsProtect) {
			__PrintDbgInfo_OutputDbgString__("Already go to ClearRemapDllUnSafeInfo");
			ClearRemapDllUnSafeInfo((__memory)hDll);
		}
	}

	// �˳�
	__PrintDbgInfo_OutputDbgString__("Exit DecryptImportTable");
	return TRUE;
}

//////////////////////////////////////////////////////////////////////////
// ���������ģ��������������������е�

/*
 * ����:
 *	����Ҫ���ڴ��н���һ���Լ��������,������ʵ�������������ת��
 *	jmp dword ptr [xxx]
 *	jmp dword ptr [yyy]
 *	jmp dword ptr [zzz]
 *	������������þ��Ǵ���һ����������
 *
 * ����:
 *	pJmpAddressTableMakeSure:
 */
__INLINE__ __integer __INTERNAL_FUNC__ RandJmpAddress(__bool *pJmpAddressTableMakeSure) {
	__integer iIndex = 0;
	
	do {
		iIndex = GenerateRandomNumber() % __MAX_JMP_ADDRESS_TABLE_COUNT__;
	} while (pJmpAddressTableMakeSure[iIndex]);
	pJmpAddressTableMakeSure[iIndex] = TRUE;
	return iIndex;
}

/*
 * ����:
 *	ȷ��һ�������õĵ�ַ�Ƿ��������ĺ���
 *
 * ����:
 *	addrTargetAddress:Ҫ��ת��Ŀ���ַ
 *	pJmpAddressTable:�����ַ���ָ��
 *	iJmpAddressTableCount:�����ַ��ļ���
 */
__INLINE__ __bool __INTERNAL_FUNC__ IsInJmpAddressTable(__address addrTargetAddress, __address *pJmpAddressTable, __integer iJmpAddressTableCount) {
	__integer i = 0;
	for (i = 0; i < iJmpAddressTableCount; i++) {
		if (addrTargetAddress == pJmpAddressTable[i])
			return TRUE;
	}
	return FALSE;
}

typedef struct _JMPTABLE_TO_ADDRESS {
	__dword dwNewRva;
	__address addrAddress;
} JMPTABLE_TO_ADDRESS, *PJMPTABLE_TO_ADDRESS;

/*
 * ����:
 *	�鿴һ��Ŀ���ַ�Ƿ��Ѿ����ڵ�ַ�����ַ����
 *
 * ����:
 *	addrTargetAddress:Ŀ���ַ
 *	pJmpTableToAddressList:��ת��ַ��
 *	iJmpAddressTableCount:��ת��ַ��ļ���
 */
#define __NOT_IN_JMPADDRESSTABLE__		0xFFFFFFFF
__INLINE__ __dword __INTERNAL_FUNC__ GetNewRvaFromJmpAddressTable(__address addrTargetAddress, PJMPTABLE_TO_ADDRESS pJmpTableToAddressList, __integer iJmpAddressTableCount) {
	__integer i = 0;
	for (i = 0; i < iJmpAddressTableCount; i++) {
		if (addrTargetAddress == pJmpTableToAddressList[i].addrAddress)
			return pJmpTableToAddressList[i].dwNewRva;
	}
	return __NOT_IN_JMPADDRESSTABLE__;
}

/*
 * ����:
 *	���������ַ����ĸ���
 */
__integer __API__ CountReleaseReferenceImportTable(PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceImportTableAddress) {
	__integer i =0;

	while (pReferenceImportTableAddress) {
		i++;
		pReferenceImportTableAddress = pReferenceImportTableAddress->pNext;
	}

	return i;
}

/*
 * ����:
 *	�ͷ����������ַ����ڴ�
 */
__void __API__ ReleaseReferenceImportTableAddress(PREFERENCE_IMPORT_TABLE_ADDRESS *pReferenceImportTableAddressPoint) {
	PREFERENCE_IMPORT_TABLE_ADDRESS pCurrReferenceImportTableAddress = *pReferenceImportTableAddressPoint;
	PREFERENCE_IMPORT_TABLE_ADDRESS pTmp = NULL;
	while (pCurrReferenceImportTableAddress) {
		pTmp = pCurrReferenceImportTableAddress->pNext;
		__logic_delete__(pCurrReferenceImportTableAddress);
		pCurrReferenceImportTableAddress = pTmp;
	}
	*pReferenceImportTableAddressPoint = NULL;
}

/*
 * ����:
 *	�������������
 * 
 * ����:
 *	pMem:Ҫ����������ӳ��
 *	addrOrigImageBase:Ҫ��������ԭ�ȵĻ���ַ
 *	pImportProtect:��������ṹָ��
 *	bOnFile:�Ƿ����ļ�
 *	ofJmpTableRva:�µ�������ת���RVA
 */
PREFERENCE_IMPORT_TABLE_ADDRESS __API__ PowerEncryptImportTable(__memory pMem, __address addrOrigImageBase, PIMPORT_PROTECT pImportProtect, __bool bOnFile, __offset ofJmpTableRva) {
	__byte bXor = 0;
	__memory pCodeStart = NULL;
	__integer iCodeSize = 0;
	__integer iProcCount = 0, iLibCount = 0, iIndex = 0;
	__address addrImageBase = 0;
	__address addrCurrOrigImageBase = 0;
	__address OrigJmpAddressTable[__MAX_JMP_ADDRESS_TABLE_COUNT__] = {0};
	__bool JmpAddressTableMakeSure[__MAX_JMP_ADDRESS_TABLE_COUNT__] = {0};//����ȷ�ϵ�ǰλ���Ƿ�ռ��
	JMPTABLE_TO_ADDRESS JmpTableToAddressList[__MAX_JMP_ADDRESS_TABLE_COUNT__] = {0};
	__integer iJmpAddressTableCount = 0;
	__dword *pJmpAddressTableRva = NULL;
	__address addrNowAddress = 0;
	PIMAGE_NT_HEADERS pNtHdr = NULL;
	PIMAGE_DATA_DIRECTORY pImageImportDataDirectory = NULL;
	PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor = NULL;
	ud_t ud_obj = {0};
	PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceImportTableAddres = NULL;
	PREFERENCE_IMPORT_TABLE_ADDRESS *pCurrReferenceImportTableAddresPoint = &pReferenceImportTableAddres;

	// ��ʼ������
	pImageImportDataDirectory = ExistDataDirectory(pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);
	if (!pImageImportDataDirectory)
		return NULL;

	/*
	 * ��ȡĿ��ӳ��Ļ���ַ
	 * Ҫ�޸��Ļ���ַ����ͨ�������Ĳ���addrOrigImageBaseָ��,���addrOrigImageBase == 0xFFFFFFFF��ʹ��
	 * addrImageBase,���addrOrigImageBase != 0xFFFFFFFF,��ʹ��addrOrigImageBase
	 */
	pNtHdr = GetNtHeader(pMem);
	addrImageBase = pImportProtect->addrImageBase;

	// ��ȡĿ���ļ�����εĵ�ַ�볤��
	if (bOnFile) {
		//addrImageBase = (__address)(pNtHdr->OptionalHeader.ImageBase);
		pImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(pMem + Rva2Raw(pMem, pImageImportDataDirectory->VirtualAddress));
		pCodeStart = pMem + GetEntryPointSection(pMem)->PointerToRawData;
	} else {
		//addrImageBase = (__address)pMem;
		pImageImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)__RvaToVa__(pMem, pImageImportDataDirectory->VirtualAddress);
		pCodeStart = pMem + GetEntryPointSection(pMem)->VirtualAddress;
	}
	iCodeSize = GetEntryPointSection(pMem)->Misc.VirtualSize;
	__logic_memset__(JmpAddressTableMakeSure, FALSE, __MAX_JMP_ADDRESS_TABLE_COUNT__);

	// ����IMAGE_PROTECT
	bXor = (__byte)GenerateRandomNumber();//������ɳ�һ��KEY
	pImportProtect->bXor = bXor;

	// ���������Ϣ����IMPORT_PROTECT�ṹ
	// ��Щ����ֻ����FirstThunk
	while (pImageImportDescriptor->FirstThunk) {
		__address addrCurrAddress = 0;
		__dword dwCurrRva = 0;
		__char *svDllName = NULL;
		PIMAGE_THUNK_DATA pTdIn = NULL;
		PIMAGE_THUNK_DATA pTdOut = NULL;
		if (bOnFile)
			svDllName = (__char *)(pMem + Rva2Raw(pMem, pImageImportDescriptor->Name));
		else
			svDllName = (__char *)__RvaToVa__(pMem, pImageImportDescriptor->Name);

		EncryptIATCallBack(iLibCount, LIB_NAME, (__memory)svDllName, __logic_strlen__(svDllName), FALSE, 0, (__void *)pImportProtect);

		// ���������ַ
		if (bOnFile) {
			pTdIn = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw(pMem, pImageImportDescriptor->OriginalFirstThunk));
			pTdOut = (PIMAGE_THUNK_DATA)(pMem + Rva2Raw(pMem, pImageImportDescriptor->FirstThunk));
			// û��OriginalFirstThunk�����
			if ((__memory)pTdIn == pMem)
				pTdIn = pTdOut;
		} else {
			pTdIn = (PIMAGE_THUNK_DATA)__RvaToVa__(pMem, pImageImportDescriptor->OriginalFirstThunk);
			pTdOut = (PIMAGE_THUNK_DATA)__RvaToVa__(pMem, pImageImportDescriptor->FirstThunk);
			// û��OriginalFirstThunk�����
			if ((__memory)pTdIn == pMem)
				pTdIn = pTdOut;
		}
		
		iProcCount = 0;
		if (addrOrigImageBase == __USE_DEF_IMAGEBASE_AS_ORIG__)
			addrCurrOrigImageBase = addrImageBase;
		else
			addrCurrOrigImageBase = addrOrigImageBase;
		addrCurrAddress = __RvaToVa__(addrCurrOrigImageBase, pImageImportDescriptor->FirstThunk);

		// ���ܺ���
		while (pTdIn->u1.Function) {
			// ���ȡ��һ��Ҫ�����λ��
			__integer iOffsetTableIndex = RandJmpAddress(&JmpAddressTableMakeSure);

			// ��ȡ�µ�FirstThunk
			__dword dwNewFirstThunk = (__dword)(ofJmpTableRva + iOffsetTableIndex * sizeof(__address));
			OrigJmpAddressTable[iJmpAddressTableCount] = addrCurrAddress;
			JmpTableToAddressList[iJmpAddressTableCount].addrAddress = addrCurrAddress;
			// �ϳ��µ������ַ��ƫ��
			JmpTableToAddressList[iJmpAddressTableCount].dwNewRva = dwNewFirstThunk;
			iJmpAddressTableCount++;//��������

			// ��������������Ժ���������
			if (pTdIn->u1.Ordinal & IMAGE_ORDINAL_FLAG32) {
				__word wOrd = pTdIn->u1.Ordinal;
				EncryptIATCallBack(iIndex, PROC_NAME, (__memory)&wOrd, 2, TRUE, dwNewFirstThunk, (__void *)pImportProtect);
			} else {
				__char *szProcName = NULL;
				PIMAGE_IMPORT_BY_NAME pIbn = NULL;
				if (bOnFile)
					pIbn = (PIMAGE_IMPORT_BY_NAME)(pMem + Rva2Raw(pMem, pTdIn->u1.AddressOfData));
				else
					pIbn = (PIMAGE_IMPORT_BY_NAME)__RvaToVa__(pMem, pTdIn->u1.AddressOfData);
				szProcName = (__char *)(pIbn->Name);
				EncryptIATCallBack(iIndex, PROC_NAME, (__memory)szProcName, __logic_strlen__(szProcName), FALSE, dwNewFirstThunk, (__void *)pImportProtect);
			}
			// ��һ������
			addrCurrAddress += sizeof(__address);
			iProcCount++;//������������
			iIndex++;//������������
			pTdIn++;
			pTdOut++;
		}/* end while */
		
		// ����һ��DLL֮ǰ,��¼��ǰ�����е�API����
		pImportProtect->iApiNumberInThisLibrary[iLibCount] = iProcCount;
		// ��һ��DLL
		pImageImportDescriptor++;
		iLibCount++;
	}/* end while */

	pJmpAddressTableRva = &(pImportProtect->dwFirstThunkList);

	// ��֤
	if (iIndex != pImportProtect->iApiNameCrc32Count)
		return NULL;

	// �����������
	ud_init(&ud_obj);
	ud_set_input_buffer(&ud_obj, pCodeStart, iCodeSize);//Ĭ��һ����������󳤶�Ϊ4096
	ud_set_mode(&ud_obj, 32);
	ud_set_syntax(&ud_obj, UD_SYN_INTEL);
	while (ud_disassemble(&ud_obj)) {
		__memory pFileSaveAddress = NULL;
		struct ud_operand *pCurrOperand = NULL;
		/*
		 * �ҵ�ӵ���ڴ����������ָ��,���ָ����ʵ��ڴ��ַ�ڱ��еĻ�,�����дΪ�µ��ڴ�
		 * ��ַ
		 */
		if (ud_obj.operand[0].type != UD_NONE) {
			// ���ӵ�в�����
			__integer iOperandCount = 0;

			///*
			// * ���������MOV,CALL,JMP ����ָ��
			// * ��Ϊ��ͬ����ֻ��������ָ�����������ɷ���
			// */
			//if (ud_obj.mnemonic == UD_Imov) {
			//	if ((ud_obj.operand[1].type == UD_OP_MEM) && (ud_obj.operand[1].base == UD_NONE) && \
			//		(ud_obj.operand[1].index == UD_NONE) && (ud_obj.operand[1].size == 32)) {
			//			pCurrOperand = &(ud_obj.operand[1]);
			//			pFileSaveAddress = (__memory)(ud_obj.inp_buff) - sizeof(__dword);
			//	}
			//} else if ((ud_obj.mnemonic == UD_Icall) || (ud_obj.mnemonic == UD_Ijmp)) {
			//	if ((ud_obj.operand[0].type == UD_OP_MEM) && (ud_obj.operand[0].base == UD_NONE) && \
			//		(ud_obj.operand[0].index == UD_NONE) && (ud_obj.operand[0].offset) && (ud_obj.operand[0].size == 32)) {
			//			pCurrOperand = &(ud_obj.operand[0]);
			//			pFileSaveAddress = (__memory)(ud_obj.inp_buff) - sizeof(__dword);
			//	}
			//}

			//////////////////////////////////////////////////////////////////////////
			// 2012.2.10 �޸�Ϊ ����ָ��ֻҪ���ڴ����
			// ����:xxx dword ptr [address]
			//      xxx dword ptr [address], reg
			//      xxx reg, dword ptr [address]
			if ((ud_obj.operand[0].type == UD_OP_MEM) && (ud_obj.operand[0].base == UD_NONE) && \
				(ud_obj.operand[0].index == UD_NONE) && (ud_obj.operand[0].offset) && (ud_obj.operand[0].size == 32)) {
					pCurrOperand = &(ud_obj.operand[0]);
					pFileSaveAddress = (__memory)(ud_obj.inp_buff) - sizeof(__dword);
			}
			else if ((ud_obj.operand[1].type == UD_OP_MEM) && (ud_obj.operand[1].base == UD_NONE) && \
				(ud_obj.operand[1].index == UD_NONE) && (ud_obj.operand[1].size == 32)) {
					pCurrOperand = &(ud_obj.operand[1]);
					pFileSaveAddress = (__memory)(ud_obj.inp_buff) - sizeof(__dword);
			}

			if (pFileSaveAddress) {
				__memory pCurrInstFileAddress = ud_obj.inp_buff - ud_obj.inp_ctr;
				/*
				 * ������ڴ���ʲ���
				 * ȡ����ַ����ת��ַ���н��м���
				 */
				__address addrTargetAddress = (__address)(pCurrOperand->lval.sdword);
				__dword dwNowRva = GetNewRvaFromJmpAddressTable(addrTargetAddress, &JmpTableToAddressList, iJmpAddressTableCount);
				if (dwNowRva != __NOT_IN_JMPADDRESSTABLE__) {
					addrNowAddress = addrImageBase + dwNowRva;
					*(__address *)pFileSaveAddress = addrNowAddress;//��������

					// ����һ����������ýṹ
					(*pCurrReferenceImportTableAddresPoint) = (PREFERENCE_IMPORT_TABLE_ADDRESS)__logic_new__(REFERENCE_IMPORT_TABLE_ADDRESS, 1);
					if (bOnFile) {
						(*pCurrReferenceImportTableAddresPoint)->ofReferenceRVA = Raw2Rva(pMem, (__integer)(pFileSaveAddress - pMem));
						(*pCurrReferenceImportTableAddresPoint)->addrReferenceMemAddress = addrImageBase + Raw2Rva(pMem, (__integer)(pCurrInstFileAddress - pMem));
					} else {
						(*pCurrReferenceImportTableAddresPoint)->ofReferenceRVA = (__integer)(pFileSaveAddress - pMem);
						(*pCurrReferenceImportTableAddresPoint)->addrReferenceMemAddress = addrImageBase + (__integer)(pCurrInstFileAddress - pMem);
					}
					(*pCurrReferenceImportTableAddresPoint)->pReferenceFileAddress = pCurrInstFileAddress;
					__logic_memcpy__(&((*pCurrReferenceImportTableAddresPoint)->ud_obj), &ud_obj, sizeof(ud_t));

					// ��������������ڴ�ָ��RVA,����������ļ�������������ʧ��ֱ���˳�
					if (pImportProtect->iImportTableReferenceAddressCount >= __MAX_IMPORT_REFERENCE_COUNT__) {
						// �ͷ��Ѿ�������ڴ�
						ReleaseReferenceImportTableAddress(&pReferenceImportTableAddres);
						return NULL;
					}/* end if */
					pImportProtect->ofImportTableReferenceAddressRVA[pImportProtect->iImportTableReferenceAddressCount] = (*pCurrReferenceImportTableAddresPoint)->ofReferenceRVA;
					pImportProtect->ofAPINowRVA[pImportProtect->iImportTableReferenceAddressCount] = dwNowRva;
					(pImportProtect->iImportTableReferenceAddressCount)++;

					// �ƶ�����һ��
					pCurrReferenceImportTableAddresPoint = &((*pCurrReferenceImportTableAddresPoint)->pNext);
				}
			}/* end if */
		}/* end if */
	}

	pImportProtect->ofJmpTableRva = ofJmpTableRva;//��������ת���RVA
	// ����ԭ�ȵ������
	if (bOnFile)
		DeleteDataDirectoryObject(pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);
	else
		DeleteDataDirectoryObjectOnMemMap(pMem, IMAGE_DIRECTORY_ENTRY_IMPORT);

	return pReferenceImportTableAddres;
}

/*
 * ����:
 *	���������ַ��,�˺�����Ŀ���ļ��е���,�����޸�
 *
 * ����:
 *	pArg:�����������������ṹ
 */
__bool __API__ PowerDecryptImportTable(PPOWERDECRYPTIMPORTTABLE_ARG pArg) {
	PIMPORT_PROTECT pImportProtect = NULL;
	FPAddThunkCodeStub pAddThunkCodeStub = NULL;
	__byte bXor = 0;
	HMODULE hDll = NULL, hCurrDll = NULL;
	__integer i = 0, j = 0, iIndex = 0;
	__integer iLength = 0;
	__char *pLibName = NULL;
	__char LibName[64] = {0};
	__dword dwLibNameCrc32 = 0;
	__bool bDllIsProtect = FALSE;

	__PrintDbgInfo_OutputDbgString__("Entry PowerDecryptImportTable");

	// ��ʼ������
	pImportProtect = pArg->pImportProtect;
	pAddThunkCodeStub = pArg->pAddThunkCodeStub;
	bXor = pImportProtect->bXor;//��ȡ��Կ�Ľ�������

	for (i = 0; i < pImportProtect->iLibraryKeyCount; i++) {
		__dword dwKey = 0;
		__memory pKey = NULL;

		dwKey = pImportProtect->dwLibraryKeyList[i];//������Կ
		pKey = (__memory)&dwKey;

		XorKey4Bytes(pKey, bXor);
		iLength = pImportProtect->iLibraryNameLengthList[i];
		__logic_memset__(LibName, 0, 64);
		pLibName = pImportProtect->LibraryNameList[i];
		XorArray(dwKey, (__memory)pLibName, (__memory)LibName, iLength);//����

		// ��ȡ������CRC32ֵ
		dwLibNameCrc32 = crc32((__memory)LibName, iLength);

		// ���ز���ӳ��DLL
		hCurrDll = g_pLoadLibraryA(LibName);

		// ������ڱ����ڵĿ�������ӳ��
		if (ThisValueIsInList(dwLibNameCrc32, pImportProtect->dwProtectDllCrc32List, pImportProtect->iProtectDllCrc32Count)) {
			// �鿴�Ƿ����ų���ִ��DllMain�Ŀ�
			if (ThisValueIsInList(dwLibNameCrc32, pImportProtect->dwProtectDllCallDllMainCrc32List, pImportProtect->iProtectDllCallDllMainCrc32Count))
				hDll = (HMODULE)RemapDll((__memory)hCurrDll, g_pVirtualAlloc, TRUE);
			else
				hDll = (HMODULE)RemapDll((__memory)hCurrDll, g_pVirtualAlloc, FALSE);

			// ���ô�DLL��������
			bDllIsProtect = TRUE;
		} else {
			hDll = (HMODULE)hCurrDll;

			// ���ô�DLLδ��������
			bDllIsProtect = FALSE;
		}

		// ��ȡAPI��ַ
		for (j = 0; j < pImportProtect->iApiNumberInThisLibrary[i]; j++) {
			__memory pHashData = NULL;
			__address addrImageBase = 0;
			__dword dwFirstThunk = 0;
			__address *paddrOut = NULL;

			pHashData = (__memory)&(pImportProtect->dwApiNameCrc32List[iIndex]);
			addrImageBase = pImportProtect->addrImageBase;
			dwFirstThunk = pImportProtect->dwFirstThunkList[iIndex];
			paddrOut = (__address *)(addrImageBase + (__address)dwFirstThunk);

			if (pImportProtect->bIsOrdList[iIndex]) {
				__dword dwOrd = pImportProtect->dwApiNameCrc32List[iIndex] & 0xFFFF;//ȡ��2�ֽ�
				__PrintDbgInfo_OutputDbgString__("Already goto xGetProcAddressByHash by API ord");
				*paddrOut = (__address)xGetProcAddressByHash(hDll, (__memory)dwOrd, 2, DecrypterHashFunc);
			} else {
				__PrintDbgInfo_OutputDbgString__("Already goto xGetProcAddressByHash by API name hash value");
				*paddrOut = (__address)xGetProcAddressByHash(hDll, pHashData, sizeof(__address), DecrypterHashFunc);
			}

			// ����Ҫ��ȡ��API�б�����Ѱ
			__PrintDbgInfo_OutputDbgString__("Already goto ThisApiIsInProtectList");
			if (ThisValueIsInList(pImportProtect->dwApiNameCrc32List[iIndex], &(pImportProtect->dwProtectApiCrc32List), pImportProtect->iProtectApiCrc32Count)) {
				__memory pNowAddress = NULL;
				__memory pNewAddress = NULL;
				__offset ofOffset = 0;
				//__integer iProcSize = 0;

				pNowAddress = (__memory)(*paddrOut);

				// ������������ĳ���
				//iProcSize = SimpleCalcThisProcedureLength(pNowAddress);

				if (pAddThunkCodeStub) {
					pNewAddress = pAddThunkCodeStub((__address)pNowAddress);
					if (!pNewAddress) goto _default_add_thunk_code_stub;//���Ϊ����ֱ��ת��Ĭ�ϴ���
				} else {
					_default_add_thunk_code_stub:
					// ����һ���м亯��
					pNewAddress = __logic_new_size__(0x10);
					// ����ƫ��
					ofOffset = CalcCurrOffsetFromAddress(32, (__address)pNowAddress, (__address)pNewAddress, 5);
					*(__byte *)pNewAddress = 0xE9;
					*(__offset *)(pNewAddress + 1) = ofOffset;
				}

				// �����趨��ַ
				*paddrOut = (__address)pNewAddress;
			}

			// ������������
			iIndex++;
		}/* end for */

		// ����ڱ�����Χ����������DLL��������Ϣ
		if (bDllIsProtect) {
			__PrintDbgInfo_OutputDbgString__("Already goto ClearRemapDllUnSafeInfo");
			ClearRemapDllUnSafeInfo(hDll);
		}/* end if */
	}

	// ����
	if (iIndex != pImportProtect->iApiNameCrc32Count)
		return FALSE;

	// ���￪��һ���ǳ���Ҫ�����޶�Ŀ������Ҫ���õ�
	__PrintDbgInfo_OutputDbgString__("Fix target code reference API memory address");
	{
		__integer i = 0;
		__memory pMem = NULL;
		
		pMem = (__memory)(pImportProtect->addrImageBase);

		for (i = 0; i < pImportProtect->iImportTableReferenceAddressCount; i++) {
			__address *pReference = NULL;
			__address addrNowAddress = 0;

			// ������õĵ�ַ
			pReference = (__address *)(pMem + pImportProtect->ofImportTableReferenceAddressRVA[i]);
			addrNowAddress = (__address)(pMem + pImportProtect->ofAPINowRVA[i]);

			// ����
			*pReference = addrNowAddress;
		}/* end for */
	}

	__PrintDbgInfo_OutputDbgString__("Exit PowerDecryptImportTable");
	return TRUE;
}
