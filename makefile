CC = cc65
AS = ca65
LN = ld65

FLASHTOOL = ~/repos/GTFO/bin/GTFO.exe

SDIR = src
ODIR = build

PORT = COM3

CFLAGS = -t none -Osr --cpu 65c02 --codesize 500 --static-locals
AFLAGS = --cpu 65C02 --bin-include-dir lib --bin-include-dir $(ODIR)/assets
LFLAGS = -C gametank-2M.cfg -m $(ODIR)/out.map -vm
LLIBS = lib/gametank.lib

_COBJS = globals.o gametank.o dynawave.o drawing_funcs.o tilemap.o random.o music.o enemies.o level.o banking.o main.o
COBJS = $(patsubst %,$(ODIR)/%,$(_COBJS))
_AOBJS = assets.o wait.o vectors.o interrupt.o draw_util.o
AOBJS = $(patsubst %,$(ODIR)/%,$(_AOBJS))

_BMPOBJS = hud_graphics.gtg.deflate \
tileset_00.gtg.deflate tileset_01.gtg.deflate tileset_02.gtg.deflate tileset_03.gtg.deflate \
chara_hero.gtg.deflate chara_spider.gtg.deflate chara_bat.gtg.deflate chara_rat.gtg.deflate chara_orc.gtg.deflate \
chara_goblin_sniper.gtg.deflate projectile_arrow.gtg.deflate chara_ghost.gtg.deflate projectile_fireball.gtg.deflate

BMPOBJS = $(patsubst %,$(ODIR)/assets/%,$(_BMPOBJS))

_SPRITEMETA = chara_hero.gsi chara_spider.gsi chara_bat.gsi chara_rat.gsi chara_orc.gsi \
chara_goblin_sniper.gsi projectile_arrow.gsi chara_ghost.gsi projectile_fireball.gsi
SPRITEMETA = $(patsubst %,$(ODIR)/assets/%,$(_SPRITEMETA))

_AUDIO_FW = audio_fw.bin.deflate
AUDIO_FW = $(patsubst %,$(ODIR)/assets/%,$(_AUDIO_FW))

_MUSIC = title.gtm2 died.gtm2 fiend_loop.gtm2 stairs.gtm2 darker_loop.gtm2 march_loop.gtm2 spooky_loop.gtm2
MUSIC = $(patsubst %,$(ODIR)/assets/%,$(_MUSIC))

_BANKS = bank00 bank01 bank02 bank03 bank04 filler bank7f
BANKS = $(patsubst %,bin/fiend.gtr.%,$(_BANKS))

bin/fiend.gtr: $(BANKS)
	cat $(BANKS) > $@

 $(BANKS): $(AOBJS) $(COBJS) $(LLIBS) gametank-2M.cfg
	mkdir -p $(@D)
	$(LN) $(LFLAGS) $(AOBJS) $(COBJS) -o bin/fiend.gtr $(LLIBS)

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

flash: $(BANKS)
	$(FLASHTOOL) bin/fiend.gtr.bank*

emulate:
	../GameTankEmulator/bin/$(OS)/GameTankEmulator bin/fiend.gtr