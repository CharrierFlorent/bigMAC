#!/bin/bash

for i in {17..30}; do
	scp bigmac run.sh charrier@lis-cluster-coala-$i:LOCAL/
done
