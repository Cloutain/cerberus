#if !defined(__CHAOSVM_CPU_X86_INTERRUPT_H__)
#define __CHAOSVM_CPU_X86_INTERRUPT_H__

/*
 *�жϺ�
 */
#define __CPU_INTERRUPT_DE__	0		/* #DE   Divide Error            DIV and IDIV instructions.          */
#define __CPU_INTERRUPT_DB__	1       /* #DB   Debug                   Any code or data reference.         */
#define __CPU_INTERRUPT_BP__	3		/* #BP   Breakpoint              INT 3 instruction.                  */
#define __CPU_INTERRUPT_OF__	4		/* #OF   Overflow                INTO instruction.                   */
#define __CPU_INTERRUPT_BR__	5       /* #BR   BOUND Range Exceeded    BOUND instruction.                  */
#define __CPU_INTERRUPT_UD__	6		/* #UD   Invalid Opcode          UD2 instruction or reserved opcode. */
										/*       (UndefinedOpcode)											 */
#define __CPU_INTERRUPT_NM__	7       /* #NM   Device Not Available    Floating-point or WAIT/FWAIT instruction.*/
										/*       (NoMath Coprocessor)                                             */
#define __CPU_INTERRUPT_DF__	8       /* #DF   Double Fault            Any instruction that can generate an exception, */
										/*                               an NMI, or an INTR.							 */
#define __CPU_INTERRUPT_GP__	13		/* #GP   General Protection      memory reference and other protection checks. */
#define __CPU_INTERRUPT_PF__	14      /* #PF   Page Fault              Any memory reference.                */
#define __CPU_INTERRUPT_MF__	16		/* #MF   x87 FPU Floating-Point  Fault No x87 FPU floating-point or WAIT/FWAIT instruction.*/
										/*       Error (Math Fault)																   */

/*
 * EFLAGS�Ĵ�����ض��� 
 */
// ����־λ��EFLAGS�Ĵ����е�λ��
#define __EFLAG_INDEX_CF__   0
#define __EFLAG_INDEX_PF__   2
#define __EFLAG_INDEX_AF__   4
#define __EFLAG_INDEX_ZF__   6
#define __EFLAG_INDEX_SF__   7
/*Trap (bit 8) �� Set to enable single-step mode for debugging; clear to
disable single-step mode. In single-step mode, the processor generates a
debug exception after each instruction. This allows the execution state of a
program to be inspected after each instruction. If an application program
sets the TF flag using a POPF, POPFD, or IRET instruction, a debug exception
is generated after the instruction that follows the POPF, POPFD, or IRET.*/
#define __EFLAG_INDEX_TF__   8 
#define __EFLAG_INDEX_IF__   9
#define __EFLAG_INDEX_DF__   10
#define __EFLAG_INDEX_OF__   11

// ռ2λ�����ڱ���ģʽ��ֻ�е������������Ȩ��CPL���ڻ��ߵ���IOPLʱ������ִ��IN/OUT/INS/OUTS/STI/CLI/LOCK,��������쳣13 (GP)
#define __EFLAG_INDEX_IOPL__ 12
// ����Ƕ���־
#define __EFLAG_INDEX_NT__   14
// �ָ���־����������ԼĴ����Ĵ���ϵ���ʹ�ã��Ա�֤���ظ�������ϵ�
#define __EFLAG_INDEX_RF	 16
/* 
 * ����8086ģʽ��־VM������ģʽ�£�VM��1ʱ��CPU����������8086ģʽ��
 * ��λ����������2�ַ�ʽ���ã�
 * 1)����ģʽ�£���0���Ĵ���ε�IRET����
 * 2)������ת��������
 */
#define __EFLAG_INDEX_VM__   17
/*
 * �������־AC������80486/Pentium��Ч����ACλΪ1����CR0�Ķ����־λAMΪ1�����
 * �֡�˫�֡����ֵı߽�����飬���򲻼�顣δ���߽���룬�����쳣17
 */
#define __EFLAG_INDEX_AC__   18
// �����ж������־VIF
#define __EFLAG_INDEX_VIF__  19
// �����жϹ����־VIP
#define __EFLAG_INDEX_VIP__  20
// CPU��ʶ��־ID��Ϊ1ʱ������ʹ��CPUIDָ��
#define __EFLAG_INDEX_ID__   21
// ��������EFLAGS�Ĵ����б����Һ�Ϊ1��λ
#define __EFLAG_OR_MASK__	0x2

// ��ͨ32�Ĵ�������
#define __GR32_EAX__	    0
#define __GR32_ECX__	    1
#define __GR32_EDX__	    2
#define __GR32_EBX__	    3
#define __GR32_ESP__	    4
#define __GR32_EBP__	    5
#define __GR32_ESI__		6
#define __GR32_EDI__	    7

// �μĴ�������
#define __SR_ES__		    0
#define __SR_CS__		    1
#define __SR_SS__		    2
#define __SR_DS__		    3
#define __SR_FS__		    4
#define __SR_GS__		    5

#endif

