#! /bin/sh

name=Reader

USAGE() {
 if [ -n "${1}" ]
 then
   printf 'error: '
   printf -- "$@"
   printf '\n'
  fi
  echo usage: "$(basename "${0}")" 'isp|arduino'
  exit 1
}

[ -z "${1}" ] && USAGE 'missing argument'


opts=
OPT() {
  opts="${opts} $*"
}

build="${PWD}/build"

hex="${build}/${name}.ino.hex"

OPT -C /usr/local/etc/arduino-avrdude.conf

OPT -p atmega1280
OPT -P /dev/cuaU0

# do not chip-erase or bootloader is removed
OPT -D

OPT -U flash:w:"${hex}"

case "${1}" in
  (isp)
    OPT -c avrisp
    OPT -b 19200
    ;;
  (arduino)
    OPT -c arduino
    OPT -b 57600
    ;;
  (*)
    USAGE 'undefined programmer: %s' "${1}"
    ;;
esac


doas arduino-avrdude ${opts}
