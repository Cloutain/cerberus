/*
 * XFILE�ļ��ṹ��һ�ֶ�������½ڵĽ������,�������½����Լ�ά����һ������PE�ṹ���ļ��ṹ
 * �˽ṹ��Ϊ��������,һ���ǽ�ͷ����'XFILE_HEADER'����PEͷ�е�DOSͷ+NTͷ�ṹ,�������Ǹ�����
 * ����'XFILE_SECTION_HEADER'�˽ṹ����PE�ڱ�ͷ����,���������1-N��.���Ĳ����ǽ����ݲ���
 * �������������¼��ͬ��������,Ŀǰ�ڲ��ֿ��Դ洢������������,һ����PE�ļ�����,һ����XFILE
 * ���������,�˽ṹ����ӵ����XFILE,���һ�����û��Զ��������.
 *
 * ��Ҫע�����,ÿ��PE�ļ���ֻ������һ��X�ļ���,���Ҵ˽�һ��Ҫ����PE�ļ���ĩβ
 */

#if !defined(__XFILE_FORMAT_H__)
#define __XFILE_FORMAT_H__

#include "Common.h"

#if defined(__cplusplus)
extern "C"
{
#endif

// X�ļ���־
#define __XFILE_SIGN__										0x83121004
#define __XFILE_SUB_SIGN__									0x83120410
// �汾��
#define __XFILE_VERSION__									0x10000000
// X��ʽ�ļ����������
#define __XFILE_SECTION_NAME_SIZE__							8

// ȷ����XFILE�ļ�
#define __IS_XFILE__(x)										(((x) == __XFILE_SIGN__) || ((x) == __XFILE_SUB_SIGN__))

// X�ļ��ĸ�ʽͷ
typedef struct _XFILE_HEADER {
	__dword dwSign;//X�ļ��ı�־
	__dword dwVersion;//X�ļ���ʽ�İ汾
	__dword dwSectionNumber;//������
	__dword dwMaxSectionNumber;//���Ľ�����
	__dword dwXFileSize;//����X�ļ��ھ�������ĳ���
	__dword dwAlignment;//�ڵĶ�������
	__dword dwSizeOfAllHeaders;//����ͷ���ܳ��Ȳ��������ȶ���

	union {
		__dword dwXFileSectionHdrFileOffset;//X�ļ���ͷ���ļ�ƫ��,������XFile
		__dword dwXFileNtSectionHdrFileOffset;//X�ļ�NTͷ���ļ�ƫ��
	};

	union {
		__dword dwXFileSectionHdrMemoryOffset;//X�ļ���ͷ���ڴ�ƫ��,������XFile
		__dword dwXFileNtSectionHdrMemoryOffset;//X�ļ�NTͷ���ڴ�ƫ��
	};
} XFILE_HEADER, *PXFILE_HEADER;

// X�ļ�������
#define __XFILE_SECTION_CHARACTERISTICS_USED__						0x01//�����˽�����ʹ����

#define __XFILE_SECTION_CHARACTERISTICS_PE__						0x8000000//�����˽��е�������һ��PE�ļ�
#define __XFILE_SECTION_CHARACTERISTICS_PE_FILE_ALIGN__				0x02//�������һ��PE�ļ���ʹ���ļ�����,���δ������ʹ���ڴ����

#define __XFILE_SECTION_CHARACTERISTICS_X__							0x80000000//�����˽ڵ�������һ��XFILE�ļ�

// X�ļ��Ľ�ͷ
typedef struct _XFILE_SECTION_HEADER {
	__char szSectionName[__XFILE_SECTION_NAME_SIZE__];//������
	__dword dwCharacteristics;//������
	__dword dwDNA;//�˽ڵ�Ψһ��ʶ��
	__dword dwPointerToRawData;//�ļ�ƫ��
	__dword dwVirtualAddress;//�ڴ�ƫ��
	__dword dwOffsetByXFile;//��XFile��ƫ��
	__dword dwSizeOfSection;//ӳ���С

	union {
		__dword dwEx1;
		__offset ofMemImageBaseOffset;//����˽�ΪPE�ļ�������ͨ���ڴ�����,����Զ��������ͷ��ƫ��
	};

	union {
		__dword dwEx2;
		__dword dwCustomDataRVA;//���ֶα���X���������Զ����һЩ������Ŀ���ļ���RVA
	};
} XFILE_SECTION_HEADER, *PXFILE_SECTION_HEADER;

// X�ļ����Ӽ�¼�ṹ
typedef struct _XFILE_ATTACH_INFO {
	__bool bUsed;//�Ƿ����ڱ�ʹ��
	__bool bResetCheckSum;//��������У���
	__bool bSetAllSectionCanWrite;//�������нڿ�д
	__bool bRandomAllSectionName;//����������нڵ�����
	__bool bCloseNX;//�ر�����ִ�б���
	__bool bCloseRandAddressMap;//�ر������ַ��ӳ��
	__bool bWillDelDataDirectoryIndexs[0x10];//ɾ������Ŀ¼������
	__bool bCompatibility;//��߼�����
	__bool bKeepSameOrigImpLib;//����ԭʼ������һЩֵ,����߼�����

	__char *szSectionName;//DIS������X�ļ���ӵ�Ŀ���ļ��Ľ���
	__char *szXSectionName;//DIS������X����ӵ�X�ļ��Ľ���
	__dword dwDNA;//DIS��DNA

	//////////////////////////////////////////////////////////////////////////
	// Ŀ���ļ����
	union {
		__tchar *szTargetFilePath;//Ŀ���ļ�·��
		__dword dwTargetId;//��ԴID
	};
	__dword dwResourceDecodeKey;//�����Դ���й�����,�����ǽ��������Key
	__tchar *szResourceType;//���������Դ������ô��Դ������
	__memory pMem;//Ŀ���ļ���ӳ��ָ��,���bUsed = TRUE,��ô��ʾpMem����Ч���ļ�ӳ��
	__integer iFileSize;//��ǰ�ļ��Ĵ�С
	__integer iOrigFileSize;//��ԭ�����ļ���С
	__bool bImageBaseRelocate;//����ַҪ��ӳ��
	IMAGE_NT_HEADERS OrigTargetNtHeader;//ԭʼĿ��PEͷ
	__memory pOrigImportDescriptor;//ԭʼ�����������
	__memory pDummyImportTable;//���������
	__integer iDummyImportTableSize;//�����������ռ�õĿռ�

	//////////////////////////////////////////////////////////////////////////
	// ĩβ�������
	__bool bKeepTailData;//����ĩβ����
	__bool bTailDataIsSecurityDataDirectory;//ĩβ������֤���
	__memory pTailData;//ĩβ����
	__offset ofOrigTailDataRawOffset;//ĩβ���ݵ��ļ�ƫ��
	__offset ofSecurityDataDirectoryRawOffsetByTailData;//��ȫ֤�����ĩβ���ݵ��ļ�ƫ��
	__integer iTailDataSize;//ĩβ���ݳ���
} XFILE_ATTACH_INFO, *PXFILE_ATTACH_INFO;

/*
 * ���º����ڸ�������ʹ��,����ƫ��ʹ���ļ�ƫ��
 */
// Ԥ�Ȼ�ȡ��X�ڵĽ�ͷ
__bool __API__ XFilePredictNewSectionHeader(__memory pMem, PXFILE_SECTION_HEADER pNewSectionHeader);
// ��ȡһ��X�ļ��ĵ�һ��X����PE�ļ���
PXFILE_SECTION_HEADER __API__ XFileGetFirstSectionInPEFile(__memory pMem);
// ��ȡһ��X�ļ������һ��X����PE�ļ���
PXFILE_SECTION_HEADER __API__ XFileGetTailSectionInPEFile(__memory pMem);
// ȷ��һ���ļ��Ƿ��Ѿ�����X�ļ�
PIMAGE_SECTION_HEADER __API__ XFileExistXFileInPEFile(__memory pMem);
// ����һ��X�ļ���ͷ�ĳ���
__integer __API__ XFileCalcXFileHeaderSize(__dword dwMaxSectionNumber);
// �Զ����X�ļ����ӽṹ
__bool __API__ XFileAttachInfoInit(PXFILE_ATTACH_INFO pXFileAttachInfo);
// ����X�ļ���һ��PE�ļ�
PIMAGE_SECTION_HEADER __API__ XFileAddXFileToPEFile(PXFILE_ATTACH_INFO pXFileAttachInfo, __integer iSectionSize, __dword dwMaxSectionNumber);
// ����X�ļ���һ��X��
PXFILE_HEADER __API__ XFileAddXFileToXFileSection(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_SECTION_HEADER pXFileSectionHeader, __integer iSectionSize, __dword dwMaxSectionNumber);
// ���X��ͨ��X�ļ�����NT��,�������Ӹ�X��,��С����������
PXFILE_SECTION_HEADER __API__ XFileAddXFileSectionByNtSectionHeader(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_SECTION_HEADER pXFileNtSectionHeader, __integer iSectionSize, __dword dwCharacteristics, __dword dwEx1, __dword dwEx2);
// ���X��ͨ��X�ļ�
PXFILE_SECTION_HEADER __API__ XFileAddXFileSectionByXFileHeader(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_HEADER pXFileHeader, __integer iSectionSize, __dword dwCharacteristics, __dword dwEx1, __dword dwEx2);
// �ͷ�Ŀ��
__void __API__ XFileReleaseXFileAttach(PXFILE_ATTACH_INFO pXFileAttachInfo);

/*
 * ���º������󲿷���������ڼ�������ʹ��
 */
// �ж�һ��ƫ���Ƿ���ĳ������
__bool __API__ XFileInThisXFileSection(PXFILE_SECTION_HEADER pSectH, __offset ofOffset, __bool bRva);
// ��һ��ƫ�ƻ�ȡ���X��ͷ
PXFILE_SECTION_HEADER __API__ XFileGetXFileSectionHeaderByOffset(PXFILE_HEADER pXFileHeader, __offset ofOffset, __bool bRva);
// ��һ��DNA��ȡ���X��ͷ
PXFILE_SECTION_HEADER __API__ XFileGetXFileSectionHeaderByDNA(__memory pMem, PXFILE_HEADER pXFileHeader, __dword dwDNA, __bool bFile, PXFILE_SECTION_HEADER *pXFatherSectionHdrPoint);

#if defined(__cplusplus)
}
#endif

#endif
