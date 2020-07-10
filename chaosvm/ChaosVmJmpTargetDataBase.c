#include "ChaosVmJmpTargetDataBase.h"
#include "logic.h"

// ��¼��ÿ��������������תĿ���ַ�ṹ
PHASHTREE g_pJmpTargetTableList[__MAX_VMP_PROCEDURE__];

/*
 * ����:
 *	pSymbolValue:���ŵ�ֵ
 *
 * �ͷŷ��Žڵ�Ļص�����,�˺�����logic.h�е�hashtree�б��ص�ʹ��
 */
__void __INTERNAL_FUNC__ JmpTargetHashNodeValueFreeCallBack(__memory pSymbolValue) {
	if (pSymbolValue)
		__logic_delete__(pSymbolValue);
}

/*
 * ����:
 *	iIndex:Ҫ����������������
 *
 * ��ʼ����תĿ��ָ��Ĺ�ϣ��
 */
#define __DEF_JMPTARGET_ROOT__			5
__void __INTERNAL_FUNC__ InitJmpTargetDataBaseTable(__integer iIndex) {
	PHASHTREE *pJmpTargetTablePoint = &(g_pJmpTargetTableList[iIndex]);
	(*pJmpTargetTablePoint) = init_hashtree(__DEF_JMPTARGET_ROOT__, JmpTargetHashNodeValueFreeCallBack);
}

/*
 * ����:
 *	iIndex:Ҫ����������������
 *	addrAddress:Ҫ��ѯ�ĵ�ַ
 *
 * ��ѯһ����ַ�Ƿ�����תĿ���ַ,����Ƿ��������תĿ���ַ��ָ��ṹ(PCHAOSVMP_JMPTARGET_INST),��������򷵻�NULL
 */
PCHAOSVMP_JMPTARGET_INST __INTERNAL_FUNC__ QueryJmpTargetInstFromTable(__integer iIndex, __address addrAddress) {
	PHASHTREE pJmpTargetTable = g_pJmpTargetTableList[iIndex];
	PHASHTREE_NODE pHashNode = query_node_hashtree(pJmpTargetTable, (__byte *)&addrAddress, sizeof(__address));
	if (pHashNode == NULL) return NULL;
	return (PCHAOSVMP_JMPTARGET_INST)(pHashNode->value);
}

/*
 * ����:
 *	iIndex:Ҫ����������������
 *	addrAddress:Ҫ��ѯ�ĵ�ַ
 *	pJmpTargetInst:��תĿ���ַ�ṹ
 *
 * ����һ����תĿ��ָ���תĿ���ַ�ṹ��
 */
PCHAOSVMP_JMPTARGET_INST __INTERNAL_FUNC__ AddJmpTargetInstToTable(__integer iIndex, __address addrAddress,  PCHAOSVMP_JMPTARGET_INST pJmpTargetInst) {
	PHASHTREE pJmpTargetTable = g_pJmpTargetTableList[iIndex];
	PHASHTREE_NODE pHashNode = NULL;
	pHashNode = add_node_hashtree(pJmpTargetTable, (__byte *)&addrAddress, sizeof(__address), (__memory)pJmpTargetInst);
	if (pHashNode == NULL)
		return NULL;
	return pJmpTargetInst;
}

/*
 * ����:
 *	iIndex:Ҫ����������������
 *
 * ����һ����תĿ�����ݿ�
 */
__void __INTERNAL_FUNC__ DestoryJmpTargetTable(__integer iIndex){
	PHASHTREE pJmpTargetTable = g_pJmpTargetTableList[iIndex];
	if (pJmpTargetTable)
		free_hashtree(&pJmpTargetTable);
}