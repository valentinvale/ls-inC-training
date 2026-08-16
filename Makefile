CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -std=c11

LINUX_TARGET = lsc
WINDOWS_TARGET = lsc.exe

LINUX_SRC = lsc.c
WINDOWS_SRC = lscwin.c

.PHONY: all linux windows clean-linux clean-windows

all:
	@echo Use "make linux" or "make windows".

linux: $(LINUX_TARGET)

windows: $(WINDOWS_TARGET)

$(LINUX_TARGET): $(LINUX_SRC)
	$(CC) $(CFLAGS) -o $@ $^

$(WINDOWS_TARGET): $(WINDOWS_SRC)
	$(CC) $(CFLAGS) -o $@ $^

clean-linux:
	rm -f $(LINUX_TARGET)

clean-windows:
	del /Q $(WINDOWS_TARGET)


