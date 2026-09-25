Méthode : durée = 1000 / fréquence (ms), puis je retire 8 ms de pipeline fixe
(capteurs + transmission + compositeur + affichage) pour connaître le budget
qui reste à mon code.

 Cadence  Durée d'une image  Reste pour mon code 

 72 Hz       13,9 ms             5,9 ms 
 90 Hz       11,1 ms             3,1 ms 
 120 Hz      8,3 ms              0,3 ms 

À 120 Hz il ne reste quasiment plus rien : plus la cadence est haute,moins le code a de marge.