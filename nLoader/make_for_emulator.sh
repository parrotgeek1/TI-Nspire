#!/bin/bash -e
cd "$(dirname "$0")"
b2="tinspirecx_boot2_4.5.0.14.img.tns"
b15="boot1.5_4.4.img.tns"
./make_for_nMaker.sh
for i in CAS_OS NONCAS_OS; do
	echo "++++++++++++++++++++++++++++++"
	echo Making emulator image for $i
	echo "++++++++++++++++++++++++++++++"
	./make_exploit_image.sh nLoader_$i.bin.tns nLoader_$i.img.tns "$b2" "$b15"
done
