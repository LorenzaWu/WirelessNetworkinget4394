# Script for Wifi Mixed Scenario

rm -f mixed_aarfcd.dat

for node in $(seq 1 10);
 do
 for iteration in $(seq 1);
  do
  ./waf --run "scratch/wifiTest --nWifi802_11b=$node --propModel=Log --scenario=mixed --wifiMgr=aarfcd" &>>mixed_aarfcd.dat
 done
done

sed -i "/\b\(waf\|build\|Compiling\)\b/d" mixed_aarfcd.dat

gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Throughput (Mbps)"
set term png
set output "throughputvsNodes.png"
plot 'mixed_aarfcd.dat' using 1:2 with lines,'' using 1:3 with lines,'' using 1:4 with lines
EOF





