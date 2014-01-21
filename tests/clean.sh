#/!bin/bash
# clean all core-ulitities programs in the current directory
# usage: ./clean.sh core-utils/all-utilities.txt

for p in `cat $1`
do
  rm "$p.bc"
  rm "$p.s"
  rm "i_$p.bc"
  rm "i_$p.s"
  rm "$p.ll"
  rm "$p.out"
done
