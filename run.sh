#! /bin/bash
PROGRAM="lu"

echo "Polybench LU. Sequential x OpenMP parallel x Pthread parallel"

# SEQUENTIAL LU
# echo "SEQUENTIAL"
file="./data/${PROGRAM}_sequential.data"
if [ -f "$file" ]
then
	rm $file
fi
for i in {1..11}
do
	 echo "SEQUENTIAL POLYBENCH - EXECUTION $i"
   if [[ $i == 1 ]]; then
     ./${PROGRAM}.out > /dev/null
   else
    # ./${PROGRAM}.out $MSIZE 2 >> testfile.data
     ./${PROGRAM}.out >> ./data/${PROGRAM}_sequential.data 2>&1
  fi
done
cat ./data/${PROGRAM}_sequential.data | sort > sort.data
sed '1d; $d' sort.data > final.data
avgs=$(awk '{s+=$1}END{print s/NR}' RS="\n" final.data)
echo $avgs
rm final.data sort.data


# PTHREAD LU
for t in 2 4 8 16
do
	file="./data/${PROGRAM}_pthread_$t.data"
	if [ -f "$file" ]
	then
		rm $file
	fi
	# echo "PTHREAD FOR $t THREADS"
	for i in {1..11}
	do
		echo "PTHREAD FOR $t THREADS - EXECUTION $i"
		if [[ $i == 1 ]]; then
			./${PROGRAM}_pthread.out $t > /dev/null
		else
			# ./${PROGRAM}.out $MSIZE 2 >> testfile.data
			./${PROGRAM}_pthread.out $t >> ./data/${PROGRAM}_pthread_$t.data 2>&1
		fi
	done
	cat ./data/${PROGRAM}_pthread_$t.data | sort > sort.data
	sed '1d; $d' sort.data > final.data
	# avgp[$((t-1))]=$(awk '{s+=$1}END{print s/NR}' RS="\n" final.data)
	# echo ${avgp[$((t-1))]}
	avgp[$t]=$(awk '{s+=$1}END{print s/NR}' RS="\n" final.data)
	echo ${avgp[$t]}
	rm final.data sort.data
done

#SAVING FILE FOR PLOTING IN GNUPLOT
file="./data/${PROGRAM}_speedup.data"
if [ -f $file ] ; then
    rm $file
fi
for t in 2 4 8 16
do
	# if [[ $t == 0 ]]; then
	# 	echo "1" "1"  "1" >> ./data/$1_speedup.data
	# else
		pthread=$(echo "$avgs / ${avgp[t]}" | bc -l)
		echo $t $pthread >> ./data/${PROGRAM}_speedup.data
 # fi
done
