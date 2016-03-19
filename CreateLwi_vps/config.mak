CC = gcc
LD = gcc
STRIP = 
CFLAGS = -g -O0  -ffast-math -Wall -std=c99 -pedantic -I. -I. -D__USE_MINGW_ANSI_STDIO=1 -I/usr/local/include  -m32  -fexcess-precision=fast -I/mingw32/include 
LDFLAGS = -L. -L/usr/local/lib -L${exec_prefix}/lib  -m32 
#STRIP = strip
#CFLAGS = -Os -ffast-math -Wall -std=c99 -pedantic -I. -I. -D__USE_MINGW_ANSI_STDIO=1 -I/usr/local/include  -m32  -fexcess-precision=fast -I/mingw32/include 
#LDFLAGS = -L. -L/usr/local/lib -L${exec_prefix}/lib  -m32  -shared -Wl,--dll,--add-stdcall-alias
LIBS = -lwinmm -L/mingw32/lib -llsmash -lm -lavformat -lavicap32 -lgdi32 -lpsapi -lole32 -lstrmiids -luuid -loleaut32 -lshlwapi -lSecur32 -lopus -lm -lopenjpeg -DOPJ_STATIC -lm -lbz2 -lz -pthread -lpsapi -ladvapi32 -lshell32 -lavcodec -lavicap32 -lgdi32 -lpsapi -lole32 -lstrmiids -luuid -loleaut32 -lshlwapi -lSecur32 -lopus -lm -lopenjpeg -DOPJ_STATIC -lm -lbz2 -lz -pthread -lpsapi -ladvapi32 -lshell32 -lswresample -lm -lswscale -lm -lavutil -lm  
SRCDIR = .
DESTDIR = 
prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib
vsplugindir = ${libdir}/vapoursynth
SRC_SOURCE =  CreateLwi.c  lsmashsource.c video_output.c libavsmash_source.c lwlibav_source.c              ../common/utils.c  ../common/qsv.c ../common/libavsmash.c                       ../common/libavsmash_video.c ../common/lwlibav_dec.c                            ../common/lwlibav_video.c ../common/lwlibav_audio.c                             ../common/lwindex.c ../common/video_output.c
BASENAME=vslsmashsource
SONAME=CreateLwi.exe
SONAME_LN=
SOSUFFIX=dll
SOFLAGS=-Wl,--enable-auto-image-base -Wl,--export-all-symbols
