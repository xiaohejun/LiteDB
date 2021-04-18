#!/bin/bash
scriptDir=$(cd "$(dirname $0)";pwd)
baseDir=$(cd "$scriptDir/../";pwd)

cd $baseDir
make
rspec ./test/*.rb