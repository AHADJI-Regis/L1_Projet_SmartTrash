Ce projet est réalisé dans le cadre du module Communication Sans Fil en Licence 1 à l’Université Nice Sophia Antipolis
<img width="1920" height="1080" alt="LOGO SMART TRASH Ppt" src="https://github.com/user-attachments/assets/31bed889-31fd-40e9-8d29-2f69fe94166b" />


# L1_Projet_SmartTrash
Mise en place d'une poubelle intelligente
# Description du projet

# Materiels utilisés 
<img src="https://github.com/FabienFerrero/UCA21/blob/main/Doc/Pictures/UCA_logo.png">

Fait par Regis AHADJI,Souleymane BALDE et Alpha BALDE.
Ce projet est réalisé dans le cadre du module Communication Sans Fil en Licence 1 à l’Université Nice Sophia Antipolis

<img width="1920" height="1080" alt="LOGO SMART TRASH Ppt" src="https://github.com/user-attachments/assets/31bed889-31fd-40e9-8d29-2f69fe94166b" />

# L1_Projet_SmartTrash

Mise en place d'une poubelle intelligente

# Description du projet

Smart Trash est une poubelle intelligente conçue pour améliorer l’hygiène et le confort d’utilisation.  
Elle utilise un capteur de mouvement (PIR) pour détecter la présence d’un utilisateur et réagit automatiquement grâce à un microcontrôleur.  
Le système informe l’utilisateur via un éclairage LED indiquant l’état de la poubelle.

**Problématique :** les poubelles classiques nécessitent un contact manuel, ce qui pose des problèmes d’hygiène, surtout dans les lieux publics ou partagés.

**Objectifs :**
- Réduire le contact physique avec la poubelle
- Améliorer l’hygiène
- Faciliter l’utilisation

**Fonctions principales :**
- Détection de présence (capteur PIR)
- Ouverture automatique du couvercle (servomoteur)
- Mesure du niveau de remplissage (capteur ultrason HC-SR04)
- Indication visuelle (LEDs : vert = vide, orange = moyen, rouge = plein)
- Envoi d’informations par email via TTN et Tago.io

**Schéma de fonctionnement :**
- Entrées : capteur PIR (pin 7), capteur ultrason (A3, A2)
- Traitement : carte UCA (microcontrôleur)
- Sorties : servomoteur (pin 6), LEDs WS2812 (pin 4), moniteur série (USB)

# Materiels utilisés

- **Carte UCA** (microcontrôleur) : contrôle et coordonne tout le système
- **Capteur PIR** : détecte la présence ou le mouvement d’un utilisateur
- **Capteur ultrason HC-SR04** : mesure la distance (niveau de remplissage)
- **Servomoteur** : ouvre et ferme automatiquement le couvercle
- **LEDs WS2812** : indication visuelle d’état
- **Alimentation** : batterie externe (avec contrainte de courant)

<img src="https://github.com/FabienFerrero/UCA21/blob/main/Doc/Pictures/UCA_logo.png" />

Fait par Regis AHADJI, Souleymane BALDE et Alpha BALDE.
