#!/bin/bash
(printf '\x80\x00\x80\x46UBootI\x80\x1350D\x80\x84\x11\x08\x00\x00'; wc -c "$1" | tr -s ' ' | cut -d ' ' -f 2 | xargs printf '807F%08X' | xxd -r -p; cat "$1"; printf '\x02\xF0\xFF\xF0') > uboot2.img
