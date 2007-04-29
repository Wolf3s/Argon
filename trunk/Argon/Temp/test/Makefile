EE_LDFLAGS  := -L$(PS2SDK)/sbv/lib -L. $(EE_LDFLAGS) -L../ -L../../../zlib/lib
EE_INCS     := -I$(PS2SDK)/sbv/include -I. $(EE_INCS)
EE_BIN       = example.elf
EE_OBJS      = example-hdd.o SMS_Timer.o SMS_Data.o
EE_LIBS      = -lhdd -lpoweroff -lstdc++ -lfileXio -lpatches -ldebug
EE_LDFLAGS  += $(EE_LIB_DIRS)

all: $(EE_BIN)
	ee-strip $(EE_BIN)
	ps2client.exe -h 192.168.1.105 reset
	ps2client.exe -h 192.168.1.105 execee host:example.elf
	

clean:
	rm *.elf *.o

	include $(PS2SDK)/Defs.make
include $(PS2SDK)/samples/Makefile.pref
include $(PS2SDK)/samples/Makefile.eeglobal

EE_CFLAGS  := -D_EE -O2 -G16384 -mgpopt -Wall
EE_ASFLAGS := -G16
