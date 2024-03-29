/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Default audio sink */
#define DEFAULT_AUDIOSINK "autoaudiosink"

/* Default audio source */
#define DEFAULT_AUDIOSRC "alsasrc"

/* Default video sink */
#define DEFAULT_VIDEOSINK "autovideosink"

/* Default video source */
#define DEFAULT_VIDEOSRC "v4l2src"

/* Default visualizer */
#define DEFAULT_VISUALIZER "goom"

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
/* #undef ENABLE_NLS */

/* gettext package name */
#define GETTEXT_PACKAGE "NULL"

/* Defined if gcov is enabled to force a rebuild due to config.h changing */
/* #undef GST_GCOV_ENABLED */

/* Default errorlevel to use */
#define GST_LEVEL_DEFAULT GST_LEVEL_NONE

/* GStreamer license */
#define GST_LICENSE "LGPL"

/* package name in plugins */
#define GST_PACKAGE_NAME "GStreamer FLV Plugins (Maemo)"

/* package origin */
#define GST_PACKAGE_ORIGIN "http://maemo.org"

/* struct v4l2_buffer missing */
/* #undef GST_V4L2_MISSING_BUFDECL */

/* Define to enable aalib ASCII Art library (used by aasink). */
/* #undef HAVE_AALIB */

/* Define to enable XML library (used by annodex). */
/* #undef HAVE_ANNODEX */

/* Define to 1 if you have the `asinh' function. */
#define HAVE_ASINH 1

/* Define to enable bz2 library for matroska . */
/* #undef HAVE_BZ2 */

/* Define to enable Cairo graphics rendering (used by cairo). */
/* #undef HAVE_CAIRO */

/* Define to 1 if you have the `cosh' function. */
#define HAVE_COSH 1

/* Define if the host CPU is an Alpha */
/* #undef HAVE_CPU_ALPHA */

/* Define if the host CPU is an ARM */
/* #undef HAVE_CPU_ARM */

/* Define if the host CPU is a CRIS */
/* #undef HAVE_CPU_CRIS */

/* Define if the host CPU is a CRISv32 */
/* #undef HAVE_CPU_CRISV32 */

/* Define if the host CPU is a HPPA */
/* #undef HAVE_CPU_HPPA */

/* Define if the host CPU is an x86 */
#define HAVE_CPU_I386 1

/* Define if the host CPU is a IA64 */
/* #undef HAVE_CPU_IA64 */

/* Define if the host CPU is a M68K */
/* #undef HAVE_CPU_M68K */

/* Define if the host CPU is a MIPS */
/* #undef HAVE_CPU_MIPS */

/* Define if the host CPU is a PowerPC */
/* #undef HAVE_CPU_PPC */

/* Define if the host CPU is a 64 bit PowerPC */
/* #undef HAVE_CPU_PPC64 */

/* Define if the host CPU is a S390 */
/* #undef HAVE_CPU_S390 */

/* Define if the host CPU is a SPARC */
/* #undef HAVE_CPU_SPARC */

/* Define if the host CPU is a x86_64 */
/* #undef HAVE_CPU_X86_64 */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
/* #undef HAVE_DCGETTEXT */

/* Define to enable DirectSound plug-in (used by directsoundsink). */
/* #undef HAVE_DIRECTSOUND */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to enable raw1394 and avc1394 library (used by 1394). */
/* #undef HAVE_DV1394 */

/* Define to enable ESounD sound daemon (used by esdsink). */
/* #undef HAVE_ESD */

/* Define to enable building of experimental plug-ins. */
#define HAVE_EXPERIMENTAL 

/* Define to enable building of plug-ins with external deps. */
/* #undef HAVE_EXTERNAL */

/* FIONREAD ioctl found in sys/filio.h */
/* #undef HAVE_FIONREAD_IN_SYS_FILIO */

/* FIONREAD ioctl found in sys/ioclt.h */
#define HAVE_FIONREAD_IN_SYS_IOCTL 1

/* Define to enable FLAC lossless audio (used by flac). */
/* #undef HAVE_FLAC */

/* Define if compiler supports gcc inline assembly */
#define HAVE_GCC_ASM 1

/* Define to enable GConf libraries (used by gconfelements). */
/* #undef HAVE_GCONF */

/* Define to enable GConf schemas. */
/* #undef HAVE_GCONFTOOL */

/* Define to enable GDK pixbuf (used by gdkpixbuf). */
/* #undef HAVE_GDK_PIXBUF */

/* Define to 1 if you have the `getpagesize' function. */
#define HAVE_GETPAGESIZE 1

/* Define if the GNU gettext() function is already present or preinstalled. */
/* #undef HAVE_GETTEXT */

/* Define to enable Video 4 Linux 2 (used by v4l2src). */
#define HAVE_GST_V4L2 

/* Define to enable HAL libraries (used by halelements). */
/* #undef HAVE_HAL */

/* Define if you have the iconv() function. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define if we have struct ip_mreqn */
#define HAVE_IP_MREQN 

/* Define to enable jpeg library (used by jpeg). */
/* #undef HAVE_JPEG */

