count=0
total=15
pstr="[==================================================================]"

echo "[                                                                         ]"
echo -e "\033[2A"

while [ $count -lt $total ]; do
  sleep 0.05 # this is work
  count=$(( $count + 1 ))
  pd=$(( $count * 73 / $total ))
  printf "\r%3d.%1d%% %.${pd}s" $(( $count * 100 / $total )) $(( ($count * 1000 / $total) % 10 )) $pstr
done
#echo -e "\033[1A                                                                                                                                                "
echo -e "\033[1K"
echo -e "\033[2A"
