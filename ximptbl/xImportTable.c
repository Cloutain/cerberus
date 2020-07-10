#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <tchar.h>

#include "resource.h"
#include "lcrypto.h"
#include "Support.h"
#include "DISAth.h"
#include "yImportTable.h"
#include "YImpTblKey.h"

// Y�������������Կ
#define __YIMPTBL_DECRYPT_KEY__									__EFILER_KEY__

//#define __TEST_DECRYPT_IMPORTTABLE__							1
//#define __TEST_DECRYPT_IMPORTTABLE_IN_DLL__					1
#define __DEF_XIMPORT_TABLE_SECTION_NAME__						"XIMPTBL"
#define __DEF_XIMPORT_TABLE_X_SECTION_NAME__					"XXIMPTBL"

// ���Ե�������
//(1) /ddd 6 /k /c /s test /n /ramc /rtl kernel32.dll /b

__void __INTERNAL_FUNC__ Usage() {
	printf("version:1.0 alpha\r\n");
	printf("email:logic.yan@gmail.com\r\n");
	printf("usage:ximptbl [options] <filepath>\r\n");
	printf("[options]\r\n");
	printf("/delete data directory <1,2,...>(ddd)\r\n");										//������ɺ�ɾ����ĳ������Ŀ¼
	printf("/keep tail data\r\n");																//����ĩβ����
	printf("/checksum reset(cr)\r\n");															//���¼���У���
	printf("/compatibility increase(ci)\r\n");													//��߼�����
	printf("/section <name>\r\n");																//�趨�½ڵĽ���
	printf("/nx compatible clear\r\n");															//����ִ�б������ݹر�
	printf("/rand address mapping clear(ramc)\r\n");											//�����ַ��ӳ����ݹر�
	printf("/rand all section names(rasn)\r\n");												//������н���
	printf("/remap target lib(rtl) <lib1,lib2,...>\r\n");										//����ӳ��Ŀ���ļ���DLL
	printf("/remap dis lib(rdl) <lib1,lib2,...>\r\n");											//����ӳ��ǵ�DLL
	printf("/recall target dllmain(rtd) <lib1,lib2,...>\r\n");									//�ٴν���Ŀ���ļ�����ӳ���DLL��dllmain
	printf("/recall dis dllmain(rdd) <lib1,lib2,...>\r\n");										//�ٴν���ǵ���ӳ���DLL��dllmain
	printf("/protect target API(pta) <api1,api2,...>\r\n");										//Ҫ�ر𱣻�Ŀ���ļ���API
	printf("/protect dis API(pda) <api1,api2,...>\r\n");										//Ҫ�ر𱣻��ǵ�API
	printf("/import table protect close(itpc)\r\n");											//�ر��������
	printf("/import table keep same orig data(itksod)\r\n");									//����һЩԭʼ������е�����,��߼�����	
	printf("/backup target file\r\n");															//����ԭ�ļ�
	printf("/x <dispath>\r\n");																	//�ⲿ�����DIS��(yimptbl.dll)
	printf("\r\n");
	printf("Waiting Fixed:some features(rtl,rdl) is not stable in win vista above\r\n");
}

// xImpTbl���ýṹ
typedef struct _XIMPORT_TABLE_CONFIGURE {
	__bool bBackupOrigFile;//����ԭ�ļ�
} XIMPORT_TABLE_CONFIGURE, *PXIMPORT_TABLE_CONFIGURE;

/*
 * ����:
 *	iArgc:�����и���
 *	pArgv:�����в���
 *	pConfigure:������������������ýṹ
 *	szTargetFilePath:Ŀ���ļ�·��
 *	pXImportTableConfigure:����̨���ýṹ
 *
 * ����:
 *	����������
 */
