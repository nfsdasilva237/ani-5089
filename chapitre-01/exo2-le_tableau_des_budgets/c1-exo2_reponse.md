
Alors j'ai passé un moment à chercher une source pour chacune des 5 étapes du tableau du cours (capteurs, transmission, appli, compositeur,affichage), et franchement je m'attendais galérer un peu mais pas à ce point : quasiment personne ne publie ce niveau de détail. Ni les fabricants ni les études que j'ai trouvées ne découpent la latence étape par étape, tout le monde donne un chiffre global.

Le truc le plus sérieux que j'ai trouvé c'est une étude de 2022 publiée dans Behavior Research Methods (Warburton, Mon-Williams, Mushtaq et Morehead), où ils ont carrément filmé plusieurs casques (Vive, Rift, Rift S, Valve Index) avec une caméra haute vitesse pour mesurer la vraie latence mouvement-vers-photon. Résultat : entre 21 et 42 ms sans
prédiction de mouvement, et ça tombe à 2-13 ms une fois que la
prédiction rentre en jeu. Le lien si ça intéresse :
https://doi.org/10.3758/s13428-022-01983-5

Et même en allant voir directement chez Oculus (leur doc SDK pour PC), pareil, ils ont une seule métrique appelée "App Motion-to-Photon Latency", pas de découpage en sous-étapes. Ça confirme que le tableau du cours est une simplification pédagogique, pas des chiffres qu'on retrouve tels quels ailleurs.
https://developer.oculus.com/documentation/native/pc/dg-hud

Donc pour être honnête : sur les 5 lignes précises du tableau (1-2 ms capteurs, 1-3 ms transmission, 5-11 ms appli, 1-2 ms compositeur, 2-5 ms affichage), je n'ai trouvé aucune source qui les confirme une par une. Je préfère dire "pas trouvé" plutôt que sortir un lien
bidon juste pour remplir la case.

Par contre le total ça matche bien : le cours dit ~20 ms, et l'étude donne 2-13 ms une fois la prédiction appliquée, donc c'est cohérent. Et la fourchette sans prédiction (21-42 ms) montre bien pourquoi le cours en fait tout un plat — sans prédiction on dépasse largement le seuil où ça devient inconfortable.