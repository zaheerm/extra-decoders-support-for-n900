plugin_LTLIBRARIES = libgstrtp.la

libgstrtp_la_SOURCES = \
	fnv1hash.c \
	gstrtp.c \
	gstrtpchannels.c \
	gstrtpdepay.c \
	gstrtpac3depay.c \
	gstrtpdvdepay.c \
	gstrtpdvpay.c \
	gstrtpilbcdepay.c \
	gstrtpilbcpay.c \
	gstrtpmpadepay.c \
	gstrtpmpapay.c \
	gstrtpmpvdepay.c \
	gstrtpmpvpay.c \
	gstrtppcmadepay.c \
	gstrtppcmudepay.c \
	gstrtppcmupay.c \
	gstrtppcmapay.c \
	gstrtpg726pay.c \
	gstrtpg726depay.c \
	gstrtpg729pay.c \
	gstrtpg729depay.c \
	gstrtpgsmdepay.c \
	gstrtpgsmpay.c \
	gstrtpamrdepay.c \
	gstrtpamrpay.c \
	gstrtph263pdepay.c \
	gstrtph263ppay.c \
	gstrtph263depay.c \
	gstrtph263pay.c \
	gstrtph264depay.c \
	gstrtph264pay.c \
	gstrtpj2kdepay.c \
	gstrtpj2kpay.c \
	gstrtpjpegdepay.c \
	gstrtpjpegpay.c \
	gstrtpL16depay.c \
	gstrtpL16pay.c \
	gstasteriskh263.c \
	gstrtpmp1sdepay.c \
	gstrtpmp2tdepay.c \
	gstrtpmp2tpay.c \
	gstrtpmp4vdepay.c \
	gstrtpmp4vpay.c \
	gstrtpmp4gdepay.c \
	gstrtpmp4gpay.c \
	gstrtpmp4adepay.c \
	gstrtpmp4apay.c \
	gstrtpsirenpay.c \
	gstrtpsirendepay.c \
	gstrtpspeexdepay.c \
	gstrtpspeexpay.c \
	gstrtpsv3vdepay.c \
	gstrtptheoradepay.c \
	gstrtptheorapay.c \
	gstrtpvorbisdepay.c \
	gstrtpvorbispay.c  \
	gstrtpvrawdepay.c  \
	gstrtpvrawpay.c 


if HAVE_WINSOCK2_H
WINSOCK2_LIBS = -lws2_32
else
WINSOCK2_LIBS =
endif

libgstrtp_la_CFLAGS = $(GST_PLUGINS_BASE_CFLAGS) $(GST_CFLAGS)
libgstrtp_la_LIBADD = $(GST_PLUGINS_BASE_LIBS) $(GST_BASE_LIBS) \
		-lgstaudio-@GST_MAJORMINOR@ \
                -lgsttag-@GST_MAJORMINOR@ \
                -lgstrtp-@GST_MAJORMINOR@ $(WINSOCK2_LIBS) \
		$(LIBM)
libgstrtp_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) 
libgstrtp_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS =			\
		 fnv1hash.h 		\
                 gstrtpchannels.h       \
		 gstrtpL16depay.h 	\
		 gstrtpL16pay.h 	\
		 gstrtpac3depay.h 	\
		 gstrtpdvdepay.h 	\
		 gstrtpdvpay.h 	        \
		 gstrtpamrdepay.h 	\
		 gstrtpamrpay.h 	\
		 gstrtpilbcdepay.h      \
		 gstrtpilbcpay.h        \
		 gstrtppcmadepay.h	\
		 gstrtppcmudepay.h	\
		 gstrtppcmupay.h 	\
		 gstrtppcmapay.h 	\
	         gstrtpg726depay.h      \
	         gstrtpg726pay.h        \
	         gstrtpg729depay.h      \
	         gstrtpg729pay.h        \
		 gstrtpgsmdepay.h 	\
		 gstrtpgsmpay.h 	\
		 gstrtpmpadepay.h 	\
		 gstrtpmpapay.h 	\
		 gstrtpmpvdepay.h \
		 gstrtpmpvpay.h \
		 gstrtph263pdepay.h 	\
		 gstrtph263ppay.h 	\
		 gstrtph263depay.h 	\
		 gstrtph263pay.h 	\
		 gstrtph264depay.h 	\
		 gstrtph264pay.h 	\
		 gstrtpj2kdepay.h 	\
		 gstrtpj2kpay.h 	\
		 gstrtpjpegdepay.h 	\
		 gstrtpjpegpay.h 	\
		 gstrtpmp1sdepay.h 	\
		 gstrtpmp2tdepay.h 	\
		 gstrtpmp2tpay.h 	\
		 gstrtpmp4vdepay.h	\
		 gstrtpmp4vpay.h	\
		 gstrtpmp4gdepay.h 	\
		 gstrtpmp4gpay.h	\
		 gstrtpmp4adepay.h      \
		 gstrtpmp4apay.h        \
		 gstrtpdepay.h		\
		 gstasteriskh263.h	\
		 gstrtpsirenpay.h 	\
		 gstrtpsirendepay.h 	\
		 gstrtpspeexdepay.h	\
		 gstrtpspeexpay.h	\
		 gstrtpsv3vdepay.h	\
	 	 gstrtptheoradepay.h    \
	 	 gstrtptheorapay.h      \
	 	 gstrtpvorbisdepay.h    \
	 	 gstrtpvorbispay.h      \
	 	 gstrtpvrawdepay.h      \
	 	 gstrtpvrawpay.h
