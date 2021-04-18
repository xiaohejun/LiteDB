#!/bin/bash
scriptDir=$(cd "$(dirname $0)";pwd)
baseDir=$(cd "$scriptDir/../";pwd)

cd $baseDir
make

db=LiteDB
./bin/$db