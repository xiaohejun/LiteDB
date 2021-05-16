#!/bin/bash
scriptDir=$(cd "$(dirname $0)";pwd)
baseDir=$(cd "$scriptDir/../";pwd)

cd $baseDir
make

db=LiteDB
rm test.db
./bin/$db test.db