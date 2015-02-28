LOGFILE='"gc.log"'
FLAGS = -DLOGFILE=${LOGFILE} -Wall -g 

all : mktrans do_trans do_fstree

do_trans : do_trans.o list.o gc.o gc_list.o
	gcc ${FLAGS} -o $@ $^

mktrans : mktrans.o list.o gc.o gc_list.o
	gcc ${FLAGS} -o $@ $^

do_fstree : do_fstree.o fstree.o gc.o gc_fstree.o
	gcc ${FLAGS} -o $@ $^

%.o : %.c 
	gcc ${FLAGS} -c $<

do_trans.o : list.h gc.h gc_list.h
mktrans.o : list.h gc.h
list.o : list.h gc.h gc_list.h
fstree.o : fstree.h gc.h gc_fstree.h
do_fstree.o :fstree.h gc.h gc_fstree.h
gc.o : gc_list.h gc.h gc_fstree.h
gc_list.o : gc.h list.h
gc_fstree.o : gc.h gc_fstree.h

clean : 
	rm *.o mktrans do_trans do_fstree
