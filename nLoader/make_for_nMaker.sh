#!/bin/bash -e
cd "$(dirname "$0")"
for i in CAS_OS NONCAS_OS; do
	echo "++++++++++++++++++++++++++++++"
	echo Making nMaker image for $i
	echo "++++++++++++++++++++++++++++++"
	cd ndless_loader
	make clean
	CFLAGS=-D$i make
	make cleantemp
	cd ..
	make -f _Makefile clean
	CFLAGS=-D$i make -f _Makefile
	rm -f casos.h ndless_loader.bin
	mv loader.bin nLoader_$i.bin.tns
done
