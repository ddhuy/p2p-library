CC = gcc
AR = ar

RM = rm -rf

BIN_DIR = ./_bin

SRCS = $(wildcard */src/*.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

CLI_OBJS = $(filter-out %/p2p_server.o, $(OBJS))
SVR_OBJS = $(filter-out %/p2p_client.o, $(OBJS))

CFLAGS = -O0 -g3 -Wall
IFLAGS = -I./server/inc -I./client/inc -I./network/inc -I./stun/inc -I./p2p/inc
LFLAGS = -lpthread

CLIENT = $(BIN_DIR)/p2p_client
SERVER = $(BIN_DIR)/p2p_server

all: $(CLIENT) $(SERVER) 

$(CLIENT): $(CLI_OBJS)
	@echo ' '
	@mkdir -p $(BIN_DIR)
	@echo Building target: $@
	@echo Invoking: $(CC) Linker
	$(CC) $^ $(LFLAGS) -o $@
	@echo Building $@ completed successfully...
	@echo ' '

$(SERVER): $(SVR_OBJS)
	@echo ' '
	@mkdir -p $(BIN_DIR)
	@echo Building target: $@
	@echo Invoking: $(CC) Linker
	$(CC) $^ $(LFLAGS) -o $@
	@echo Building $@ completed successfully...
	@echo ' '

%.o: %.c 
	@echo Compiling: $<
	@echo Invoking: $(CC) Compiler
	$(CC) $(CFLAGS) $(IFLAGS) $(LFLAGS) -c $< -o $@
	@echo Compiling done: $@
	@echo ' '
	
clean:
	@echo Cleaning all:
	$(RM) $(OBJS)
	$(RM) $(CLIENT)
	$(RM) $(SERVER)
	$(RM) $(BIN_DIR)
