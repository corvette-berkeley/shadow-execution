#/!bin/bash
# clean all core-ulitities programs in the current directory
# usage: ./clean.sh core-utils/all-utilities.txt

for p in `cat $1`
do
  svn rm "$p.bc"
  svn rm "$p.s"
  svn rm "i_$p.bc"
  svn rm "i_$p.s"
  svn rm "$p.ll"
  svn rm "$p.out"
done
