#!/bin/sh

function linktype {
    ln -s cmap.effect."$2"" beam".pcx zap."$1"."$2".pcx
}

linktype "$1" "horizontal"
linktype "$1" "vertical"
linktype "$1" "left slant"
linktype "$1" "right slant"
