set logscale x 10
set autoscale
set title "Nombre de sommets en fonction du nombre de points"

set terminal png size 640,480 
set output "graham-sommet.png"

plot "graham-disque-sommet.dat" using 1:2 with lines title "Disque", \
     "graham-losange-sommet.dat" using 1:2 with lines title "Losange"