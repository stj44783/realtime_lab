for i in $(seq 0 11);
do
    sudo cpufreq-set -u 3500000 -c $i -r
done

sleep 5

for i in $(seq 0 11);
do
    sudo cpufreq-set -g performance -c $i -r
done
