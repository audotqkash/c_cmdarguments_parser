
CC   = gcc
AR   = ar
TARGET := parser
SHARETARGET := lib$(TARGET).a
CFLAGS = -Wall -O2
LIBOPTFLAGS = -shared -fPIC
OBJDIR = ./objs
SOURCES = $(wildcard *.c)
OBJS = $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

all: $(TARGET)

library: $(SHARETARGET)

clean_obj:
	rm -f ./objs/*.o

clean: clean_obj
	rm -f $(TARGET) $(SHARETARGET)


$(TARGET): $(OBJS)
	$(CC) -o $@ $^ 

$(SHARETARGET): $(filter-out $(OBJDIR)/main.o, $(OBJS))
	$(AR) rcs $@ $^

$(OBJDIR)/%.o: %.c
ifeq ($(MAKECMDGOALS),library)
	$(CC) $(CFLAGS) -o $@ -c $<
else
	$(CC) $(CFLAGS) -o $@ -c $<
endif

