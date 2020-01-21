LEVELS = lev0.o lev1.o lev2.o lev3.o lev4.o lev5.o lev6.o lev7.o lev8.o lev9.o lev10.o lev11.o lev12.o lev13.o lev14.o lev15.o lev16.o lev17.o lev18.o lev19.o lev20.o lev21.o lev22.o lev23.o lev24.o lev25.o lev26.o lev27.o lev28.o lev29.o lev30.o lev31.o lev32.o lev33.o lev34.o lev35.o lev36.o lev37.o lev38.o lev39.o lev40.o lev41.o lev42.o lev43.o lev44.o lev45.o lev46.o lev47.o lev48.o lev49.o

OBJS=$(LEVELS) iceblock.o tree.o mainpeng.o babypeng.o bucket.o embed_file.o texmain24.o texsecond24.o water24.o clouds24.o woohoo.o boom.o wheee.o whooo.o cheers.o shot.o splosh.o penguinspuzzle.o buffer.o framebuffer.o keys.o matrix.o model.o penguins.o shaders.o texture.o view.o audio.o drm_gbm.o

BIN=penguinspuzzle.bin

CFLAGS=-std=gnu99

LDFLAGS+=-ldrm -lgbm -lEGL -lGL -lm -lasound

CFLAGS+=-DSTANDALONE -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -DTARGET_POSIX -D_LINUX -fPIC -DPIC -D_REENTRANT -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64 -U_FORTIFY_SOURCE -Wall -g -DHAVE_LIBOPENMAX=2 -DOMX -DOMX_SKIP64BIT -ftree-vectorize -pipe -DUSE_EXTERNAL_OMX -DHAVE_LIBBCM_HOST -DUSE_EXTERNAL_LIBBCM_HOST -DUSE_VCHIQ_ARM -Wno-psabi -Wno-unused-function

INCLUDES+=-I/usr/include/libdrm

#LDFLAGS+=-L$(SDKSTAGE)/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -lm -L$(SDKSTAGE)/opt/vc/src/hello_pi/libs/ilclient -L$(SDKSTAGE)/opt/vc/src/hello_pi/libs/vgfont -L$(SDKSTAGE)/opt/vc/src/hello_pi/libs/revision
#INCLUDES+=-I$(SDKSTAGE)/opt/vc/include/ -I$(SDKSTAGE)/opt/vc/include/interface/vcos/pthreads -I$(SDKSTAGE)/opt/vc/include/interface/vmcs_host/linux -I./ -I$(SDKSTAGE)/opt/vc/src/hello_pi/libs/ilclient -I$(SDKSTAGE)/opt/vc/src/hello_pi/libs/vgfont -I$(SDKSTAGE)/opt/vc/src/hello_pi/libs/revision
INCLUDES+=-I/opt/vc/include/ -I/opt/vc/include/interface/vmcs_host/linux  -I/opt/vc/src/hello_pi/libs/ilclient 
LDFLAGS+=-lilclient -L/opt/vc/lib/ -lopenmaxil -lbcm_host -lvcos -lvchiq_arm -lpthread -lrt -L/opt/vc/src/hello_pi/libs/ilclient

all: $(BIN) $(LIB)

%.o: %.c
	@rm -f $@ 
	$(CC) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations

%.o: %.cpp
	@rm -f $@ 
	$(CXX) $(CFLAGS) $(INCLUDES) -g -c $< -o $@ -Wno-deprecated-declarations
	
%.o: sound/%.pi
	@rm -f $@ 
	objcopy --input-target binary --output-target elf32-littlearm --binary-architecture arm  $< $@

%.o: data/%.bmp
	@rm -f $@ 
	objcopy --input-target binary --output-target elf32-littlearm --binary-architecture arm  $< $@

%.o: data/%.pi
	@rm -f $@ 
	objcopy --input-target binary --output-target elf32-littlearm --binary-architecture arm  $< $@


%.bin: $(OBJS)
	$(CC) -o $@ -Wl,--whole-archive $(OBJS) $(LDFLAGS) -Wl,--no-whole-archive -rdynamic

%.a: $(OBJS)
	$(AR) r $@ $^

clean:
	for i in $(OBJS); do (if test -e "$$i"; then ( rm $$i ); fi ); done
	@rm -f $(BIN) $(LIB)


