#!/usr/bin/perl -w
use strict;

# launch a gst-launch pipeline for the supplied media file
# use the extension to determine the gst-launch pipeline
# make use of default output sinks

my (%pipes, %cfg);

sub extension
{
  my $path = shift;
  my $ext;

  # get only the bit after the last period.  We don't deal with
  # .tar.gz extensions do we ?
  if ($path =~ /\./)
  {
    $ext = $path;
    $ext =~ s/^.*\.//;
  }
  else { $ext = ""; }

  return $ext;
}

sub read_config
{
  my $command = shift;

  my $config_file = $ENV{HOME}."/.gst";
  if (-e $config_file)
  {
    open CONFIG, $config_file;
    while (<CONFIG>)
    {
      chomp;
      s/#.*//;
      s/\s+$//;
      next unless length;
      my ($var, $value) = split (/\s*=\s*/, $_, 2);
      $cfg{$var} = $value;
    }
    if (!($cfg{AUDIOSINK}))
    {
      print "Please add an AUDIOSINK to $config_file !\n";
    }
    if (!($cfg{VIDEOSINK}))
    {
      print "Please add a VIDEOSINK to $config_file !\n";
    }
  }
  else
  {
    print "No configuration file $config_file found.  You might want to create one.\n";
    print "This is not an error, just a friendly reminder... Check the man page.\n\n";
  }
  if (!defined $cfg{AUDIOSINK})  { $cfg{AUDIOSINK} = "osssink"; }
  if (!defined $cfg{VIDEOSINK})  { $cfg{VIDEOSINK} = "ffmpegcolorspace ! xvimagesink"; }
  if (!defined $cfg{CVS_PATH})   { $cfg{CVS_PATH} =  $ENV{HOME}."/gst/cvs"; }

  if ($command =~ /(.+)\/gst-launch-ext-@GST_MAJORMINOR@$/)
  { $cfg{COMMAND_PATH} = "$1"; }
  else
  { $cfg{COMMAND_PATH} = ""; }
}

sub playfile($$)
{
    my ($file, $ext) = @_;
    my $command;
    my $pipe;
    my $path = "\$PATH:".$cfg{CVS_PATH}."/gstreamer/tools";

    if ($cfg{COMMAND_PATH} ne "") {
      $path = $cfg{COMMAND_PATH}.":$path";
    }

    $ext = lc $ext;

    if ($cfg{VISUALIZER} && ($pipe = $pipes{"vis." . $ext}))
    {
	$command = "gst-launch-@GST_MAJORMINOR@ filesrc location=\"$file\" ! $pipe";
	print "Running command-line\n$command\n\n";
	system ("PATH=$path $command");
    }
    elsif ($pipe = $pipes{$ext})
    {
	$command = "gst-launch-@GST_MAJORMINOR@ filesrc location=\"$file\" ! $pipe";
	print "Running command-line\n$command\n\n";
	system ("PATH=$path $command");
    }
    else
    {
	print "No suitable pipe found for extension $ext.\n";
    }
}

### main

read_config ($0);

