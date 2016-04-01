#!/bin/bash

export JAVA_HOME="/usr/lib/jvm/java-7-openjdk-amd64/"
#bin/hadoop jar share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.2.jar grep input output 'to'
bin/hadoop jar share/hadoop/tools/lib/hadoop-*streaming*.jar \
  -D mapred.reduce.tasks=1 \
  -file buildmap.py    -mapper buildmap.py \
  -file buildreduce.py   -reducer buildreduce.py \
  -input input -output output
