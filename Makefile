CC	= gcc
CFLAGS	= -g -Wall -Wvla -fsanitize=address

%: %.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf $(TARGET) *.o *.a *.dylib *.dSYM
