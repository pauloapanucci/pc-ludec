#! /bin/bash
PROGRAM="cholesky"
# out1="./${PROGRAM}.out"
# out2="./${PROGRAM}_omp.out"
# out3="./${PROGRAM}_pthread.out"
# if [ -f "$out1" ] -o [ -f "$out2" ] -o [ -f "$out3" ];
# then
# 	echo "Please, compile your code using <make compile> before runing the script"
# 	exit 1
# fi

echo "Polybench Cholesky. Sequential x OpenMP parallel x Pthread parallel"

# SEQUENTIAL CHOLESKY
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

# SEQUENTIAL CHOLESKY
# echo "SEQUENTIAL"
file="./data/${PROGRAM}_developed_sequential.data"
if [ -f "$file" ]
then
	rm $file
fi
for i in {1..11}
do
	 echo "SEQUENTIAL DEVELOPED - EXECUTION $i"
   if [[ $i == 1 ]]; then
     ./${PROGRAM}_developed.out > /dev/null
   else
    # ./${PROGRAM}_developed.out $MSIZE 2 >> testfile.data
     ./${PROGRAM}_developed.out >> ./data/${PROGRAM}_developed_sequential.data 2>&1
  fi
done
cat ./data/${PROGRAM}_developed_sequential.data | sort > sort.data
sed '1d; $d' sort.data > final.data
avgsd=$(awk '{s+=$1}END{print s/NR}' RS="\n" final.data)
echo $avgs
rm final.data sort.data

# OMP CHOLESKY
for t in 2 4 8 16 32
do
  # echo "OMP FOR $t THREADS"
  file="./data/${PROGRAM}_omp_$t.data"
  if [ -f "$file" ]
  then
  	rm $file
  fi
  for i in {1..11}
  do
		 echo "OMP FOR $t THREADS - EXECUTION $i"
     if [[ $i == 1 ]]; then
       ./${PROGRAM}_omp.out $t > /dev/null
     else
      # ./${PROGRAM}.out $MSIZE 2 >> testfile.data
       ./${PROGRAM}_omp.out $t >> ./data/${PROGRAM}_omp_$t.data 2>&1
    fi
  done
  cat ./data/${PROGRAM}_omp_$t.data | sort > sort.data
  sed '1d; $d' sort.data > final.data
  # avgomp[$((t-1))]=$(awk '{s+=$1}END{print s/NR}' RS="\n" final.data)
  # echo ${avgomp[$((t-1))]}
	avgomp[$t]=$(awk '{s+=$1}END{print s/NR}' RS="\n" final.data)
  echo ${avgomp[$t]}
  rm final.data sort.data
done

# PTHREAD CHOLESKY
for t in 2 4 8 16 32
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
for t in 2 4 8 16 32
do
	# if [[ $t == 0 ]]; then
	# 	echo "1" "1"  "1" >> ./data/$1_speedup.data
	# else
		pthread=$(echo "$avgs / ${avgp[t]}" | bc -l)
		omp=$(echo "$avgs / ${avgomp[t]}" | bc -l)
		pthread_d=$(echo "$avgsd / ${avgp[t]}" | bc -l)
		omp_d=$(echo "$avgsd / ${avgomp[t]}" | bc -l)
		echo $t $omp $pthread >> ./data/${PROGRAM}_speedup.data
		echo $t $omp_d $pthread_d >> ./data/${PROGRAM}_speedup.data
 # fi
done
