include $(PUB)/Arch/arch.gnu-c++11.generic

BANG	=	$(shell expr match `hostname` ccom-bang)
BANG-COMPUTE   =  $(shell expr match `hostname` compute)
BANG-BANG = $(shell echo $(BANG)\&$(BANG-COMPUTE))



ifeq ($(vec),1)
C++FLAGS += -ftree-vectorizer-verbose=2 -DSSE_VEC
C++FLAGS += -msse -msse2
#C++FLAGS += -ftree-vectorize -ftree-vectorizer-verbose=2 -march=native -DSSE_VEC
endif

app:		apf

OBJECTS = apf.o solve.o Plotting.o cmdLine.o Report.o Timer.o utils.o


apf:	        $(OBJECTS) 
		$(C++LINK) $(LDFLAGS) -o $@ $(OBJECTS)  $(LDLIBS)


.PHONY: clean
clean:	
	$(RM) *.o apf;
	$(RM) core;
