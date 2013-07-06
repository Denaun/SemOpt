#/bin/bash

# Useful paths
INSTANCES=`ls ./test-input/random*.dl`;
RESULTS="./results/";
EXE="./SemOpt";
PREOPT_EXE="./SemOpt-PreOpt";

# Solve all the random instances with the preopt exec and the actual one
for i in ${INSTANCES[@]}
do
	FILENAME=$(basename $i);
	$EXE $1 $i > $RESULTS"opt_result_"$FILENAME 2>&1;
	$PREOPT_EXE $1 $i > $RESULTS"preopt_result_"$FILENAME 2>&1;
done;
