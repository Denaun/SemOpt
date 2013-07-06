#/bin/bash

# Useful paths
INSTANCES=`ls ./test-input/random*.dl`;
RESULTS="./results/";
SYNTHESIS="./synthesis/";
EXE="./SemOpt-Opt";
PREOPT_EXE="./SemOpt-PreOpt";

NOW=$(date +"%d-%m-%Y_%H-%M-%S");

# Solve all the random instances with the preopt exec and the actual one
for i in ${INSTANCES[@]}
do
	FILENAME=$(basename $i);
	OPT_FILENAME=$RESULTS"opt_result_"$FILENAME;
	PREOPT_FILENAME=$RESULTS"preopt_result_"$FILENAME;

	echo ".::Solving $FILENAME::.";

	$EXE $1 $i > $OPT_FILENAME 2>&1;
	$PREOPT_EXE $1 $i > $PREOPT_FILENAME 2>&1;

	SYNTHESIS_FILENAME=$SYNTHESIS"synthesis_result"$NOW;

	echo -e ".::Instance $FILENAME::.\nDiffering lines:" >> $SYNTHESIS_FILENAME;
	diff $OPT_FILENAME $PREOPT_FILENAME >> $SYNTHESIS_FILENAME;
	echo "" >> $SYNTHESIS_FILENAME;
done;
