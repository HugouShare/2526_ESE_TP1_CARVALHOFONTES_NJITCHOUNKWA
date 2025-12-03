<<<<<<< HEAD
# Rapport de TP  

## Commande MCC basique  

Dans un premier temps, nous allons implémenter une commande basique de la MCC.  

Nos objectifs sont les suivants :  
- Générer 4 PWM en complémentaire décalée pour contrôler en boucle ouverte le moteur en respectant le cahier des charges  
- Inclure le temps mort
- Vérifier les signaux à l'oscilloscope
- Prendre en main le hacheur 
- Faire un premier essai de commande moteur  

### Génération de 4 PWMs  

=======
# Compte rendu du TP  

## Introduction  
Durant ces TPs de bus & réseaux, l'objectif est de mettre en place le système suivant :  
<img width="1333" height="614" alt="image" src="https://github.com/user-attachments/assets/5432dc45-abcd-4308-863c-21bd4d93261a" />

## Mise en place de la partie I2C du système  
Dans un premier temps, nous allons mettre en place la partie I2C du projet.  
<img width="714" height="208" alt="image" src="https://github.com/user-attachments/assets/b8640915-dbf7-40d3-b937-7191a3778152" />  
Nous utilisons une communication I2C afin de communiquer avec le capteur BMP280.  

### Capteur BMP280  
La datasheet du capteur BMP280 a été répertoriée dans le dossier "_Ressources/Datasheets_".  
A partir de la datasheet, nous obtenons alors les informations suivantes :  
- Adresses possibles pour ce composant :
  - Nous la trouvons page 28 de la datasheet
  - Connecting SDO to GND results in slave address 1110110 (0x76)
  - Connecting it to VDDIO results in slave address 1110111 (0x77)
  - ATTENTION : le pin SDO ne peut être laissé flotant sinon l'adresse du device sera non définie
- Registre et valeur permettant d'identifier ce composant :
  - Nous la trouvons page 24 de la datasheet
  - Register 0xD0 “id”
- Registre et valeur permettant de placer le composant en mode normal :
  - Nous la trouvons page 26 de la datasheet
  - Register 0xF5 “config”
- Registre et valeur contenant l'étalonnage du composant :
  - Nous la trouvons page 24 de la datasheet
    <img width="1012" height="491" alt="image" src="https://github.com/user-attachments/assets/aeae1bc2-6f52-4e1f-8372-a8b8b6ad733a" />  
  - Ainsi, les valeurs de calibrations sont contenues dans les registres 0xA1 jusque 0x88
- Registre et valeur contenant la température :
  - Nous la trouvons page 27 de la datasheet
    <img width="1003" height="412" alt="image" src="https://github.com/user-attachments/assets/555e1898-cf1a-4148-b814-3edacf96738e" />  
  - Ainsi, les valeurs de calibrations sont contenues dans les registres 0xFA jusque 0xFC et se nomme "temp"
- Registre et valeur contenant la pression :
  - Nous la trouvons page 26 de la datasheet
    <img width="1007" height="417" alt="image" src="https://github.com/user-attachments/assets/f1e04610-0a8c-4655-9bdc-58421fed378e" />  
  - Ainsi, les valeurs de calibrations sont contenues dans les registres 0xF7 jusque 0xF9 et se nomme "press"
- Fonctions permettant le calcul de la température et de la pression compensées, en format entier 32 bits :
  - Nous les trouvons page 45 et 46 de la datasheet
  - Les fonctions ont les prototypes suivants :
    - BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
    - BMP280_U32_t bmp280_compensate_P_int32(BMP280_S32_t adc_P)
    <img width="935" height="610" alt="image" src="https://github.com/user-attachments/assets/06d8ba80-5315-4519-a674-fa952471575e" />
    <img width="953" height="498" alt="image" src="https://github.com/user-attachments/assets/cc29adfa-8975-487a-b960-b28b5b4f4a08" />

### Setup du STM32  
Nous configurons maintenant notre carte de développement STM. Il s'agit d'une NUCLEO-F446RE.  
Voici le pinout de la carte :  
<img width="486" height="430" alt="image" src="https://github.com/user-attachments/assets/ef70ffe8-98e6-4a36-8700-6dcad772d5dc" />  

