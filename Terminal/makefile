bin=crash

CFLAGS += -Wall -g
LDFLAGS +=

src=shell.c history.c timer.c command.c execution.c prompt.c backprocs.c
obj=$(src:.c=.o)

all: $(bin)

$(bin): $(obj)
	$(CC) $(CFLAGS) $(LDFLAGS) $(obj) -o $@

history.o: history.c history.h
shell.o: shell.c
timer.o: timer.c timer.h
command.o: command.c command.h
execution.o: execution.c execution.h
prompt.o: prompt.c prompt.h
backprocs.o: backprocs.c backprocs.h

clean:
	rm -f $(bin) $(obj)

