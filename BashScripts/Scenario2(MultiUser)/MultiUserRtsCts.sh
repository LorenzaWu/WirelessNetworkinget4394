rm -f output_file_arf.dat
rm -f output_file_aarf.dat
rm -f output_file_aarfcd.dat
>temp1arf.dat
>temp1aarf.dat
>temp1aarfcd.dat


for iteration in $(seq 1 10);
 do
 for node in $(seq 1 12);
  do
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=$node --rtscts=y --wifiMgr=arf" &>>output_file_arf.dat
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=$node --rtscts=y --wifiMgr=aarf" &>>output_file_aarf.dat
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=$node --wifiMgr=aarfcd" &>>output_file_aarfcd.dat
 done
  sed -i "/\b\(waf\|build\|Compiling\)\b/d" output_file_arf.dat
  sed -i "/\b\(waf\|build\|Compiling\)\b/d" output_file_aarf.dat
  sed -i "/\b\(waf\|build\|Compiling\)\b/d" output_file_aarfcd.dat
  paste output_file_arf.dat temp1arf.dat>temp2.dat
  mv temp2.dat temp1arf.dat
  paste output_file_aarf.dat temp1aarf.dat>temp2.dat
  mv temp2.dat temp1aarf.dat
  paste output_file_aarfcd.dat temp1aarfcd.dat>temp2.dat
  mv temp2.dat temp1aarfcd.dat
  rm -f output_file_arf.dat
  rm -f output_file_aarf.dat
  rm -f output_file_aarfcd.dat
done


#Calculating Average of the values
awk '{print ($1+$3+$5+$7+$9+$11+$13+$15+$17+$19)/10 " " ($2+$4+$6+$8+$10+$12+$14+$16+$18+$20)/10}' temp1arf.dat>output_file_arf.dat  
awk '{print ($1+$3+$5+$7+$9+$11+$13+$15+$17+$19)/10 " " ($2+$4+$6+$8+$10+$12+$14+$16+$18+$20)/10}' temp1aarf.dat>output_file_aarf.dat 
awk '{print ($1+$3+$5+$7+$9+$11+$13+$15+$17+$19)/10 " " ($2+$4+$6+$8+$10+$12+$14+$16+$18+$20)/10}' temp1aarfcd.dat>output_file_aarfcd.dat


gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Throughput (Mbps)"
set term png
set output "throughputvsNodes(RTSCTS).png"
plot 'output_file_arf.dat' w l lc rgb 'black' title "ARF",'output_file_aarf.dat' w l lc rgb 'red' title "AARF",'output_file_aarfcd.dat' w l lc rgb 'green' title "AARF-CD"
EOF





