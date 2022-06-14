# forestroom
* `esp_generique.ino` permet de contrôler un DC
* `esp_generique_bounce.ino` permet de contrôler un Stepper

Dans le cas du Stepper, on a trois modes :

* **constant** : le stepper tourne à vitesse constante, reçue depuis Vezer
* **bounce** : le stepper oscille avec une certaine vitesse et amplitude, reçue depuis Vezer
* **disabled** : stepper débloqué, afin d'éviter qu'il chauffe

Dans Vezer, pour les steppers ESP10 : ne pas interpoler les points, sinon il y a un phénomène de "file d'attente" qui désynchronise vezer et le moteur.

|ESP|Fonction|Type moteur|Nombre moteurs|IP |
|---|--------|-----------|--------------|---|
|10 |Feuilles|Stepper    |1             |10.10.10.10|
|11 |Tonnerre|DC         |1             |10.10.10.11|
|12 |Billes A|DC         |2             |10.10.10.12|
|13 |Billes B|DC         |2             |10.10.10.13|
|14 |Tonnerre|DC         |1             |10.10.10.14|
|15 |Eclair  |Solenoide  |1             |10.10.10.15|
|16 |Pluie   |Stepper    |1             |10.10.10.16|
|17 |Grenouilles|Stepper    |1             |10.10.10.17|
|18 |Ailes   |Stepper    |1             |10.10.10.18|
|19 |        |           |              |10.10.10.19|
|20 |        |           |              |10.10.10.20|


## TODO (soft)
* régler le bug qui empêche la vitesse de changer en mode **bounce**
* dans le mode **constant**, supprimer à terme la boucle `for` et faire un step par loop
* simplifier `update_direction` dans `esp_generique`
* trouver bonne valeur dans setMaxAccleration
* poursuivre la branche `random_variation`
* pour la feuille : mouvement brusque, en gros 1/10 de tour de cercle en qq fractions de secondes ; pas d'accélération linéaire comme dans les autres cas

## todo (physique)
* DC tonnerre : bruits parasites, trouver une structure qui ne rentre pas en vibration
* passer les arduino en ESP (grenouilles, ailes)
* ajuster les fils des plaques métalliques (tonnerre)
* changer moteur sur Feuilles (ou autre probleme, mais l'oscillation rapide ne marche pas sur lui)