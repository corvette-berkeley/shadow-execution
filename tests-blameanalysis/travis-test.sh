#!/bin/bash
export THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$THIS_DIR"

echo $THIS_DIR

while read program
do
	echo "Instrumenting $program ..."
	./instrument-forward.sh $program > /dev/null 2>/dev/null
	echo "Running $program ..."
	python2 travis-main.py $program
	if [ "$?" != "0" ]
	then
		exit 1
	fi
done < travis-tests.txt
