#!/bin/bash
scriptDir=$(cd "$(dirname $0)";pwd)
baseDir=$(cd "$scriptDir/../";pwd)

cd $baseDir
rspec ./test/*.rb