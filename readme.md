# forestroom
* `esp_generique.ino` permet de contrôler un DC
* `esp_generique_bounce.ino` permet de contrôler un Stepper

Dans le cas du Stepper, on a trois modes :

* **constant** : le stepper tourne à vitesse constante, reçue depuis Vezer
* **bounce** : le stepper oscille avec une certaine vitesse et amplitude, reçue depuis Vezer
* **disabled** : stepper débloqué, afin d'éviter qu'il chauffe

Dans Vezer, pour les steppers ESP10 : ne pas interpoler les points, sinon il y a un phénomène de "file d'attente" qui désynchronise vezer et le moteur.

## TODO (soft)
* régler le bug qui empêche la vitesse de changer en mode **bounce**
* dans le mode **constant**, supprimer à terme la boucle `for` et faire un step par loop
* simplifier `update_direction` dans `esp_generique`
* tableau excel des ESP et de leurs rôles
* trouver bonne valeur dans setMaxAccleration
* poursuivre la branche `random_variation`

## todo (physique)
* DC tonnerre : bruits parasites, trouver une structure qui ne rentre pas en vibration
* passer les arduino en ESP (grenouilles, ailes)
* ajuster les fils des plaques métalliques