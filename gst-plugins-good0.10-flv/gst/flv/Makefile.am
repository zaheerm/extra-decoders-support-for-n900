plugin_LTLIBRARIES = libgstflv.la

libgstflv_la_CFLAGS = $(GST_BASE_CFLAGS) $(GST_CFLAGS)
libgstflv_la_LIBADD = $(GST_BASE_LIBS) $(GST_LIBS)
libgstflv_la_LDFLAGS = ${GST_PLUGIN_LDFLAGS} 
libgstflv_la_SOURCES = gstflvdemux.c gstflvparse.c gstflvmux.c gstbytereader.c
libgstflv_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = gstflvdemux.h gstflvparse.h gstflvmux.h gstbytereader.h
