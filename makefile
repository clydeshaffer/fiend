CC = cc65
AS = ca65
LN = ld65

SDIR = src
ODIR = build

PORT = COM3

CFLAGS = -t none -O --cpu 65sc02
AFLAGS = --cpu 65sc02 --bin-include-dir lib --bin-include-dir $(ODIR)/assets
LFLAGS = -C gametank-32k.cfg -m out.map
LLIBS = lib/gametank.lib

_COBJS = gametank.o dynawave.o drawing_funcs.o main.o
COBJS = $(patsubst %,$(ODIR)/%,$(_COBJS))
_AOBJS = assets.o wait.o vectors.o interrupt.o
AOBJS = $(patsubst %,$(ODIR)/%,$(_AOBJS))

_BMPOBJS = gamesprites.gtg.deflate testmap.gtg.deflate chara_hero.gtg.deflate
BMPOBJS = $(patsubst %,$(ODIR)/assets/%,$(_BMPOBJS))

_SPRITEMETA = chara_hero.gsi
SPRITEMETA = $(patsubst %,$(ODIR)/assets/%,$(_SPRITEMETA))

bin/fiend.gtr: $(AOBJS) $(COBJS) $(LLIBS)
	mkdir -p $(@D)
	$(LN) $(LFLAGS) $(AOBJS) $(COBJS) -o $@ $(LLIBS)

$(ODIR)/assets.o: src/assets.s $(BMPOBJS) $(SPRITEMETA)
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

$(ODIR)/assets/%.gtg: assets/%.bmp
	mkdir -p $(@D)
	cd scripts ;\
	node sprite_convert.js ../$< ../$@

$(ODIR)/assets/%.gtg.deflate: $(ODIR)/assets/%.gtg
	mkdir -p $(@D)
	zopfli --deflate $<

$(ODIR)/assets/%.gsi: assets/%.json
	mkdir -p $(@D)
	cd scripts ;\
	node sprite_metadata.js ../$< ../$@

.PHONY: clean

clean:
	rm -rf $(ODIR)/*

flash:
	cd ../eepromProgrammer/flashtool ;\
	 node flashtool.js --sector 126 --offset 0 -f ~/repos/fiend/bin/fiend.gtr -p $(PORT)

emulate:
	../GameTankEmulator/bin/$(OS)/GameTankEmulator bin/fiend.gtr