set logscale x 10
set yrange [-1:20] 
set xrange [5:1000]
set title "Temps de calcul en fonction du nombre de points"

set terminal png size 640,480 
set output "jarvis-graham-losange-temps.png"

filenames = "graham-losange-temps jarvis-losange-temps "
plot for [file in filenames] file.".dat" using 1:2 with lines title file

