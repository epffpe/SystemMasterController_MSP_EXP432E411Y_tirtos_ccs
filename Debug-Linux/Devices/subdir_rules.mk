################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
Devices/%.obj: ../Devices/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/home/lazyhd/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="/home/lazyhd/git/systemmastercontroller_msp_exp432e411y_tirtos_ccs" --include_path="/home/lazyhd/ti/simplelink_msp432e4_sdk_3_40_01_02/source/third_party/CMSIS/Include" --include_path="/home/lazyhd/ti/simplelink_msp432e4_sdk_3_40_01_02/source/ti/net/bsd" --include_path="/home/lazyhd/ti/simplelink_msp432e4_sdk_3_40_01_02/source/ti/posix/ccs" --include_path="/home/lazyhd/ti/ccs930/ccs/tools/compiler/ti-cgt-arm_18.12.4.LTS/include" --define=DeviceFamily_MSP432E4 --define=__MSP432E411Y__ -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="Devices/$(basename $(<F)).d_raw" --obj_directory="Devices" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


