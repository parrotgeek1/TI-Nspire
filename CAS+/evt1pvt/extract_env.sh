#!/bin/bash
if [ "$#" != 2 ]; then
	echo "usage: $0 <boot1.bin> <out.txt>" >&2
	exit 1
fi
dd if="$1" bs=1 count=32764 skip=262148 2>/dev/null | tr -s '\0' '\n' | dos2unix -q > "$2"
