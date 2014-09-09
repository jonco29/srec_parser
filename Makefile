OBJECTS = \
		  srec.o \
		  ucryptr_interface.o



all: srec2c srec2mem jonathan

srec2c: srec2c.o 
	g++ -g $(OBJECTS) $< -o $@

srec2mem: srec2mem.o  
	g++ -g $(OBJECTS) $< -o $@

srec2c.o: SRecMem.h srec.h $(OBJECTS) 
srec2mem.o: SRecMem.h $(OBJECTS) 
ucryptr_interface.o: ucryptr_interface.h

jonathan:
	@echo "hello there"

clean:
	rm -rf *.o
	rm -rf srec2c
	rm -rf srec2mem

COMMON_HEADERS = srec.h
srec2c.o:   $(COMMON_HEADERS) SRecMem.h
$(objects):  $(COMMON_HEADERS)

.cpp.o: 
	g++ -g -c $<
