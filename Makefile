SRC=file_operations.c
OBJC_M=file_operations.o
CC=gcc
CFLAG=-D__KERNEL__ -O2
INCLUDE=/usr/src/linux/drivers
$(OBJC_M):$(SRC)
	$(CC) -I$(INCLUDE) $(CFLAG) -c $(SRC) $(OBJC)
clean:
	rm -f *.o
fresh:
	make && rmmod file_operations && insmod file_operations.o
echo:
	make fresh; echo a > /dev/morse23
