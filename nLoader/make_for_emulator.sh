#!/bin/bash -e
cd "$(dirname "$0")"
./make_for_nMaker.sh
for i in CAS_OS NONCAS_OS; do
	echo Emulator image $i
	./make_exploit_image.sh nLoader_$i.bin.tns nLoader_$i.img.tns
done
