CC = cc65
AS = ca65
LN = ld65

SDIR = src
ODIR = build

PORT = COM3

CFLAGS = -t none -Osir --cpu 65c02
AFLAGS = --cpu 65C02 --bin-include-dir lib --bin-include-dir $(ODIR)/assets
LFLAGS = -C gametank-32k.cfg -m $(ODIR)/out.map
LLIBS = lib/gametank.lib

_COBJS = gametank.o dynawave.o drawing_funcs.o tilemap.o random.o music.o main.o
COBJS = $(patsubst %,$(ODIR)/%,$(_COBJS))
_AOBJS = assets.o wait.o vectors.o interrupt.o enemy.o draw_util.o
AOBJS = $(patsubst %,$(ODIR)/%,$(_AOBJS))

_BMPOBJS = gamesprites.gtg.deflate testmap.gtg.deflate chara_hero.gtg.deflate chara_spider.gtg.deflate
BMPOBJS = $(patsubst %,$(ODIR)/assets/%,$(_BMPOBJS))

_SPRITEMETA = chara_hero.gsi chara_spider.gsi
SPRITEMETA = $(patsubst %,$(ODIR)/assets/%,$(_SPRITEMETA))

_AUDIO_FW = audio_fw.bin.deflate
AUDIO_FW = $(patsubst %,$(ODIR)/assets/%,$(_AUDIO_FW))

_MUSIC = title.gtm2 died.gtm2 fiend_loop.gtm2
MUSIC = $(patsubst %,$(ODIR)/assets/%,$(_MUSIC))

bin/fiend.gtr: $(AOBJS) $(COBJS) $(LLIBS)
	mkdir -p $(@D)
	$(LN) $(LFLAGS) $(AOBJS) $(COBJS) -o $@ $(LLIBS)

$(ODIR)/assets.o: src/assets.s $(BMPOBJS) $(SPRITEMETA) $(AUDIO_FW) $(MUSIC)
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

$(ODIR)/assets/%.gtm2: assets/%.mid
	mkdir -p $(@D)
	cd scripts ;\
	node midiconvert.js ../$< ../$@

$(ODIR)/assets/%.deflate: $(ODIR)/assets/%
	mkdir -p $(@D)
	zopfli --deflate $<

$(ODIR)/assets/%.gsi: assets/%.json
	mkdir -p $(@D)
	cd scripts ;\
	node sprite_metadata.js ../$< ../$@

$(ODIR)/assets/audio_fw.bin.deflate: $(ODIR)/assets/audio_fw.bin
	zopfli --deflate $<

$(ODIR)/assets/audio_fw.bin: src/audio_fw.asm gametank-acp.cfg
	$(AS) --cpu 65C02 src/audio_fw.asm -o $(ODIR)/assets/audio_fw.o
	$(LN) -C gametank-acp.cfg $(ODIR)/assets/audio_fw.o -o $(ODIR)/assets/audio_fw.bin

.PHONY: clean

clean:
	rm -rf $(ODIR)/*

flash:
	cd ../eepromProgrammer/flashtool ;\
	 node flashtool.js --sector 126 --offset 0 -f ~/repos/fiend/bin/fiend.gtr -p $(PORT)

emulate:
	../GameTankEmulator/bin/$(OS)/GameTankEmulator bin/fiend.gtr