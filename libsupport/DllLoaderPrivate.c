/*
 * ����
 */
#define __SIZE_OF_PARAMETER_BLOCK__ 4096

/*
 * DLL����
 */
typedef struct _DLL_LIST {
	__void *pImageBase;
	__tchar svName[MAX_PATH];
	__dword dwFlags;
	__integer iLockCount;
	struct _DLL_LIST *pNext;
} DLL_LIST, *PDLL_LIST;

/*
 * ȫ�ֱ���
 */
PDLL_LIST g_pDLLHead = NULL;//���ڴ���DLL����
CRITICAL_SECTION g_DLLCrit;//����ͬ��DLL�ļ���

//����DLL����
/*
 * ����ֵ:
 * 0:��ʾ�����µ�����
 * 1:��ʾ�µ�DLL����
 * -1:��ʾ����
 */
__integer __INTERNAL_FUNC__ AddDLLReference(__memory pImageBase, __tchar *svName, __dword dwFlags) {
	PDLL_LIST pCur = NULL;
	EnterCriticalSection(&g_DLLCrit);

	// Ѱ��DLL���б�
	for(pCur = g_pDLLHead ; pCur != NULL; pCur = pCur->pNext) {
		if(pCur->pImageBase == pImageBase) 
			break;
	}

	if(pCur) {
		// ����DLL����
		pCur->iLockCount++;
		LeaveCriticalSection(&g_DLLCrit);
		return 0;
	}

	// �����µ�DLL������б�
	pCur = __logic_new__(DLL_LIST, 1);
	if(!pCur) {
		LeaveCriticalSection(&g_DLLCrit);
		return -1;
	}

	pCur->pImageBase = pImageBase;
	if(svName) {
		__logic_tcsncpy__(pCur->svName, svName, MAX_PATH);
	} else {
		pCur->svName[0] = _T('\0');
	}
	pCur->iLockCount = 1;
	pCur->dwFlags = dwFlags;
	pCur->pNext = g_pDLLHead;
	g_pDLLHead = pCur;

	LeaveCriticalSection(&g_DLLCrit);
	return 1;
}

__integer __INTERNAL_FUNC__ RemoveDLLReference(__memory pImageBase, __tchar *svName, __dword *pdwFlags) {
	PDLL_LIST pCurr = NULL, pPrev = NULL;
	EnterCriticalSection(&g_DLLCrit);

	// ��ѯDLL���б�
	for(pCurr = g_pDLLHead; pCurr != NULL; pCurr = pCurr->pNext) {
		if(pCurr->pImageBase == pImageBase) break;
		pPrev = pCurr;
	}
	if(!pCurr) {
		LeaveCriticalSection(&g_DLLCrit);
		return -1;
	}

	pCurr->iLockCount--;//����DLL����
	*pdwFlags = pCurr->dwFlags;//��ѯDLL��Ϣ
	__logic_tcsncpy__(svName, pCurr->svName, MAX_PATH);

	if (pCurr->iLockCount == 0) {//�Ƴ�
		if(!pPrev) {
			g_pDLLHead = g_pDLLHead->pNext;
			__logic_delete__(pCurr);
		} else {
			pCurr->pNext = pCurr->pNext;
			__logic_delete__(pCurr);
		}

		LeaveCriticalSection(&g_DLLCrit);
		return 1;
	}

	LeaveCriticalSection(&g_DLLCrit);
	return 0;
}

__bool __INTERNAL_FUNC__ RunDLLMain(__memory pImageBase, __dword dwImageSize, __bool bDetach) {
	__bool bRet;
	FPDllMain pDllMain;
	PIMAGE_NT_HEADERS pNt = GetNtHeader(pImageBase);
	if ((pNt->FileHeader.Characteristics & IMAGE_FILE_DLL) == 0) return TRUE;
	pDllMain =(FPDllMain)__RvaToVa__(pImageBase, pNt->OptionalHeader.AddressOfEntryPoint);
	if (bDetach)
		bRet = pDllMain((HMODULE)pImageBase, DLL_PROCESS_DETACH, NULL);
	else
		bRet = pDllMain((HMODULE)pImageBase, DLL_PROCESS_ATTACH, NULL);

	return bRet;
}
