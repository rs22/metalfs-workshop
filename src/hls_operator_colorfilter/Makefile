SOLUTION_NAME ?= hls_operator_colorfilter_sln
SOLUTION_DIR ?= hls_operator_colorfilter_sln

HLS_CFLAGS=-I$(METAL_ROOT)/src/metal_fpga/hw/hls/include

srcs += hls_operator_colorfilter.cpp

WRAPPER ?= hls_operator_colorfilter

include $(SNAP_ROOT)/actions/hls.mk
include $(METAL_ROOT)/src/metal_fpga/hw/hls/hls.mk
