#/bin/bash
# First argument: executable folder (OSX/NIX)
# Second argument: version
# Third argument: test folder
# Fourth argument: debug flag or another parameter to be passed

# Constants
NOW=$(date +"%Y-%m-%d_%H-%M-%S");
TIME_CMD='import time\nprint "{:.0f}".format( time.time() * 10 ** 1 )';

# Useful paths
INSTANCES=`ls $3random*.dl`;
RESULTS="./debug/";
EXE="./$1/SemOpt-$2";

# Solve all the random instances with the preopt exec and the actual one
for i in ${INSTANCES[@]}
do
	FILENAME=$(basename $i);
	FILENAME=$RESULTS$FILENAME;

	echo ".::Solving $FILENAME::.";

	$EXE $4 $i > $FILENAME 2>&1;
done;