%pipes = ( 
  "ac3", "a52dec ! $cfg{AUDIOSINK}",
  "au", "auparse ! $cfg{AUDIOSINK}",
  "avi", "decodebin name=d { d. ! queue ! $cfg{VIDEOSINK} } { d. ! queue ! audioconvert ! audioscale ! $cfg{AUDIOSINK} }",
  "asf", "decodebin name=d ! { d. ! queue ! $cfg{VIDEOSINK} } { d. ! queue ! audioconvert ! audioscale ! $cfg{AUDIOSINK} }",
  "flac", "flacdec ! $cfg{AUDIOSINK}",
  "fli", "flxdec ! colorspace ! $cfg{VIDEOSINK}",
  "m1v", "mpeg2dec ! $cfg{VIDEOSINK}",
  "m2v", "mpeg2dec ! $cfg{VIDEOSINK}",
  "m4a", "qtdemux .audio_00 ! { queue ! faad ! $cfg{AUDIOSINK} }",
  "aac", "faad ! $cfg{AUDIOSINK}",
  "mod", "modplug !  $cfg{AUDIOSINK}",
  "mpc", "musepackdec ! $cfg{AUDIOSINK}",
  "mp2", "mad ! $cfg{AUDIOSINK}",
  "mp3", "mad ! $cfg{AUDIOSINK}",
  "mp4", "decodebin name=d { d. ! queue ! ffmpegcolorspace ! videoscale ! $cfg{VIDEOSINK} } { d. ! queue ! audioconvert ! audioscale ! $cfg{AUDIOSINK} }",
  "mpeg", "mpegdemux name=d { d.audio_00 ! queue ! mad ! audioconvert ! audioscale ! $cfg{AUDIOSINK} } { d.video_00 ! queue ! mpeg2dec ! $cfg{VIDEOSINK} }",
  "mpg", "mpegdemux name=demux { demux.video_00 ! queue ! mpeg2dec ! ffmpegcolorspace ! $cfg{VIDEOSINK} } { demux.audio_00 ! queue ! mad ! $cfg{AUDIOSINK} }",
  "ogg", "oggdemux ! vorbisdec ! audioconvert ! $cfg{AUDIOSINK}",
  "sid", "siddec ! $cfg{AUDIOSINK}",
  "swf", "swfdec name=swfdec ! { queue ! colorspace ! $cfg{VIDEOSINK} }  { swfdec. ! queue ! $cfg{AUDIOSINK} }",
  "vob", "mpegdemux name=d { d.video_00 ! queue ! mpeg2dec ! $cfg{VIDEOSINK} } { d.audio_00 ! queue ! a52dec ! audioconvert ! audioscale ! $cfg{AUDIOSINK} }",
  "wav", "wavparse ! $cfg{AUDIOSINK}",
  "wm", "asfdemux name=demux ! { queue ! spider ! $cfg{VIDEOSINK} } { demux. ! queue ! spider ! $cfg{AUDIOSINK} }",
### a wma file can use wmav1 or wmav2 codec so we must use spider to decode it  
  "wma", "asfdemux name=demux ! spider ! $cfg{AUDIOSINK}",
  "wmv", "asfdemux name=demux ! { queue ! spider ! $cfg{VIDEOSINK} } { demux. ! queue ! spider ! $cfg{AUDIOSINK} }",
  "mkv", "matroskademux name=demux ! { queue ! spider ! $cfg{VIDEOSINK} } { demux. ! queue ! spider ! $cfg{AUDIOSINK} }",
  "mka", "matroskademux ! spider ! $cfg{AUDIOSINK}",
  "mov", "decodebin name=d { d. ! queue ! ffmpegcolorspace ! videoscale ! $cfg{VIDEOSINK} } { d. ! queue ! audioconvert ! audioscale ! $cfg{AUDIOSINK} }",
);

if ($cfg{VISUALIZER}) {
  %pipes = (
    %pipes,
    "vis.mp3", "mad ! tee name=tee silent=true ! queue leaky=1 ! { $cfg{VISUALIZER} ! ffmpegcolorspace ! $cfg{VIDEOSINK} } tee. ! $cfg{AUDIOSINK}",
    "vis.ogg", "vorbisdec ! tee name=tee silent=true ! queue leaky=1 ! { $cfg{VISUALIZER} ! ffmpegcolorspace ! $cfg{VIDEOSINK} } tee. ! $cfg{AUDIOSINK}",
    "vis.wav", "wavparse ! tee name=tee silent=true ! queue leaky=1 ! { $cfg{VISUALIZER} ! ffmpegcolorspace ! $cfg{VIDEOSINK} } tee. ! $cfg{AUDIOSINK}",
  );
}              

if ($#ARGV == -1) {
    print STDERR "Usage: gst-launch-ext-@GST_MAJORMINOR@ filename[s]\n";
    exit 1;
}

my $file;
while ($file = shift @ARGV) {
    my $ext = extension ($file);
    if (!$ext) { 
      print "file $file doesn't have an extension !\n";
      exit;
    }
    if ($ext eq 'm3u')
    {
	open (PLAYLIST, '<', $file);
	my $file2;
	while ($file2 = <PLAYLIST>) {
	    chomp $file2;
	    my $ext2 = extension ($file2);
	    playfile($file2, $ext2);
	}
	close PLAYLIST;
    } else {
	playfile($file, $ext);
    }
}
