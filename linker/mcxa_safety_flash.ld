/*
 * Copyright 2025 NXP.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * IEC60730 Class-B Safety Linker Script
 * - Separates Safety and Standard memory regions.
 * - Places critical objects in Safety Flash/RAM.
 * - Defines symbols for CRC and Memory Tests.
 */

GROUP (
  "libcr_nohost_nf.a"
  "libcr_c.a"
  "libcr_eabihelpers.a"
  "libgcc.a"
)

MEMORY
{
  /* --- SAFETY REGIONS (Segregated) --- */
  /* Reserved 64KB for Vector Table, Startup Code, and Safety Logic */
  SAFETY_FLASH  (rx) : ORIGIN = 0x0, LENGTH = 0x10000     
  
  /* Reserved 12KB at the beginning of RAM for Safety Variables */
  SAFETY_RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 0x3000 

  /* --- STANDARD REGIONS (Application) --- */
  /* Remaining Flash: 128KB Total - 64KB Safety = 64KB Available */
  PROGRAM_FLASH (rx) : ORIGIN = 0x10000, LENGTH = 0x10000 
  
  /* Remaining RAM: 4KB*/
  SRAM         (rwx) : ORIGIN = 0x20003000, LENGTH = 0x1000  
  
  /* Additional RAM Banks */
  SRAMX0       (rwx) : ORIGIN = 0x4000000, LENGTH = 0x2000  /* 8K bytes */ 
  SRAMX1       (rwx) : ORIGIN = 0x4002000, LENGTH = 0x2000  /* 8K bytes */ 
}

/* Define symbols for memory regions */
__base_PROGRAM_FLASH = 0x10000; 
__top_PROGRAM_FLASH  = 0x0 + 0x20000; 
__base_SRAM     = 0x20003000; 
__top_SRAM      = 0x20003000 + 0x1000; 

ENTRY(ResetISR)

