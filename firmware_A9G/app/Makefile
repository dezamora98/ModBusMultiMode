## ----------------------------------------------------------- ##
## Don't touch the next line unless you know what you're doing.##
## ----------------------------------------------------------- ##

# Name of the module
LOCAL_NAME := app

# List of submodules which contain code we need to include in the final lib

LOCAL_MODULE_DEPENDS := ${LOCAL_NAME}/libs/tiny-json\
                        ${LOCAL_NAME}/libs/freemodbus\

LOCAL_API_DEPENDS := \

LOCAL_ADD_INCLUDE := include\
                    include/std_inc \
                    include/api_inc \
                    $(LOCAL_NAME)/inc \
                    $(LOCAL_NAME)/libs/tiny-json/inc \
                    $(LOCAL_NAME)/libs/freemodbus/inc \ 

IS_TOP_LEVEL := yes

# Set this to any non-null string to signal a module which 
# generates a binary (must contain a "main" entry point). 
# If left null, only a library will be generated.
IS_ENTRY_POINT := no

## ------------------------------------ ##
## 	Add your custom flags here          ##
## ------------------------------------ ##
MYCFLAGS += -std=c99

## ------------------------------------- ##
##	List all your sources here           ##
## ------------------------------------- ##
S_SRC := ${notdir ${wildcard src/*.s}}
C_SRC := $(notdir $(wildcard src/*.c))

## ------------------------------------- ##
##  Do Not touch below this line         ##
## ------------------------------------- ##
include ${SOFT_WORKDIR}/platform/compilation/cust_rules.mk