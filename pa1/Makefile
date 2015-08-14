include $(PUB)/Arch/arch.gnu-c++11.generic


# XTRAFLAGS += -fsource-asm -S

#
# You may override the optimization level
# set in the "arch" file by uncommenting the
# next line and modifying the optimization level as needed
#
# OPTIMIZATION    =  -O0


PNG_DIR = /share/class/public/cse160-sp15/lib
INCLUDES += -I$(PNG_DIR)/include
INCLUDES += -DNO_FREETYPE

LDLIBS += -L$(PNG_DIR)/lib -lpng -lpngwriter

UTIL    = Timer.o util.o Plotter.o

OBJ     = main.o pmdb.o smdb.o Report.o cmdLine.o $(UTIL)

mdb:		$(OBJ)
		$(C++LINK) $(LDFLAGS) -o $@ $(OBJ) $(LDLIBS)

.PHONY: clean
clean:
	$(RM) *.o mdb;
	$(RM) core;

cleanall:
	$(RM) *.o mdb *.png *.rgb log;
	$(RM) core;
