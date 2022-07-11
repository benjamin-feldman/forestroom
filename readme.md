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
|15 |Tonnerre  |DC  |1             |10.10.10.15|
|16 |Pluie   |Stepper    |1             |10.10.10.16|
|17 |Grenouilles|Stepper    |1             |10.10.10.17|
|18 |Ailes   |Stepper    |1             |10.10.10.18|
|19 |        |           |              |10.10.10.19|
|20 |        |           |              |10.10.10.20|


Juin 2022 : code finalisé, reste à finir d'assembler les circuits.

7/7/22 : tests simultanés de tous les circuits : OK.

## TODO (soft)
* batterie de tests pour stepper (vérifier cas limites)
* dans le mode **constant**, supprimer à terme la boucle `for` et faire un step par loop
* poursuivre la branche `random_variation`

## todo (physique)
* DC tonnerre : bruits parasites, trouver une structure qui ne rentre pas en vibration
* ajuster les fils des plaques métalliques (tonnerre)
* régler les grenouilles : calibration temps réel avec un potentiomètre sur le côté (qui contrôle un offset et permet de caler parfaitement les boules)

## Points importants et erreurs bêtes

* FONDAMENTAL : dans Vezer, ne pas interpoler les courbes pour les Stepper (sinon : envoie des nouveaux OSC en continu, alors que chaque rotation met un certain temps à s'effectuer ; phénomène de file d'attente)
* ne pas oublier de bien connecter le Mac au routeur avant de lancer Vezer (une fois Vezer lancé, on peut changer de réseau wifi autant qu'on veut)
* passer tous les drivers stepper en 2.0A/2.2A ON OFF OFF (S4-S5-S6)
* mettre les pistes en ON sur Vezer (oubli fréquent...)
* assigner une vitesse aux stepper depuis Vezer, sinon ils tournent à 100steps/min par défaut (très lent)
* vérifier que les régulateurs bleus sortent bien du 5V (sinon, ajuster)

