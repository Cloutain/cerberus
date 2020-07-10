// 执行成功
#define __DISATH_ERROR_SUCCESS__											0

// 跳转流程
#define __DISATH_ERROR_SUCCESS_GOTO__										0x70000000
#define __DISATH_ERROR_SUCCESS_GOTO_0__										__DISATH_ERROR_SUCCESS_GOTO__ + 0
#define __DISATH_ERROR_SUCCESS_GOTO_1__										__DISATH_ERROR_SUCCESS_GOTO__ + 1
#define __DISATH_ERROR_SUCCESS_GOTO_2__										__DISATH_ERROR_SUCCESS_GOTO__ + 2
#define __DISATH_ERROR_SUCCESS_GOTO_3__										__DISATH_ERROR_SUCCESS_GOTO__ + 3
#define __DISATH_ERROR_SUCCESS_GOTO_4__										__DISATH_ERROR_SUCCESS_GOTO__ + 4
#define __DISATH_ERROR_SUCCESS_GOTO_5__										__DISATH_ERROR_SUCCESS_GOTO__ + 5
#define __DISATH_ERROR_SUCCESS_GOTO_6__										__DISATH_ERROR_SUCCESS_GOTO__ + 6
#define __DISATH_ERROR_SUCCESS_GOTO_7__										__DISATH_ERROR_SUCCESS_GOTO__ + 7
#define __DISATH_ERROR_SUCCESS_GOTO_8__										__DISATH_ERROR_SUCCESS_GOTO__ + 8
#define __DISATH_ERROR_SUCCESS_GOTO_9__										__DISATH_ERROR_SUCCESS_GOTO__ + 9
#define __DISATH_ERROR_SUCCESS_GOTO_A__										__DISATH_ERROR_SUCCESS_GOTO__ + 0xA
#define __DISATH_ERROR_SUCCESS_GOTO_B__										__DISATH_ERROR_SUCCESS_GOTO__ + 0xB
#define __DISATH_ERROR_SUCCESS_GOTO_C__										__DISATH_ERROR_SUCCESS_GOTO__ + 0xC
#define __DISATH_ERROR_SUCCESS_GOTO_D__										__DISATH_ERROR_SUCCESS_GOTO__ + 0xD
#define __DISATH_ERROR_SUCCESS_GOTO_E__										__DISATH_ERROR_SUCCESS_GOTO__ + 0xE
#define __DISATH_ERROR_SUCCESS_GOTO_F__										__DISATH_ERROR_SUCCESS_GOTO__ + 0xF
#define __DISATH_ERROR_SUCCESS_GOTO_10__									__DISATH_ERROR_SUCCESS_GOTO__ + 0x10
#define __DISATH_ERROR_SUCCESS_GOTO_11__									__DISATH_ERROR_SUCCESS_GOTO__ + 0x11
#define __DISATH_ERROR_SUCCESS_GOTO_12__									__DISATH_ERROR_SUCCESS_GOTO__ + 0x12

// 执行错误
#define __DISATH_ERROR_ERROR__												0x80000000
#define __DISATH_ERROR_MAPPING_FILE_FAILED__								0x80000001
#define __DISATH_ERROR_ADD_XFILE_FAILED__									0x80000002
#define __DISATH_ERROR_PREDICT_NEW_XSECTION_FAILED__						0x80000003
#define __DISATH_ERROR_MAP_DIS_FROM_RESOURCE_FAILED__						0x80000004
#define __DISATH_ERROR_MAP_DIS_FROM_FILE_FAILED__							0x80000005
#define __DISATH_ERROR_ADD_XSECTION_FAILED__								0x80000006
#define __DISATH_ERROR_GET_DISCONFIGURE_FAILED__							0x80000007
#define __DISATH_ERROR_ENCRYPT_DIS_IMPORT_TABLE_FAILED__					0x80000008
#define __DISATH_ERROR_ENCRYPT_TARGET_IMPORT_TABLE_FAILED__					0x80000009

// 由回调函数引发的通用错误
#define __DISATH_ERROR_STEP_ALLOC__											0x80001000

// 测试是否成功
#define __DISATH_SUCCESS__(x)												((x & __DISATH_ERROR_ERROR__) != __DISATH_ERROR_ERROR__)
// 是由于回调函数引发的错误
#define __DISATH_ERROR_FROM_STEP__(x)										(x & 0x1000)

// 自定义错误
#define __DISATH_ERROR_CUSTOM_ERROR__										(__DISATH_ERROR_ERROR__ | 0x100)
// 自定义错误最大值
#define __DISATH_MAX_CUSTOM_ERROR__											0xFF
// 产生自己的错误
#define __MAKE_DISATH_CUSTOM_ERROR__(x)										(__DISATH_ERROR_CUSTOM_ERROR__ + (x))
