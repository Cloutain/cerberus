#include "RandRoll.h"

__dword __INTERNAL_FUNC__ GetRandDword() {
	__dword dwRandSeed = 0;
	__integer i = 100, j = 0;
	while (i > 0) {//����һ��ʱ��
		i--;
	}
	dwRandSeed = __logic_rand__();
	dwRandSeed ^= __logic_rand__();
	return dwRandSeed;
}

__bool __INTERNAL_FUNC__ RollIt(__bool *pInBoolArray) {
	__dword dwRoll = GetRandDword() % 100;
	return pInBoolArray[dwRoll];
}

__void __INTERNAL_FUNC__ MakeRandArray(__bool *pOutBoolArray, __dword dwSeed) {
	// ������СҪ100����λ
	__dword dwCurrSeed = dwSeed;
	__dword dwPlug = 4;//100 / 4 = 25 ƽ������
	__dword dwRand = 0;
	__integer i = 0;
	__logic_srand__((unsigned)time(NULL));
	__logic_memset__(pOutBoolArray, FALSE, (sizeof(__bool) * 100));
	while (dwCurrSeed > 0) {
		for (i = 0; ((i < 100) && (dwCurrSeed > 0)); i++) {
			dwRand = GetRandDword();
			if (!(dwRand % 2)) {
				if (!pOutBoolArray[i]) {
					pOutBoolArray[i] = TRUE;
					dwCurrSeed--;
				} else {
					dwPlug--;
					if (dwPlug == 0)
						dwPlug = 4;
				}
				i += dwPlug;//����ҵ�һ����ݽ�,ƽ����ɢ
			}/* end if */
		}/* end for */
	}/* end while */
}
