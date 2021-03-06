######################################################################
#	makefile
#
######################################################################

###################项目路径和程序名称#################################
DIR=$(shell pwd)
BIN_DIR=$(DIR)/bin
LIB_DIR=$(DIR)/lib
SRC_DIR=$(DIR)/src
INCLUDE_DIR=$(DIR)/include
OBJ_DIR=$(DIR)/obj
DEPS_DIR=$(DIR)/deps
PROGRAM=$(BIN_DIR)/LiteDB

###################OBJ文件及路径############################################
EXTENSION=c
OBJS=$(patsubst $(SRC_DIR)/%.$(EXTENSION), $(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.$(EXTENSION)))
DEPS=$(patsubst $(OBJ_DIR)/%.o, $(DEPS_DIR)/%.d, $(OBJS))

###################include头文件路径##################################
INCLUDE=\
		-I$(INCLUDE_DIR)
		
###################lib文件及路径######################################

###################编译选项及编译器###################################
CC=gcc
CFLAGS=-Wall -W -g 
LDFLAGS=

###################编译目标###########################################
.PHONY: all clean rebuild

all:$(OBJS) 
	@echo $(DEPS_DIR)
	$(CC) -o $(PROGRAM) $(OBJS) $(LDFLAGS) 


$(DEPS_DIR)/%.d: $(SRC_DIR)/%.$(EXTENSION)
	$(CC) -MM $(INCLUDE) $(CFLAGS) $< | sed -e 1's,^,$(OBJ_DIR)/,' > $@

sinclude $(DEPS)

$(OBJ_DIR)/%.o:$(SRC_DIR)/%.$(EXTENSION) 
	$(CC) $< -o $@ -c $(CFLAGS) $(INCLUDE) 

rebuild: clean all

clean:
	rm -rf $(OBJS) $(LIB_DIR)/lib* $(BIN_DIR)/* 