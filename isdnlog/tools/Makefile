CC=gcc -m486 -O2 -pg -Wall -DSTANDALONE -I. -I.. -I../isdnlog

all: rate zone country holiday rate-at

rate: rate.u zone.u country.u holiday.u
	${CC} -o rate rate.u country.u holiday.u

zone: zone.c zone.h
	${CC} -DZONETEST -lgdbm -o zone zone.c

country: country.c country.h
	${CC} -DCOUNTRYTEST -o country country.c

holiday: holiday.c holiday.h
	${CC} -DHOLITEST -lm -o holiday holiday.c

rate-at: rate-at.c
	${CC} -o rate-at rate-at.c

rate.u: rate.c rate.h holiday.h
	${CC} -c -o rate.u rate.c

zone.u: zone.c zone.h
	${CC} -c -o zone.u zone.c

country.u: country.c country.h
	${CC} -c -o country.u country.c

holiday.u: holiday.c holiday.h
	${CC} -c -o holiday.u holiday.c

clean:
	rm -f rate holiday *.u *~