__bool __INTERNAL_FUNC__ HandleArguments(__integer iArgc, __tchar *pArgv[], PDISATH_CONFIGURE pDisAthConfigure, PXIMPORT_TABLE_CONFIGURE pImportTableConfigure) {
	__integer i = 1;

	// ��������С��
	if (iArgc == 1) {
		Usage();
		return FALSE;
	}

	// Ĭ�����ñ���ѡ��
	pDisAthConfigure->bProtectMyIAT = TRUE;
	pDisAthConfigure->bProtectTargetIAT = TRUE;

	// ����������
	for (i = 1; i < (iArgc - 1); i++) {
		if (pArgv[i][0] == _T('/')) {
			switch (pArgv[i][1]) {
			case _T('d'):
			case _T('D'): {
				if (_tcsicmp(pArgv[i], _T("/ddd")) == 0) {
					// Ҫɾ��������Ŀ¼
					__char seps[] = ",";
					__char *token = NULL;
					__char szString[2048] = {0};
					__integer iIndex = 0;

					// ָ�����
					i++;

					// ȡ��Ҫɾ��������Ŀ¼
					UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), szString, 2048);
					token = strtok(szString, seps);
					while (token) {
						iIndex = atoi(token);
						pDisAthConfigure->XFileAttachInfo.bWillDelDataDirectoryIndexs[iIndex] = TRUE;
						token = (__char *)strtok(NULL, seps);
					}/* end while */
				} else {
					Usage();
					return FALSE;
				}
			} break;
			case _T('k'):
			case _T('K'): {
				if (pArgv[i][2] != _T('\0')) {
					Usage();
					return FALSE;
				}
				pDisAthConfigure->XFileAttachInfo.bKeepTailData = TRUE;
			} break;
			case _T('c'):
			case _T('C'): {
				if (_tcsicmp(pArgv[i], _T("/cr")) == 0) {
					pDisAthConfigure->XFileAttachInfo.bResetCheckSum = TRUE;
				} else if (_tcsicmp(pArgv[i], _T("/ci")) == 0) {
					pDisAthConfigure->XFileAttachInfo.bCompatibility = TRUE;
				} else {
					Usage();
					return FALSE;
				}
			} break;
			case _T('s'):
			case _T('S'): {
				// ָ�����
				pDisAthConfigure->XFileAttachInfo.szSectionName = (__char *)__logic_new_size__(0x10);
				i++;
				UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), pDisAthConfigure->XFileAttachInfo.szSectionName, 8);
			} break;
			case _T('b'):
			case _T('B'): {
				pImportTableConfigure->bBackupOrigFile = TRUE;
			} break;
			case _T('n'):
			case _T('N'): {
				pDisAthConfigure->XFileAttachInfo.bCloseNX = TRUE;
			} break;
			case _T('r'):
			case _T('R'): {
				__char seps[] = ",";
				__char *token = NULL;
				__char szString[2048] = {0};
				__integer iCount = 0;

				if (_tcsicmp(pArgv[i], _T("/ramc")) == 0) {
					pDisAthConfigure->XFileAttachInfo.bCloseRandAddressMap = TRUE;
				} else if (_tcsicmp(pArgv[i], _T("/rasn")) == 0) {
					pDisAthConfigure->XFileAttachInfo.bRandomAllSectionName = TRUE;
				} else if (_tcsicmp(pArgv[i], _T("/rtl")) == 0) {

					// ָ�����
					i++;

					UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), szString, 2048);
					token = strtok(szString, seps);
					while (token) {
						 pDisAthConfigure->dwTargetProtectDllCrc32List[iCount++] = crc32(token, strlen(token));
						token = (__char *)strtok(NULL, seps);
					}/* end while */
					pDisAthConfigure->iTargetProtectDllCrc32Count = iCount;

				} else if (_tcsicmp(pArgv[i], _T("/rdl")) == 0) {

					// ָ�����
					i++;

					UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), szString, 2048);
					token = strtok(szString, seps);
					while (token) {
						pDisAthConfigure->dwDISProtectDllCrc32List[iCount++] = crc32(token, strlen(token));
						token = (__char *)strtok(NULL, seps);
					}/* end while */
					pDisAthConfigure->iDISProtectDllCrc32Count = iCount;

				} else if (_tcsicmp(pArgv[i], _T("/rtd")) == 0) {

					// ָ�����
					i++;

					UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), szString, 2048);
					token = strtok(szString, seps);
					while (token) {
						pDisAthConfigure->dwTargetProtectDllCallDllMainCrc32List[iCount++] = crc32(token, strlen(token));
						token = (__char *)strtok(NULL, seps);
					}/* end while */
					pDisAthConfigure->iTargetProtectDllCallDllMainCrc32Count = iCount;

				} else if (_tcsicmp(pArgv[i], _T("/rdd")) == 0) {

					// ָ�����
					i++;

					UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), szString, 2048);
					token = strtok(szString, seps);
					while (token) {
						pDisAthConfigure->dwDISProtectDllCallDllMainCrc32List[iCount++] = crc32(token, strlen(token));
						token = (__char *)strtok(NULL, seps);
					}/* end while */
					pDisAthConfigure->iDISProtectDllCallDllMainCrc32Count = iCount;

				} else {
					Usage();
					return FALSE;
				}
			} break;
			case _T('p'):
			case _T('P'): {
				__char seps[] = ",";
				__char *token = NULL;
				__char szString[2048] = {0};
				__integer iCount = 0;
				__integer *pCountOut = NULL;
				__dword *pCrc32List = NULL;

				if (_tcsicmp(pArgv[i], _T("/pta")) == 0) {
					// Ҫ���Ᵽ��Ŀ���ļ���API�б�
					pCrc32List = (__dword *)&(pDisAthConfigure->dwTargetProtectApiCrc32List);
					pCountOut = (__integer *)&(pDisAthConfigure->iTargetProtectApiCrc32Count);
				} else if (_tcsicmp(pArgv[i], _T("/pda")) == 0) {
					// Ҫ���Ᵽ���ǵ�API�б�
					pCrc32List = (__dword *)&(pDisAthConfigure->dwDISProtectApiCrc32List);
					pCountOut = (__integer *)&(pDisAthConfigure->iDISProtectApiCrc32Count);
				} else {
					Usage();
					return FALSE;
				}

				// ָ�����
				i++;

				// ȡ��Ҫ������API����, ����Ҫ�趨�ĵ�ַ
				UnicodeToAnsi(pArgv[i], _tcslen(pArgv[i]), szString, 2048);
				token = strtok(szString, seps);
				while (token) {
					pCrc32List[iCount++] = crc32(token, strlen(token));
					token = (__char *)strtok(NULL, seps);
				}/* end while */
				*pCountOut = iCount;

			} break;
			case _T('i'):
			case _T('I'): {
				if (_tcsicmp(pArgv[i], _T("/itpc")) == 0) {
					pDisAthConfigure->bProtectMyIAT = FALSE;
					pDisAthConfigure->bProtectTargetIAT = FALSE;
				} else if (_tcsicmp(pArgv[i], _T("/itksod")) == 0) {
					pDisAthConfigure->XFileAttachInfo.bKeepSameOrigImpLib = TRUE;
				} else {
					Usage();
					return FALSE;
				}
			} break;
			default: {
				Usage();
				return FALSE;
			}}/* end switch */
		}  else {
			Usage();
			return FALSE;
		}/* end else */
	}/* end for */

	// �趨���ýṹ
	pDisAthConfigure->bIfXFileExistFailed = TRUE;//���Ŀ���ļ��Ѿ�����X�ļ������X�ļ�ʧ��
	pDisAthConfigure->szResourceType = (__tchar *)_T("BIN");
	pDisAthConfigure->dwDISResourceKey = __YIMPTBL_DECRYPT_KEY__;
	pDisAthConfigure->wResourceID = IDR_YIMPORTTABLE;
	pDisAthConfigure->XFileAttachInfo.bSetAllSectionCanWrite = TRUE;
	pDisAthConfigure->XFileAttachInfo.dwDNA = __Y_IMPORT_TABLE_DNA__;

	// PE�ڽ���
	if (!pDisAthConfigure->XFileAttachInfo.szSectionName) {
		pDisAthConfigure->XFileAttachInfo.szSectionName = (__char *)__logic_new_size__(0x10);
		if (strlen(pDisAthConfigure->XFileAttachInfo.szSectionName) == 0)
			strcpy(pDisAthConfigure->XFileAttachInfo.szSectionName, __DEF_XIMPORT_TABLE_SECTION_NAME__);
	}

	// X�ڽ���
	pDisAthConfigure->XFileAttachInfo.szXSectionName = (__char *)__logic_new_size__(0x10);
	strcpy(pDisAthConfigure->XFileAttachInfo.szXSectionName, __DEF_XIMPORT_TABLE_X_SECTION_NAME__);

	// Ŀ���ļ�·��
	pDisAthConfigure->XFileAttachInfo.szTargetFilePath = pArgv[i];

	return TRUE;
}

