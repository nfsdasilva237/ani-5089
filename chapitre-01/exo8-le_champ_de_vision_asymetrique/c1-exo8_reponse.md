
Alors déjà, avant de chercher, faut comprendre ce qu'on cherche vraiment.
Le cours parle de 4 angles séparés pour un œil (gauche, droite, haut, bas), parce que la lentille n'est pas centrée sur l'œil. Sauf que quand j'ai été chercher les specs des casques du commerce, aucun fabricant ne donne ces 4 angles-là. Ils donnent juste un FOV global, genre "96degrés", point.

J'ai regardé les fiches produit Meta pour le Quest 3 par exemple : ils annoncent "96 degrés field of view", avec 96° horizontal et 90°
vertical. C'est un chiffre marketing, symétrique, pas les 4 angles séparés du cours.
Source : https://www.meta.com/quest/compare/

Après j'ai creusé côté technique, et là je suis tombé sur la structure qu'utilise OpenXR (le standard que le module du cours suit justement) : XrFovf, qui a bien 4 champs, angleLeft, angleRight, angleUp, angleDown. Donc la notion des 4 angles existe bel et bien dans le standard. Le hic c'est que ces valeurs ne sont pas publiées quelque part en dur : elles sont calculées au runtime par le casque lui-même, via une fonction qui s'appelle xrLocateViews, et ça change en fonction de l'utilisateur (son écart pupillaire, la position du casque sur sa tête, etc). Donc ça n'a même pas de sens de chercher "la" valeur fixe pour un casque donné, puisque c'est propre à chaque perso qui le porte.
Source : doc du struct XrFovf, par exemple
https://javadoc.lwjgl.org/org/lwjgl/openxr/XrFovf.html

Bref, ce que je rends : le chiffre public (96° pour le Quest 3, source Meta) est un total symétrique, pas les 4 angles demandés. Les vrais angles asymétriques existent dans OpenXR mais sont calculés à l'exécution, pas publiés par le fabricant.

Pour la question "qu'est-ce qui se passerait avec un champ symétrique de même surface" : vu que la lentille est décentrée par rapport à l'œil (le nez bloque le champ côté intérieur, alors que côté extérieur y'a de la marge), si on force un FOV symétrique de même surface totale on perd soit de l'espace d'affichage inutilisé du côté du nez, soit on coupe une partie du champ périphérique du côté extérieur — qui est justement la partie la plus utile pour la vision périphérique.