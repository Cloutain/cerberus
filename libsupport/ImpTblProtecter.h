#define __MAX_LIBRARY_KEY_COUNT__					256										//������̬����Կ��������
#define __MAX_API_NAME_CRC_COUNT__					2048									//�������뺯����CRC�б���������
#define __MAX_LIBRARY_COUNT__						__MAX_LIBRARY_KEY_COUNT__				//������̬����������
#define __MAX_API_COUNT__							__MAX_API_NAME_CRC_COUNT__				//�������뺯�����������
#define __MAX_LIBRARY_NAME_SIZE__					32										//������̬������������
#define __MAX_JMP_ADDRESS_TABLE_COUNT__				__MAX_API_COUNT__ * 2					//�����������תָ���ļ���
#define __USE_DEF_IMAGEBASE_AS_ORIG__				0xFFFFFFFF								//ʹ��Ĭ�ϵĻ���ַ
#define __MAX_IMPORT_REFERENCE_COUNT__				4096									//ָ�������ַ

/*
 * ��������ṹ
 */
typedef struct _IMPORT_PROTECT {
	__byte bXor;//���ڽ���CRC32ֵ��KEY

	__integer iLibraryKeyCount;//�������Կ����
	__dword dwLibraryKeyList[__MAX_LIBRARY_KEY_COUNT__];//�������Կ����
	__integer iLibraryNameLengthList[__MAX_LIBRARY_KEY_COUNT__];//�������ȶ���
	__char LibraryNameList[__MAX_LIBRARY_COUNT__][__MAX_LIBRARY_NAME_SIZE__];//�����б�

	__integer iApiNumberInThisLibrary[__MAX_LIBRARY_COUNT__];//��ǰ�����ж��ٸ�API

	// ������4��ӵ��ͬһ����������iApiNameCrc32Count
	__integer iApiNameCrc32Count;//API����CRC32����
	__bool bIsOrdList[__MAX_API_NAME_CRC_COUNT__];//ȷ���Ƿ�������������
	__dword dwApiNameCrc32List[__MAX_API_NAME_CRC_COUNT__];//���CRC32��ֵ
	__dword dwFirstThunkList[__MAX_API_NAME_CRC_COUNT__];//��Ŵ˺�����Ӧ��FirstThunkֵRVA

	__address addrImageBase;//ӳ��Ļ���ַ
	__offset ofJmpTableRva;//��ת��RVA

	HMODULE hDll;//��ǰDLL���

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�ΪҪ��ӳ��DLL��������ֶ�
	__dword dwProtectDllCrc32List[__MAX_LIBRARY_COUNT__];//��Ҫ������ӳ���DLL��CRCֵ
	__integer iProtectDllCrc32Count;//��Ҫ������ӳ���DLL��CRCֵ�б����
	__dword dwProtectDllCallDllMainCrc32List[__MAX_LIBRARY_COUNT__];//ִ��DllMain�Ŀ�
	__integer iProtectDllCallDllMainCrc32Count;//��ִ��DllMain�Ŀ��б����

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�Ϊ����API��ȡ�����ֶ�
	__dword dwProtectApiCrc32List[__MAX_API_NAME_CRC_COUNT__];//��Ҫ����API��ȡ��API, CRC32ֵ
	__integer iProtectApiCrc32Count;//��Ҫ����API��ȡ��API����

	//////////////////////////////////////////////////////////////////////////
	// �����ֶ�Ϊ��������APIָ���ȡ�ڴ��RVA
	__offset ofImportTableReferenceAddressRVA[__MAX_IMPORT_REFERENCE_COUNT__];
	__offset ofAPINowRVA[__MAX_IMPORT_REFERENCE_COUNT__];
	__integer iImportTableReferenceAddressCount;
} IMPORT_PROTECT, *PIMPORT_PROTECT;

// ��ʼ���������
__void __API__ InitImportProtecter();
// ���������
__bool __API__ EncryptImportTable(__memory pImageBase, PIMPORT_PROTECT pImportProtect, __bool bOnFile, __memory pOutImportTable);
// ���������,��Ŀ������е���
__bool __API__ DecryptImportTable(PIMPORT_PROTECT pImportProtect);

//////////////////////////////////////////////////////////////////////////
// ������ǿ���������ӽ����������غ�����ṹ�Ķ���

// �ڱ������������ɻ�ָ��ĺ����ص�ָ��
typedef __memory (__INTERNAL_FUNC__ *FPAddThunkCodeStub)(__address addrNowApiAddress);
typedef struct _POWERDECRYPTIMPORTTABLE_ARG {
	PIMPORT_PROTECT pImportProtect;//ָ��һ����������ṹ
	FPAddThunkCodeStub pAddThunkCodeStub;//�û��Զ���ı���STUB����
} POWERDECRYPTIMPORTTABLE_ARG, *PPOWERDECRYPTIMPORTTABLE_ARG;

// �����¼��¼�˴������������������λ��
typedef struct _REFERENCE_IMPORT_TABLE_ADDRESS {
	ud_t ud_obj;//��ָ���UD_T�ṹ
	__address addrReferenceMemAddress;//�����ڴ��ַ
	__memory pReferenceFileAddress;//�����ļ���ַ
	__offset ofReferenceRVA;//���õ�RVA
	struct _REFERENCE_IMPORT_TABLE_ADDRESS *pNext;//��һ���ڵ�
} REFERENCE_IMPORT_TABLE_ADDRESS, *PREFERENCE_IMPORT_TABLE_ADDRESS;

// �ͷ������ַ����
__void __API__ ReleaseReferenceImportTableAddress(PREFERENCE_IMPORT_TABLE_ADDRESS *pReferenceImportTableAddressPoint);
// ���������ַ����ĸ���
__integer __API__ CountReleaseReferenceImportTable(PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceImportTableAddress);
// �������������
PREFERENCE_IMPORT_TABLE_ADDRESS __API__ PowerEncryptImportTable(__memory pMem, __address addrOrigImageBase, PIMPORT_PROTECT pImportProtect, __bool bOnFile, __offset ofJmpTableRva);
// �������������,��Ŀ���ļ��е���
__bool __API__ PowerDecryptImportTable(PPOWERDECRYPTIMPORTTABLE_ARG pArg);
