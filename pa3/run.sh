#!/bin/bash

iters="25"

export JAVA_HOME="/usr/lib/jvm/default-java"
mkdir -p input
if [ -d output ]; then
  while true; do
    read -p "Output directory detected. Delete? " yn
    case $yn in
        [Yy]* ) echo "Deleting output directory."; rm -rf output; break;;
        [Nn]* ) echo "Okay. Hadoop is going to shit itself for no reason, then."; exit;;
        * ) echo "I didn't understand your input."; exit;;
    esac
  done
fi

echo "Preprocessing."
./preprocess.pl < data.xml > input/input1.txt
echo "Finished preprocessing. Vomiting Java junk."
for i in $(seq 1 $iters); do
  bin/hadoop jar share/hadoop/tools/lib/hadoop-*streaming*.jar \
    -D mapred.reduce.tasks=1 \
    -file prmap.py    -mapper prmap.py \
    -file prreduce.py   -reducer prreduce.py \
    -input input -output output
  cp output/part-00000 output${i}.txt
  ./reprocess.pl input/input${i}.txt < output/part-00000 > input/input$(($i + 1)).txt
  rm input/input${i}.txt
  cp input/input$(($i + 1)).txt input$(($i + 1)).txt
  if [[ $i -lt $iters ]]; then
    rm -rf output
  fi
done
