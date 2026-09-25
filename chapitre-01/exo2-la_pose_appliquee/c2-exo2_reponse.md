La pose appliquée

J'ai fait une structure Pose (position + quaternion), avec une fonction
Applique() qui suit l'ordre du cours : rotation d'abord, translation
ensuite (p_espace = orientation * p_entite + position).

Le programme lit une position (3 réels), un quaternion (4 réels, déjà
normalisé), puis un point (3 réels), et affiche le point transformé.

Testé avec deux cas :
- Position (1,2,3), quaternion identité (0,0,0,1), point (0,0,0) →
  résultat (1.0000, 2.0000, 3.0000). Logique : sans rotation, on
  applique juste la translation.
- Position (0,0,0), quaternion représentant une rotation de 90° autour
  de l'axe y (0, 0.7071, 0, 0.7071), point (0,0,-1) — c'est-à-dire le
  vecteur Avant() du chapitre précédent → résultat (-1.0000, 0.0000,
  -0.0000), qui correspond exactement à Droite(). Ça confirme que la
  rotation fonctionne dans le bon sens.

Fichier joint : exo2_pose_appliquee.cpp