OBJECTS = \
		  srec.o \
		  srec2c.o


srec2c: $(OBJECTS) 
	g++ -g $(OBJECTS) -o $@

all: $(OBJECTS)
	g++ -g $(OBJECTS)

clean:
	rm *.o
	rm a.out

COMMON_HEADERS = srec.h
srec2c.o:   $(COMMON_HEADERS) SRecMem.h
$(objects):  $(COMMON_HEADERS)

.cpp.o: 
	g++ -g -c $<
