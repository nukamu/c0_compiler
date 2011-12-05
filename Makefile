CC=gcc
OBJS=parser.o cogen_main.o syntree.o tokenizer.o cogen.o env.o
CFLAGS=-Wall -g

cogen : $(OBJS)
	$(CC) -o cogen $(OBJS)

clean :
	rm -f $(OBJS)
	rm -f cogen