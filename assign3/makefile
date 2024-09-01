CFLAGS = -Wall -g  

OBJS = diskinfo.o disklist.o diskget.o diskput.o

all: diskinfo disklist diskget diskput

diskinfo: 
	$(CC) $(CFLAGS) diskinfo.c -o diskinfo 

disklist: $(OBJS)
	$(CC) $(CFLAGS) disklist.c -o disklist 

diskget:  $(OBJS)
	$(CC) $(CFLAGS) diskget.c -o diskget 

diskput:  $(OBJS)
	$(CC) $(CFLAGS) diskput.c -o diskput

clean: 
	rm -rf $(OBJS) a3 

