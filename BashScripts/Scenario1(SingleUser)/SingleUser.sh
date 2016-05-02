rm -f throughput_single_userarf.dat
rm -f throughput_single_useraarf.dat
rm -f throughput_single_useraarfcd.dat
rm -f throughput_single_userconstant.dat
>temparf.dat
>tempconstant.dat
>tempaarf.dat
>tempaarfcd.dat

#6 Simulation Runs
 for iteration in $(seq 1 6);
  do
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=1 --propModel=Log --scenario=single --wifiMgr=arf"
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=1 --propModel=Log --scenario=single --wifiMgr=constant"
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=1 --propModel=Log --scenario=single --wifiMgr=aarf"
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=1 --propModel=Log --scenario=single --wifiMgr=aarfcd"
   sed -i "/\b\(plot\|e\)\b/d" throughput_single_userarf.dat
   sed -i "/\b\(plot\|e\)\b/d" throughput_single_userconstant.dat
   sed -i "/\b\(plot\|e\)\b/d" throughput_single_useraarf.dat
   sed -i "/\b\(plot\|e\)\b/d" throughput_single_useraarfcd.dat

   paste throughput_single_userarf.dat temparf.dat>temp2.dat
   mv temp2.dat temparf.dat
   paste throughput_single_userconstant.dat tempconstant.dat>temp2.dat
   mv temp2.dat tempconstant.dat
   paste throughput_single_useraarf.dat tempaarf.dat>temp2.dat
   mv temp2.dat tempaarf.dat
   paste throughput_single_useraarfcd.dat tempaarfcd.dat>temp2.dat
   mv temp2.dat tempaarfcd.dat
 done
  
#Calculating Average of the values
awk '{print ($1+$3+$5+$7+$9+$11)/6 " " ($2+$4+$6+$8+$10+$12)/6}' temparf.dat>throughput_single_userarf.dat  
awk '{print ($1+$3+$5+$7+$9+$11)/6 " " ($2+$4+$6+$8+$10+$12)/6}' tempconstant.dat>throughput_single_userconstant.dat
awk '{print ($1+$3+$5+$7+$9+$11)/6 " " ($2+$4+$6+$8+$10+$12)/6}' tempaarf.dat>throughput_single_useraarf.dat  
awk '{print ($1+$3+$5+$7+$9+$11)/6 " " ($2+$4+$6+$8+$10+$12)/6}' tempaarfcd.dat>throughput_single_useraarfcd.dat  

gnuplot <<- EOF
set xlabel "Distance(Meters)"
set ylabel "Throughput (Mbps)"
set xrange [20:160]
set term png
set output "throughputvsDistance(SingleUser).png"
plot 'throughput_single_userarf.dat' w l lc rgb 'black' title "ARF",'throughput_single_userconstant.dat' w l lc rgb 'red' title "Constant",'throughput_single_useraarf.dat' w l lc rgb 'green' title "AARF",'throughput_single_useraarfcd.dat' w l lc rgb 'blue' title "AARFCD"
EOF





