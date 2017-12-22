PROJ := life
CC := gcc
SRCS := $(wildcard *.c) $(wildcard cJSON/*.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
CFLAGS += -IcJSON -lm

$(PROJ):$(OBJS)
	$(CC) $^ $(CFLAGS) -o $@

.PHONEY:clean

clean:
	@rm -rf $(PROJ) $(OBJS)
