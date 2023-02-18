################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
build-1354287935: ../ebike.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"C:/ti/ccs1220/ccs/utils/sysconfig_1.15.0/sysconfig_cli.bat" -s "C:/ti/c2000/C2000Ware_4_02_00_00/.metadata/sdk.json" -b "LAUNCHXL_F280049C" -d "F28004x" --script "C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce/ebike.syscfg" -o "syscfg" --package F28004x_100PZ --part F28004x_100PZ --compiler ccs
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/board.c: build-1354287935 ../ebike.syscfg
syscfg/board.h: build-1354287935
syscfg/pinmux.csv: build-1354287935
syscfg/c2000ware_libraries.cmd.genlibs: build-1354287935
syscfg/c2000ware_libraries.opt: build-1354287935
syscfg/c2000ware_libraries.c: build-1354287935
syscfg/c2000ware_libraries.h: build-1354287935
syscfg/: build-1354287935

syscfg/%.obj: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce" --include_path="C:/ti/c2000/C2000Ware_4_02_00_00" --include_path="C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce/device" --include_path="C:/ti/c2000/C2000Ware_4_02_00_00/driverlib/f28004x/driverlib" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/include" --define=DEBUG --define=_LAUNCHXL_F280049C --define=_FLASH --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="syscfg/$(basename $(<F)).d_raw" --include_path="C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce/EBIKE_CFG/syscfg" --obj_directory="syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

%.obj: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla2 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu0 -Ooff --include_path="C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce" --include_path="C:/ti/c2000/C2000Ware_4_02_00_00" --include_path="C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce/device" --include_path="C:/ti/c2000/C2000Ware_4_02_00_00/driverlib/f28004x/driverlib" --include_path="C:/ti/ccs1220/ccs/tools/compiler/ti-cgt-c2000_22.6.0.LTS/include" --define=DEBUG --define=_LAUNCHXL_F280049C --define=_FLASH --diag_suppress=10063 --diag_warning=225 --diag_wrap=off --display_error_number --gen_func_subsections=on --abi=eabi --preproc_with_compile --preproc_dependency="$(basename $(<F)).d_raw" --include_path="C:/Users/Zulima Ike/git/FH17EBikeSoftware/Mo_Test_Sapce/EBIKE_CFG/syscfg" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


