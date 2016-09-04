# LibJPEG 9b (http://www.ijg.org/)

# Renamed jquant2.c "box" to "j_box" and "boxptr" to "j_boxptr" so that to avoid conflict with iPhoneSimulator.platform. See readme.txt

add_library(
    jpeg

    STATIC

    jcapimin.c jcapistd.c jccoefct.c jccolor.c jcdctmgr.c jchuff.c
	jcinit.c jcmainct.c jcmarker.c jcmaster.c jcomapi.c jcparam.c
	jcprepct.c jcsample.c jctrans.c jdapimin.c jdapistd.c
	jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c jddctmgr.c jdhuff.c
	jdinput.c jdmainct.c jdmarker.c jdmaster.c jdmerge.c
	jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c jfdctfst.c
	jfdctint.c jidctflt.c jidctfst.c jidctint.c jquant1.c
	jquant2.c jutils.c jmemmgr.c jcarith.c jdarith.c jaricom.c

    # Use the no backing store memory manager provided by libjpeg. See install.txt
	jmemnobs.c
)
