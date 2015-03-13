# Troubleshooting The Installation #

Installation may take a while. Be patient. If Media Player goes empty after installation you may need to restart your device. Then you have to wait a while for indexing, but if all this fails, then you may have to reset Tracker by issuing the following command in X Terminal as a _normal user_ (not root):

`tracker-processes -r`

Remember that re-indexing your media from scratch may take a very long time, depending on how large your collection is. Be patient. You can check the status of Tracker in X Terminal by issuing the following command as a _normal user_ (not root):

`tracker-status`

"Indexing" means you have to wait, "Paused" means that indexing is done. If it's "Stopped" or any other status, then you may need to restart your device.

# Troubleshooting Playback #

Here are some tips and answers you may be looking for.

## My Video Doesn't Work ##

**Are you sure it's supported in the first place?**

Remember that H.264 Main and High profiles (mainly used in .MKV and .MP4 containers) are not supported. If this sounds like black magic to you, don't worry, all you need to know is that the file extension (.AVI, .MP4, .MKV, .FLV) does not indicate the codec used in that file. These extensions indicate the "container" format. This means that one .MP4 can be perfectly playable, while another .MP4 will refuse to play. [A table of supported formats](http://wiki.maemo.org/N900_Media_Support) is available and [some more information](http://wiki.maemo.org/Video_encoding).

**Are you sure the file is not in High Definition resolution?**

The N900 does not support video resolution over 848x480 pixels. Possibly a few pixels more here and there, but not much more than that. High Definition video is 1280x720 and up (up to 1920x1080). These resolutions are not supported, even if the file is detected by the Media Player.

## Video Playback Is Slow ##

None of the added formats are hardware-accelerated. This means that they need a lot of processing power from the CPU. Even though the CPU in the N900 is powerful, there are limits. Use formats that are supported by default if you want a good experience.

## Video Playback Eats My Battery ##

None of the added formats are hardware-accelerated. This means that they need a lot of processing power from the CPU. This consumes the battery rapidly. It's OK for audio files though.

## Which Formats Should I Use ##

Ideally you should use H.264 Baseline 3.0 for video and AAC for audio. The preferred container format is MP4. The resolution must not exceed 848x480.

## How Do I Convert (Transcode) My Video Files ##

Download both HandBrake profiles from this [wiki page](http://wiki.maemo.org/Video_Encoding_Basic_Guide). Get [HandBrake](http://handbrake.fr). Import the profiles to HandBrake and use them for encoding video files for the N900.

## Other ##

**Can I switch between different video/audio/subtitle tracks in MKV files?**

No. The Media Player is currently closed-source, so we can't do anything about it. The Maemo community has already asked Nokia to open the code, you can [vote](https://bugs.maemo.org/show_bug.cgi?id=1235) for it.

**How can I support the project?**

You can support the project by donating [via PayPal](http://stateless.jogger.pl/files/decoders_donate.html). Your help is very appreciated!

**What's the answer to life the universe and everything?**

[42](http://en.wikipedia.org/wiki/Phrases_from_The_Hitchhiker%27s_Guide_to_the_Galaxy#Answer_to_the_Ultimate_Question_of_Life.2C_the_Universe.2C_and_Everything_.2842.29).