Après configuration de la carte NUCLEO, nous avons le fichier .ioc suivant :  
<img width="626" height="593" alt="image" src="https://github.com/user-attachments/assets/379734e7-69cd-4aee-b1a9-441d4edb5905" />

Afin de pouvoir déboguer à l'aide de la fonction printf, nous ajoutons le bout de code donné dans le sujet dans le fichier "_stm32f4xx_hal_msp.c_".  
Nous testons celle-ci avec un simple Hello world. Tout fonctionne comme il se doit :  
<img width="421" height="339" alt="image" src="https://github.com/user-attachments/assets/6faafb6c-84e6-41bc-9305-0afb7450d000" />

### Communications I2C avec le BMP280  
Afin de communiquer en I2C avec le module BMP280, nous allons principalement utiliser les deux fonctions suivantes :
- HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)
- HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size, uint32_t Timeout)

Avec : 
  - I2C_HandleTypeDef hi2c: structure stockant les informations du contrôleur I²C
  - uint16_t DevAddress: adresse I³C du périphérique Slave avec lequel on souhaite interagir.
  - uint8_t *pData: buffer de données
  - uint16_t Size: taille du buffer de données
  - uint32_t Timeout: peut prendre la valeur HAL_MAX_DELAY

Ces fonctions vont nous permettre d'accéder directement aux différents registres du module et donc d'écrire ou lire des données depuis les registres du module. 
A des fins de lisibilité et de clarté de code, nous décidons de créer les fonctions :  
- HAL_StatusTypeDef BMP280_WriteReg(uint8_t reg, uint8_t value);
- HAL_StatusTypeDef BMP280_ReadReg(uint8_t reg, uint8_t *value);
- HAL_StatusTypeDef BMP280_ReadMulti(uint8_t reg, uint8_t *buf, uint16_t len);

Permettant respectivement de :
- écrire dans un registre nommé _reg_ une valeur _value_
- lire dans un registre nommé _reg_ une valeur et l'écrire dans la variable nommée _value_
- lire dans _len_ registres à partir du registre _reg_ des valeurs et les écrire dans le buffer nommé _buf_
  
Afin d'écrire dans un registre, il suffit simplement d'utiliser la fonction HAL_I2C_Master_Transmit en précisant :  
- l'adresse I2C du module auquel on souhaite accéder
- un buffer de taille 2 contenant respectivement : le registre où l'on veut écrire et la valeur que l'on veut écrire dans ce registre

Afin de lire dans un registre, il suffit simplement :
- d'utiliser la fonction HAL_I2C_Master_Transmit en précisant :  
  - l'adresse I2C du module auquel on souhaite accéder
  - l'adresse du registre que l'on souhaite lire
- d'utiliser la fonction HAL_I2C_Master_Receive en précisant :  
  - un pointeur sur la variable dans laquelle on veut écrire se qui se trouve dans le registre

#### Identification du BMP280  
Tout d'abord, nous commençons par identifier le module BMP280, c'est-à-dire lire dans son registre ID.  
Pour cela, nous utilisons les informations de la datasheet et écrivons le code correspondant dans une fonction nommée "_BMP280_Init(void)_".  
Après exécution de celle-ci, nous obtenons bien un ID de 0x58 cohérent avec ce qui est écrit dans la datasheet.  

#### Configuration du BMP280  
Suite à cela, nous configurons le module BMP280 afin de spécifier de quelle manière nous voulons utiliser le capteur.  
Dans notre cas à nous : mode normal, Pressure oversampling x16, Temperature oversampling x2.  
Nous ajoutons alors à la fonction "_BMP280_Init(void)_" la configuration du capteur.  

#### Récupération de l'étalonnage, de la température et de la pression  
Afin de récupérer en une fois le contenu des registres d'étalonnages du BMP280, nous écrivons la fonction "_BMP280_Calibration(void)_".  
Dans cette fonction, nous remplissons tout simplement le buffer "_uint8_t calibration_values [26]_" via l'appel de fonction "_BMP280_ReadMulti(BMP280_CALIBRATION, &calibration_values, 26)_".  
La fonction _BMP280_ReadMulti_ va alors remplir le buffer _calibration_values_ en commencant à lire au registre _BMP280_CALIBRATION_ (valant 0xA1) et en incrémentant automatiquement l'adresse du registre 26 fois, soit jusqu'à avoir fini de lire dans l'ensemble des registres d'étalonnage du module.  