SECTIONS
{
    /* ========================================================== */
    /* SAFETY TEXT SECTION (Located in SAFETY_FLASH)              */
    /* ========================================================== */
    .safety_text : ALIGN(4)
    {
        FILL(0xff)
        __vectors_start__ = ABSOLUTE(.) ;
        KEEP(*(.isr_vector))
        
        /* -------------------------------------------------------- */
        /* MCX Boot Configuration Area (0x400)                      */
        /* Critical for device boot. Must remain at this offset.    */
        /* -------------------------------------------------------- */
        . = 0x400;
        KEEP(*(.FlashConfig))
        KEEP(*(.ConfigVal))
        
        . = ALIGN(4) ;

        /* --- GLOBAL SECTION TABLE --- */
        /* Used by startup code to initialize RAM regions */
        __section_table_start = .;
        __data_section_table = .;
        
        /* 1. Standard Data Copy Information */
        LONG(LOADADDR(.data));
        LONG(    ADDR(.data));
        LONG(  SIZEOF(.data));

        /* 2. Safety Data Copy Information */
        LONG(LOADADDR(.safety_data));
        LONG(    ADDR(.safety_data));
        LONG(  SIZEOF(.safety_data));

        __data_section_table_end = .;

        __bss_section_table = .;
        /* 3. Standard BSS Zeroing Information */
        LONG(    ADDR(.bss));
        LONG(  SIZEOF(.bss));
        
        __bss_section_table_end = .;
        __section_table_end = . ;
        
        /* --- SAFETY CODE PLACEMENT --- */
        . = ALIGN(4);
        
        /* PC Test Object (Fixed address for Program Counter Test) */
        __flash_test_start = .;
        KEEP(*iec60730b_cm33_pc_object.o(.text*))
        
        /* -------------------------------------------------------- */
        /* CRITICAL SAFETY OBJECTS LIST                             */
        /* Includes Startup, Main, Safety Logic, and Library Files  */
        /* -------------------------------------------------------- */
        *(.after_vectors*)
        
        /* Startup and System Initialization */
        *startup_mcux.o(.text* .rodata* .constdata*)
        *board.o(.text* .rodata* .constdata*)   
        *clock_config.o(.text* .rodata* .constdata*)
        *pin_mux.o(.text* .rodata* .constdata*)   
        *system_MCXA166.o(.text* .rodata* .constdata*)    
        *fsl_aio.o(.text* .rodata* .constdata*)    
        *fsl_clock.o(.text* .rodata* .constdata*)    
        *fsl_common_arm.o(.text* .rodata* .constdata*)    
        *fsl_common.o(.text* .rodata* .constdata*)    
        *fsl_ctimer.o(.text* .rodata* .constdata*)    
        *fsl_dac.o(.text* .rodata* .constdata*)    
        *fsl_gpio.o(.text* .rodata* .constdata*)    
        *fsl_inputmux.o(.text* .rodata* .constdata*)    
        *fsl_lpadc.o(.text* .rodata* .constdata*)    
        *fsl_lpcmp.o(.text* .rodata* .constdata*)    
        *fsl_reset.o(.text* .rodata* .constdata*)    
        *fsl_spc.o(.text* .rodata* .constdata*)    
        
        
        
        /* Safety Application and Library Logic */            	
       	*safety_cm33_mcx.o(.text* .rodata* .constdata*)
		*iec60730b_aio.o(.text* .rodata* .constdata*)
       	*iec60730b_clock.o(.text* .rodata* .constdata*)
       	*iec60730b_dio_ext.o(.text* .rodata* .constdata*)
       	*iec60730b_dio.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_flash.o(.text* .rodata* .constdata*)
       	*iec60730b_invariable_memory.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_pc.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_ram.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_reg_dsp_ext.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_reg_fpu.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_reg.o(.text* .rodata* .constdata*)
       	*iec60730b_cm33_stack.o(.text* .rodata* .constdata*)
       	*iec60730b_tsi.o(.text* .rodata* .constdata*)
       	*iec60730b_wdog.o(.text* .rodata* .constdata*)	
       	
       	   
		*brd.o(.text* .rodata* .constdata*)	
		*system.o(.text* .rodata* .constdata*)	
		*dw_app.o(.text* .rodata* .constdata*)	
		*dw_doorSwitch.o(.text* .rodata* .constdata*)	
		*dw_drainTimer.o(.text* .rodata* .constdata*)	
		*dw_motors.o(.text* .rodata* .constdata*)
		/**dw_relays.o(.text* .rodata* .constdata*)*/
		*dw_faults.o(.text* .rodata* .constdata*)			
		*mc.o(.text* .rodata* .constdata*)	
		*hard_brake.o(.text* .rodata* .constdata*)	
		*torq_compensation.o(.text* .rodata* .constdata*)		
		*foc.o(.text* .rodata* .constdata*)
 		*fs.o(.text* .rodata* .constdata*)
 		*fsglobals.o(.text* .rodata* .constdata*)
 		*mcinit.o(.text* .rodata* .constdata*)
 		*mcloop.o(.text* .rodata* .constdata*)
     	*mcobserver.o(.text* .rodata* .constdata*)
		*mcsampling.o(.text* .rodata* .constdata*)
		*mcstate.o(.text* .rodata* .constdata*)
		*mctest.o(.text* .rodata* .constdata*)     		
		*controllers.o(.text* .rodata* .constdata*) 
		*dsp.o(.text* .rodata* .constdata*) 
		*math_.o(.text* .rodata* .constdata*)
		*mcglobals.o(.text* .rodata* .constdata*) 

		
		
        
        . = . + 1; /* Ensure size is not zero padding */
        . = ALIGN(4);
        __flash_test_end = .;   
        
    } > SAFETY_FLASH


    /* ========================================================== */
    /* STANDARD TEXT SECTION (Located in PROGRAM_FLASH)           */
    /* ========================================================== */
    /* Holds all other application code not listed above */
    .text : ALIGN(4)
    {
       *(.text*)
       *(.rodata .rodata.* .constdata .constdata.*)
       . = ALIGN(4);
    } > PROGRAM_FLASH

    /* C++ Exception Handling / Unwind Tables */
    .ARM.extab : ALIGN(4)
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > PROGRAM_FLASH

    .ARM.exidx : ALIGN(4)
    {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > PROGRAM_FLASH
 
    _etext = .;
        
    /* ========================================================== */
    /* SAFETY DATA SECTION (Located in SAFETY_RAM)                */
    /* ========================================================== */
    .safety_data : ALIGN(4)
    {
        . = ALIGN(4);
        __start_safety_ram = .;
        m_safety_ram_start = .; 
        
        /* Filter Safety Variables from Object Files */
        /* Startup and System Initialization */
        *startup_mcux.o(.data* .bss*)
        *board.o(.data* .bss*)   
        *clock_config.o(.data* .bss*)
        *pin_mux.o(.data* .bss*)   
        *system_MCXA166.o(.data* .bss*)    
        *fsl_aio.o(.data* .bss*)    
        *fsl_clock.o(.data* .bss*)    
        *fsl_common_arm.o(.data* .bss*)    
        *fsl_common.o(.data* .bss*)    
        *fsl_ctimer.o(.data* .bss*)    
        *fsl_dac.o(.data* .bss*)    
        *fsl_gpio.o(.data* .bss*)    
        *fsl_inputmux.o(.data* .bss*)    
        *fsl_lpadc.o(.data* .bss*)    
        *fsl_lpcmp.o(.data* .bss*)    
        *fsl_reset.o(.data* .bss*)    
        *fsl_spc.o(.data* .bss*)    
        
        
        
        /* Safety Application and Library Logic */            	
       	*safety_cm33_mcx.o(.data* .bss*)
		*iec60730b_aio.o(.data* .bss*)
       	*iec60730b_clock.o(.data* .bss*)
       	*iec60730b_dio_ext.o(.data* .bss*)
       	*iec60730b_dio.o(.data* .bss*)
       	*iec60730b_cm33_flash.o(.data* .bss*)
       	*iec60730b_invariable_memory.o(.data* .bss*)
       	*iec60730b_cm33_pc.o(.data* .bss*)
  	   	*iec60730b_cm33_pc_object.o(.data* .bss*)
       	*iec60730b_cm33_ram.o(.data* .bss*)
       	*iec60730b_cm33_reg_dsp_ext.o(.data* .bss*)
       	*iec60730b_cm33_reg_fpu.o(.data* .bss*)
       	*iec60730b_cm33_reg.o(.data* .bss*)
       	*iec60730b_cm33_stack.o(.data* .bss*)
       	*iec60730b_tsi.o(.data* .bss*)
       	*iec60730b_wdog.o(.data* .bss*)	
       	
       	   
		*brd.o(.data* .bss*)	
		*system.o(.data* .bss*)	
		*dw_app.o(.data* .bss*)	
		*dw_doorSwitch.o(.data* .bss*)	
		*dw_drainTimer.o(.data* .bss*)	
		*dw_motors.o(.data* .bss*)
		/**dw_relays.o(.data* .bss*)*/
		*dw_faults.o(.data* .bss*)			
		*mc.o(.data* .bss*)	
		*hard_brake.o(.data* .bss*)	
		*torq_compensation.o(.data* .bss*)		
		*foc.o(.data* .bss*)
 		*fs.o(.data* .bss*)
 		*fsglobals.o(.data* .bss*)
 		*mcinit.o(.data* .bss*)
 		*mcloop.o(.data* .bss*)
     	*mcobserver.o(.data* .bss*)
		*mcsampling.o(.data* .bss*)
		*mcstate.o(.data* .bss*)
		*mctest.o(.data* .bss*)     		
		*controllers.o(.data* .bss*) 
		*dsp.o(.data* .bss*) 
		*math_.o(.data* .bss*)
		*mcglobals.o(.data* .bss*)  		
		
		
     		
        . = . + 0x20; /* reserve minimal size for RAM test pass */   
        /* Reserve space even if empty to prevent HardFaults */
        LONG(0); 
        KEEP(*(.safety_data*)) 
        . = ALIGN(4);
        __end_safety_ram = .;
        m_safety_ram_end = .;
        
    } > SAFETY_RAM AT> SAFETY_FLASH


    /* Define Source Address (LMA) for Safety Lib Copy Routine */
    m_safety_ram_src = LOADADDR(.safety_data);


    /* ========================================================== */
    /* STANDARD DATA SECTION (Located in SRAM)                    */
    /* ========================================================== */
    .data : ALIGN(4)
    {
       FILL(0xff)
       _data = . ;
       PROVIDE(__start_data_RAM = .) ;
       PROVIDE(__start_data_SRAM = .) ;
       *(vtable)
       KEEP(*(CodeQuickAccess))
       KEEP(*(DataQuickAccess))
       *(.data*)
       . = ALIGN(4) ;
       _edata = . ;
       PROVIDE(__end_data_RAM = .) ;
       PROVIDE(__end_data_SRAM = .) ;
    } > SRAM AT> PROGRAM_FLASH


    /* ========================================================== */
    /* CRC HEADER PLACEMENT (End of SAFETY_FLASH)                 */
    /* ========================================================== */
    /* Calculate CRC location based on SAFETY_FLASH size */
    __size_flash_crc__   = 0x10;
    m_fs_flash_crc_end   = (ORIGIN(SAFETY_FLASH) + LENGTH(SAFETY_FLASH));
    m_fs_flash_crc_start = (m_fs_flash_crc_end - __size_flash_crc__);
    
    .SEC_CRC m_fs_flash_crc_start : ALIGN(4)
    {
        FILL(0xff)
        KEEP(*(.flshcrc*))
    } > SAFETY_FLASH

    
    /* ========================================================== */
    /* BSS & UNINIT SECTIONS                                      */
    /* ========================================================== */
    .uninit_RESERVED (NOLOAD) : ALIGN(4)
    {
        _start_uninit_RESERVED = .;
        KEEP(*(.bss.$RESERVED*))
       . = ALIGN(4) ;
        _end_uninit_RESERVED = .;
    } > SRAM AT> SRAM

    .bss (NOLOAD) : ALIGN(4)
    {
        _bss = .;
        PROVIDE(__start_bss_RAM = .) ;
        PROVIDE(__start_bss_SRAM = .) ;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4) ;
        _ebss = .;
        PROVIDE(__end_bss_RAM = .) ;
        PROVIDE(__end_bss_SRAM = .) ;
        PROVIDE(end = .);
    } > SRAM AT> SRAM

    .noinit (NOLOAD): ALIGN(4)
    {
        _noinit = .;
        PROVIDE(__start_noinit_RAM = .) ;
        *(.noinit*)
         . = ALIGN(4) ;
        _end_noinit = .;
       PROVIDE(__end_noinit_RAM = .) ;
    } > SRAM AT> SRAM
    
    /* Stack Test Configuration Symbols */
    _BackupSize = 0x40;
    _StackSize = 0x600;
    _StackTestSize = 0x20;

    /* ========================================================== */
    /* RAM BACKUP SECTION (Placed BEFORE Stack)                   */
    /* ========================================================== */
    .ramBackup _end_noinit (NOLOAD) :  ALIGN(4)
    {
        _ramBackupStart = .; 
        __backup_data_start__ = .;
        
        /* Explicitly place the volatile backup array here */
        KEEP(*(.ramBackup*))
        
        . = ALIGN(4);
        _ramBackupEnd = .;
    } > SRAM

    /* ========================================================== */
    /* STACK SECTIONS                                             */
    /* ========================================================== */
    /* Pre-Stack Test Area */
    .stackTestPreBase _ramBackupEnd (NOLOAD) :  ALIGN(4)
    {
        _stackTestPreBaseStart = .;
        . += _StackTestSize;
        . = ALIGN(4);
        _stackTestPreBaseEnd = .;
    } > SRAM
    
    /* Main Stack Area */
    .stack _stackTestPreBaseEnd (NOLOAD) :  ALIGN(4)
    {
        _vStackBase = .;
        . += _StackSize;
        . = ALIGN(4);
        _vStackTop = .;
    } > SRAM

    /* Post-Stack Test Area */
    .stackTestPostTop _vStackTop (NOLOAD) :  ALIGN(4)
    {
        _stackTestPostTopStart = .;
        . += _StackTestSize;
        . = ALIGN(4);
        _stackTestPostTopEnd = .;
    } > SRAM
  
    /* Image Information for Debuggers */
    _image_start = LOADADDR(.safety_text);
    _image_end = LOADADDR(.data) + SIZEOF(.data);
    _image_size = _image_end - _image_start;
}