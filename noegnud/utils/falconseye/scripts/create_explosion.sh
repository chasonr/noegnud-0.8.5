#!/bin/sh

function linktype {
    ln -s explosion.dark."$2".pcx explosion."$1"."$2".pcx
}

linktype "$1" "bottom center"
linktype "$1" "bottom left"
linktype "$1" "bottom right"
linktype "$1" "middle center"
linktype "$1" "middle left"
linktype "$1" "middle right"
linktype "$1" "top center"
linktype "$1" "top left"
linktype "$1" "top right"
