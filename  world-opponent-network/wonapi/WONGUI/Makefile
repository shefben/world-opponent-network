OBJS = \
	KernelBuild.o\
	ComponentBuild.o\
	UtilBuild.o\
	XBuild.o

SRCS = \
	KernelBuild.cpp\
	ComponentBuild.cpp\
	UtilBuild.cpp\
	XBuild.cpp

CFLAGS = -g -D_LINUX -I. -I../
CPPFLAGS = $(CFLAGS)
LDFLAGS = -L/usr/X11R6/lib
LDLIBS = -lX11 -lpthread libWONGUI.a ../libWON.a jpeglib/libjpeg.a
CC = g++
 
all: libWONGUI.a

libWONGUI.a: $(OBJS)
	$(AR) $(ARFLAGS)s libWONGUI.a $(OBJS)

main: main.o libWONGUI.a 

clean:
	find . -name '*.o' -type f -exec rm {} \;

depend:
	makedepend -- $(CFLAGS) -- $(SRCS)




	
