CC?=gcc
CFLAGS?=-O0 -g --std=c89 -funsafe-loop-optimizations $(C_NINJA)
CPPFLAGS=-I./inc -I/usr/local/include
ARFLAGS?=rc
RM?=rm -f

SOURCES = src/egl_glx.c
OBJECTS = $(SOURCES:.c=.o)

libeglproxy.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $(OBJECTS)

$(OBJECTS): inc/EGL/egl.h inc/EGL/eglext.h inc/EGL/eglplatform.h \
	inc/KHR/khrplatform.h

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

.PHONY : clean oclint tags astyle cppcheck
clean:	
	$(RM) ./libeglproxy.a $(OBJECTS)

oclint:
	oclint --enable-global-analysis $(SOURCES) -- -c $(CPPFLAGS)

tags:
	ctags -R

astyle:
	astyle --options=./astylerc $(SOURCES)

cppcheck:
	cppcheck -q --template=gcc --std=c89 --enable=all $(CPPFLAGS) -D__unix__ -I/usr/include $(SOURCES)
