################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/aj755/git/ExampleImport/Mo_Test_Sapce" --include_path="C:/ti/c2000/C2000Ware_4_01_00_00" --include_path="C:/Users/aj755/git/ExampleImport/Mo_Test_Sapce/device" --include_path="C:/ti/c2000/C2000Ware_4_01_00_00/driverlib/f28004x/driverlib" --include_path="C:/ti/ccs1200/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/include" --define=DEBUG --define=_LAUNCHXL_F280049C --define=_FLASH --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/aj755/git/ExampleImport/Mo_Test_Sapce/CPU1_LAUNCHXL_FLASH/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


