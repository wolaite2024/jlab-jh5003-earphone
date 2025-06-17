#if BISTO_FEATURE_SUPPORT
#include "app_bisto_reset.h"
#include "bisto_api.h"
#include "hardfault_record.h"
#include "rtl876x.h"
#include "trace.h"

//RESET_DBG must be DBG_DIRECT for APP_PRINT_TRACE should run in thread!
#define RESET_DBG(...)  DBG_DIRECT(__VA_ARGS__)


#define MAX_STACK_SIZE  384


static bool app_bisto_reset_check_ptr(const char *func_name, const uint32_t line_no,
                                      const void *const p, const char *p_name)
{
    if (NULL == p)
    {
        RESET_DBG("%s is NULL", p_name);

        return BISTO_MEM_ACCESS_FAILED;
    }

    return true;
}


static bool app_bisto_reset_extract_system_regs(T_EXCEPTION_RECORD *p_ExceptionRecord,
                                                const uint32_t lr_value)
{
    p_ExceptionRecord->bus_fault_address = SCB->BFAR;
    p_ExceptionRecord->memmanage_fault_address = SCB->MMFAR;
    p_ExceptionRecord->cfsr = SCB->CFSR;

    p_ExceptionRecord->msp = __get_MSP();
    p_ExceptionRecord->PRIMASK = __get_PRIMASK();
    p_ExceptionRecord->BASEPRI = __get_BASEPRI();
    p_ExceptionRecord->EXC_RETURN = lr_value;
    p_ExceptionRecord->HFSR = SCB->HFSR;
    p_ExceptionRecord->DFSR = SCB->DFSR;
    p_ExceptionRecord->AFSR = SCB->AFSR;

    return true;
}


static bool app_bisto_reset_preprocess_exception_stack(T_EXCEPTION_RECORD *p_ExceptionRecord,
                                                       const uint32_t *hardfault_args)
{
    p_ExceptionRecord->stacked_r0 = ((uint32_t)hardfault_args[0]);
    p_ExceptionRecord->stacked_r1 = ((uint32_t)hardfault_args[1]);
    p_ExceptionRecord->stacked_r2 = ((uint32_t)hardfault_args[2]);
    p_ExceptionRecord->stacked_r3 = ((uint32_t)hardfault_args[3]);
    p_ExceptionRecord->stacked_r12 = ((uint32_t)hardfault_args[4]);
    p_ExceptionRecord->stacked_lr = ((uint32_t)hardfault_args[5]);
    p_ExceptionRecord->stacked_pc = ((uint32_t)hardfault_args[6]);
    p_ExceptionRecord->stacked_psr = ((uint32_t)hardfault_args[7]);

    return true;
}


static bool app_bisto_reset_preprocess_calleesaved_stack(T_EXCEPTION_RECORD *p_ExceptionRecord,
                                                         const uint32_t *callee_saved_registers)
{
    p_ExceptionRecord->stacked_r4 = callee_saved_registers[0];
    p_ExceptionRecord->stacked_r5 = callee_saved_registers[1];
    p_ExceptionRecord->stacked_r6 = callee_saved_registers[2];
    p_ExceptionRecord->stacked_r7 = callee_saved_registers[3];
    p_ExceptionRecord->stacked_r8 = callee_saved_registers[4];
    p_ExceptionRecord->stacked_r9 = callee_saved_registers[5];
    p_ExceptionRecord->stacked_r10 = callee_saved_registers[6];
    p_ExceptionRecord->stacked_r11 = callee_saved_registers[7];

    return true;
}



static bool app_bisto_reset_pinout_sp_before_hardfault(T_EXCEPTION_RECORD *p_ExceptionRecord,
                                                       const uint32_t *hardfault_args,
                                                       const uint32_t lr_value)
{
    if ((p_ExceptionRecord->stacked_psr & BIT9) == BIT9)        /* Check bit 9 in PSR */
    {
        if ((lr_value & BIT4) == BIT4)       /* Check FPCA in EXC_RETURN */
        {
            p_ExceptionRecord->old_sp = (uint32_t)hardfault_args + 0x24;
        }
        else
        {
            p_ExceptionRecord->old_sp = (uint32_t)hardfault_args + 0x6c;
        }
    }
    else
    {
        if ((lr_value & BIT4) == BIT4)
        {
            p_ExceptionRecord->old_sp = (uint32_t)hardfault_args + 0x20;
        }
        else
        {
            p_ExceptionRecord->old_sp = (uint32_t)hardfault_args + 0x68;
        }
    }

    RESET_DBG("The sp before hardfault is 0x%8x", p_ExceptionRecord->old_sp);

    return true;
}


