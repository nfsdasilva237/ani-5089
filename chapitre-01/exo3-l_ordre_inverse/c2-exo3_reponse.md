L'ordre inverse

J’ai ajouté une seconde version du calcul, notée AppliqueOrdreInverse(), qui applique d’abord la translation puis la rotation. C’est l’envers du comportement standard enseigné dans le cours, et c’est précisément ce qui fait apparaître le décalage du point de pivot.

Pour le vérifier, le programme compare les deux résultats sur le même point et la même pose. J’ai testé plusieurs configurations :

1. Cas classique : position $(1,2,3)$, rotation de 90° autour de $y$, point $(0,0,-1)$. Les deux ordres ne donnent pas le même résultat : $(0,2,3)$ contre $(2,2,-1)$. C’est bien le phénomène décrit par le cours, c’est-à-dire le bug lié au point de rotation.

2. Rotation nulle : si l’angle vaut 0, les deux méthodes reviennent au même résultat, puisqu’il n’y a plus de rotation à faire.

3. Translation nulle : même logique, car il ne reste alors qu’une simple rotation et il n’y a pas de décalage à appliquer.

4. Cas particulier intéressant : si la translation est alignée avec l’axe de rotation, les résultats restent identiques même si la rotation n’est pas nulle. C’est une situation où le vecteur de translation est invariant sous la rotation.

Pourquoi il y a des cas où ça coïncide

Mathématiquement, l’ordre standard donne $R \cdot p + t$, tandis que l’ordre inverse donne $R \cdot (p + t) = R \cdot p + R \cdot t$. Les deux expressions sont égales si et seulement si $t = R \cdot t$, donc si la translation est un point fixe de la rotation. Cela se produit naturellement quand $R$ est l’identité, quand $t = 0$, ou encore quand $t$ appartient à l’axe de rotation, puisqu’une rotation ne déplace pas les points situés sur cet axe.

Le fichier associé est exo3_ordre_inverse.cpp.