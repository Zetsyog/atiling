CC ?= gcc
CFLAGS  ?= -Wall -Wextra -ggdb3
LDLIBS  ?= -losl

INCLUDE_PATH = ./include
TARGET   = trahrhe
SRCDIR   = src
OBJDIR   = build
SOURCES  := $(wildcard $(SRCDIR)/**/*.c) $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCLUDE_PATH)/**/*.h) $(wildcard $(INCLUDE_PATH)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)

$(TARGET): $(OBJECTS)
	$(CC) -g -o $@ $^ $(CFLAGS) $(LDLIBS)

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	mkdir -p $(dir $@)
	$(CC) -g -o $@ -c $< $(CFLAGS) -I$(INCLUDE_PATH)

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/*
	rm -f $(BINDIR)/$(TARGET)