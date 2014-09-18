GCC = /usr/bin/g++-4.8
OBJECTS = \
		  srec.o \
		  ucryptr_interface.o \
		  srec2mem.o \
		  combinedSrec2mem.o


all: srec2c srec2mem flashloader combinedSrec2mem jonathan

srec2c: srec2c.o 
	$(GCC) -g $(OBJECTS) $< -o $@

srec2mem: srec2memTester.o  
	$(GCC) -g $(OBJECTS) $< -o $@

flashloader: flashloader.o  srec2mem.o ucryptr_interface.o
	$(GCC) -g $(OBJECTS) $< -o $@

combinedSrec2mem: combinedSrecTester.o srec2mem.o ucryptr_interface.o
	$(GCC) -g $(OBJECTS) $< -o $@

srec2c.o: SRecMem.h srec.h $(OBJECTS) 
srec2mem.o: SRecMem.h $(OBJECTS) 
combinedSrec2mem.o:  SRecMem.h $(OBJECTS) 
ucryptr_interface.o: ucryptr_interface.h
flashloader.o: ucryptr_interface.h

jonathan:
	$(GCC) --version
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