DISATH_CONFIGURE g_DisAthConfigure = {0};
XIMPORT_TABLE_CONFIGURE g_xImportTableConfigure = {0};
__integer _tmain(__integer iArgc, __tchar *pArgv[]) {
	__tchar szTargetFile[MAX_PATH] = {0};
	__dword dwRet = 0;

	// ���������в��������ýṹ
	if (!HandleArguments(iArgc, pArgv, &g_DisAthConfigure, &g_xImportTableConfigure))
		return -1;

	// �Ƿ���Ҫ����
	{
		__tchar szBackUpFilePath[MAX_PATH] = {0};
		_stprintf(szBackUpFilePath, _T("%s.bk"), g_DisAthConfigure.XFileAttachInfo.szTargetFilePath);
		if (g_xImportTableConfigure.bBackupOrigFile)
			CopyFile(g_DisAthConfigure.XFileAttachInfo.szTargetFilePath, szBackUpFilePath, FALSE);
	}

	// ����
	dwRet = DISAthAttach(&g_DisAthConfigure);
	if (__DIS_SUCCESS__(dwRet)) {
		printf("^_^ happy now\r\n");
	} else {
		printf("-_- not happy\r\n");
	}

	// ����DLL
#if defined(__TEST_DECRYPT_IMPORTTABLE_IN_DLL__)
	{
		HMODULE hTestDll = LoadLibrary((LPCTSTR)szTargetFile);
	}
#endif

	return 0;
}