Ensuite, nous définissons la fonction "_void BMP280_ReadRawData(int32_t *raw_temp, int32_t *raw_press)_", permettant d'obtenir respectivement les valeurs brutes de température et de pression lues par le capteur, sans traitement.  
Une fois encore, le principe est le même : nous commencons à lire à l'adresse _BMP280_PRESS_MSB_ (valant 0xF7) jusqu'au registre 0xFC, puis nous mettons en forme les données lues dans les variables raw_temp et raw_press, conformément à ce qui est écrit dans la datasheet.  

#### Calcul des températures et des pression compensées  
Pour finir, nous utilisons les fonctions données dans la datasheet page 45 et 46 afin d'appliquer un traitement sur les valeurs de température et de pression mesurée par le capteur, en vue d'obtenir des valeurs les plus correctes possible.  
Nous reprenons directement le contenu fournit dans la datasheet.  

Une fois cela fait, nous utilisons la boucle _while(1)_ du fichier "_main.c_" afin d'effectuer des mesures de pression et de température et comparer les valeurs brutes aux valeurs avec traitement.  
Nous observons que... A CONTINUER !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!  

## Mise en place de l'interfaçage STM32-Raspberry  
Dans un second temps, nous allons mettre en place un interfaçage entre notre carte STM32 et Raspberry.  
<img width="866" height="479" alt="image" src="https://github.com/user-attachments/assets/4e31f893-ab4f-4cc3-8550-9a39669d01b6" />  
Nous utiliserons un script Python afin d'interroger la carte STM32 depuis la Raspberry.  

### Mise en route du Raspberry PI Zéro  

#### Préparation du Raspberry  
Informations saisies lors de la création de l'image via Raspberry Pi Imager :  
- hostname : HugoCFArthurNN
- nom utilisateur : hugoarthur
- mdp : ensea2526
- SSH : C304_DTI_AP
- mdp : ilovelinux

#### Premier démarrage  
Nous flashons alors la carte SD avec les configurations faites via Raspberry Pi Imager.  
La Raspberry a obtenu son adresse IP sur le réseau de la même manière qu’un ordinateur classique : via le protocole DHCP.  
Son adresse IP correspond à la théorie vue en cours :  
![OIP NHsl6SOwA1YLv5Rn71-dHwHaFj](https://github.com/user-attachments/assets/d01dba0f-2ff9-4451-8b7e-074d66e4c709)  

Nous nous connectons alors à notre Raspberry PI Zero en suivant le protocole suivant :  
- ouverture du terminal de cmd Windows
- ecrire dans le terminal : ssh hugoarthur@192.168.4.207
- ecrire dans le terminal le mdp : ensea2526  
Nous obtenons alors l'interface suivante :
<img width="1184" height="222" alt="image" src="https://github.com/user-attachments/assets/4fa3d9a4-2ad5-4112-aebc-d88fb25c0e67" />  
  
### Port série  

#### Loopback  
Dans un premier temps, nous rebouclons la pin RX sur la pin TX.
![BOARD-Layout-CMPLETE_800x506-768x486](https://github.com/user-attachments/assets/c5023909-3ba7-494d-9369-463907a953ff)  

Nous installons minicom via la commande : 
_sudo apt update_  
_sudo apt install minicom_  

Suite à cela, nous écrivons dans le terminal de cmd Windows :  
- sudo minicom -D /dev/ttyAMA0

Cela nous permet alors de configurer le port série. Nous le configurons de la manière suivante (en pressant CTRL+A suivi de O) :  
<img width="892" height="429" alt="image" src="https://github.com/user-attachments/assets/f1b7800b-caa1-465a-a40a-2741d357a99f" />  

Nous pouvons effectivement écrire et visualiser les caractères écrits en même temps : 
<img width="872" height="362" alt="image" src="https://github.com/user-attachments/assets/8d1e3471-f603-4273-9bae-c4aa8c7cd734" />

En connectant notre sortie RX de notre Rasberry avec la sortie TX du STM32, nous parvenons à lire les valeurs envoyées par notre STM32 : 
<img width="985" height="740" alt="image" src="https://github.com/user-attachments/assets/b76f8cf8-f76b-4bf6-bebf-335ca62c7af7" />





>>>>>>> e02bff9581f187c18b4a90509f5c4585d3ff3af7
