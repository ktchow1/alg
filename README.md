awk -F'[=,]' '{a+=$2; b+=$4; c+=$6; n++} END {print a/n, b/n, c/n}'
