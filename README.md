# ATMEGA16_gestion_port
## Allumage d’une LED à la fois en mode chenille

Pour réaliser cela nous donnons d’abord la valeur initial pour allumer la première LED, c’est à dire 0xFE. Ensuite nous décalons de 1 la valeur de la LED que nous voulons allumer. Nous pensons bien-sûr à revenir à l’état de la première LED allumée quand nous arrivons à la fin de notre rangée de LED.


# Interruption

Pour gérer l’interruption nous faisons celà en plusieurs étapes :

1. Création d’une fonction ISR avec en argument notre vecteur d’interruption. Cette fonction sera appelé lors de l’interruption et aura pour but de changer la valeur de x.

2. Nous mettons dans notre programme principal les variables nécessaire au bon traitement de l’interruption.