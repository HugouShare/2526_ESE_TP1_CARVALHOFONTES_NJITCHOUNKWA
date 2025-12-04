# Rapport de TP  

## Général  

Notre montage de controle est le suivant :  
![Sans titre](https://github.com/user-attachments/assets/1e9c7aae-84dd-4a48-b9eb-9924310aa0af)  

Le moteur que nous souhaitons controler est le suivant :  
![PXL_20251204_072950907~2](https://github.com/user-attachments/assets/7d9460ae-5942-4d46-a579-d310764c0a4e)  

## Commande MCC basique  

Dans un premier temps, nous allons implémenter une commande basique de la MCC.  

Nos objectifs sont les suivants :  
- Générer 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en respectant le cahier des charges  
- Inclure le temps mort
- Vérifier les signaux à l'oscilloscope
- Prendre en main le hacheur 
- Faire un premier essai de commande moteur  

### Génération de 4 PWMs  

Dans un premier temps, nous générons quatres PWMs afin de pouvoir controler le hacheur.  

Notre cahier des charges est le suivant :  
- Fréquence de la PWM : 20kHz
- Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
- Résolution minimum : 10bits.

Les transistors que nous allons 
