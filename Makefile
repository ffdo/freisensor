ESP_OPEN_SDK ?= /Volumes/esp8266-rtos-sdk/esp-open-rtos

PROJECT_ROOT := $(dir $(lastword $(MAKEFILE_LIST)))

ESPPORT=/dev/cu.usbserial

PROGRAM=freisensor
# EXTRA_LDFLAGS = -lm
COMPONENTS = FreeRTOS lwip core $(PROJECT_ROOT)/coap $(PROJECT_ROOT)/json $(PROJECT_ROOT)/freisensor
PROGRAM_SRC_DIR=./src
include $(ESP_OPEN_SDK)/common.mk