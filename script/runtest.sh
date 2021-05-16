#!/bin/bash
scriptDir=$(cd "$(dirname $0)";pwd)
baseDir=$(cd "$scriptDir/../";pwd)

cd $baseDir
rm ./bin/LiteDB
make
rspec ./test/*.rb