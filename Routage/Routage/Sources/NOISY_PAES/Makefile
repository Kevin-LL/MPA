LDFLAGS = -lm


OBJ = ../LIBRARY/isochrone.o ../LIBRARY/data.o  ../LIBRARY/model.o  ../LIBRARY/operators.o  ../LIBRARY/paes.o  ../LIBRARY/sol.o  ../LIBRARY/util.o noisypaes.o
#OBJ = isochrone.o data.o  model.o  operators.o  paes.o  sol.o  util.o

calc = calculator.univ-brest.fr


# Compiler
CC = gcc -g

# Compiler options
#CFLAGS = -DWITHTACK
#CFLAGS = -I../LIBRARY -DWITHSTRESS -g -lm -Wall
CFLAGS = -I../LIBRARY -DWITHSTRESS -g -lm -Wall -fPIC -shared
#CFLAGS = -DWITHSTRESS -g -lm -Wall

# all object files

noisypaes : $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o noisypaes $(LDFLAGS)
	$(CC) $(CFLAGS) $(OBJ) -o noisypaes.so $(LDFLAGS)

clean:
	rm -f *~ *.o noisypaes ../LIBRARY/*.o

test:
	@echo testing...
	./noisypaes params
	@echo end


calc:
	scp Makefile *.[ch] ${calc}:Isochrone/Sources/NOISY_PAES
	ssh ${calc} 'cd Isochrone/Sources/NOISY_PAES; make' 
