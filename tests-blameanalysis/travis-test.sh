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

	sed -i 's/IID [0-9]*/ IID <ommitted>/' "$program"".out.ba"
	sed -i 's/IID [0-9]*/ IID <ommitted>/' "$program"".ref.ba"

	if diff "$program"".out.ba" "$program"".ref.ba"
	then
		echo "No output change"
	else
		echo "Output changed!"
		exit 1
	fi
done < travis-tests.txt
