#!/bin/bash
if [ "$#" != 2 ]; then
	echo "usage: $0 <in.txt> <boot1.bin>" >&2
	exit 1
fi
cat "$1" | dos2unix -q | tr '\n' '\0' | dd bs=32764 count=1 conv=sync 2>/dev/null | dd of="$2" seek=262148 bs=1 conv=notrunc 2>/dev/null
# fix env crc
dd if="$2" bs=1 count=32764 skip=262148 2>/dev/null | crc32 /dev/stdin | grep -o .. | tail -r | tr -d '\n' | xxd -r -p | dd of="$2" bs=1 count=4 seek=262144 conv=notrunc 2>/dev/null
