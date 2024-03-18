#! /bin/sh

if [ "$#" -ne 1 ]; then
  echo "Usage: run.sh <file.alpha>"
  exit 1
fi

make alpha
./alpha run "$@"
