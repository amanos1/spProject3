CC	= gcc
CFLAGS	= -g -Wall -Wvla -fsanitize=address

shell: shell.c builtins.c children.c
	$(CC) $(CFLAGS) -o $@ $^

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET) *.o *.a *.dylib *.dSYM
