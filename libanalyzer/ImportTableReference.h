typedef struct _IMPORT_TABLE_REFERENCE {
	ud_t ud_obj;//��ָ���UD_T�ṹ
	__char szLibName[0x20];//������

	__bool bOrdinal;//���������
	union {
		__word wOrdinal;//���
		__char szProcName[0x40];//������
	};

	__address addrFirstThunk;//���ڱ����ַ���ڴ��ַ
	__offset ofFirstThunkFileOffset;//���ڱ����ַ���ļ�ƫ��
	__offset ofFirstThunk;//���ڱ����ַ��RVA
	__address addrReferenceMemAddress;//�����ڴ��ַ
	__offset ofReferenceFileOffset;//�����ļ�ƫ��
	__offset ofReferenceRVA;//���õ�RVA

	__address addrAddressReferenceMemAddress;//���õ�ַ�ڴ��ַ
	__offset ofAddressReferenceFileOffset;//���õ�ַ�ļ�ƫ��
	__offset ofAddressReferenceRVA;//���õ�ַ��RVA

	struct _IMPORT_TABLE_REFERENCE *pNext;//��һ���ڵ�
} IMPORT_TABLE_REFERENCE, *PIMPORT_TABLE_REFERENCE;

PIMPORT_TABLE_REFERENCE __API__ GenerateImportTableReference(__memory pMem);
__void __API__ ReleaseImportTableReference(PIMPORT_TABLE_REFERENCE *pImportTableReferencePoint);