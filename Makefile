LEVELS = lev0.o lev1.o lev2.o lev3.o lev4.o lev5.o lev6.o lev7.o lev8.o lev9.o lev10.o lev11.o lev12.o lev13.o lev14.o lev15.o lev16.o lev17.o lev18.o lev19.o lev20.o lev21.o lev22.o lev23.o lev24.o lev25.o lev26.o lev27.o lev28.o lev29.o lev30.o lev31.o lev32.o lev33.o lev34.o lev35.o lev36.o lev37.o lev38.o lev39.o lev40.o lev41.o lev42.o lev43.o lev44.o lev45.o lev46.o lev47.o lev48.o lev49.o
OBJS=$(LEVELS) iceblock.o tree.o mainpeng.o babypeng.o bucket.o embed_file.o texmain24.o texsecond24.o water24.o clouds24.o woohoo.o boom.o wheee.o whooo.o cheers.o shot.o splosh.o audio.o penguinspuzzle.o buffer.o framebuffer.o keys.o matrix.o model.o penguins.o shaders.o texture.o view.o
BIN=penguinspuzzle.bin
CFLAGS=-std=gnu99
LDFLAGS+=-lilclient

include ../Makefile.include

%.o: sound/%.pi
	@rm -f $@ 
	objcopy --input-target binary --output-target elf32-littlearm --binary-architecture arm  $< $@

%.o: data/%.bmp
	@rm -f $@ 
	objcopy --input-target binary --output-target elf32-littlearm --binary-architecture arm  $< $@

%.o: data/%.pi
	@rm -f $@ 
	objcopy --input-target binary --output-target elf32-littlearm --binary-architecture arm  $< $@

