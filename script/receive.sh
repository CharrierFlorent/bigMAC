#!/bin/bash

for i in {17..30}; do
	scp charrier@lis-cluster-coala-$i:LOCAL/nohup.out ./
	mv nohup.out $i
done
