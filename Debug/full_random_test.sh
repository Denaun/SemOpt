#/bin/bash
# First argument: executable folder (OSX/NIX)
# Second argument: debug flag or another parameter to be passed

# Constants
NOW=$(date +"%Y-%m-%d_%H-%M-%S");
TIME_CMD='import time\nprint "{:.0f}".format( time.time() * 10 ** 3 )';

# Useful paths
INSTANCES=`ls ./test-input/random*.dl`;
RESULTS="./results/";
SYNTHESIS="./synthesis/";
EXE="./$1/SemOpt-Opt";
PREOPT_EXE="./$1/SemOpt-PreOpt";

# Solve all the random instances with the preopt exec and the actual one
for i in ${INSTANCES[@]}
do
	FILENAME=$(basename $i);
	OPT_FILENAME=$RESULTS"opt_result_"$FILENAME".txt";
	PREOPT_FILENAME=$RESULTS"preopt_result_"$FILENAME".txt";

	echo ".::Solving $FILENAME::.";

	echo -e "\tPreOpt";
	PREOPT_START=$( echo -e "$TIME_CMD" | python );
	$PREOPT_EXE $2 $i > $PREOPT_FILENAME 2>&1;
	PREOPT_STOP=$( echo -e "$TIME_CMD" | python );

	echo -e "\tOpt";
	OPT_START=$( echo -e "$TIME_CMD" | python );
	$EXE $2 $i > $OPT_FILENAME 2>&1;
	OPT_STOP=$( echo -e "$TIME_CMD" | python );

	SYNTHESIS_FILENAME=$SYNTHESIS"synthesis_result_"$NOW".txt";

	TIME_DIFFERENCE=$(($(($OPT_STOP-$OPT_START))-$(($PREOPT_STOP-$PREOPT_START))));

	echo -e ".::Instance $FILENAME::.\nTime difference: $TIME_DIFFERENCE ms\nDiffering lines:" >> $SYNTHESIS_FILENAME;
	diff $OPT_FILENAME $PREOPT_FILENAME >> $SYNTHESIS_FILENAME;
	echo "" >> $SYNTHESIS_FILENAME;
done;

mkdir $RESULTS$NOW;
# Strange barbatrick
$( $( echo mv $RESULTS"*.txt" $RESULTS$NOW ) );