/* Define to enable libcaca coloured ASCII art (used by cacasink). */
/* #undef HAVE_LIBCACA */

/* Define to enable libdv DV demuxer/decoder (used by dv). */
/* #undef HAVE_LIBDV */

/* Define to enable Portable Network Graphics library (used by png). */
/* #undef HAVE_LIBPNG */

/* Define to 1 if you have the `socket' library (-lsocket). */
/* #undef HAVE_LIBSOCKET */

/* soup gnome integration */
/* #undef HAVE_LIBSOUP_GNOME */

/* Whether libv4l2 is available for video buffer conversion */
/* #undef HAVE_LIBV4L2 */

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have a working `mmap' system call. */
#define HAVE_MMAP 1

/* Define to 1 if you have the <netinet/in.h> header file. */
#define HAVE_NETINET_IN_H 1

/* Define to enable OSS audio (used by ossaudio). */
#define HAVE_OSS 

/* Define if OSS includes are in /machine/ */
/* #undef HAVE_OSS_INCLUDE_IN_MACHINE */

/* Define if OSS includes are in / */
/* #undef HAVE_OSS_INCLUDE_IN_ROOT */

/* Define if OSS includes are in /sys/ */
#define HAVE_OSS_INCLUDE_IN_SYS 

/* Define to enable OSX audio (used by osxaudio). */
/* #undef HAVE_OSX_AUDIO */

/* Define to enable OSX video (used by osxvideosink). */
/* #undef HAVE_OSX_VIDEO */

/* Define to enable pulseaudio plug-in (used by pulseaudio). */
/* #undef HAVE_PULSE */

/* defined if pulseaudio >= 0.9.11 is available */
/* #undef HAVE_PULSE_0_9_11 */

/* defined if pulseaudio >= 0.9.12 is available */
/* #undef HAVE_PULSE_0_9_12 */

/* defined if pulseaudio >= 0.9.13 is available */
/* #undef HAVE_PULSE_0_9_13 */

/* Define if RDTSC is available */
/* #undef HAVE_RDTSC */

/* Define to 1 if you have the `rint' function. */
#define HAVE_RINT 1

/* Define to enable Shoutcast/Icecast client library (used by shout2). */
/* #undef HAVE_SHOUT2 */

/* Define to 1 if you have the `sinh' function. */
#define HAVE_SINH 1

/* Define to enable soup http client plugin (2.4) (used by souphttpsrc). */
/* #undef HAVE_SOUP */

/* Define to enable speex speech codec (used by speex). */
/* #undef HAVE_SPEEX */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to enable Sun Audio (used by sunaudio). */
/* #undef HAVE_SUNAUDIO */

/* Define to 1 if you have the <sys/socket.h> header file. */
#define HAVE_SYS_SOCKET_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to enable taglib tagging library (used by taglib). */
/* #undef HAVE_TAGLIB */

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Define if valgrind should be used */
/* #undef HAVE_VALGRIND */

/* Define to enable wavpack plug-in (used by wavpack). */
/* #undef HAVE_WAVPACK */

/* Define to 1 if you have the <winsock2.h> header file. */
/* #undef HAVE_WINSOCK2_H */

/* Define to enable X libraries and plugins (used by ximagesrc). */
#define HAVE_X 

/* Define to enable X Shared Memory extension. */
#define HAVE_XSHM 

/* Define to enable X11 XVideo extensions. */
#define HAVE_XVIDEO 

/* Define to enable zlib support for id3demux/qtdemux/matroska. */
/* #undef HAVE_ZLIB */

/* the host CPU */
#define HOST_CPU "i486"

/* gettext locale dir */
#define LOCALEDIR "/usr/share/locale"

/* Define to 1 if your C compiler doesn't accept -c and -o together. */
/* #undef NO_MINUS_C_MINUS_O */

/* Name of package */
#define PACKAGE "gst-plugins-good"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://bugzilla.gnome.org/enter_bug.cgi?product=GStreamer"

/* Define to the full name of this package. */
#define PACKAGE_NAME "GStreamer Good Plug-ins"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "GStreamer Good Plug-ins 0.10.23"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "gst-plugins-good"

/* Define to the version of this package. */
#define PACKAGE_VERSION "0.10.23"

/* directory where plugins are located */
#define PLUGINDIR "/usr/lib/gstreamer-0.10"

/* The size of `char', as computed by sizeof. */
/* #undef SIZEOF_CHAR */

/* The size of `int', as computed by sizeof. */
/* #undef SIZEOF_INT */

/* The size of `long', as computed by sizeof. */
/* #undef SIZEOF_LONG */

/* The size of `short', as computed by sizeof. */
/* #undef SIZEOF_SHORT */

/* The size of `void*', as computed by sizeof. */
/* #undef SIZEOF_VOIDP */

/* defined if speex 1.0.x API detected */
/* #undef SPEEX_1_0 */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "0.10.23"

/* old wavpack API */
/* #undef WAVPACK_OLD_API */

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef WORDS_BIGENDIAN */

/* Define to 1 if the X Window System is missing or not being used. */
/* #undef X_DISPLAY_MISSING */
