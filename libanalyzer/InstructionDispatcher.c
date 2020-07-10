// ���ָ����ǲ������Ŀ
#define __MAX_INST_DISP__		561
// ָ����ǲHOOK����ԭ��
typedef __void (__INTERNAL_FUNC__ *FPInstructionDispatcher)(PX86INSTRUCTION pInstruction);

__void __INTERNAL_FUNC__ DefInstructionDispatcher(PX86INSTRUCTION pInstruction) {
	// �����κ�����
}

FPInstructionDispatcher* __INTERNAL_FUNC__ CreateDefDispatcherList() {
	__integer i = 0;
	FPInstructionDispatcher *pList = __logic_new__(FPInstructionDispatcher, __MAX_INST_DISP__);
	if (!pList) return NULL;
	for (i = 0; i < __MAX_INST_DISP__; i++)
		pList[i] = DefInstructionDispatcher;
	return pList;
}	

__void __INTERNAL_FUNC__ DestroyDispatcherList(FPInstructionDispatcher **pDispList) {
	__logic_delete__(*pDispList);
	*pDispList = NULL;
}

__void __INTERNAL_FUNC__ InstructionDispatcher(FPInstructionDispatcher *pDispList, PX86INSTRUCTION pInstruction) {
	ud_t *pud_t = &(pInstruction->ud_obj);
	__word wIndex = (__word)pud_t->mnemonic;
	FPInstructionDispatcher pDisp = pDispList[wIndex];
	pDisp(pInstruction);
}
