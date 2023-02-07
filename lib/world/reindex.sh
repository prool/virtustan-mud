#!/bin/sh
# reindex world
# cd mob
# ../reindex.sh
# cd ../obj
# ../reindex.sh
# ...
# PROFIT
#
# prool
#
echo Reindex `pwd`
mv index index.bak
ls -1 [1-9]* | sort -n > index
echo $ >> index
