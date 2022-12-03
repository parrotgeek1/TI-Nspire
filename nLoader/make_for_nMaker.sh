#!/bin/bash -e
cd "$(dirname "$0")"
for i in CAS_OS NONCAS_OS; do
	echo $i
	echo "#define $i" > casos.h
	cd ndless_loader
	make clean
	make
	make cleanrem
	cd ..
	make -f _Makefile clean
	make -f _Makefile
	rm -f casos.h ndless_loader.bin
	mv loader.bin nLoader_$i.bin.tns
done
