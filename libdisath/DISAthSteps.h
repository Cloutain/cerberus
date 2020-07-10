/*
 * ��������:
 *	
 * ����ֵ:
 *	�ڻص���������������__DIS_ERROR_SUCCESS__,�����ĳ���ص������е���ִ�������Ҫֱ�ӵ�ĳ���ص���������
 *	����__DISATH_ERROR_SUCCESS_GOTO_X__(XΪ����),��ôִ����Ϻ��ֱ��ִ�е�X�Ļص�����
 *	�����Ҫ���ش�����ʹ��__MAKE_DISATH_CUSTOM_ERROR__(����)�����Լ��Ĵ������,�������Ϊ__DISATH_MAX_CUSTOM_ERROR__
 *
 * ����ʱע��:
 *
 * ��������:
 *
 * �ļ�����:
 *	DISAthError.h:������DIS�������ĳ������
 *	DISAthSteps.h:�����˻ص�������ԭ��������
 *	DISAth.h:��������Ҫ�����ݽṹ
 */

#include "DISAthError.h"

/*
 * ����:
 *	pConfigure:DIS���������ýṹָ��
 */
__dword __INTERNAL_FUNC__ DISAthStep0(PDISATH_CONFIGURE pConfigure);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pConfigure:DIS���������ýṹָ��
 */
__dword __INTERNAL_FUNC__ DISAthStep1(PXFILE_ATTACH_INFO pXFileAttachInfo, PDISATH_CONFIGURE pConfigure);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pXFileNtSectionHeader:X�ļ�����PE��
 */
__dword __INTERNAL_FUNC__ DISAthStep2(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_SECTION_HEADER pXFileNtSectionHeader);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pTargetNtHeader:Ҫ���ӵ�Ŀ���ļ�NTͷ
 */
__dword __INTERNAL_FUNC__ DISAthStep3(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pNewXFileSection:DIS����X�ڽ�ͷ
 */
__dword __INTERNAL_FUNC__ DISAthStep4(PXFILE_ATTACH_INFO pXFileAttachInfo, PXFILE_SECTION_HEADER pNewXFileSection);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	addrDISImageBaseInTarget:DIS��Ŀ���ļ��еĻ���ַ
 *	ofDISOffsetByXSectionBody:DIS��X�ڽ����ƫ��
 */
__dword __INTERNAL_FUNC__ DISAthStep5(PXFILE_ATTACH_INFO pXFileAttachInfo, __address addrDISImageBaseInTarget, __offset ofDISOffsetByXSectionBody);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDIS:DIS��ӳ��ָ��
 *	iDISSize:DIS�ĳ���
 */
__dword __INTERNAL_FUNC__ DISAthStep6(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDIS:DIS��ӳ��ָ��
 *	iDISSize:DIS�ĳ���
 *	iDISSizeOfImage:DIS��ӳ�䳤��
 *	ofDISOffsetByXSectionBody:DIS��X�����ƫ��
 *	piXSectionSize:�˲���������DIS��X�ڵĳ���,���ǿ��Է����µ�X�ڳ���
 */
__dword __INTERNAL_FUNC__ DISAthStep7(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize, __integer iDISSizeOfImage, __offset ofDISOffsetByXSectionBody, __integer *piXSectionSize);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDIS:DIS��ӳ��ָ��
 *	iDISSize:DIS�ĳ���
 *	pXFileNtSectionHeader:X�ļ�����PEͷ
 *	pXSection:DIS��X��
 *	dwDNA:DIS��DNA
 *	ofDISConfigureRVA:DIS���ýṹ��Ŀ���е�RVA
 */
__dword __INTERNAL_FUNC__ DISAthStep8(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __integer iDISSize, PIMAGE_SECTION_HEADER pXFileNtSectionHeader, PXFILE_SECTION_HEADER pXSection, __dword dwDNA, __offset ofDISConfigureRVA);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pTargetNtHeader:Ŀ���ļ���PEͷ
 */
__dword __INTERNAL_FUNC__ DISAthStep9(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDIS:DISӳ��ָ��
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	iDISSizeOfImage:DIS��ӳ�䳤��
 */
__dword __INTERNAL_FUNC__ DISAthStep10(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDIS, __memory pDISInTarget, __integer iDISSizeOfImage);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	pDISConfigure:DIS�����ýṹ,��Ŀ���ļ���
 *	ofDISNewAddressTableRva:DIS�����µ�������RVA
 */
__dword __INTERNAL_FUNC__ DISAthStep11(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, __offset ofDISNewAddressTableRva);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	pDISConfigure:DIS�����ýṹ,��Ŀ���ļ���
 *	pReferenceDISImportTableAddress:DIS����������ýṹ
 */
__dword __INTERNAL_FUNC__ DISAthStep12(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceDISImportTableAddress);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	pDISConfigure:DIS�����ýṹ,��Ŀ���ļ���
 *	ofTargetNewAddressTableRva:Ŀ���ļ������µ�������RVA
 */
__dword __INTERNAL_FUNC__ DISAthStep13(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, __offset ofTargetNewAddressTableRva);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	pDISConfigure:DIS�����ýṹ,��Ŀ���ļ���
 *	pReferenceTargetImportTableAddress:Ŀ���ļ�����������ýṹ
 */
__dword __INTERNAL_FUNC__ DISAthStep14(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure, PREFERENCE_IMPORT_TABLE_ADDRESS pReferenceTargetImportTableAddress);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	pDISConfigure:DIS�����ýṹ,��Ŀ���ļ���
 */
__dword __INTERNAL_FUNC__ DISAthStep15(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pTargetNtHeader:Ŀ���ļ���PEͷ
 */
__dword __INTERNAL_FUNC__ DISAthStep16(PXFILE_ATTACH_INFO pXFileAttachInfo, PIMAGE_NT_HEADERS pTargetNtHeader);

/*
 * ����:
 *	pXFileAttachInfo:X�ļ�������Ϣ
 *	pDISInTarget:DIS��Ŀ���ļ��е�λ��
 *	pDISConfigure:DIS�����ýṹ,��Ŀ���ļ���
 */
__dword __INTERNAL_FUNC__ DISAthStep17(PXFILE_ATTACH_INFO pXFileAttachInfo, __memory pDISInTarget, PDIS_CONFIGURE pDISConfigure);

/*
 * ����:
 *	pConfigure:DIS���ӽṹ��ָ��
 */
__dword __INTERNAL_FUNC__ DISAthStep18(PDISATH_CONFIGURE pConfigure);
