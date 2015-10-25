#!/usr/bin/env bash

# much of this was copied shamelessly from osquery

set -e

# SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
# SOURCE_DIR="$SCRIPT_DIR/../.."
# BUILD_DIR="$SOURCE_DIR/build/linux"
# export PATH="$PATH:/usr/local/bin"

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
PROJECT_ROOT="$SCRIPT_DIR/../.."
PROJECT_BUILD_DIR="$PROJECT_ROOT/build"
BUILD_DIR="$PROJECT_BUILD_DIR/deb"

PROJECT_BIN_BUILD_DIR="$PROJECT_ROOT/build/bin"
BUILT_SERVER_BINARY="$PROJECT_BIN_BUILD_DIR/src/relevanced_static"

PACKAGE_VERSION="0.9.4"

PACKAGE_ARCH=`uname -m`
PACKAGE_ITERATION=""
PACKAGE_TYPE=""
DESCRIPTION="A server for persistent text-similarity models."
PACKAGE_NAME="relevanced"
if [[ $PACKAGE_VERSION == *"-"* ]]; then
  DESCRIPTION="$DESCRIPTION (unstable/latest version)"
fi
OUTPUT_PKG_PATH="$BUILD_DIR/$PACKAGE_NAME-$PACKAGE_VERSION."

# Config files
INITD_SRC="$SCRIPT_DIR/relevanced.initd"
INITD_DST="/etc/init.d/relevanced"
RELEVANCED_LOG_DIR="/var/log/relevanced/"
RELEVANCED_VAR_DIR="/var/lib/relevanced"
RELEVANCED_ETC_DIR="/etc/relevanced"

WORKING_DIR=/tmp/relevanced_packaging
INSTALL_PREFIX=$WORKING_DIR/prefix

function log() {
  echo "[+] $1"
}

function fatal() {
  echo "[!] $1"
  exit 1
}

function usage() {
  fatal "Usage: $0 -t deb|rpm -i REVISION -d DEPENDENCY_LIST"
}

function parse_args() {
  while [ "$1" != "" ]; do
    case $1 in
      -t | --type )           shift
                              PACKAGE_TYPE=$1
                              ;;
      -i | --iteration )      shift
                              PACKAGE_ITERATION=$1
                              ;;
      -d | --dependencies )   shift
                              PACKAGE_DEPENDENCIES="${@}"
                              ;;
      -h | --help )           usage
                              ;;
    esac
    shift
  done
}

function check_parsed_args() {
  if [[ $PACKAGE_TYPE = "" ]] || [[ $PACKAGE_ITERATION = "" ]]; then
    usage
  fi

  OUTPUT_PKG_PATH=$OUTPUT_PKG_PATH$PACKAGE_TYPE
}

function main() {
  parse_args $@
  check_parsed_args

  rm -rf $WORKING_DIR
  rm -f $OUTPUT_PKG_PATH
  mkdir -p $INSTALL_PREFIX
  rm -rf $INSTALL_PREFIX/*
  mkdir -p $BUILD_DIR
  log "copying binaries"
  BINARY_INSTALL_DIR="$INSTALL_PREFIX/usr/bin/"
  mkdir -p $BINARY_INSTALL_DIR
  cp $BUILT_SERVER_BINARY $BINARY_INSTALL_DIR/relevanced
  chmod g-w $BINARY_INSTALL_DIR/relevanced
  strip $BINARY_INSTALL_DIR/*

  # Create the prefix log dir and copy source configs
  log "copying configurations"
  mkdir -p $INSTALL_PREFIX/$RELEVANCED_VAR_DIR/data
  mkdir -p $INSTALL_PREFIX/$RELEVANCED_LOG_DIR
  mkdir -p $INSTALL_PREFIX/$RELEVANCED_ETC_DIR

  mkdir -p `dirname $INSTALL_PREFIX$INITD_DST`
  cp $SCRIPT_DIR/relevanced.json $INSTALL_PREFIX/$RELEVANCED_ETC_DIR/relevanced.json
  cp $INITD_SRC $INSTALL_PREFIX$INITD_DST

  chmod g-w $INSTALL_PREFIX$INITD_DST
  chmod a+x $INSTALL_PREFIX$INITD_DST

  log "creating package"
  IFS=',' read -a deps <<< "$PACKAGE_DEPENDENCIES"
  PACKAGE_DEPENDENCIES=
  for element in "${deps[@]}"
  do
    element=`echo $element | sed 's/ *$//'`
    PACKAGE_DEPENDENCIES="$PACKAGE_DEPENDENCIES -d \"$element\""
  done

  OS="ubuntu"
  DISTRO="trusty"
  FPM="fpm"

  CMD="$FPM -s dir -t $PACKAGE_TYPE       \
    -n $PACKAGE_NAME -v $PACKAGE_VERSION  \
    --iteration $PACKAGE_ITERATION        \
    -a $PACKAGE_ARCH                      \
    $PACKAGE_DEPENDENCIES                 \
    -p $OUTPUT_PKG_PATH                   \
    --config-files etc/relevanced/relevanced.json \
    --url http://www.relevanced.org       \
    -m scott.ivey@gmail.com               \
    --license MIT                         \
    --description \"$DESCRIPTION\"        \
    \"$INSTALL_PREFIX/=/\""
  eval "$CMD"
  log "package created at $OUTPUT_PKG_PATH"
}

main $@
