#! /bin/sh
# build a program

USAGE() {
 if [ -n "${1}" ]
 then
   printf 'error: '
   printf -- "$@"
   printf '\n'
  fi
  echo usage: "$(basename "${0}")" 'program-name' 'uno|mega|leonardo'
  exit 1
}

[ -z "${1}" ] && USAGE 'missing argument'


opts=
OPT() {
  opts="${opts} $*"
}

build="${PWD}/build"

OPT -verbose
#OPT -debug-level 10
OPT -build-path "${build}"
OPT -libraries /usr/local/arduino/libraries
OPT -tools /usr/local/arduino/tools
OPT -tools /usr/local/arduino/tools-builder
OPT -hardware /usr/local/arduino/hardware

name="${1}"
board="${2}"

[ -z "${name}" ] && USAGE 'program name missing'
src="${name}"
[ ! -f "${src}" ] && src="${name}.ino"

[ ! -f "${src}" ] && USAGE 'cannot open: %s' "${name}"


case "${board}" in
  (uno)
    OPT -fqbn arduino:avr:uno
    ;;
  (leonardo)
    OPT -fqbn arduino:avr:leonardo
    ;;
  (mega)
    OPT -fqbn arduino:avr:mega
    OPT -prefs build.mcu=atmega1280
    OPT -prefs build.board=AVR_MEGA1280
    ;;
  ("")
    USAGE 'missing board'
    ;;
  (*)
    USAGE 'undefined board: %s' "${board}"
    ;;
esac

rm -rf "${build}"
mkdir "${build}"

arduino-builder ${opts} -compile "${src}"
