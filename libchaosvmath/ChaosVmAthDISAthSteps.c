__dword __INTERNAL_FUNC__ DISAthStep0(PDISATH_CONFIGURE pConfigure) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep1(PXFILE_ATTACH_INFO pXFileAttachInfo, PDISATH_CONFIGURE pConfigure) {
	PCHAOSVMATH_RET_INFO pRetInfo = NULL;

	// ��ʼ������������������ṹ
	g_pChaosVmAth = (PCHAOSVMATH)__logic_new__(CHAOSVMATH, 1);//���������������
	if (!g_pChaosVmAth) {
		// ������������������ʧ��
	}
	__logic_memset__(g_pChaosVmAth, 0, sizeof(CHAOSVMATH));
	g_pChaosVmAth->pConfigure = (PCHAOSVMATH_CONFIGURE)(pConfigure->pUserData0);//��ǰ�����ýṹָ��
	g_pChaosVmAth->pXFileAttachInfo = pXFileAttachInfo;

	// ��¼Ŀ�����ԭʼ��Ϣ
	pRetInfo = (PCHAOSVMATH_RET_INFO)(pConfigure->pUserData1);
	if (pRetInfo) {
		pRetInfo->iTargetCodeSectionIndex = GetEntrySectionIndex(pXFileAttachInfo->pMem);
	}

	// �����ļ�����
	if (!pXFileAttachInfo->bImageBaseRelocate)
		g_pChaosVmAth->FileType = CFT_EXE;
	else
		g_pChaosVmAth->FileType = CFT_DLL;

	// ������ڷ���ģʽ�»����ڸ�Ⱦģʽ��ָ��ʹ���ֽ����ļ�,�����һ���ֽ����ļ��ṹ
	if ((g_pChaosVmAth->pConfigure->Mode == CRM_EMULATION) || 
		(g_pChaosVmAth->pConfigure->bUseByteCodeFile) && (g_pChaosVmAth->pConfigure->Mode == CRM_INFECT)) {
		g_pChaosVmAth->pByteCodeFile = ChaosVmByteCodeFileGenByteCodeFile();
		if (!(g_pChaosVmAth->pByteCodeFile)) {
			// �����ֽ����ļ��ṹʧ��
		}
	}

	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep2(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_SECTION_HEADER pXFileNtSectionHeader) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep3(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep4(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_SECTION_HEADER pNewXFileSection) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep5(PXFILE_ATTACH_INFO pXFileAttachInfo, __address addrDISImageBaseInTarget, __offset ofDISOffsetByXSectionBody) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep6(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize) {
	g_pChaosVmAth->pChaosVm = pDIS;
	g_pChaosVmAth->iChaosVmSize = iDISSize;

	// ���︴��һ����������,����δ����֧��,���ͷ�pChaosVmAth�ṹʱ�ͷ�
	g_pChaosVmAth->pOrigChaosVm = (__memory)__logic_new_size__(g_pChaosVmAth->iChaosVmSize);
	if (!g_pChaosVmAth->pOrigChaosVm)
		return __DISATH_ERROR_STEP_ALLOC__;
	__logic_memcpy__(g_pChaosVmAth->pOrigChaosVm, pDIS, iDISSize);

	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep7(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize, __integer iDISSizeOfImage, __offset ofDISOffsetByXSectionBody, __integer *piXSectionSize) {
	__integer iChaosVmDataSize = 0, iChaosVmRuntimeSize = 0, iPowerProtecterEncryptProcedureSize = 0;
	__integer iXSectionSize = 0;
	__memory pMem = NULL;

	pMem = pXFileAttachInfo->pMem;

	// ��������������̬���ݵĳ���,�Լ�������������̬���ݵĳ���
	iChaosVmDataSize = ChaosVmAthCountVmpProcedureTotalSize(g_pChaosVmAth->pConfigure);//����������������򳤶�
	g_pChaosVmAth->iChaosVmDataSize = iChaosVmDataSize;
	iChaosVmRuntimeSize = g_pChaosVmAth->pConfigure->iProcedureCount * sizeof(CHAOSVM_RUNTIME);//����������������������л���ʱ�ṹ���е��ܴ�С
	g_pChaosVmAth->iChaosVmRuntimeSize = iChaosVmRuntimeSize;
	iPowerProtecterEncryptProcedureSize = PowerProtecterAthCountAllEncryptProceduresSize(pMem, &(g_pChaosVmAth->pConfigure->PowerProtecterAttachConfigure.PowerProtecterProcedure));
	g_pChaosVmAth->iPowerProtecterEncryptProceduresSize = iPowerProtecterEncryptProcedureSize;

	// ���û����������ӳ���С
	g_pChaosVmAth->iChaosVmSizeOfImage = iDISSizeOfImage;

	// �õ��������������������X�ڵ�ƫ��
	g_pChaosVmAth->ofChaosVmInTargetOffsetByXSectionBody = ofDISOffsetByXSectionBody;

	// �����������������X������ĳ���
	iXSectionSize = *piXSectionSize;
	iXSectionSize = iXSectionSize + iChaosVmDataSize + iChaosVmRuntimeSize + iPowerProtecterEncryptProcedureSize;
	g_pChaosVmAth->iChaosVmSectionSize = iXSectionSize;
	*piXSectionSize = iXSectionSize;

	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep8(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize, PIMAGE_SECTION_HEADER pXFileNtSectionHeader, PXFILE_SECTION_HEADER pXSection, __dword dwDNA, __offset ofDISConfigureRVA) {
	__offset ofChaosVmInTargetOffsetByXSectionBody = 0;
	__integer iChaosVmSizeOfImage = 0, iChaosVmDataSize = 0, iChaosVmRuntimeSize = 0;
	PXFILE_SECTION_HEADER pChaosVmXSectionHeader = NULL;
	__memory pTarget = NULL;
	PIMAGE_NT_HEADERS pTargetNtHdr = NULL;
	PIMAGE_NT_HEADERS pChaosVmNtHdr = NULL;
	__address addrTargetImageBase = 0, addrChaosVmOrigImageBase = 0, addrChaosVmImageBaseInTarget = 0;

	__offset ofChaosVmInTargetRVA = 0, ofChaosVmInTargetOffset = 0;
	__offset ofChaosVmDataRvaByTarget = 0, ofChaosVmDataOffsetByTarget = 0;
	__offset ofChaosVmRuntimeRvaByTarget = 0, ofChaosVmRuntimeOffsetByTarget = 0;
	__offset ofPowerProtecterEncryptProcedureRvaByTarget = 0, ofPowerProtecterEncryptProcedureOffsetByTarget = 0;

	// ��ȡ������������ݳ���
	ofChaosVmInTargetOffsetByXSectionBody = g_pChaosVmAth->ofChaosVmInTargetOffsetByXSectionBody;
	iChaosVmSizeOfImage = g_pChaosVmAth->iChaosVmSizeOfImage;
	iChaosVmDataSize = g_pChaosVmAth->iChaosVmDataSize;
	iChaosVmRuntimeSize = g_pChaosVmAth->iChaosVmRuntimeSize;
	pChaosVmXSectionHeader = &(g_pChaosVmAth->ChaosVmXSectionHeader);
	pTarget = g_pChaosVmAth->pXFileAttachInfo->pMem;

	// ��ȡĿ���ļ����ڴ�ӳ�����ַ
	pTargetNtHdr = GetNtHeader(pTarget);
	addrTargetImageBase = pTargetNtHdr->OptionalHeader.ImageBase;
	g_pChaosVmAth->addrTargetImageBase = addrTargetImageBase;

	// ��ȡ���������ԭʼ�ڴ����ַ
	pChaosVmNtHdr = GetNtHeader(pDIS);
	addrChaosVmOrigImageBase = pChaosVmNtHdr->OptionalHeader.ImageBase;
	g_pChaosVmAth->addrChaosVmOrigImageBase = addrChaosVmOrigImageBase;

	// ��ȡ�����������Ŀ���ļ��е��ڴ����ַ
	addrChaosVmImageBaseInTarget = addrTargetImageBase + pXSection->dwVirtualAddress + ofChaosVmInTargetOffsetByXSectionBody;//����õ���X�����ڴ��еĵ�ַ
	g_pChaosVmAth->addrChaosVmImageBaseInTarget = addrChaosVmImageBaseInTarget;

	//////////////////////////////////////////////////////////////////////////
	// �õ��������������������Ŀ���ļ���RVA
	// �õ��������������������Ŀ���ļ���ƫ��
	ofChaosVmInTargetRVA = pXSection->dwVirtualAddress + ofChaosVmInTargetOffsetByXSectionBody;
	ofChaosVmInTargetOffset = pXSection->dwPointerToRawData + ofChaosVmInTargetOffsetByXSectionBody;
	g_pChaosVmAth->ofChaosVmInTargetRVA = ofChaosVmInTargetRVA;
	g_pChaosVmAth->ofChaosVmInTargetOffset = ofChaosVmInTargetOffset;

	//////////////////////////////////////////////////////////////////////////
	// �õ��������������������Ŀ���ļ�ͷ��RVA
	// �õ����������������������������Ŀ���ļ���ƫ��
	ofChaosVmDataRvaByTarget = pXSection->dwVirtualAddress + ofChaosVmInTargetOffsetByXSectionBody + iChaosVmSizeOfImage;
	ofChaosVmDataOffsetByTarget = pXSection->dwPointerToRawData + ofChaosVmInTargetOffsetByXSectionBody + iChaosVmSizeOfImage;
	g_pChaosVmAth->ofChaosVmDataRvaByTarget = ofChaosVmDataRvaByTarget;
	g_pChaosVmAth->ofChaosVmDataOffsetByTarget = ofChaosVmDataOffsetByTarget;

	//////////////////////////////////////////////////////////////////////////
	// �õ��������������ʱ������Ŀ���ļ�ͷ��RVA
	// �õ����������������ʱ��������������Ŀ���ļ���ƫ��
	ofChaosVmRuntimeRvaByTarget = ofChaosVmDataRvaByTarget + iChaosVmDataSize;
	ofChaosVmRuntimeOffsetByTarget = ofChaosVmDataOffsetByTarget + iChaosVmDataSize;
	g_pChaosVmAth->ofChaosVmRuntimeRvaByTarget = ofChaosVmRuntimeRvaByTarget;
	g_pChaosVmAth->ofChaosVmRuntimeOffsetByTarget = ofChaosVmRuntimeOffsetByTarget;

	//////////////////////////////////////////////////////////////////////////
	// �õ������������ܺ����ṹ��Ŀ���ļ�ͷ��RVA
	// �õ������������ܺ����ṹ����������Ŀ���ļ���ƫ��
	ofPowerProtecterEncryptProcedureRvaByTarget = ofChaosVmRuntimeRvaByTarget + iChaosVmRuntimeSize;
	ofPowerProtecterEncryptProcedureOffsetByTarget = ofChaosVmRuntimeOffsetByTarget + iChaosVmRuntimeSize;
	g_pChaosVmAth->ofPowerProtecterEncryptProceduresRvaByTarget = ofPowerProtecterEncryptProcedureRvaByTarget;
	g_pChaosVmAth->ofPowerProtecterEncryptProceduresOffsetByTarget = ofPowerProtecterEncryptProcedureOffsetByTarget;
	//////////////////////////////////////////////////////////////////////////

	// ���ƻ������������X�ڵĽ�ͷ
	__logic_memcpy__(pChaosVmXSectionHeader, pXSection, sizeof(XFILE_SECTION_HEADER));

	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep9(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep10(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __memory pDISInTarget, __integer iDISSizeOfImage) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep11(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, __offset ofDISNewAddressTableRva) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep12(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceDISImportTableAddress) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep13(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, __offset ofTargetNewAddressTableRva) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep14(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceTargetImportTableAddress) {
	return __DISATH_ERROR_SUCCESS__;
}

__INLINE__ __void __INTERNAL_FUNC__ ClearChaosVmOpcodeDispatch() {
	// ���ȫ�ֱ���
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_FF, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_FE, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_C6, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_C7, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_F6, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_F7, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_80, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_81, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_83, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_8F, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_C0, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_C1, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_D0, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_D1, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_D2, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForOne_D3, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOpcodeExtensionTableForTwo_0FBA, 0, sizeof(PARGS_DISPATCH_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pOneByteOpcodeTable, 0, sizeof(ONE_OPCODE_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pTwoByteOpcodeTable, 0, sizeof(TWO_BYTE_OPCODE_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pEFlagContionTable, 0, sizeof(EFLAG_CONDITION_TABLE_ENTRY));
	__logic_memset__(g_pChaosVmAth->pModRMAnalyseRoutineTable, 0, sizeof(MODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY));
}

__INLINE__ __void __INTERNAL_FUNC__ GetChaosVmProcAddress() {
	HMODULE hChaosVmInTarget = (HMODULE)(g_pChaosVmAth->pChaosVmInTarget);

	// �˺���ֻ�ڸ�Ⱦģʽ��������
	if (g_pChaosVmAth->pConfigure->Mode == CRM_INFECT) {
		/*
		 * ��ȡ����������ӿں���
		 */
		g_pChaosVmAth->pChaosVmEntry = (FPChaosVmEntry)xGetProcAddress(hChaosVmInTarget, "ChaosVmEntry");
		g_pChaosVmAth->ofChaosVmEntry = (__offset)((__memory)(g_pChaosVmAth->pChaosVmEntry) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pChaosVmInit = (FPChaosVmInit)xGetProcAddress(hChaosVmInTarget, "ChaosVmInit");
		g_pChaosVmAth->ofChaosVmInit = (__offset)((__memory)(g_pChaosVmAth->pChaosVmInit) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pChaosVmRuntimeList = (PCHAOSVM_RUNTIME)xGetProcAddress(hChaosVmInTarget, "g_pChaosVmRuntimeList");
		g_pChaosVmAth->ofChaosVmRuntimeList = (__offset)((__memory)(g_pChaosVmAth->pChaosVmRuntimeList) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->piVmpProcedureCount = (__integer *)xGetProcAddress(hChaosVmInTarget, "g_iVmpProcedureCount");
		g_pChaosVmAth->ofVmpProcedureCount = (__offset)((__memory)(g_pChaosVmAth->piVmpProcedureCount) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pChaosVmConfigure = (PCHAOSVM_CONFIGURE)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmConfigure");
		g_pChaosVmAth->ofChaosVmConfigure = (__offset)((__memory)(g_pChaosVmAth->pChaosVmConfigure) - (__memory)hChaosVmInTarget);

		g_pChaosVmAth->pOpcodeExtensionTableForOne_FF = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_FF");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_FF = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_FF) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_FE = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_FE");
		g_pChaosVmAth->ofOpcodeExtensioTableForOne_FE = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_FE) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_C6 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_C6");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_C6 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_C6) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_C7 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_C7");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_C7 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_C7) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_F6 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_F6");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_F6 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_F6) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_F7 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_F7");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_F7 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_F7) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_80 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_80");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_80 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_80) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_81 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_81");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_81 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_81) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_83 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_83");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_83 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_83) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_8F = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_8F");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_8F = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_8F) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_C0 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_C0");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_C0 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_C0) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_C1 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_C1");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_C1 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_C1) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_D0 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_D0");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_D0 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_D0) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_D1 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_D1");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_D1 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_D1) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_D2 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_D2");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_D2 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_D2) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForOne_D3 = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForOne_D3");
		g_pChaosVmAth->ofOpcodeExtensionTableForOne_D3 = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForOne_D3) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOpcodeExtensionTableForTwo_0FBA = (PPARGS_DISPATCH_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOpcodeExtensionTableForTwo_0FBA");
		g_pChaosVmAth->ofOpcodeExtensionTableForTwo_0FBA = (__offset)((__memory)(g_pChaosVmAth->pOpcodeExtensionTableForTwo_0FBA) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pOneByteOpcodeTable = (PONE_OPCODE_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefOneByteOpcodeTable");
		g_pChaosVmAth->ofOneByteOpcodeTable = (__offset)((__memory)(g_pChaosVmAth->pOneByteOpcodeTable) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pTwoByteOpcodeTable = (PTWO_BYTE_OPCODE_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefTwoByteOpcodeTable");
		g_pChaosVmAth->ofTwoByteOpcodeTable = (__offset)((__memory)(g_pChaosVmAth->pTwoByteOpcodeTable) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pEFlagContionTable = (PEFLAG_CONDITION_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefEFlagContionTable");
		g_pChaosVmAth->ofEFlagContionTable = (__offset)((__memory)(g_pChaosVmAth->pEFlagContionTable) - (__memory)hChaosVmInTarget);
		g_pChaosVmAth->pModRMAnalyseRoutineTable = (PMODRM_BYTE_ANALYSE_ROUTINE_TABLE_ENTRY)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmCpuDefModRMAnalyseRoutineTable");
		g_pChaosVmAth->ofModRMAnalyseRoutineTable = (__offset)((__memory)(g_pChaosVmAth->pModRMAnalyseRoutineTable) - (__memory)hChaosVmInTarget);

		//////////////////////////////////////////////////////////////////////////
		// �����������
		g_pChaosVmAth->pPowerProtecterEncryptProcedures = (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)xGetProcAddress(hChaosVmInTarget, "g_pPowerProtecterEncryptProcedures");
		g_pChaosVmAth->ofPowerProtecterEncryptProcedures = (__offset)((__memory)(g_pChaosVmAth->pPowerProtecterEncryptProcedures) - (__memory)hChaosVmInTarget);

		g_pChaosVmAth->piPowerProtecterEncryptProceduresCount = (__integer *)xGetProcAddress(hChaosVmInTarget, "g_iPowerProtecterEncryptProcedures");
		g_pChaosVmAth->ofPowerProtecterEncryptProceduresCount = (__offset)((__memory)(g_pChaosVmAth->piPowerProtecterEncryptProceduresCount) - (__memory)hChaosVmInTarget);
	} else if (g_pChaosVmAth->pConfigure->Mode == CRM_EMULATION) {
		g_pChaosVmAth->pChaosVmLdrConfigure = (PCHAOSVM_LOADER_CONFIGURE)xGetProcAddress(hChaosVmInTarget, "g_ChaosVmLdrConfigure");
		g_pChaosVmAth->ofChaosVmLdrConfigure = (__offset)((__memory)(g_pChaosVmAth->pChaosVmConfigure) - (__memory)hChaosVmInTarget);
	} else if (g_pChaosVmAth->pConfigure->Mode == CRM_EMULATION_SYS) {
		;
	} else if (g_pChaosVmAth->pConfigure->Mode == CRM_NETWORK) {
		;
	} else {
		;
	}/* end else */
}

/*
 * ÿ��ģʽ�µĲ���15
 */
__INLINE__ __dword __INTERNAL_FUNC__ DISAthStep15InInfectMode(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	// �����趨
	g_pChaosVmAth->pModRmSibExchange = NULL;
	g_pChaosVmAth->pOpcodeExchange = NULL;
	g_pChaosVmAth->pVmHash = ChaosVmAthHash;
	g_pChaosVmAth->pVmEncrypt = ChaosVmAthEncrypt;
	g_pChaosVmAth->pVmInstRemainEncrypt = ChaosVmAthInstRemainEncrypt;
	g_pChaosVmAth->pChaosVmInTarget = pDISInTarget;

	// ��ȡ�������������������
	GetChaosVmProcAddress();

	// ���ñ�������������Ϊ0
	*(g_pChaosVmAth->piVmpProcedureCount) = 0;
	*(g_pChaosVmAth->piPowerProtecterEncryptProceduresCount) = 0;

	/*
	 * ���û��������
	 */
	g_pChaosVmAth->pChaosVmConfigure->bUseByteCodeFile = g_pChaosVmAth->pConfigure->bUseByteCodeFile;
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmConfigure->szChaosVmByteCodeFileName, g_pChaosVmAth->pConfigure->szChaosVmByteCodeFileName);//�����ʹ��bytecode�ļ���Ϊ��
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmConfigure->szMessageBoxTitle, g_pChaosVmAth->pConfigure->szMessageBoxTitle);
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmConfigure->szMessageBoxContext, g_pChaosVmAth->pConfigure->szMessageBoxOnByteCodeFileNotExist);
	g_pChaosVmAth->pChaosVmConfigure->dwMessageStyle = g_pChaosVmAth->pConfigure->dwMessageStyle;

	g_pChaosVmAth->pChaosVmConfigure->FileType = g_pChaosVmAth->FileType;
	g_pChaosVmAth->pChaosVmConfigure->dwTmp = 0;

	g_pChaosVmAth->pChaosVmConfigure->PowerProtecterConfigure.iTmp = 0;


	// ɾ������������ĵ��Է��ű�
	DeleteDataDirectoryObjectOnMemMap(g_pChaosVmAth->pChaosVmInTarget, IMAGE_DIRECTORY_ENTRY_DEBUG);

	/*
	 * ���û�����Ϣ
	 */
	g_pChaosVmAth->pChaosVm = g_pChaosVmAth->pOrigChaosVm;//���ø���ָ��
	g_pChaosVmAth->pProcedureWriteTo = g_pChaosVmAth->pXFileAttachInfo->pMem + g_pChaosVmAth->ofChaosVmDataOffsetByTarget;
	//*(__address *)(g_pChaosVmAth->pChaosVmRuntimeList) = g_pChaosVmAth->addrTargetImageBase + g_pChaosVmAth->ofChaosVmRuntimeRvaByTarget;//�������л���ʱ���ڴ��ַ
	*(__offset *)(g_pChaosVmAth->pChaosVmRuntimeList) = g_pChaosVmAth->ofChaosVmRuntimeRvaByTarget;//�������л���ʱ���ڴ��ַRVA
	g_pChaosVmAth->pChaosVmRuntimeList = (PCHAOSVM_RUNTIME)(g_pChaosVmAth->pXFileAttachInfo->pMem + g_pChaosVmAth->ofChaosVmRuntimeOffsetByTarget);
	*(__offset *)(g_pChaosVmAth->pPowerProtecterEncryptProcedures) = g_pChaosVmAth->ofPowerProtecterEncryptProceduresRvaByTarget;//���������������ܺ����ṹ���ڴ��ַRVA
	g_pChaosVmAth->pPowerProtecterEncryptProcedures = (PPOWER_PROTECTER_ENCRYPT_PROCEDURE)(g_pChaosVmAth->pXFileAttachInfo->pMem + g_pChaosVmAth->ofPowerProtecterEncryptProceduresOffsetByTarget);

	// �������ýṹ��ѡ���������ļ�ָ��
	ChaosVmAthSetFileAddressToProcedureList(g_pChaosVmAth->pXFileAttachInfo->pMem, g_pChaosVmAth->pConfigure);

	// ����������������Ĳ���
	PowerProtecterAthSetArguments(&(g_pChaosVmAth->pConfigure->PowerProtecterAttachConfigure));

	// ������������
	PowerProtectAthGo(g_pChaosVmAth->pXFileAttachInfo->pMem, &(g_pChaosVmAth->pConfigure->PowerProtecterAttachConfigure.PowerProtecterProcedure), g_pChaosVmAth->pPowerProtecterEncryptProcedures);

	// ��������
	if (!ChaosVmAthSelected(g_pChaosVmAth))
		return __DISATH_ERROR_ERROR__;

	// ������������ȫ�ֺ���
	ClearChaosVmOpcodeDispatch();

	return __DISATH_ERROR_SUCCESS__;
}

__INLINE__ __dword __INTERNAL_FUNC__ DISAthStep15InEmulationMode(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	// �����趨
	g_pChaosVmAth->pModRmSibExchange = NULL;
	g_pChaosVmAth->pOpcodeExchange = NULL;
	g_pChaosVmAth->pVmHash = ChaosVmAthHash;
	g_pChaosVmAth->pVmEncrypt = ChaosVmAthEncrypt;
	g_pChaosVmAth->pVmInstRemainEncrypt = ChaosVmAthInstRemainEncrypt;
	g_pChaosVmAth->pChaosVmInTarget = pDISInTarget;

	// ��ȡ�������������������
	GetChaosVmProcAddress();

	// ���ü����������ýṹ
	g_pChaosVmAth->pChaosVmLdrConfigure->bUseChaosVmEmulationInResource = g_pChaosVmAth->pConfigure->bUseChaosVmEmulationInResource;
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmLdrConfigure->szByteCodeFileName, g_pChaosVmAth->pConfigure->szChaosVmByteCodeFileName);
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmLdrConfigure->szChaosVmEmulationFileName, g_pChaosVmAth->pConfigure->szChaosVmEmulationFileName);
	g_pChaosVmAth->pChaosVmLdrConfigure->ChaosVmLoaderData.ofChaosVmRuntimeRVA = g_pChaosVmAth->ofChaosVmRuntimeRvaByTarget;//�������л���ʱ��RVA

	__logic_tcscpy__(g_pChaosVmAth->pChaosVmLdrConfigure->szMessageBoxTitle, g_pChaosVmAth->pConfigure->szMessageBoxTitle);
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmLdrConfigure->szMessageBoxContextOnByteCodeFileNotExist, g_pChaosVmAth->pConfigure->szMessageBoxOnByteCodeFileNotExist);
	__logic_tcscpy__(g_pChaosVmAth->pChaosVmLdrConfigure->szMessageBoxContextOnEmulationFileNotExist, g_pChaosVmAth->pConfigure->szMessageBoxOnEmulationFileNotExist);
	g_pChaosVmAth->pChaosVmLdrConfigure->dwMessageStyle = g_pChaosVmAth->pConfigure->dwMessageStyle;


	/*
	 * �������������ģʽ��,���Խ��е��Թ���
	 * ֻ���ڷ����ģʽ�²ſ��Խ���
	 * 2012.2.9 ��������
	 */
	__logic_memcpy__(&(g_pChaosVmAth->pChaosVmLdrConfigure->DebugConfigure), &(g_pChaosVmAth->pConfigure->DebugConfigure), sizeof(CHAOSVM_EMULATION_CONFIGURE_DEBUG_CONFIGURE));

	// ɾ������������������ĵ��Է��ű�
	DeleteDataDirectoryObjectOnMemMap(g_pChaosVmAth->pChaosVmInTarget, IMAGE_DIRECTORY_ENTRY_DEBUG);

	/*
	 * ���û�����Ϣ
	 */
	g_pChaosVmAth->pChaosVm = g_pChaosVmAth->pOrigChaosVm;//���ø���ָ��
	g_pChaosVmAth->pProcedureWriteTo = g_pChaosVmAth->pXFileAttachInfo->pMem + g_pChaosVmAth->ofChaosVmDataOffsetByTarget;
	g_pChaosVmAth->pChaosVmRuntimeList = (PCHAOSVM_RUNTIME)(g_pChaosVmAth->pXFileAttachInfo->pMem + g_pChaosVmAth->ofChaosVmRuntimeOffsetByTarget);//���ô��Runtime���ļ�ָ��

	// �������ýṹ��ѡ���������ļ�ָ��
	ChaosVmAthSetFileAddressToProcedureList(g_pChaosVmAth->pXFileAttachInfo->pMem, g_pChaosVmAth->pConfigure);

	// ��������
	if (!ChaosVmAthSelected(g_pChaosVmAth))
		return __DISATH_ERROR_ERROR__;

	return __DISATH_ERROR_SUCCESS__;
}

