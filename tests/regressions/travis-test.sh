#!/bin/bash
export THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$THIS_DIR"

echo $THIS_DIR

python2 small-regressions.py tests.txt | grep -v 'writing debug map' > small_regressions_tests.test.out

if diff small_regressions_tests.ref small_regressions_tests.test.out
then
	if diff log.ref log.out
	then
		echo "No change!"
		exit 0
	else
		echo "Change!"
		exit 1
	fi
else
	echo "Change!"
	exit 1
fi
