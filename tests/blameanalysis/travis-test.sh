#!/bin/bash
export THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$THIS_DIR"

function test_this_dir() {
	while read program
	do
		echo "Instrumenting $program ..."
#		./instrument-forward.sh $program > /dev/null 2>/dev/null
		./instrumentfp2.sh $program > /dev/null 2>/dev/null
		echo "Running $program ..."
		python2 ../travis-main.py $program
		if [ "$?" != "0" ]
		then
			exit 1
		fi

#		sed -i 's/IID [0-9]\+/ IID <ommitted>/' "$program"".out.ba"
#		sed -i 's/IID [0-9]\+/ IID <ommitted>/' "$program"".ref.ba"

if diff <(sort -u "$program"".out_27.ba") <(sort -u "$program"".ref.ba")
		then
			echo "No output change"
			rm "$program"".out_27.ba"
		else
			echo "Output changed!"
			exit 1
		fi
	done < travis-tests.txt
}

for a in `find . -maxdepth 1 -mindepth 1 -type d`
do
	echo $a
	cd $a
	test_this_dir
	cd ..
done
