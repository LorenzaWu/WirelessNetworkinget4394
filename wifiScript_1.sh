rm -f output_file_aarfcd.dat

for node in $(seq 1 15);
 do
 for iteration in $(seq 1);
  do
  ./waf --run "scratch/wifiTest --nWifi802_11b=$node --wifiMgr=aarfcd" &>>output_file_aarfcd.dat
 done
done

sed -i "/\b\(waf\|build\|Compiling\)\b/d" output_file_arf.dat
sed -i "/\b\(waf\|build\|Compiling\)\b/d" output_file_aarf.dat
sed -i "/\b\(waf\|build\|Compiling\)\b/d" output_file_aarfcd.dat

gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Throughput (Mbps)"
set term png
set output "throughputvsNodes.png"
plot 'output_file_arf.dat' w l lc rgb 'black' title "ARF",'output_file_aarf.dat' w l lc rgb 'red' title "AARF",'output_file_aarfcd.dat' w l lc rgb 'green' title "AARF-CD"
EOF





