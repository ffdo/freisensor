# Component makefile for Freisensor

FREISENSOR_DIR = freisensor
INC_DIRS += $(FREISENSOR_DIR)

# args for passing into compile rule generation
freisensor_INC_DIR =  # all in INC_DIRS, needed for normal operation
freisensor_SRC_DIR = $(freisensor_ROOT) 

$(eval $(call component_compile_rules,freisensor))