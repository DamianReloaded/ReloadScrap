TARGET_NAME = scrap
MODE = debug
DEPENDENCIES = 
LIBRARIES = curl gumbo
DEFINES =  
SRC_DIR = source
INC_DIR = 
DEP_DIR = 
BIN_DIR = bin/$(MODE)
OBJ_DIR = obj/$(MODE)/$(TARGET_NAME)
DLL_DIR =
CLIBS := $(foreach LIB,$(DEPENDENCIES),-l$(LIB)) $(foreach LIB,$(LIBRARIES),-l$(LIB))
CC = g++

CFLAGS = -Wall $(addprefix -I, $(INC_DIR)) -Wno-deprecated-declarations -std=c++23
ifeq ($(MODE), release)
    CFLAGS += -O3
else
    CFLAGS += -g
endif

SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

.PHONY: all clean run rebuild

all: $(BIN_DIR)/$(TARGET_NAME)

$(BIN_DIR)/$(TARGET_NAME): $(OBJ)
	@mkdir -p $(BIN_DIR)
	$(foreach LIB,$(DEPENDENCIES),make -C $(DEP_DIR)/$(LIB) MODE=$(MODE);)
	$(CC) $(CFLAGS) $(foreach DEF,$(DEFINES),-D$(DEF)) -o $@ $^ $(foreach DIR,$(DLL_DIR),-L$(DIR)) $(CLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(foreach DEF,$(DEFINES),-D$(DEF)) -c -o $@ $< 

clean:
	$(foreach LIB,$(DEPENDENCIES),make -C $(DEP_DIR)/$(LIB) clean MODE=$(MODE);)
	@rm -rf $(OBJ_DIR)
	@rm -rf $(BIN_DIR)

run: $(BIN_DIR)/$(TARGET_NAME)
	$(BIN_DIR)/$(TARGET_NAME)

rebuild: clean all

$(OBJ_DIR):
	@mkdir -p $@
