ECCODES_DIR := $(file < eccodes_dir.txt)

CC := gcc
CFLAGS := -fPIC -Wall -Wextra -O2 -g
LDFLAGS := -shared
CPPFLAGS := -I$(ECCODES_DIR)/include -MMD -MP
LDLIBS := -leccodes

ifeq ($(OS),Windows_NT)
    LDFLAGS += -L$(ECCODES_DIR)/lib
    TARGET := libgribwind.dll
    RM := del /Q /F
else
    LDFLAGS += -L$(ECCODES_DIR)/lib -Wl,-rpath=$(ECCODES_DIR)/lib
    TARGET := libgribwind.so
    RM := rm -f
endif

SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DEPS := $(SRCS:.c=.d)

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

.PHONY: clean
clean:
	$(RM) $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
