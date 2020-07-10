/*
 * ����ļ�ʵ���������ڸ��ӵ�Ŀ������Ժ�Ի���������ĵ���
 */

/*
 * ����:
 *	pRegisters:�Ĵ�������
 *
 * ����:
 *	��ӡ�����������ǰ�ļĴ�����ֵ
 */
__void __INTERNAL_FUNC__ ChaosVmDebugerPrintCurrRegisters(PCPU_BASIC_REGISTERS pRegisters) {
	PrintDbgInfo_DebugerWriteFormatStringInteger("EAX = ", pRegisters->GeneralRegister.Interface.EAX.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("ECX = ", pRegisters->GeneralRegister.Interface.ECX.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("EDX = ", pRegisters->GeneralRegister.Interface.EDX.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("EBX = ", pRegisters->GeneralRegister.Interface.EBX.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("ESP = ", pRegisters->GeneralRegister.Interface.ESP.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("EBP = ", pRegisters->GeneralRegister.Interface.EBP.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("ESI = ", pRegisters->GeneralRegister.Interface.ESI.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("EDI = ", pRegisters->GeneralRegister.Interface.EDI.dwValue32);
	PrintDbgInfo_DebugerWriteFormatStringInteger("EFL = ", pRegisters->EFlag.BitArray);
}

/*
 * ����:
 *	pCPU:����CPU
 *
 * ����:
 *	��ӡ�����������ǰ��CPU��״̬
 */
__void __INTERNAL_FUNC__ ChaosVmDebugerPrintCurrCpu(PCHAOSVM_CPU pCPU) {
	PrintDbgInfo_DebugerWriteFormatStringInteger("EIP = ", pCPU->CurrentRegistersStatus.EIP);
	ChaosVmDebugerPrintCurrRegisters(&(pCPU->CurrentRegistersStatus.BasicRegister));//��ӡ�Ĵ���
}

/*
 * ����:
 *	pProcedure:��ǰ�����������Ľṹ
 *
 * ����:
 *	��ӡ�����������ǰ�ı������ĺ�����Ϣ
 */
__void __INTERNAL_FUNC__ ChaosVmDebugerPrintCurrProcedure(PCHAOSVMP_PROCEDURE pProcedure) {
	// �Ƿ���ͷ����
	if (pProcedure->bHeader) {
		PrintDbgInfo_DebugerWriteLine("This Procedure is Header");
	} else {
		PrintDbgInfo_DebugerWriteLine("This Procedure is Normal");
	}

	PrintDbgInfo_DebugerWriteFormatStringInteger("Rel Address = ", (__integer)(pProcedure->addrRealAddress));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Vis Address = ", (__integer)(pProcedure->addrVisualAddress));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Procedure Size = ", (__integer)(pProcedure->iSize));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Procedure Init Key = ", (__integer)(pProcedure->dwKey));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Procedure Instruction Count = ", (__integer)(pProcedure->iInstCount));
}

/*
 * ����:
 *	pRuntime:��ǰ���������������л���ʱ
 *
 * ����:
 *	��ӡ�����������ǰ�ı������ĺ��������л���ʱ
 */
__void __INTERNAL_FUNC__ ChaosVmDebugerPrintCurrRuntime(PCHAOSVM_RUNTIME pRuntime) {
	PrintDbgInfo_DebugerWriteFormatStringInteger("Stack Address = ", (__integer)(pRuntime->pStack));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Stack Size = ", (__integer)(pRuntime->iStackSize));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Index = ", (__integer)(pRuntime->iIndex));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Next Key = ", (__integer)(pRuntime->dwKey));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Prev Procedure Address = ", (__integer)(pRuntime->addrPrevProcedureAddress));
	PrintDbgInfo_DebugerWriteFormatStringInteger("Prev Procedure Size = ", (__integer)(pRuntime->iPrevProcedureSize));
}

/*
 *
 * ����:
 *	�������������������
 */

typedef struct _CHAOSVM_DEBUGER_CONFIGURE {
	__bool bSkipAllBreakPoint;//�������жϵ�
} CHAOSVM_DEBUGER_CONFIGURE, *PCHAOSVM_DEBUGER_CONFIGURE;
CHAOSVM_DEBUGER_CONFIGURE g_ChaosVmDebugerConfigure;//ȫ�ֱ���

#define __CHAOSVM_DEBUGER_COMMAND_SIZE__				1024
#define __CHAOSVM_DEBUGER_CONFIGURE_TITLE__				"chaosvm-debuger-configure#"
__void __INTERNAL_FUNC__ ChaosVmDebugerConfigure() {
	__char szCommand[__CHAOSVM_DEBUGER_COMMAND_SIZE__];

	while (1) {
		// ���������
		__PrintDbgInfo_DebugerWrite__(__CHAOSVM_DEBUGER_CONFIGURE_TITLE__);
		// ��������
		__PrintDbgInfo_DebugerReadLine__(szCommand, __CHAOSVM_DEBUGER_COMMAND_SIZE__);

		if (__logic_strncmp__(szCommand, "skip-all-breakpoint", __logic_strlen__("skip-all-breakpoint")) == 0) {
			// �������жϵ�
			g_ChaosVmDebugerConfigure.bSkipAllBreakPoint = TRUE;
		} else if (__logic_strncmp__(szCommand, "exit-configure", __logic_strlen__("exit-configure")) == 0) {
			// �˳�����
			break;
		}
	}/* end while */
}

/*
 * ����:
 *	pCommand:��ǰҪ���������
 *	pParam:�����еĸ��Ӳ���
 *
 * ����:
 *	���������������
 */
__bool __INTERNAL_FUNC__ ChaosVmDebuger(__char *pCommand, __void *pParam) {
	if (!pParam) {
		PrintDbgInfo_DebugerWriteLine("Param is NULL");
		return FALSE;
	}

	if ((__logic_strncmp__(pCommand, "print-cpu", __logic_strlen__("print-cpu")) == 0) ||
		(__logic_strncmp__(pCommand, "pc", __logic_strlen__("pc")) == 0)) {
		ChaosVmDebugerPrintCurrCpu((PCHAOSVM_CPU)pParam);
	} else if ((__logic_strncmp__(pCommand, "print-procedure", __logic_strlen__("print-procedure")) == 0) ||
		(__logic_strncmp__(pCommand, "pp", __logic_strlen__("pp")) == 0)) {
		ChaosVmDebugerPrintCurrProcedure((PCHAOSVMP_PROCEDURE)pParam);
	} else if ((__logic_strncmp__(pCommand, "print-registers", __logic_strlen__("print-registers")) == 0) ||
		(__logic_strncmp__(pCommand, "pr", __logic_strlen__("pr")) == 0)) {
		ChaosVmDebugerPrintCurrRegisters((PCPU_BASIC_REGISTERS)pParam);
	} else if ((__logic_strncmp__(pCommand, "prt", __logic_strlen__("prt")) == 0) ||
		(__logic_strncmp__(pCommand, "print-runtime", __logic_strlen__("print-runtime")) == 0)) {
		ChaosVmDebugerPrintCurrRuntime((PCHAOSVM_RUNTIME)pParam);
	} else if (__logic_strncmp__(pCommand, "chaosvm-debuger-configure", __logic_strlen__("chaosvm-debuger-configure")) == 0) {
		ChaosVmDebugerConfigure();
		return g_ChaosVmDebugerConfigure.bSkipAllBreakPoint;//����
	} else {
		// ��ʱû������
	}
	return FALSE;
}
