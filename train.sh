#!/bin/bash

for i in {1..10}
do
	for j in {1..5}
	do
		time ./train ${i}00 ./model_init.txt data/train_seq_0${j}.txt model_0${j}.txt
	done
	./test modellist.txt data/test_seq.txt result.txt > re.out
	./test_acc ./re.out ./data/test_lbl.txt > acc_${i}.out
done
