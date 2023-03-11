CC=gcc
CCFLAGS= -Wall -Werror

prog: main.o grib_wind.o grib_wind_tools.o
	$(CC) main.o grib_wind.o grib_wind_tools.o -o prog -lm -I/home/kevin/build/include/ -L/home/kevin/build/lib/ -Wl,-rpath,/home/kevin/build/lib/ -leccodes

main.o: main.c grib_wind.h
	$(CC) -c $(CCFLAGS) main.c

grib_wind.o: grib_wind.c grib_wind.h grib_wind_tools.h
	$(CC) -c $(CCFLAGS) grib_wind.c

grib_wind_tools.o: grib_wind_tools.c grib_wind_tools.h
	$(CC) -c $(CCFLAGS) grib_wind_tools.c
clean:
	rm -f prog *.o
