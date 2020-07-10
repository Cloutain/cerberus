/*
 * ��������:
 *	����ĺ��������ⲿ�ṩʵ�ִ���,��7�Ժ��ض�λ��ɿ���ʹ�ù�������,��8�Ժ���������������ɿ���ʹ��API
 *	0�Ų���Ϊ,���̴������ж��Լ�����,���Բ���Ҫ���ش���
 *	ʹ�ù����������:G,ʹ��API���:I
 *
 * ����ֵ:
 *	�ڻص���������������__DIS_ERROR_SUCCESS__,�����ĳ���ص������е���ִ�������Ҫֱ�ӵ�ĳ���ص���������
 *	����__DISATH_ERROR_SUCCESS_GOTO__(XΪ����),��ôִ����Ϻ��ֱ��ִ�е�X�Ļص�����,�������__DIS_ERROR_SUCCESS_GOTO_ORIG_ENTRY__
 *	��ֱ�ӵ���11�Żص�����,�����Ҫ���ش�����ʹ��__MAKE_DIS_CUSTOM_ERROR__(����)�����Լ��Ĵ������,�������Ϊ__DIS_MAX_CUSTOM_ERROR__
 *
 * ����ʱע��:
 *	����ʱ�뽫DISEntry,DISEntryInDll��������,g_DISConfigureһ��ȫ�ֱ�������,libdisath��̬�⽫��������
 *
 * ��������:
 *	DISAddThunkCodeStub,�ǽ���������ʱ,���Ϊĳ��API�趨�˱���,��ô����ô˺��������Լ���API Invoke Stub.���Ҫʹ��Ĭ�Ϻ���,�뽫�˺���
 *	����NULLֵ
 *
 * �ļ�����:
 *	DISError.h:������DIS�����ֵ
 *	DISKernel.h:�����˹ؼ������ݽṹ����������ԭ��
 *	DISSteps.h:�����˻ص�����ԭ��������
 */

#include "DISError.h"

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,�����Ϊ0
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *
 * ����:
 *	NG,NI
 */
__void __INTERNAL_FUNC__ DISStep0(__dword hModule, __dword ul_reason_for_call, __void *lpReserved);

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,��ǰֵ��Ч
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *
 * ����:
 *	NG,NI
 */
__dword __INTERNAL_FUNC__ DISStep1(__dword hModule, __dword ul_reason_for_call, __void *lpReserved);

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,��ǰֵ��Ч
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *	pNtSectionHeader:������ӳ���NTͷ
 *
 * ����:
 *	NG,NI
 */
__dword __INTERNAL_FUNC__ DISStep2(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader);

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,��ǰֵ��Ч
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *	pNtSectionHeader:������ӳ���NTͷ
 *	pXFileHeader:DIS���ڵ�X�ļ�
 *	pOutMyDNA:�����ǰDIS��DNA
 *
 * ����:
 *	�˺����ǱȽ���Ҫ�ĺ���,�ڴ˻ص�������Ҫ���ص�ǰDIS��DNA
 *
 * ����:
 *	NG,NI
 */
__dword __INTERNAL_FUNC__ DISStep3(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, __dword *pOutMyDNA);

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,��ǰֵ��Ч
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *	pNtSectionHeader:������ӳ���NTͷ
 *	pXFileHeader:DIS���ڵ�X�ļ�
 *	pXFileSectionHeader:DIS���ڵ�X��
 *	pXFileFatherSectionHeader:���DISӵ�и�DIS��,���Ϊ��Чֵ,������ΪNULL,�����ֵ��Ч��ǰDIS��ֱ������������
 *
 * ����:
 *	NG,NI
 */
__dword __INTERNAL_FUNC__ DISStep4(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, PXFILE_SECTION_HEADER pXFileSectionHeader, PXFILE_SECTION_HEADER pXFileFatherSectionHeader);

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,��ǰֵ��Ч
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *	pNtSectionHeader:������ӳ���NTͷ
 *	pXFileHeader:DIS���ڵ�X�ļ�
 *	pXFileSectionHeader:DIS���ڵ�X��
 *	pXFileFatherSectionHeader:���DISӵ�и�DIS��,���Ϊ��Чֵ,������ΪNULL,�����ֵ��Ч��ǰDIS��ֱ������������
 *	pDISConfigure:DIS�����ýṹ
 *
 * ����:
 *	NG,NI
 */
__dword __INTERNAL_FUNC__ DISStep5(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, PXFILE_SECTION_HEADER pXFileSectionHeader, PXFILE_SECTION_HEADER pXFileFatherSectionHeader, PDIS_CONFIGURE pDISConfigure);

/*
 * ����:
 *	hModule:��ǰ����ӳ��Ļ���ַ,�����EXE,��ǰֵ��Ч
 *	ul_reason_for_call:�������ӳ����DLL,��ô�˲����ǹ��ص�ԭ��,�����EXE��ԶΪ0
 *	lpReserved:�������ӳ����DLL,��ôֵΪdllmain����������,�����EXE��ԶΪNULL
 *	pNtSectionHeader:������ӳ���NTͷ
 *	pXFileHeader:DIS���ڵ�X�ļ�
 *	pXFileSectionHeader:DIS���ڵ�X��
 *	pXFileFatherSectionHeader:���DISӵ�и�DIS��,���Ϊ��Чֵ,������ΪNULL,�����ֵ��Ч��ǰDIS��ֱ������������
 *	pDISConfigure:DIS�����ýṹ
 *	pDISImageBase:DIS��ǰ��ӳ�����ַ
 *	addrDISOrigImageBase:DIS��ӳ���ԭ����ַ
 *
 * ����:
 *	NG,NI
 */
__dword __INTERNAL_FUNC__ DISStep6(__dword hModule, __dword ul_reason_for_call, __void *lpReserved, PIMAGE_SECTION_HEADER pNtSectionHeader, PXFILE_HEADER pXFileHeader, PXFILE_SECTION_HEADER pXFileSectionHeader, PXFILE_SECTION_HEADER pXFileFatherSectionHeader, PDIS_CONFIGURE pDISConfigure, __memory pDISImageBase, __address addrDISOrigImageBase);

/*
 * ����:
 *	pDISConfigure:DIS�����ýṹ
 *	pDISImageBase:DIS��ǰ��ӳ�����ַ
 *	addrDISOrigImageBase:DIS��ӳ���ԭ����ַ
 *
 * ����:
 *	G,NI
 */
__dword __INTERNAL_FUNC__ DISStep7(PDIS_CONFIGURE pDISConfigure, __memory pDISImageBase, __address addrDISOrigImageBase);

/*
 * ����:
 *	G,I
 */
__dword __INTERNAL_FUNC__ DISStep8();

/*
 * ����:
 *	pTargetImageBase:DIS���ڵ�Ŀ���ļ��Ļ���ַ
 *	pDISImageBase:DIS��ǰ��ӳ�����ַ
 *	addrDISOrigImageBase:DIS��ӳ���ԭ����ַ
 *
 * ����:
 *	G,I
 */
__dword __INTERNAL_FUNC__ DISStep9(__memory pTargetImageBase);

/*
 * ����:
 *	G,I
 */
__dword __INTERNAL_FUNC__ DISStep10();

/*
 * ����:
 *	addrOrigEntryAddress:ԭʼ��ڵ��ַ
 *
 * ����:
 *	G,I
 */
__dword __INTERNAL_FUNC__ DISStep11(__address addrOrigEntryAddress);
