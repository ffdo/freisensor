# Component makefile for COAP

JSON_DIR = json
INC_DIRS += $(JSON_DIR)

# args for passing into compile rule generation
json_INC_DIR =  # all in INC_DIRS, needed for normal operation
json_SRC_DIR = $(json_ROOT) 

$(eval $(call component_compile_rules,json))
