#Scenario 3. Final output on GNUPLOT Mixed_basic_avg.png,Mixed_basic_mob.png,throughputvsNodes(Static).png


>mixed_basic_avg_arf.dat
>mixed_basic_avg_aarf.dat
>mixed_basic_avg_aarfcd.dat
>mixed_basic_mob_arf.dat
>mixed_basic_mob_aarf.dat
>mixed_basic_mob_aarfcd.dat
>mixed_basic_sta_arf.dat
>mixed_basic_sta_aarf.dat
>mixed_basic_sta_aarfcd.dat
>temp_avg_arf.dat
>temp_avg_aarf.dat
>temp_avg_aarfcd.dat
>temp_mob_arf.dat
>temp_mob_aarf.dat
>temp_mob_aarfcd.dat
>temp_sta_arf.dat
>temp_sta_aarf.dat
>temp_sta_aarfcd.dat



for iteration in $(seq 1 5);
 do
 for node in $(seq 2 12);
  do
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=$node --propModel=Log --scenario=mixed --wifiMgr=arf"
  cat avg.dat>>temp_avg_arf.dat
  cat mob.dat>>temp_mob_arf.dat
  cat sta.dat>>temp_sta_arf.dat   
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=$node --propModel=Log --scenario=mixed --wifiMgr=aarf"
  cat avg.dat>>temp_avg_aarf.dat
  cat mob.dat>>temp_mob_aarf.dat
  cat sta.dat>>temp_sta_aarf.dat  
  ./waf --run "scratch/wifiRateAd --nWifi802_11b=$node --propModel=Log --scenario=mixed --wifiMgr=aarfcd"
  cat avg.dat>>temp_avg_aarfcd.dat
  cat mob.dat>>temp_mob_aarfcd.dat
  cat sta.dat>>temp_sta_aarfcd.dat  
 done

  paste temp_avg_arf.dat mixed_basic_avg_arf.dat>temp2.dat
  mv temp2.dat mixed_basic_avg_arf.dat
  paste temp_mob_arf.dat mixed_basic_mob_arf.dat>temp2.dat
  mv temp2.dat mixed_basic_mob_arf.dat
  paste temp_sta_arf.dat mixed_basic_sta_arf.dat>temp2.dat
  mv temp2.dat mixed_basic_sta_arf.dat

  paste temp_avg_aarf.dat mixed_basic_avg_aarf.dat>temp2.dat
  mv temp2.dat mixed_basic_avg_aarf.dat
  paste temp_mob_aarf.dat mixed_basic_mob_aarf.dat>temp2.dat
  mv temp2.dat mixed_basic_mob_aarf.dat
  paste temp_sta_aarf.dat mixed_basic_sta_aarf.dat>temp2.dat
  mv temp2.dat mixed_basic_sta_aarf.dat
  
  paste temp_avg_aarfcd.dat mixed_basic_avg_aarfcd.dat>temp2.dat
  mv temp2.dat mixed_basic_avg_aarfcd.dat
  paste temp_mob_aarfcd.dat mixed_basic_mob_aarfcd.dat>temp2.dat
  mv temp2.dat mixed_basic_mob_aarfcd.dat
  paste temp_sta_aarfcd.dat mixed_basic_sta_aarfcd.dat>temp2.dat
  mv temp2.dat mixed_basic_sta_aarfcd.dat



  rm -f temp_avg_arf.dat
  rm -f temp_mob_arf.dat
  rm -f temp_sta_arf.dat
  rm -f temp_avg_aarf.dat
  rm -f temp_mob_aarf.dat
  rm -f temp_sta_aarf.dat
  rm -f temp_avg_aarfcd.dat
  rm -f temp_mob_aarfcd.dat
  rm -f temp_sta_aarfcd.dat

done


#Calculating Average of the values
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_avg_arf.dat>mixed_basic_final_avg_arf.dat
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_mob_arf.dat>mixed_basic_final_mob_arf.dat
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_sta_arf.dat>mixed_basic_final_sta_arf.dat

awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_avg_aarf.dat>mixed_basic_final_avg_aarf.dat
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_mob_aarf.dat>mixed_basic_final_mob_aarf.dat
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_sta_aarf.dat>mixed_basic_final_sta_aarf.dat

awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_avg_aarfcd.dat>mixed_basic_final_avg_aarfcd.dat
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_mob_aarfcd.dat>mixed_basic_final_mob_aarfcd.dat
awk '{print ($1+$3+$5+$7+$9)/5 " " ($2+$4+$6+$8+$10)/5}' mixed_basic_sta_aarfcd.dat>mixed_basic_final_sta_aarfcd.dat


gnuplot <<- EOF
set xlabel "Number of Nodes"
set ylabel "Network Throughput (Mbps)"
set xrange [2:12]
set term png
set output "Mixed_basic_avg.png"
plot 'mixed_basic_final_avg_arf.dat' w l lc rgb 'black' title "ARF",'mixed_basic_final_avg_aarf.dat' w l lc rgb 'red' title "AARF",'mixed_basic_final_avg_aarfcd.dat' w l lc rgb 'green' title "AARF-CD"

set ylabel "Mobile Node Throughput (Mbps)"
set output "Mixed_basic_mob.png"
plot 'mixed_basic_final_mob_arf.dat' w l lc rgb 'black' title "ARF",'mixed_basic_final_mob_aarf.dat' w l lc rgb 'red' title "AARF",'mixed_basic_final_mob_aarfcd.dat' w l lc rgb 'green' title "AARF-CD"

set ylabel "Static Node Throughput (Mbps)"
set output "throughputvsNodes(Static).png"
plot 'mixed_basic_final_sta_arf.dat' w l lc rgb 'black' title "ARF",'mixed_basic_final_sta_aarf.dat' w l lc rgb 'red' title "AARF",'mixed_basic_final_sta_aarfcd.dat' w l lc rgb 'green' title "AARF-CD"
EOF





