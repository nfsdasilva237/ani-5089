L'ordre inverse

J'ai ajouté une seconde fonction, AppliqueOrdreInverse(), qui fait la translation avant la rotation — l'inverse de ce que fait le cours.

Le programme affiche les deux résultats pour la même pose et le même point. Quatre cas testés :

1. Cas général (position (1,2,3), rotation 90° autour de y, point (0,0,-1)) : les deux ordres donnent des résultats différents, (0, 2, 3) contre (2, 2, -1). C'est le comportement attendu — c'est le bug du pivot que décrit le cours.

2. Avec une rotation identité (pas de vraie rotation) : les deux
   ordres coïncident, logiquement, puisqu'il n'y a rien à "mal
   ordonner".

3. Avec une translation nulle : les deux ordres coïncident aussi, puisque seule la rotation compte alors.

4. Cas plus surprenant que j'ai trouvé en cherchant : même avec une translation ET une rotation non nulles, si la translation est alignée sur l'axe de rotation (ici, translation le long de l'axe y, rotation autour de l'axe y), les deux ordres coïncident aussi.

Pourquoi ça coïncide dans ces cas

Mathématiquement, "ordre correct" donne R·p + t, et "ordre inverse" donne R·(p+t) = R·p + R·t. Les deux sont égaux si et seulement si t = R·t, c'est-à-dire si la translation t est un point fixe de la rotation R. C'est vrai dans deux cas triviaux (R = identité, ou t = 0), mais aussi dans un cas moins évident : quand t se trouve exactement sur l'axe de rotation, puisqu'une rotation ne déplace pas les points situés sur son propre axe.

Fichier joint : exo3_ordre_inverse.cpp