TARGET := namestackclient_apidemo

CFLAGS := -g -I../../../kernel/include -I../../../API -DDEBUG

C_SRCS := \
          client.c

OBJECTS := $(patsubst %.c,%.o,$(C_SRCS))

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $@

.PHONY: clean
clean:
	-rm $(TARGET) $(OBJECTS)