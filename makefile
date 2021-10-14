

CFLAGS = -Wall

LDLIBS = -lm


objects = mosaico.o mosaic_utils.o






all: mosaico


mosaico: $(objects)
	gcc $(objects) -o mosaico $(LDLIBS)

mosaico.o: mosaico.c mosaic_utils.h
	gcc -c mosaico.c $(CFLAGS)

mosaic_utils.o: mosaic_utils.c mosaic_utils.h
	gcc -c mosaic_utils.c $(CFLAGS)

clean: 
	-rm -f *~ *.o


purge: clean
	-rm -f mosaico
