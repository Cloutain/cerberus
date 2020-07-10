__bool __API__ DISEntry() {
	__memory pHostImageBase = NULL;
	__memory pTargetImageBase = NULL;
	__memory pDISImageBase = NULL;

	PXFILE_HEADER pXFileHeader = NULL;
	PXFILE_SECTION_HEADER pXFileSectionHeader = NULL;
	PXFILE_SECTION_HEADER pXFileFatherSectionHeader = NULL;
	PIMAGE_SECTION_HEADER pNtSectionHeader = NULL;

	POWERDECRYPTIMPORTTABLE_ARG PowerDecryptDISImportTableArg = {0};
	POWERDECRYPTIMPORTTABLE_ARG PowerDecryptTargetImportTableArg = {0};

	__address addrDISOrigImageBase = 0;
	__address addrOrigEntryAddress = 0;

	__dword dwStepRet = __DIS_ERROR_SUCCESS__;

	PDIS_CONFIGURE pDISConfigure = NULL;

	// ����0,���������Լ�ʵ��
_call_back_0:
	DISStep0(0, 0, 0);

	// �ҵ�Ŀ���ļ���ӳ�����ַ
	pHostImageBase = (__memory)__GetNowImageBase__();

_call_back_1:
	dwStepRet = DISStep1((__dword)pHostImageBase, 0, NULL);
	__HandleDISStepReturnValue__(1, dwStepRet);

	// �ƶ���ĩβ��
	pNtSectionHeader = GetTailSection(pHostImageBase);

_call_back_2:
	dwStepRet = DISStep2((__dword)pHostImageBase, 0, NULL, pNtSectionHeader);
	__HandleDISStepReturnValue__(2, dwStepRet);

	// �жϴ��Ƿ���X�ļ�
	pXFileHeader = (PXFILE_HEADER)(pHostImageBase + pNtSectionHeader->VirtualAddress);
	if (!__IS_XFILE__(pXFileHeader->dwSign))
		return FALSE;

_call_back_3:
	{
		__dword dwMyDNA = __DIS_KERNEL_DNA__;//Ĭ�ϵ�DNAֵ
		dwStepRet = DISStep3((__dword)pHostImageBase, 0, NULL, pNtSectionHeader, pXFileHeader, &dwMyDNA);
		__HandleDISStepReturnValue__(3, dwStepRet);

		// ͨ�������DNA�ҵ��Լ�
		pXFileSectionHeader = XFileGetXFileSectionHeaderByDNA(pHostImageBase, pXFileHeader, dwMyDNA, FALSE, &pXFileFatherSectionHeader);
		if (!pXFileSectionHeader)
			return FALSE;
	}

_call_back_4:
	dwStepRet = DISStep4((__dword)pHostImageBase, 0, NULL, pNtSectionHeader, pXFileHeader, pXFileSectionHeader, pXFileFatherSectionHeader);
	__HandleDISStepReturnValue__(4, dwStepRet);

	// ��ȡX�ڵ���չ����
	pDISConfigure = (PDIS_CONFIGURE)(pHostImageBase + pXFileSectionHeader->dwCustomDataRVA);

	// ȷ���Ƿ��ǵ�һ������
	if (!pDISConfigure->bFirstRun) {
		goto _call_back_11;
	}
	pDISConfigure->bFirstRun = FALSE;//���ñ�־Ϊ�ǵ�һ������

_call_back_5:
	dwStepRet = DISStep5((__dword)pHostImageBase, 0, NULL, pNtSectionHeader, pXFileHeader, pXFileSectionHeader, pXFileFatherSectionHeader, pDISConfigure);
	__HandleDISStepReturnValue__(5, dwStepRet);

	// ��ȡDIS����ĵ�ǰ����ַ
	pDISImageBase = pHostImageBase + pXFileSectionHeader->dwVirtualAddress;
	pDISImageBase = (__memory)AligImageBase((__address)pDISImageBase);
	addrDISOrigImageBase = GetNtHeader(pDISImageBase)->OptionalHeader.ImageBase;//ԭ����ַ

_call_back_6:
	dwStepRet = DISStep6((__dword)pHostImageBase, 0, NULL, pNtSectionHeader, pXFileHeader, pXFileSectionHeader, pXFileFatherSectionHeader, pDISConfigure, pDISImageBase, addrDISOrigImageBase);
	__HandleDISStepReturnValue__(6, dwStepRet);

	// ���¶�λ����
	if (!BaseRelocation(pDISImageBase, addrDISOrigImageBase, (__address)pDISImageBase, FALSE))
		return FALSE;

_call_back_7:
	dwStepRet = DISStep7(pDISConfigure, pDISImageBase, addrDISOrigImageBase);
	__HandleDISStepReturnValue__(7, dwStepRet);

	// �Ƿ�����Լ��������
	if (pDISConfigure->bProtectMyIAT) {
		// ��ʼ�����������
		InitImportProtecter();
		// �����޶�DIS����Ļ���ַ�����뱣���ṹ��
		pDISConfigure->DISImportProtect.addrImageBase = (__address)pDISImageBase;

		// �޸�����������
		PowerDecryptDISImportTableArg.pImportProtect = &(pDISConfigure->DISImportProtect);
		PowerDecryptDISImportTableArg.pAddThunkCodeStub = DISAddThunkCodeStub;
		if (!PowerDecryptImportTable(&PowerDecryptDISImportTableArg))
			return FALSE;
	}

_call_back_8:
	dwStepRet = DISStep8();
	__HandleDISStepReturnValue__(8, dwStepRet);

	// ��DIS�Ƿ񱣻���������DIS,��ȡ���ı�������Ļ���ַ
	if (pXFileFatherSectionHeader) {
		pTargetImageBase = pHostImageBase + pXFileFatherSectionHeader->dwVirtualAddress + pXFileFatherSectionHeader->ofMemImageBaseOffset;
	} else {
		pTargetImageBase = pHostImageBase;
	}

_call_back_9:
	dwStepRet = DISStep9(pTargetImageBase);
	__HandleDISStepReturnValue__(9, dwStepRet);

	// �Ƿ����Ŀ�����������
	if (pDISConfigure->bProtectTargetIAT) {
		// ��ʼ�����������
		InitImportProtecter();

		// �����޶�DIS�������ĳ���������
		pDISConfigure->TargetImportProtect.addrImageBase = (__address)pTargetImageBase;

		// �޸�Ŀ�����������
		PowerDecryptTargetImportTableArg.pImportProtect = &(pDISConfigure->TargetImportProtect);
		PowerDecryptTargetImportTableArg.pAddThunkCodeStub = DISAddThunkCodeStub;
		if (!PowerDecryptImportTable(&PowerDecryptTargetImportTableArg))
			return FALSE;
	}
_call_back_10:
	dwStepRet = DISStep10();
	__HandleDISStepReturnValue__(10, dwStepRet);

	// ����ԭʼ��ڵ�
_call_back_11:
	addrOrigEntryAddress = (__address)(pHostImageBase + pDISConfigure->ofOrigEntryRVA);
	dwStepRet = DISStep11(addrOrigEntryAddress);
	__HandleDISStepReturnValue__(11, dwStepRet);
	__asm {
		push addrOrigEntryAddress
		ret
	}

	// ��ָ��

	return TRUE;
}
