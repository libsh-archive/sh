#!/bin/bash
for i in shader*; do 
  if test -x $i; then
    "./$i"
    for j in rds*graph; do
      mv $j $i-$j
      dot -Tgif -o $i-$j.gif $i-$j
    done
  fi
done
