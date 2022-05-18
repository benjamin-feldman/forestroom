# forestroom
* `esp_generique.ino` permet de contrôler un DC
* `esp_generique_bounce.ino` permet de contrôler un Stepper

Dans le cas du Stepper, on a trois modes :

* **constant** : le stepper tourne à vitesse constante, reçue depuis Vezer
* **bounce** : le stepper oscille avec une certaine vitesse et amplitude, reçue depuis Vezer
* **disabled** : stepper débloqué, afin d'éviter qu'il chauffe

## TODO
* régler le bug qui empêche la vitesse de changer en mode **bounce**
* dans le mode **constant**, supprimer à terme la boucle `for` et faire un step par loop
* simplifier `update_direction` dans `esp_generique`