static bool app_bisto_reset_fill_registers(const T_EXCEPTION_RECORD *p_HardFaultRecord,
                                           uint32_t *reg_record)
{
    bool ret = true;

    ret = app_bisto_reset_check_ptr(__func__, __LINE__, reg_record, "reg_record");
    if (ret != BISTO_OK)
    {
        return ret;
    }

    reg_record[0] = p_HardFaultRecord->stacked_r0;
    reg_record[1] = p_HardFaultRecord->stacked_r1;
    reg_record[2] = p_HardFaultRecord->stacked_r2;
    reg_record[3] = p_HardFaultRecord->stacked_r3;
    reg_record[4] = p_HardFaultRecord->stacked_r4;
    reg_record[5] = p_HardFaultRecord->stacked_r5;
    reg_record[6] = p_HardFaultRecord->stacked_r6;
    reg_record[7] = p_HardFaultRecord->stacked_r7;
    reg_record[8] = p_HardFaultRecord->stacked_r8;
    reg_record[9] = p_HardFaultRecord->stacked_r9;
    reg_record[10] = p_HardFaultRecord->stacked_r10;
    reg_record[11] = p_HardFaultRecord->stacked_r11;
    reg_record[12] = p_HardFaultRecord->old_sp;
    reg_record[12] = p_HardFaultRecord->stacked_lr;
    reg_record[13] = p_HardFaultRecord->stacked_pc;
    reg_record[14] = p_HardFaultRecord->stacked_psr;
    reg_record[15] = p_HardFaultRecord->msp;
    reg_record[16] = p_HardFaultRecord->HFSR;
    reg_record[17] = p_HardFaultRecord->DFSR;
    reg_record[18] = p_HardFaultRecord->AFSR;

    return true;
}


static bool app_bisto_reset_fill_stack_info(GSoundCrashDumpElement *p_stack_record,
                                            const uint8_t *sp)
{
    bool ret = true;

    ret = app_bisto_reset_check_ptr(__func__, __LINE__, p_stack_record, "p_stack_record");
    if (ret != true)
    {
        return ret;
    }

    ret = app_bisto_reset_check_ptr(__func__, __LINE__, sp, "sp");
    if (ret != true)
    {
        return ret;
    }

    p_stack_record->data = (uint8_t *)sp;
    p_stack_record->len = MAX_STACK_SIZE;

    RESET_DBG("the p_stack is 0x%8x, stack length is %d.\r\n", p_stack_record->data,
              p_stack_record->len);

    return true;
}


bool app_bisto_hardfault(uint32_t *hardfault_args, uint32_t lr_value,
                         uint32_t *callee_saved_registers, bool isHardFault)
{
    bool ret = true;
    BISTO_RESET *p_reset = bisto_reset_get_instance();
    T_EXCEPTION_RECORD aExceptionRecord;

    RESET_DBG("app_bisto_hardfault");

    extern void HardFault_Handler_C(uint32_t *stack_frame, uint32_t lr_value, uint32_t *callee_stack);

    void (*origin_hardfault_isr)(uint32_t *, uint32_t, uint32_t *) = HardFault_Handler_C;

    ret = app_bisto_reset_check_ptr(__func__, __LINE__, p_reset, "p_reset");
    if (ret != true)
    {
        return ret;
    }

    ret = app_bisto_reset_check_ptr(__func__, __LINE__, hardfault_args, "hardfault_args");
    if (ret != true)
    {
        return ret;
    }

    ret = app_bisto_reset_check_ptr(__func__, __LINE__, callee_saved_registers,
                                    "callee_saved_registers");
    if (ret != true)
    {
        return ret;
    }

    app_bisto_reset_preprocess_exception_stack(&aExceptionRecord, hardfault_args);

    app_bisto_reset_preprocess_calleesaved_stack(&aExceptionRecord, callee_saved_registers);

    app_bisto_reset_extract_system_regs(&aExceptionRecord, lr_value);

    app_bisto_reset_pinout_sp_before_hardfault(&aExceptionRecord, hardfault_args, lr_value);

    app_bisto_reset_fill_registers(&aExceptionRecord, (uint32_t *)p_reset->dump.registers.data);

    app_bisto_reset_fill_stack_info(&p_reset->dump.stack, (uint8_t *)aExceptionRecord.old_sp);

    if (origin_hardfault_isr == NULL)
    {
        RESET_DBG("app_bisto_hardfault: origin_hardfault_isr is NULL");
    }

    origin_hardfault_isr(hardfault_args, lr_value, callee_saved_registers);

    return true;
}


__asm void app_bisto_hardfault_asm(void)
{
    TST     LR, #4
    ITE     EQ
    MRSEQ   R0, MSP
    MRSNE   R0, PSP
    MOV     R1, LR
    PUSH    {r4 - r11}
    MRS     R2, MSP
    MOV     R3, #1
    B       app_bisto_hardfault
}


void app_bisto_reset_hardfault_handler_interposition(IRQ_Fun user_hardfault_handler)
{
    RamVectorTableUpdate(HardFault_VECTORn, user_hardfault_handler);
}


void app_bisto_reset_init(void)
{
    bisto_reset_create();

    app_bisto_reset_hardfault_handler_interposition(app_bisto_hardfault_asm);
}
#endif

