#!/bin/sh

VPJ_FILE=../vs/esp32-weather-epd.vpj

# someday just index non-library files?  Reindexing takes forever for 1k+ files!
#dot_d_2vs.sh ${VPJ_FILE} ./platformio/.pio/build/dfrobot_firebeetle2_esp32e/src

#dot_d_2vs.sh ${VPJ_FILE} .pio/build/dfrobot_firebeetle2_esp32e
dot_d_2vs.sh ${VPJ_FILE} .pio/build/photo_painter
cat ${VPJ_FILE} | sed -e 's!\\ ! !g' > e.vpj
mv e.vpj ${VPJ_FILE}

