#!/bin/sh

read CC SS FF

if [ $CC = "track" ]; then
    omxplayer --no-osd -b --pos 00:00:00 $SS $FF < /tmp/omxcmd
fi
