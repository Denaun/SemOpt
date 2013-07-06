#/bin/bash

GENERATOR="./random_input.sh";

# Generate the number of random inputs indicated as first parameter. The dimension of every input is determined randomly in the interval set by the second and third parameters
for ((i=1; i <= $1; i++))
do
	DIMENSION=$(($RANDOM%$(($3-$2))+$2));
	$GENERATOR $DIMENSION;
done;
