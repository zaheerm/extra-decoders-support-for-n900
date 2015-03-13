# Introduction #

The purpose of the project is to get some specific missing multimedia formats indexed and supported by the built-in Media Player. There is also support for seamless playback of most supported formats from the File Manager.

# Features #

Currently supported formats (0.4):

  * AC-3 audio in video files (indexed)
  * Vorbis OGG (indexed)
  * FLAC (not indexed, waiting for ogg-support 1.0.6, seeking not supported)
  * Theora (presumably indexed)
  * Musepack (indexed, seeking works, but might fail)
  * FLV container (indexed, some formats used with this container may not be supported)
  * Real Media formats (may not be indexed, but seamless playback from built-in File Manager, seeking works, but might fail)
  * Matroska container (indexed, some formats used with this container may not be supported )
  * MPEG2 Transport Stream (mostly untested, works only for some people)
  * WebM (local WebM files tested, indexed, work fine)



# What Happens Under The Hood #

Tracker is used to register new formats and index them for the Media Player. Standard freedesktop-compliant XML file with mimetypes and a .desktop file are used for seamless playback from the built-in File Manager. GStreamer plugins from custom packages are used, installed and registered for use by the built-in Media Player.