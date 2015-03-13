# Rationale #

1) More robust installation and removal process

Currently mafw is restarted so many times and tracker invoked so many times that it takes ages to install decoders-support and it often breaks tracker.

2) Easier maintanenance

It doesn't matter where the code would be hosted, but all of it needs to be in one place and we need to have all people involved to have the same access to it, so that providing updates is not held up by that one guy who is busy and can't update dependencies in a metapackage. We also need same access level for extras-devel.

3) Less confusion for consumers

Most are not sure what Ogg Support is, and when they install decoders-support they suddenly have ogg-support on their installed applications list.

# Technical stuff #

ogg, flac, vorbis, theora - ogg-support

mkv, rm, musepack, vp8, flv  - decoders-support

1) Move all dependencies of ogg-support to decoders-support instead of the current "decoders-support depends on ogg-support".

2) Upgrade to newest versions and move mimetype registering of ogg, flac, vorbis and theora to decoders-support (afterwards ogg-support needs to be removed from repository, as it will duplicate entries if installed alongside decoders-support).

3) What about upload rights to extras-devel, I have decoders-support, -rm and -musepack, zaheerm has -mkv, -vp8, -flv, kulve has -ogg, -flac, -vorbis, -theora, ogg-support. Fragmentation leads to delays. We need to sort this out.


# Transitional period #
1) Centralize mimetype registering for current set of decoders owned by decoders-support. (in progress, 0.4)

2) Have ogg-support centralize its mimetype registering of owned decoders, then make decoders-support depend on that version of ogg-support (0.5?)

3) Move on to centralize it further into decoders-support, therefore phasing out ogg-support. (1.0?)