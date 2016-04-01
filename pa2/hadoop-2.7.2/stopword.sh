#!/bin/bash

export JAVA_HOME="/usr/lib/jvm/java-7-openjdk-amd64/"
#bin/hadoop jar share/hadoop/mapreduce/hadoop-mapreduce-examples-2.7.2.jar grep input output 'to'
bin/hadoop jar share/hadoop/tools/lib/hadoop-*streaming*.jar \
  -D mapred.reduce.tasks=1 \
  -file stopwordmap.py    -mapper stopwordmap.py \
  -file stopwordreduce.py   -reducer stopwordreduce.py \
  -input input -output stopwords
