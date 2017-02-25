RMB_OBJECT_FILES = rmb.o 
MSG_OBJECT_FILES = msgserv.o msgserv_utils.o

CFLAGS = -ggdb -Wall
LIBS =
CC = gcc  

all: rmb msgserv cleanup
rmb: $(RMB_OBJECT_FILES)  
msg: $(MSG_OBJECT_FILES) 

rmb msg:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@  

rmb.o msg.o: 

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f rmb msgserv msgserv_utils *.o *~ 