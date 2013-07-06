#/bin/bash

# File generation
INPUT_PATH="./test-input/random_test_";
NOW=$(date +"%d-%m-%Y_%H-%M-%S");

# As first parameter the number of nodes must be inserted
# Generating nodes
for ((i=1; i <= $1; i++));
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