__dword __INTERNAL_FUNC__ DISAthStep0(PDISATH_CONFIGURE pConfigure) {
	_tprintf(_T("Start protect %s import table\r\n"), pConfigure->XFileAttachInfo.szTargetFilePath);
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep1(PXFILE_ATTACH_INFO pXFileAttachInfo, PDISATH_CONFIGURE pConfigure) {
	printf("[+] Mapping target file success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep2(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_SECTION_HEADER pXFileNtSectionHeader) {
	printf("[+] Add X File to target success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep3(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader) {
	printf("[+] Get target NT header success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep4(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_SECTION_HEADER pNewXFileSection) {
	printf("[+] Predict new X section success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep5(PXFILE_ATTACH_INFO pXFileAttachInfo, __address addrDISImageBaseInTarget, __offset ofDISOffsetByXSectionBody) {
	printf("[+] Get DIS in target ImageBase success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep6(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize) {
	printf("[+] Mapping DIS success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep7(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize, __integer iDISSizeOfImage, __offset ofDISOffsetByXSectionBody, __integer *piXSectionSize) {
	printf("[+] Calc new X section size success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep8(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize, PIMAGE_SECTION_HEADER pXFileNtSectionHeader, PXFILE_SECTION_HEADER pXSection, __dword dwDNA, __offset ofDISConfigureRVA) {
	printf("[+] Add new X section success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep9(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader) {
	printf("[+] Reget target NT header success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep10(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __memory pDISInTarget, __integer iDISSizeOfImage) {
	printf("[+] Copy DIS to target on memory alignemtn success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep11(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, __offset ofDISNewAddressTableRva) {
	printf("Already encrypt DIS import table\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep12(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceDISImportTableAddress) {
	printf("[+] Encrypt DIS import table success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep13(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, __offset ofTargetNewAddressTableRva) {
	printf("Already encrypt target import table\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep14(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceTargetImportTableAddress) {
	printf("[+] Encrypt target import table success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep15(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	printf("Already modify target entry address\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep16(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader) {
	printf("[+] Modify target entry address success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}

#if defined(__TEST_DECRYPT_IMPORTTABLE__)
// ���Խ��������
__void __INTERNAL_FUNC__ TestDecryptImportTable(PXFILE_ATTACH_INFO pXFileAttachInfo, PDIS_CONFIGURE pDISConfigure) {
	POWERDECRYPTIMPORTTABLE_ARG PowerDecryptTargetImportTableArg = {0};
	PIMPORT_PROTECT pTargetImportProtect = NULL;
	__memory pNew = pXFileAttachInfo->pMem;
	PIMAGE_NT_HEADERS pNtHdr = GetNtHeader(pNew); 
	__memory pTo = (__memory)VirtualAlloc((LPVOID)NULL, pNtHdr->OptionalHeader.SizeOfImage, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// �ض�λ
	CopyMemToMemBySecAlign(pNew, pTo, pNtHdr->OptionalHeader.SizeOfImage);
	BaseRelocation(pTo, pNtHdr->OptionalHeader.ImageBase, (__address)pTo, FALSE);

	// ���������
	pTargetImportProtect = &(pDISConfigure->TargetImportProtect);
	pTargetImportProtect->addrImageBase = (__address)pTo;

	// �޸�Ŀ�����������
	PowerDecryptTargetImportTableArg.pImportProtect = pTargetImportProtect;
	PowerDecryptTargetImportTableArg.pAddThunkCodeStub = NULL;
	PowerDecryptImportTable(&PowerDecryptTargetImportTableArg);

	VirtualFree(pTo, pNtHdr->OptionalHeader.SizeOfImage, MEM_DECOMMIT);
	return;
}
#endif

__dword __INTERNAL_FUNC__ DISAthStep17(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	printf("[+] Modify target some PE infos success\r\n");
	
#if defined(__TEST_DECRYPT_IMPORTTABLE__)
	// ���Խ��������
	InitImportProtecter();
	TestDecryptImportTable(pXFileAttachInfo, pDISConfigure);
#endif

	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep18(PDISATH_CONFIGURE pConfigure) {
	printf("[+] Unmaping target success\r\n");
	return __DISATH_ERROR_SUCCESS__;
}
