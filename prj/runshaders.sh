#!/bin/bash
for i in shader*; do 
  if test -x $i; then
    echo Running $i
    "./$i"
    for j in rds*graph; do
      mv $j $i-$j
      dot -Tgif -o $i-$j.gif $i-$j > /dev/null 2>&1
    done
  fi
done
