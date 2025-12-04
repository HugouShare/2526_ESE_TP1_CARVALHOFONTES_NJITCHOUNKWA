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

#### Temps mort minimum  

Les transistors que nous allons commander sont les IRF540N.  
D'après leur datasheet, nous obtenons les informations suivantes :  
<img width="640" height="563" alt="image" src="https://github.com/user-attachments/assets/7fd0d88c-8b12-435d-980f-849f29509e01" />  
Nous avons :  
- Turn-ON delay time + Rise time = 46ns
- Turn-OFF delay time + Fall time = 74ns
- Reverse recovery time = 170ns (max)
Nous choisissons donc le temps mort le plus contraignant soit 170ns.

#### Calcul des paramètres afin d'avoir une résolution minimale de 10 bits  

Afin d'avoir une résolution minimale de 10 bits, nous devons verifier l'inégalité suivante :  
ARR + 1 >= 2^10 (= 1024)  
Nous choisissons donc ARR = 1023.  

Comme nous voulons obtenir une PWM de fréquence 20kHz.  
Nous choisissons donc PSC = 7.30  soit 7.  

#### Ecriture du code  

Nous configurons donc le fichier .ioc en apportant les modifications suivantes :  
- **ARR+1 = 1024**
- **PSC+1 = 8**
- **Center-edge aligned mode 3** (PWMs centrées et compare up & down)
- Dead-time de 170ns
  - Nous avons tDTS = 1/170MHz = 5.88ns
  - 170ns/5.88ns ≃ 29
  - Le registre BDTR.DTG fonctionnant selon 4 zones, nous sommes bien dans la zone et la valeur à inscrire est donc bien** 29**

Nous écrivons maintenant le code C permettant de générer une PWM avec un rapport cyclique de 60%.  






