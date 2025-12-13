# Rapport de TP  

Lien vers le sujet de TP : [Sujet de TP](https://moodle.ensea.fr/mod/book/view.php?id=27299&chapterid=476)   

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
```
- Fréquence de la PWM : 20kHz
- Temps mort minimum : à voir selon la datasheet des transistors (faire valider la valeur)
- Résolution minimum : 10bits.
```  

#### Temps mort minimum  

Les transistors que nous allons commander sont les ```IRF540N```.  
D'après leur datasheet, nous obtenons les informations suivantes :  
<img width="640" height="563" alt="image" src="https://github.com/user-attachments/assets/7fd0d88c-8b12-435d-980f-849f29509e01" />  
Nous avons :  
- Turn-ON delay time + Rise time = 46ns
- Turn-OFF delay time + Fall time = 74ns
- Reverse recovery time = 170ns (max)

Nous choisissons donc le temps mort le plus contraignant soit 170ns.

#### Calcul des paramètres afin d'avoir une résolution minimale de 10 bits  

Afin d'avoir une résolution minimale de 10 bits, nous voulons vérifier : ```ARR+1 >= 2^10 = 1024```.  
De plus, nous voulons une fréquence de PWM égale à 20kHz. Nous avons alors ```(PSC+1).(ARR+1).2 = fCPU/fTIM = 170MHz/20kHz```.  
> Remarque : le x2 ci-dessus provient du fait que nous sommes en mode center-aligned. Pour plus d'informations regarder "Rappel".  

Nous choisissons PSC+1 = 1 => ```PSC = 1-1```.  
D'où, il vient : (ARR+1).2 = fCPU/fTIM = 8500 => ```ARR = 4250-1```.  

Rappel :  
![5023790e-cd04-47f0-ae8b-94e9d88e5b23~1](https://github.com/user-attachments/assets/441d6d71-ffc7-4299-837c-d86731694fb7)  

#### Ecriture du code  

Nous configurons donc le fichier .ioc en apportant les modifications suivantes :  
```
- **ARR+1 = 4250**
- **PSC+1 = 1**
- **Center-edge aligned mode 3** (PWMs centrées et compare up & down)
- Dead-time de 170ns
  - Nous avons tDTS = 1/170MHz = 5.88ns
  - 170ns/5.88ns ≃ 29
  - Le registre BDTR.DTG fonctionnant selon 4 zones, nous sommes bien dans la zone 1 et la valeur à inscrire est donc bien **29**  
```  

Nous écrivons maintenant le code C permettant de générer une PWM avec un rapport cyclique de 60%.  
Les fonctions utiles sont donc les suivantes :  
- ```motor_init()``` : permet de lancer les TIMERs en mode PWM et PWMN, ainsi que d'établir une commande complémentaire décalée avec un rapport de cyclique de 50%
- ```motor_rapport_cyclique_50()``` : permet de générer quatres PWMs en complémentaires décalées avec un rapport cyclique de 50%   

#### Analyse à l'oscilloscope  

Nous utilisons une carte ```NUCLEO-G474RE```. Voici le pinout :  
<img width="474" height="474" alt="OIP M19V6Q1KuxN2mdSN2n9ECAHaHa" src="https://github.com/user-attachments/assets/0bd9e742-f743-4030-ae13-04fcb33429a7" />

A l'oscilloscope, nous obtenons alors le résultat suivant :  
![PXL_20251204_095913614](https://github.com/user-attachments/assets/0dd267ec-db8e-4371-ac0e-0158c1fd6478)  
![PXL_20251204_095942142](https://github.com/user-attachments/assets/98fc4f27-1443-4f46-a9e3-3b39c2b08dae)  

Nous obtenons donc bien les résultats souhaités : un temps mort d'environ 170ns et une commande complémentaire décalée avec un rapport cyclique d'environ 50%.  

### Commande de vitesse  

Nous souhaitons maintenant pouvoir configurer la valeur de CCR via le shell directement en tapant : ```SETCCR XXXX``` où :  
- SETCCR : nom de la commande du shell
- XXXX : valeur de CCR1 que l'on souhaite imposer

Pour ce faire, nous ajoutons les fonctions suivantes :  
- ```motor_control(int SET_CCR)``` : permet de configurer la valeur de CCR1 et par conséquent celle de CCR2 via la relation ARR = CCR1 + CCR2
- ```int motor_set_ccr(h_shell_t* h_shell, int argc, char** argv)``` : fonction appelée via le shell et permettant de configurer la valeur de CCR

Dans les grandes lignes, le fonctionnement de la fonction motor_set_ccr est le suivant :  
- 1 : on vérifie que deux arguments ont bien été donnés lors de l'appel à cette fonction
- 2 : si la valeur de CCR indiquée est trop grande, on modifie CCR à la valeur maximale en l'indiquant à l'utilisateur
- 3 : si la valeur de CCR indiquée est inférieure à 0, on n'apporte aucune modification en l'indiquant à l'utilisateur
- 4 : si la valeur de CCR se situe entre 0 et la valeur maximale, on configure CCR à la valeur souhaitée
- 5 : si aucun des cas ci-dessus n'a été vérifié, on retourne un message d'erreur spécifiant la formulation exacte permettant l'utilisation de cette fonction

### Premiers tests  

Avant de procéder aux essais sur moteur, nous procédons aux essais sur carte directement et observons les signaux de PWMs à l'oscilloscope.  

Nous testons d'abord en configurant, via le shell, la valeur de CCR de sorte à obtenir un rapport cyclique de 50%, puis de 25% et 80%.  

Après exécution du code, sans avoir entrer de commande permettant de modifier la valeur de CCR via le shell, nous avons par défaut un rapport cyclique de 50%.  
Nous obtenons alors le résultat suivant :  
![PXL_20251212_132147944](https://github.com/user-attachments/assets/bb764012-49a6-42a6-945c-090823625f30)  

Après avoir entré la commande ```setccr 1000``` dans le shell (soit un rapport cyclique de 25% environ), nous obtenons :  
![PXL_20251212_132203651](https://github.com/user-attachments/assets/84431c84-df79-4d0b-b253-671cf31fb776)  

Enfin, après avoir entré la commande ```setccr 3500``` dans le shell (soit un rapport cyclique de 80% environ), nous obtenons :  
![PXL_20251212_132219742](https://github.com/user-attachments/assets/4b06d9a3-8d49-49e6-add5-2429cf2682af)  

Nous obtenons donc des résultats très satisfaisants.  

## Commande en boucle ouverte, mesure de courant et de vitesse  

### Commande de la vitesse  

Nous ajoutons maintenant les deux fonctionnalités suivantes :  

```C
motor_start
```
Dans laquelle on écrit le code suivant :  
```C
int motor_start (h_shell_t* h_shell, int argc, char** argv)
{
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2);
	motor_rapport_cyclique_50();
	return HAL_OK;
}
```
Cette fonctionnalité permet donc de lancer les PWMs et de configurer le rapport cyclique à 50%.  

Ainsi que :  
```C
motor_stop
```
Dans laquelle on écrit le code suivant :  
```C
int motor_stop (h_shell_t* h_shell, int argc, char** argv)
{
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_2);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_1);
	HAL_TIMEx_PWMN_Stop(&htim1, TIM_CHANNEL_2);
	return HAL_OK;
}
```
Cette fonctionnalité permet d'arrêter la génération de PWMs.  

### Mesure de courant  

D'après le fichier KiCad, nous obtenons :  
<img width="453" height="547" alt="image" src="https://github.com/user-attachments/assets/94dc6368-d55e-4cce-845e-1e75e9a23aef" />  
Les courants à mesurer sont donc :  
- U_Imes
- V_Imes
- W_Imes

Dans notre cas à nous, nous n'utilisons que deux phases et nous avons seulement U_Imes à mesurer (puisque U_Imes = - V_Imes).

Le module que nous utilisons afin de mesurer le courant est le ```GO 10-SME/SP3```. Sa datasheet est fournie dans le dossier _Ressources_.  
D'après celle-ci, nous pouvons alors déterminer la fonction de transfert suivante :  
```
I_mes = (V_mes-1.65)/0.05
```
De fait : 
- Le capteur fonctionne de manière linéaire entre la tension mesurée et le courant mesuré.
- La plage de fonctionnement du capteur est [0;3.3V]. Nous prenons donc la moitié de cette plage de fonctionnement : 1.65V.
- Le coefficient de proportionnalité vaut Sn = 0.05 V/A
D'où la fonction de transfert donnée.

D'après le fichier KiCad, nous obtenons les informations suivantes :  
<img width="1278" height="564" alt="image" src="https://github.com/user-attachments/assets/a17b7a5e-6fd2-43bf-af83-01411eaf255e" />  
Nous utilisons donc le ```PIN PA1``` afin de procéder à la mesure de U_Imes.  

##### Mesure du courant par polling  

Nous commençons dans un premier temps par réaliser la fonction ```input_analog_init``` d'initialisation des mesures ADC :  
```C
int	input_analog_init(void)
{
	HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    if (HAL_ADC_Start(&hadc1) != HAL_OK)
    {
        return HAL_ERROR;
    }
    shell_add(&hshell1, "getcurrent", input_analog_get_current, "Get current");
    return HAL_OK;

}
```

Afin de pouvoir déclencher une mesure de courant depuis le shell immédiatement, nous créons ensuite la fonction ```input_analog_get_current``` :  
```C
int input_analog_get_current(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getcurrent\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	float measured_current = measure_current_polling();
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_current: %f \n\r", measured_current);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
```

Pour finir, afin de réaliser une mesure de courant via l'ADC nous écrivons la fonction ```measure_current_polling``` :  
```C
float measure_current_polling(void)
{
    uint32_t raw;
    float v_meas, i_meas;

    raw = HAL_ADC_GetValue(&hadc1);

    v_meas = ((float)raw / ADC_RESOLUTION) * VREF;    // tension lue par l'ADC
    i_meas = (v_meas-1.47)/0.05;                      // courant en A

    return i_meas;
}
```
L'idée est alors la suivante : on récupère la valeur mesurée par l'ADC, on la transforme en une valeur de tension et on en déduit, via la fonction de transfert du capteur fournie précédemment, la valeur du courant mesuré.  
> NOTE : Après plusieurs essais, nous observons une erreur constante introduite dans les mesures. Nous modifions donc la méthode de calcul du courant en passant du seuil milieu de 3.3V à un seuil personalisé.

##### Mesure du courant par DMA  

Nous voulons maintenant mesurer le courant via DMA.  

Nous commençons donc par activer le DMA dans le fichier "_.ioc_".  
Nous le configurons comme suit :  
<img width="1010" height="310" alt="image" src="https://github.com/user-attachments/assets/cea492e4-69d9-4b01-aa54-22b9e7945f4a" />  

Nous écrivons enfin le code correspondant à la fonction ```measure_current_DMA``` :   
```C
float measure_current_DMA(void)
{	
    uint32_t raw;
    float v_meas, i_meas;

	HAL_ADC_Start_DMA(&hadc1, &raw, sizeof(raw));     // courant en A

    v_meas = ((float)raw / ADC_RESOLUTION) * VREF;    // tension lue par l'ADC
    i_meas = (v_meas-1.47)/0.05; 

    return i_meas;
}
```

### Mesure de vitesse  

