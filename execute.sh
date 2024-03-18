#! /bin/sh

if [ "$#" -ne 1 ]; then
  echo "Usage: execute.sh <file.s>"
  exit 1
fi

make alpha

gcc -g -no-pie $@ -o program
./program
rm program
