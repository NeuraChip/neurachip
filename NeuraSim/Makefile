# Created Kaustubh Shivdikar -- All Rights Reserved


TARGET = chippy.bin
CC = g++

ROOT_DIR=$(PWD)
CODE_DIR=$(ROOT_DIR)/src/code
HEADER_DIR=$(ROOT_DIR)/src/lib
LIB_DIR=$(ROOT_DIR)/src/lib
OBJ_DIR=$(ROOT_DIR)/src/obj
DRAM_SIM_ROOT_DIR=$(ROOT_DIR)/../DRAMsim3-1.0.0

FMT_LIB_DIR=$(DRAM_SIM_ROOT_DIR)/ext/fmt/include
INI_LIB_DIR=$(DRAM_SIM_ROOT_DIR)/ext/headers
JSON_LIB_DIR=$(DRAM_SIM_ROOT_DIR)/ext/headers
ARGS_LIB_DIR=$(DRAM_SIM_ROOT_DIR)/ext/headers
DRAM_SIM_SRC_DIR=$(DRAM_SIM_ROOT_DIR)/src

DRAM_SIM_INC=-I$(DRAM_SIM_SRC_DIR) -I$(FMT_LIB_DIR) -I$(INI_LIB_DIR) -I$(ARGS_LIB_DIR) -I$(JSON_LIB_DIR) -L$(DRAM_SIM_ROOT_DIR)
DRAM_SIM_CXXFLAGS=-std=c++11 -DFMT_HEADER_ONLY=1
#-fPIC

BOOST_INC=-I/home/ktb/git/neurachip_deploy/softwares/boost/boost_1_81_0

# ASAN_FLAGS=-fsanitize=address -static-libasan
ASAN_FLAGS=

CFLAGS=-Wall -pg -O3 -fopenmp -std=c++0x -g $(ASAN_FLAGS) -ldramsim3 -lm $(DRAM_SIM_CXXFLAGS)
LDFLAGS=


STATIC_FLAGS=
# STATIC_FLAGS=-static




OBJS_LIST=	$(OBJ_DIR)/functions.o \
			$(OBJ_DIR)/debugger/debugger.o \
			$(OBJ_DIR)/neuracore/neuracore.o \
			$(OBJ_DIR)/neuramem/neuramem.o \
			$(OBJ_DIR)/neurarouter/neurarouter.o \
			$(OBJ_DIR)/memreq/memrequest.o \
			$(OBJ_DIR)/memresp/memresponse.o \
			$(OBJ_DIR)/counters/globalcounters.o \
			$(OBJ_DIR)/neuratile/neuratile.o \
			$(OBJ_DIR)/connector/conn.o \
			$(OBJ_DIR)/instx/instx.o \
			$(OBJ_DIR)/dispatcher/dispatcher.o \
			$(OBJ_DIR)/hashreq/hashrequest.o \
			$(OBJ_DIR)/hashresp/hashresponse.o \
			$(OBJ_DIR)/writer/writer.o \
			$(OBJ_DIR)/mc/memorycontroller.o \
			$(OBJ_DIR)/hashpad/hashpad.o \
			$(OBJ_DIR)/hashline/hashline.o \
			$(OBJ_DIR)/cache/cache.o \
			$(OBJ_DIR)/cacheline/cacheline.o \
			$(OBJ_DIR)/mongo/mongo.o \
			$(OBJ_DIR)/argparse/argparse.o
			



OBJ_LINK_LIST=	$(OBJ_DIR)/functions.o \
				$(OBJ_DIR)/debugger.o \
				$(OBJ_DIR)/neuracore.o \
				$(OBJ_DIR)/neuramem.o \
				$(OBJ_DIR)/neurarouter.o \
				$(OBJ_DIR)/memrequest.o \
				$(OBJ_DIR)/memresponse.o \
				$(OBJ_DIR)/globalcounters.o \
				$(OBJ_DIR)/neuratile.o \
				$(OBJ_DIR)/conn.o \
				$(OBJ_DIR)/instx.o \
				$(OBJ_DIR)/dispatcher.o \
				$(OBJ_DIR)/hashrequest.o \
				$(OBJ_DIR)/hashresponse.o \
				$(OBJ_DIR)/writer.o \
				$(OBJ_DIR)/memorycontroller.o \
				$(OBJ_DIR)/hashpad.o \
				$(OBJ_DIR)/hashline.o \
				$(OBJ_DIR)/cache.o \
				$(OBJ_DIR)/cacheline.o \
				$(OBJ_DIR)/mongo.o \
				$(OBJ_DIR)/argparse.o

#PKG_CONFIG=	-I/opt/mongo-cxx-driver/include/mongocxx/v_noabi \
#			-I/opt/mongo-cxx-driver/include/bsoncxx/v_noabi \
#			-L/opt/mongo-cxx-driver/lib \
#			-lmongocxx \
#			-lbsoncxx
PKG_CONFIG=$$(pkg-config --cflags --libs libmongocxx)

# PKG_CONFIG=	-I/usr/local/include/mongocxx/v_noabi \
# 			-I/usr/local/include/bsoncxx/v_noabi \
# 			-L/usr/local/lib \
# 			-lmongocxx \
# 			-lbsoncxx



OBJ=$(OBJS_LIST) $(CODE_DIR)/main.o
OBJ_WITH_TARGET=$(OBJ_DIR)/main.o $(OBJ_LINK_LIST)


.PHONY: all
all: $(OBJ) $(ROOT_DIR)/$(TARGET)
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $(ROOT_DIR)/$(TARGET) $(OBJ_WITH_TARGET)
#	cp $(BIN_DIR)/$(TARGET) $(ROOT_DIR)

$(OBJ_DIR)/%.o: $(LIB_DIR)/%.cpp
	$(CC) $(STATIC_FLAGS) -I$(LIB_DIR) $(DRAM_SIM_INC) $(BOOST_INC) -L$(DRAM_SIM_ROOT_DIR) $(CFLAGS) -c $< $(PKG_CONFIG) -ldramsim3 -lboost_program_options
	mv $(@F) $(OBJ_DIR)

$(CODE_DIR)/%.o: $(CODE_DIR)/%.cpp
	$(CC) $(STATIC_FLAGS) -I$(LIB_DIR) $(DRAM_SIM_INC) $(BOOST_INC) -L$(DRAM_SIM_ROOT_DIR) $(CFLAGS) -c $< $(PKG_CONFIG) -ldramsim3 -lboost_program_options
	mv $(@F) $(OBJ_DIR)

$(ROOT_DIR)/$(TARGET): $(OBJ)
	$(CC) $(STATIC_FLAGS) $(CFLAGS) -L$(DRAM_SIM_ROOT_DIR) $(BOOST_INC) $(LDFLAGS) -o $@ $(OBJ_WITH_TARGET) $(PKG_CONFIG) -ldramsim3 -lboost_program_options
	echo "\n\n\e[1;32m    Done \e[0m \n\n"
#	$(CC) $(CFLAGS) $(LDFLAGS) -o $(ROOT_DIR)/$(TARGET) $(OBJ_WITH_TARGET)
#	cp $(BIN_DIR)/$(TARGET) $(ROOT_DIR)

.PHONY: only_objects
only_objects: $(OBJ)

.PHONY: only_target
only_target: $(ROOT_DIR)/$(TARGET)

.PHONY: clean
clean:
	printf "\n\e[1;32mCleaning all files\e[0m\n"
	rm -f $(OBJ_DIR)/*.o $(ROOT_DIR)/*.bin

.PHONY: run
run: all
	$(ROOT_DIR)/$(TARGET)

.PHONY: test
test:
	echo "Test Start"
