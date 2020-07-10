#if !defined(__FLOWCODEGEN_H__)
#define __FLOWCODEGEN_H__

typedef struct _FLOWCODER *PFLOWCODER;

typedef enum _FLOW_IF_TYPE
{
	FIT_IF = 0,
	FIT_ELSE_IF,
	FIT_ELSE
} FLOW_IF_TYPE;

typedef enum _FLOW_SWITCH_TYPE
{
	FST_SWITCH = 0,
	FST_CASE,
	FST_DEF
} FLOW_SWITCH_TYPE;

typedef enum _FLOW_TYPE
{
	FT_IF = 0,
	FT_SWITCH,
	FT_WHILE
} FLOW_TYPE;

// IF������нṹ
typedef struct _FLOW_IF
{
	struct _FLOW_IF *pNext;//��һ��IF�ڵ�
	PDUMMYCODE pDummyCode;//���ܴ���
	PFLOWCODER pInside;//��Ƕ����

	FLOW_IF_TYPE Type;//IF����
} FLOW_IF, *PFLOW_IF;

// WHILE������нṹ
typedef struct _FLOW_WHILE
{
	PDUMMYCODE pDummyCode;//���ܴ���
	PFLOWCODER pInside;//��Ƕ����
} FLOW_WHILE, *PFLOW_WHILE;

// SWITCH������нṹ
typedef struct _FLOW_SWITCH
{
	struct _FLOW_SWITCH *pNext;//��һ���ڵ�
	PDUMMYCODE pDummyCode;//���ܴ���
	PFLOWCODER pInside;//��Ƕ����

	FLOW_SWITCH_TYPE Type;//SWITCH����
} FLOW_SWITCH, *PFLOW_SWITCH;

// ����ģ��
typedef struct _FLOWCODER
{
	struct _FLOWCODER *pNext;//��һ������
	
	FLOW_TYPE Type;//����

	PFLOW_IF pIf;
	PFLOW_WHILE pWhile;
	PFLOW_SWITCH pSwitch;

} FLOWCODER;

#endif
