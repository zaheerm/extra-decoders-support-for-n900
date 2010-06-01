#!/bin/sh
#
# A simple RTP server 
#

# change these to change the server sync. This causes the server to send the
# packets largly out-of-sync, the client should use the RTCP SR packets to
# restore proper lip-sync between the streams.
AOFFSET=0
VOFFSET=0

VCAPS="video/x-raw-yuv,width=352,height=288,framerate=15/1"

DEST=192.168.1.126

gst-launch -v gstrtpbin name=rtpbin \
           v4l2src ! $VCAPS ! videorate ! ffmpegcolorspace ! ffenc_h263p ! rtph263ppay ! rtpbin.send_rtp_sink_0      \
                     rtpbin.send_rtp_src_0 ! queue ! udpsink host=$HOST port=5000 ts-offset=$AOFFSET      \
                     rtpbin.send_rtcp_src_0 ! udpsink host=$HOST port=5001 sync=false async=false         \
                     udpsrc port=5005 ! rtpbin.recv_rtcp_sink_0                                           \
           alsasrc ! audioconvert ! amrnbenc ! rtpamrpay ! rtpbin.send_rtp_sink_1                         \
	             rtpbin.send_rtp_src_1 ! queue ! udpsink host=$HOST port=5002 ts-offset=$VOFFSET      \
	             rtpbin.send_rtcp_src_1 ! udpsink host=$HOST port=5003 sync=false async=false         \
                     udpsrc port=5007 ! rtpbin.recv_rtcp_sink_1