__INLINE__ __dword __INTERNAL_FUNC__ DISAthStep15InEmulationSysMode(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	return __DISATH_ERROR_SUCCESS__;
}

__INLINE__ __dword __INTERNAL_FUNC__ DISAthStep15InNetworkMode(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep15(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	if (g_pChaosVmAth->pConfigure->Mode == CRM_INFECT) {
		return DISAthStep15InInfectMode(pXFileAttachInfo, pDISInTarget, pDISConfigure);
	} else if (g_pChaosVmAth->pConfigure->Mode == CRM_EMULATION) {
		return DISAthStep15InEmulationMode(pXFileAttachInfo, pDISInTarget, pDISConfigure);
	} else if (g_pChaosVmAth->pConfigure->Mode == CRM_EMULATION_SYS) {
		return DISAthStep15InEmulationSysMode(pXFileAttachInfo, pDISInTarget, pDISConfigure);
	} else if (g_pChaosVmAth->pConfigure->Mode == CRM_NETWORK) {
		return DISAthStep15InNetworkMode(pXFileAttachInfo, pDISInTarget, pDISConfigure);
	}

	// �������,ֱ�ӷ���ʧ��
	return __DISATH_ERROR_ERROR__;
}

__dword __INTERNAL_FUNC__ DISAthStep16(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader) {
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep17(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure) {
	PCHAOSVMATH_CONFIGURE pConfigure = NULL;
	pConfigure = g_pChaosVmAth->pConfigure;
	//// ������������Ϣ
	//__integer iChaosVmPeHdrLen = 0;

	//iChaosVmPeHdrLen = GetNtHeader(g_pChaosVmAth->pChaosVmInTarget)->OptionalHeader.SizeOfHeaders;
	//__logic_memset__(g_pChaosVmAth->pChaosVmInTarget, 0, iChaosVmPeHdrLen);//���PE

	// ����ֽ����ļ�
	if (g_pChaosVmAth->pByteCodeFile) {
		__tchar szChaosVmByteCodeFilePath[MAX_PATH] = {0};
		GetLocalPath(NULL, szChaosVmByteCodeFilePath);//��ȡ����Ŀ¼
		__logic_tcscat__(szChaosVmByteCodeFilePath, pConfigure->szChaosVmByteCodeFileName);
		ChaosVmByteCodeFileOutputByteCodeFileToFile(szChaosVmByteCodeFilePath, g_pChaosVmAth->pByteCodeFile);
		ChaosVmByteCodeFileReleaseByteCodeFile(&(g_pChaosVmAth->pByteCodeFile));
	}

	// �ͷ�pChaosVmAth
	FreeChaosVmAth(g_pChaosVmAth);
	g_pChaosVmAth = NULL;
	return __DISATH_ERROR_SUCCESS__;
}

__dword __INTERNAL_FUNC__ DISAthStep18(PDISATH_CONFIGURE pConfigure) {
	return __DISATH_ERROR_SUCCESS__;
}
