MODEL=DELIVERY_BOY

BUILD_DIR = build

TARGET = rn1host

CFLAGS = -D$(MODEL) -DMAP_DIR=\"/home/hrst/rn1-host\" -DSERIAL_DEV=\"/dev/serial0\" -Wall -Winline -std=c99 -g
LDFLAGS =

#DEPS = mapping.h uart.h map_memdisk.h datatypes.h hwdata.h tcp_comm.h tcp_parser.h routing.h map_opers.h pulutof.h
# C sources
C_SOURCES =  \
 rn1host.c mapping.c map_memdisk.c uart.c hwdata.c tcp_comm.c tcp_parser.c routing.c map_opers.c pulutof.c

# list of objects
OBJ = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects

all: $(BUILD_DIR)/$(TARGET)

#CFLAGS += -DSIMULATE_SERIAL
CFLAGS += -DPULUTOF1
CFLAGS += -DPULUTOF_ROBOT_SER_1_TO_4
#CFLAGS += -DPULUTOF_ROBOT_SER_5_UP
#CFLAGS += -DMOTCON_PID_EXPERIMENT

$(BUILD_DIR)/%.o: %.c $(DEPS) | $(BUILD_DIR)
	gcc -c -o $@ $< $(CFLAGS) -pthread

$(BUILD_DIR)/$(TARGET): $(OBJ)
	gcc $(LDFLAGS) -o $(TARGET) $^ -lm -pthread

$(BUILD_DIR):
	mkdir $@

e:
	gedit --new-window rn1host.c datatypes.h mapping.h mapping.c hwdata.h hwdata.c tcp_parser.h tcp_parser.c routing.c routing.h tof3d.h tof3d.cpp tcp_comm.c tcp_comm.h uart.c uart.h mcu_micronavi_docu.c map_memdisk.c map_memdisk.h pulutof.h pulutof.c &
