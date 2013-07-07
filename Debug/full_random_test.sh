#/bin/bash

# Constants
NOW=$(date +"%d-%m-%Y_%H-%M-%S");
TIME_CMD='import time\nprint "{:.0f}".format( time.time() * 10 ** 3 )';

# Useful paths
INSTANCES=`ls ./test-input/random*.dl`;
RESULTS="./results/";
SYNTHESIS="./synthesis/";
EXE="./SemOpt-Opt";
PREOPT_EXE="./SemOpt-PreOpt";

# Solve all the random instances with the preopt exec and the actual one
for i in ${INSTANCES[@]}
do
	FILENAME=$(basename $i);
	OPT_FILENAME=$RESULTS"opt_result_"$FILENAME;
	PREOPT_FILENAME=$RESULTS"preopt_result_"$FILENAME;

	echo ".::Solving $FILENAME::.";

	OPT_START=$( echo -e "$TIME_CMD" | python );
	$EXE $1 $i > $OPT_FILENAME 2>&1;
	OPT_STOP=$( echo -e "$TIME_CMD" | python );

	PREOPT_START=$( echo -e "$TIME_CMD" | python );
	$PREOPT_EXE $1 $i > $PREOPT_FILENAME 2>&1;
	PREOPT_STOP=$( echo -e "$TIME_CMD" | python );

	SYNTHESIS_FILENAME=$SYNTHESIS"synthesis_result"$NOW;

	TIME_DIFFERENCE=$(($(($OPT_STOP-$OPT_START))-$(($PREOPT_STOP-$PREOPT_START))));

	echo -e ".::Instance $FILENAME::.\nTime difference: $TIME_DIFFERENCE ms\nDiffering lines:" >> $SYNTHESIS_FILENAME;
	diff $OPT_FILENAME $PREOPT_FILENAME >> $SYNTHESIS_FILENAME;
	echo "" >> $SYNTHESIS_FILENAME;
done;
