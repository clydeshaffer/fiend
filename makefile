CC = cc65
AS = ca65
LN = ld65

CFLAGS = -t none -O --cpu 65sc02
AFLAGS = --cpu 65sc02 --bin-include-dir lib --bin-include-dir assets
LFLAGS = -C gametank-32k.cfg -m out.map
LLIBS = lib/gametank.lib

SDIR = src
ODIR = build

_COBJS = gametank.o dynawave.o drawing_funcs.o main.o
COBJS = $(patsubst %,$(ODIR)/%,$(_COBJS))
_AOBJS = assets.o wait.o vectors.o interrupt.o
AOBJS = $(patsubst %,$(ODIR)/%,$(_AOBJS))

ASSETDEPS = assets/gamesprites.gtg.deflate lib/dynawave.acp.deflate lib/inflate_e000_0200.obx

bin/fiend.gtr: $(AOBJS) $(COBJS) $(LLIBS) sprites
	mkdir -p $(@D)
	$(LN) $(LFLAGS) $(AOBJS) $(COBJS) -o $@ $(LLIBS)

$(ODIR)/assets.o: src/assets.s sprites testmap
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

$(ODIR)/%.si: src/%.c src/%.h
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.si: src/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $<

$(ODIR)/%.o: $(ODIR)/%.si
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

$(ODIR)/%.o: src/%.s
	mkdir -p $(@D)
	$(AS) $(AFLAGS) -o $@ $<

lib/gametank.lib: src/crt0.s
	$(AS) src/crt0.s -o build/crt0.o
	ar65 a lib/gametank.lib build/crt0.o

sprites: assets/gamesprites.bmp
	cd assets ;\
	tail -c 16384 gamesprites.bmp > gamesprites.gtg ;\
	zopfli --deflate gamesprites.gtg

testmap: assets/testmap.bmp
	cd assets ;\
	tail -c 1024 testmap.bmp > testmap.gtg ;\
	zopfli --deflate testmap.gtg
	

.PHONY: clean

clean:
	rm -rf $(ODIR)/*

flash:
	cd ../eepromProgrammer/flashtool ;\
	 node flashtool.js --sector 126 --offset 0 -f ~/Repos/Fiend/bin/fiend.gtr -p COM3

emulate:
	../GameTankEmulator/bin/Windows_NT/GameTankEmulator.exe bin/fiend.gtr