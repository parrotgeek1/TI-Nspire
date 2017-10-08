#!/bin/bash
if [ "$#" != 2 ]; then
	echo "usage: $0 <boot2.raw> <base addr>" >&2
	exit 1
fi
hex_size=`(echo 'obase=16;'; wc -c "$1" | awk '{print $1}') | bc`
hex_rounded=$(($(($(($hex_size+512))/512))*512))
cat << EOF
Type into RS232:

loadb $2

Then send $1 with Kermit

Then type:

nand erase 4000 $hex_rounded
nand write $2 4000 $hex_size
setenv bootcmd nand read $2 4000 $hex_size; go $2
saveenv
reset
EOF
