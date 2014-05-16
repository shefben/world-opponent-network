OBJS = \
	jcapimin.o\
	jcapistd.o\
	jccoefct.o\
	jccolor.o\
	jcdctmgr.o\
	jchuff.o\
	jcinit.o\
	jcmainct.o\
	jcmarker.o\
	jcmaster.o\
	jcomapi.o\
	jcparam.o\
	jcphuff.o\
	jcprepct.o\
	jcsample.o\
	jctrans.o\
	jdapimin.o\
	jdapistd.o\
	jdatadst.o\
	jdatasrc.o\
	jdcoefct.o\
	jdcolor.o\
	jddctmgr.o\
	jdhuff.o\
	jdinput.o\
	jdmainct.o\
	jdmarker.o\
	jdmaster.o\
	jdmerge.o\
	jdphuff.o\
	jdpostct.o\
	jdsample.o\
	jdtrans.o\
	jerror.o\
	jfdctflt.o\
	jfdctfst.o\
	jfdctint.o\
	jidctflt.o\
	jidctfst.o\
	jidctint.o\
	jidctred.o\
	jmemmgr.o\
	jmemnobs.o\
	jquant1.o\
	jquant2.o\
	jutils.o\
	JPGDecoder.o

CPPFLAGS = -g  -D_LINUX -I. -I../ -I../..

libjpeg.a: $(OBJS)
	$(AR) $(ARFLAGS)s libjpeg.a $(OBJS)


