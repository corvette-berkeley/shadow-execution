#!/bin/bash
export THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
cd "$THIS_DIR"

echo $THIS_DIR
#output is sanitized to remove
# the file that the debug info is written to
# the line numbers for safe_assert faults (since they change by version)
# the address of pointers in the program (which are highly volatile)
# the value of undefined values (which are highly volatile)
python2 small-regressions.py tests.txt 2>/dev/null | sed -e 's/debug map \(\/\([^\/]*\)\)*/\2/g' | sed -e 's/line: \([0-9]*\)/line: \*/g' | grep -v -i 'address of' | grep -v -i 'undefined value' > small_regressions_tests.test.out

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
