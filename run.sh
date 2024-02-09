#! /bin/sh

if [ "$#" -ne 1 ]; then
  echo "Usage: run.sh <file>"
  exit 1
fi

make build/alpha
./build/alpha run "$@"
