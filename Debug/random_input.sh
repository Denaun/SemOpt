#/bin/bash

# File generation
if [[ $# -lt 2 ]]
then
	FOLDER="test-input"
else
	FOLDER=$2
fi

INPUT_PATH="./$2/random_test_";
TIME_CMD='import datetime\nprint datetime.datetime.now().isoformat()';
NOW=$( echo -e "$TIME_CMD" | python );

# As first parameter the number of nodes must be inserted
# Generating nodes
for ((i=1; i <= $1; i++))
do
	echo "arg(a$i)." >> $INPUT_PATH$NOW".dl";
	DIAG=$(($(($1+1))*$(($i-1))+$i));
	not_assignable[$DIAG]=1;
done;

# Then generate a random number of attacks
ATTACKS=$(($RANDOM%$1+$1*3/2));

#echo $ATTACKS >> $INPUT_PATH$NOW".dl";

for ((i=1; i <= $ATTACKS; i++));
do
	j=$(($RANDOM%$1+1));
	k=$(($RANDOM%$1+1));
	
	POS=$(($(($1+1))*$(($j-1))+$k));
	
	while [[ ${not_assignable[$POS]} == 1 ]]
	do
		j=$(($RANDOM%$1+1));
		k=$(($RANDOM%$1+1));
	
		POS=$(($(($1+1))*$(($j-1))+$k));
	done;

	not_assignable[$POS]=1;

	echo "att(a$j,a$k)." >> $INPUT_PATH$NOW".dl";
done;
