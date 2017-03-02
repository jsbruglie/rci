RMB_OBJECT_FILES = rmb_utils.o rmb.o 
MSG_OBJECT_FILES = msg_table.o msgserv_utils.o msgserv.o

CFLAGS = -ggdb -Wall
LIBS =
CC = gcc  

all: rmb msgserv cleanup
rmb: $(RMB_OBJECT_FILES)  
msgserv: $(MSG_OBJECT_FILES) 

rmb msgserv:
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

rmb.o msgserv.o: 

%.o: %.c
	$(CC) -c $<  

cleanup:
	rm -f *.o

clean:
	rm -f rmb msgserv msgserv_utils msg:table *.o *~ 