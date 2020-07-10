CPU_STATUS __INTERNAL_FUNC__ FuncOne_UnImplement(PCHAOSVM_CPU pCPU);    
CPU_STATUS __INTERNAL_FUNC__ FuncOne_InvalidOpcode(PCHAOSVM_CPU pCPU);//[F1]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Prefix_66(PCHAOSVM_CPU pCPU);//[66]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Prefix_67(PCHAOSVM_CPU pCPU);//[67]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Prefix_F2(PCHAOSVM_CPU pCPU);//[F2]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Prefix_F3(PCHAOSVM_CPU pCPU);//[F3]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Prefix_SegOverride(PCHAOSVM_CPU pCPU);//[2E,36,3E,26,64,65]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Prefix_F0(PCHAOSVM_CPU pCPU);//[F0]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_68(PCHAOSVM_CPU pCPU);//[68]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_6A(PCHAOSVM_CPU pCPU);//[6A]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_50_57(PCHAOSVM_CPU pCPU);//[50,51,52,53,54,55,56,57]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_ES(PCHAOSVM_CPU pCPU);//[06]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_CS(PCHAOSVM_CPU pCPU);//[0E]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_SS(PCHAOSVM_CPU pCPU);//[16]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSH_DS(PCHAOSVM_CPU pCPU);//[1E]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_POP_58_5F(PCHAOSVM_CPU pCPU);//[58,59,5A,5B,5C,5D,5E,5F]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_POP_ES(PCHAOSVM_CPU pCPU);//[07]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_POP_SS(PCHAOSVM_CPU pCPU);//[17]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_POP_DS(PCHAOSVM_CPU pCPU);//[1F]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSHAD_60(PCHAOSVM_CPU pCPU);//[60]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_POPAD_61(PCHAOSVM_CPU pCPU);//[61]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_NOP_90(PCHAOSVM_CPU pCPU);//[90]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Jcc(PCHAOSVM_CPU pCPU);//[70,71,72,73,74,75,76,77,78,79,7A,7B,7C,7D,7E,7F]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_JMP_E9(PCHAOSVM_CPU pCPU);//[E9]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_JMP_EB(PCHAOSVM_CPU pCPU);//[EB]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CALL_E8(PCHAOSVM_CPU pCPU);//[E8]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_RET_C3(PCHAOSVM_CPU pCPU);//[C3]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_RETN_C2(PCHAOSVM_CPU pCPU);//[C2]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_LOOPE_E0(PCHAOSVM_CPU pCPU);//[E0]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_LOOPNE_E1(PCHAOSVM_CPU pCPU);//[E1]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_LOOP_E2(PCHAOSVM_CPU pCPU);//[E2]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_JRCXZ_E3(PCHAOSVM_CPU pCPU);//[E3]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CLC_F8(PCHAOSVM_CPU pCPU);//[F8]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_STC_F9(PCHAOSVM_CPU pCPU);//[F9]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CLI_FA(PCHAOSVM_CPU pCPU);//[FA]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_STI_FB(PCHAOSVM_CPU pCPU);//[FB]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CLD_FC(PCHAOSVM_CPU pCPU);//[FC]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_STD_FD(PCHAOSVM_CPU pCPU);//[FD]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CWDE_98(PCHAOSVM_CPU pCPU);//[98]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CDQ_99(PCHAOSVM_CPU pCPU);//[99]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_PUSHFD_9C(PCHAOSVM_CPU pCPU);//[9C]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_POPFD_9D(PCHAOSVM_CPU pCPU);//[9D]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_ENTER_C8(PCHAOSVM_CPU pCPU);//[C8]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_LEAVE_C9(PCHAOSVM_CPU pCPU);//[C9]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_INT3_CC(PCHAOSVM_CPU pCPU);//[CC]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_INT_CD(PCHAOSVM_CPU pCPU);//[CD]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_INTO_CE(PCHAOSVM_CPU pCPU);//[CE]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_LEA_8D(PCHAOSVM_CPU pCPU);//[8D]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_XCHG_86(PCHAOSVM_CPU pCPU);//[86]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_XCHG_87(PCHAOSVM_CPU pCPU);//[87]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_XCHG_RAX_RNX(PCHAOSVM_CPU pCPU);//[91,92,93,94,95,96,97]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_LAHF_9F(PCHAOSVM_CPU pCPU);//[9F]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_SAHF_9E(PCHAOSVM_CPU pCPU);//[9E]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_XLAT_D7(PCHAOSVM_CPU pCPU);//[D7]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_SALC_D6(PCHAOSVM_CPU pCPU);//[D6]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_CMC_F5(PCHAOSVM_CPU pCPU);//[F5]
CPU_STATUS __INTERNAL_FUNC__ FuncOne_Two_MainDispatch(PCHAOSVM_CPU pCPU);//[0F]
