## Compte-rendus

### 26/05/2022

* correction de bugs, le code "stepper" fonctionne désormais complètement
* ne surtout pas mettre d'interpolation dans vezer pour les courbes speed/amplitude (stepper mode bounce), sinon ça crée un décalage temporel (phénomène de "file d'attente)
* création d'une branche "random_variation" : réflexion sur l'ajout de microvariations aléatoires pour chaque paramètre (par ex : légers changement de vitesse pour le mode Constant du stepper)
* mousse sur le stepper feuilles : on l'entend un peu moins
* DC tonnerre : fixation problématique qui fait entrer la structure en résonance. Par ailleurs : les fils vibrent contre la plaque métallique, ce qui crée un bruit parasite
