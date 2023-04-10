#!/bin/bash

set -euo pipefail

cd "$(dirname "$0")"

boot2_image="tinspirecx_boot2_4.5.0.14.img.tns"
boot1p5_image="boot1.5_4.4.img.tns"

./make_for_nMaker.sh

for i in CAS_OS NONCAS_OS; do
	echo "==================================="
	echo "Making emulator image for $i"
	echo "==================================="

	./make_exploit_image.sh nLoader_$i.bin.tns "$boot1p5_image" "$boot2_image" nLoader_$i.img.tns 
done
