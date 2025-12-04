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

Afin d'avoir une résolution minimale de 10 bits, nous voulons vérifier : ARR+1 >= 2^10 = 1024. De plus, nous voulons une fréquence de PWM égale à 20kHz.  
Nous avons alors (PSC+1).(ARR+1).2 = fCPU/fTIM = 170MHz/20kHz.    
Nous choisissons PSC+1= 1 => **PSC = 0**.  
D'où, il vient : (ARR+1).2 = fCPU/fTIM = 8500 => **ARR = 4250**.  

#### Ecriture du code  

Nous configurons donc le fichier .ioc en apportant les modifications suivantes :  
- **ARR+1 = 4250**
- **PSC+1 = 1**
- **Center-edge aligned mode 3** (PWMs centrées et compare up & down)
- Dead-time de 170ns
  - Nous avons tDTS = 1/170MHz = 5.88ns
  - 170ns/5.88ns ≃ 29
  - Le registre BDTR.DTG fonctionnant selon 4 zones, nous sommes bien dans la zone et la valeur à inscrire est donc bien** 29**

Nous écrivons maintenant le code C permettant de générer une PWM avec un rapport cyclique de 60%.  
Les fonctions utiles sont donc les suivantes :  
- PWM_Init() : permet d'initialiser les PWMs
- PWM_RapportCyclique60() : permet de générer quatres PWMs en complémentaires décalées avec un rapport cyclique de 60%  

#### Analyse à l'oscilloscope  

Nous utilisons une carte NUCLEO-G474RE. Voici le pinout :  
<img width="474" height="474" alt="OIP M19V6Q1KuxN2mdSN2n9ECAHaHa" src="https://github.com/user-attachments/assets/0bd9e742-f743-4030-ae13-04fcb33429a7" />

A l'oscilloscope, nous obtenons alors le résultat suivant :  
![PXL_20251204_095913614](https://github.com/user-attachments/assets/0dd267ec-db8e-4371-ac0e-0158c1fd6478)  
![PXL_20251204_095942142](https://github.com/user-attachments/assets/98fc4f27-1443-4f46-a9e3-3b39c2b08dae)  

Nous obtenons donc bien les résultats souhaités.  

### Commande de vitesse  






