CPPFLAGS = -std=c++11 -o
CXX = g++
DEBUGER = gdb

RSA: RSA.cpp   
	$(CXX) $? $(CPPFLAGS) $@.o
	
debug: RSA.cpp
	$(CXX) -g $? $(CPPFLAGS) $@.o
	$(DEBUGER) $@.o

clean: 
	rm -f *~ 
	rm -f *.o
