include $(PUB)/Arch/arch.gnu-c++11.generic
WARNINGS += -Wall -pedantic


ifeq ($(qsort), 1)
	C++FLAGS += -DQSORT
	CFLAGS += -DQSORT
endif

ifeq ($(debug), 1)
	OPTIMIZATION = 
	C++FLAGS += -g -DDEBUG
	CFLAGS += -g -DDEBUG
	LDFLAGS += -g 
endif

# Defines the macro "SERIAL"
# To enable selective compilaton of a serial version of the code
ifeq ($(serial), 1)
	C++FLAGS += -DSERIAL
	CFLAGS += -DSERIAL
endif
 
MSORT_OBJ	= mSortDriver.o MergeSort.o Timer.o cmdLine.o verify.o


msort:	        $(MSORT_OBJ)
		$(C++LINK) $(LDFLAGS) -o $@ $(MSORT_OBJ)  $(LDLIBS)

.PHONY: clean

clean:
	$(RM) *.o msort
	$(RM) core
