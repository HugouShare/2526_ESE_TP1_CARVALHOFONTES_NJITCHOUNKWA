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
_Remarque : le x2 ci-dessus provient du fait que nous sommes en mode center-aligned. Pour plus d'informations regarder "Rappel"._  
Nous choisissons PSC+1 = 1 => **PSC = 1-1**.  
D'où, il vient : (ARR+1).2 = fCPU/fTIM = 8500 => **ARR = 4250-1**.  

Rappel :  
![5023790e-cd04-47f0-ae8b-94e9d88e5b23~1](https://github.com/user-attachments/assets/441d6d71-ffc7-4299-837c-d86731694fb7)  

#### Ecriture du code  

Nous configurons donc le fichier .ioc en apportant les modifications suivantes :  
- **ARR+1 = 4250**
- **PSC+1 = 1**
- **Center-edge aligned mode 3** (PWMs centrées et compare up & down)
- Dead-time de 170ns
  - Nous avons tDTS = 1/170MHz = 5.88ns
  - 170ns/5.88ns ≃ 29
  - Le registre BDTR.DTG fonctionnant selon 4 zones, nous sommes bien dans la zone 1 et la valeur à inscrire est donc bien **29**  

Nous écrivons maintenant le code C permettant de générer une PWM avec un rapport cyclique de 60%.  
Les fonctions utiles sont donc les suivantes :  
- motor_init() : permet de lancer les TIMERs en mode PWM et PWMN, ainsi que d'établir une commande complémentaire décalée avec un rapport de cyclique de 60%
- motor_rapport_cyclique_60() : permet de générer quatres PWMs en complémentaires décalées avec un rapport cyclique de 60%   

#### Analyse à l'oscilloscope  

Nous utilisons une carte NUCLEO-G474RE. Voici le pinout :  
<img width="474" height="474" alt="OIP M19V6Q1KuxN2mdSN2n9ECAHaHa" src="https://github.com/user-attachments/assets/0bd9e742-f743-4030-ae13-04fcb33429a7" />

A l'oscilloscope, nous obtenons alors le résultat suivant :  
![PXL_20251204_095913614](https://github.com/user-attachments/assets/0dd267ec-db8e-4371-ac0e-0158c1fd6478)  
![PXL_20251204_095942142](https://github.com/user-attachments/assets/98fc4f27-1443-4f46-a9e3-3b39c2b08dae)  

Nous obtenons donc bien les résultats souhaités : un temps mort d'environ 170ns et une commande complémentaire décalée avec un rapport cyclique d'environ 60%.  

### Commande de vitesse  

Nous souhaitons maintenant pouvoir configurer la valeur de CCR via le shell directement en tapant : SETCCR XXXX où :  
- SETCCR : nom de la commande du shell
- XXXX : valeur de CCR1 que l'on souhaite imposer

Pour ce faire, nous ajoutons les fonctions suivantes :  
- motor_control(int SET_CCR) : permet de configurer la valeur de CCR1 et par conséquent celle de CCR2 via la relation ARR = CCR1 + CCR2
- int motor_set_ccr(h_shell_t* h_shell, int argc, char** argv) : fonction appelée via le shell et permettant de configurer la valeur de CCR

Dans les grandes lignes, le fonctionnement de la fonction motor_set_ccr est le suivant :  
- 1 : on vérifie que deux arguments ont bien été donnés lors de l'appel à cette fonction
- 2 : si la valeur de CCR indiquée est trop grande, on modifie CCR à la valeur maximale en l'indiquant à l'utilisateur
- 3 : si la valeur de CCR indiquée est inférieure à 0, on n'apporte aucune modification en l'indiquant à l'utilisateur
- 4 : si la valeur de CCR se situe entre 0 et la valeur maximale, on configure CCR à la valeur souhaitée
- 5 : si aucun des cas ci-dessus n'a été vérifié, on retourne un message d'erreur spécifiant la formulation exacte permettant l'utilisation de cette fonction

### Premiers tests  

Avant de procéder aux essais sur moteur, nous procédons aux essais sur carte directement et observons les signaux de PWMs à l'oscilloscope.  

Nous testons d'abord en configurant, via le shell, la valeur de CCR de sorte à obtenir un rapport cyclique de 50%, puis de 70%.  
Les résultats sont les suivants :  
§§§§§§§§§§§§§§§§§§§§§§§§§§§§ A CONTINUER §§§§§§§§§§§§§§§§§§§§§§§§§§§§
