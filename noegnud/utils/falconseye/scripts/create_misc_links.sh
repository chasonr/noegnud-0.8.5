#!/bin/sh

#
# hmm, seems falcons eye developers clean forgot about a few tiles ...
# wonder how these "orphans" are represented in the game itself ?
# ahh well, let's just make em work for noegnud ;)
#

function generic_mons {
    ln -s 'monster.'"$1"'.'"$2".pcx 'monster.'"$1"'.'"$3".pcx
}

function generic_objs {
    ln -s 'object.'"$1"'.'"$2".pcx 'object.'"$1"'.'"$3".pcx
}

function link_wall_type {
    ln -s custom_tiles/_dungeon.pcx cmap.wall."$1".pcx
    ln -s custom_tiles/_gehennom.pcx cmap.wall."$1".gehennom.pcx
    ln -s custom_tiles/_knox.pcx cmap.wall."$1".knox.pcx
    ln -s custom_tiles/_mine.pcx cmap.wall."$1".mine.pcx
    ln -s custom_tiles/_sokoban.pcx cmap.wall."$1".sokoban.pcx
}

function link_warning {
    ln -s custom_tiles/_warning.pcx warning."$1".pcx
}

generic_mons "dog or other canine" "dog" "cerberus"
generic_mons "dragon" "baby silver dragon" "baby shimmering dragon"
generic_mons "dragon" "silver dragon" "shimmering dragon"
generic_mons "eye or sphere" "floating eye" "beholder"
generic_mons "human or elf" "human" "charon"
generic_mons "human or elf" "human" "earendil"
generic_mons "human or elf" "human" "elwing"
generic_mons "human or elf" "elf" "high-elf"
generic_mons "jabberwock" "jabberwock" "vorpal jabberwock"
generic_mons "orc" "goblin" "goblin king"
generic_mons "vampire" "vampire" "vampire mage"

generic_objs "armor" "silver dragon scale mail" "shimmering dragon scale mail"
generic_objs "armor" "silver dragon scales" "shimmering dragon scales"

link_wall_type "bottom left corner"
link_wall_type "bottom right corner"
link_wall_type "crosswall"
link_wall_type "horizontal"
link_wall_type "tee down"
link_wall_type "tee left"
link_wall_type "tee right"
link_wall_type "tee up"
link_wall_type "top left corner"
link_wall_type "top right corner"
link_wall_type "vertical"

link_warning "0"
link_warning "1"
link_warning "2"
link_warning "3"
link_warning "4"
link_warning "5"

# 
# ... and here my arrogance shows itself ;)
# these are tiles that I think are better suited as defaults than falconseye's defaults.
#
cp -f custom_tiles/replacements/* .
