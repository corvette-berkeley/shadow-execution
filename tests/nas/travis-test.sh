#!/bin/bash
export THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$THIS_DIR"

echo $THIS_DIR

bash instrument-all.sh travis-tests.txt 2>/dev/null

while read program
do
	echo "Running $program ..."
	python2 travis-main.py $program
	if [ "$?" != "0" ]
	then
		exit 1
	fi
done < travis-tests.txt
