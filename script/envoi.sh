#!/bin/bash

for i in {17..30}; do
	scp -r src/ charrier@lis-cluster-coala-$i:LOCAL/
done
