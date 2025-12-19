# Rapport de TP  

Lien vers le sujet de TP : [sujet de TP](https://moodle.ensea.fr/mod/book/view.php?id=27299&chapterid=476)   

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

Nous écrivons maintenant le code C permettant de générer une PWM avec un rapport cyclique de 50%.  
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

Dans les grandes lignes, le fonctionnement de la fonction `motor_set_ccr` est le suivant :  
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
- Le coefficient de proportionnalité vaut Sn = 0.05 V/A.

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

Afin de pouvoir déclencher une mesure de courant par polling depuis le shell immédiatement, nous créons ensuite la fonction ```input_analog_get_current_polling``` :  
```C
int input_analog_get_current_polling(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getcurrentpolling\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	float measured_current = measure_current_polling();
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_current: %f \n\r", measured_current);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
```

Pour finir, afin de réaliser une mesure de courant via l'ADC en mode **POLLING** nous écrivons la fonction ```measure_current_polling``` :  
```C
float measure_current_polling(void)
{
    float v_meas, i_meas;

    adc_raw = HAL_ADC_GetValue(&hadc1);

    v_meas = ((float)adc_raw / ADC_RESOLUTION) * VREF;    // tension lue par l'ADC
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

Afin de pouvoir déclencher une mesure de courant en DMA depuis le shell immédiatement, nous implémentons alors la fonction ```input_analog_get_current_DMA``` :  
```C
int input_analog_get_current_DMA(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getcurrentdma\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	float measured_current = measure_current_DMA();
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_current: %f \n\r", measured_current);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
```  

Pour finir, afin de réaliser une mesure de courant via l'ADC en mode **DMA** nous écrivons la fonction ```measure_current_DMA``` :  
```C
float measure_current_DMA(void)
{
    float v_meas, i_meas;

    if (!adc_ready)
        return 0.0f;
    adc_ready = 0;

    v_meas = ((float)adc_raw / ADC_RESOLUTION) * VREF;
    i_meas = (v_meas - 1.47f) / 0.05f;

    return i_meas;
}
```  
L'idée est alors la suivante : on récupère la valeur mesurée par l'ADC, via le DMA. On la transforme en une valeur de tension et on en déduit, via la fonction de transfert du capteur fournie précédemment, la valeur du courant mesuré.  

> [!IMPORTANT]
> Afin de rendre le code modulable entre version DMA et version POLLING, nous passons la variable `uint16_t adc_raw;` en global.   

### Mesure de vitesse  

Nous voulons maintenant procéder à la mesure de vitesse de notre moteur. Nous allons faire cela via un TIMER en mode ENCODEUR sur notre carte STM32.   

D'après le fichier KiCad, nous obtenons :  
<img width="791" height="348" alt="image" src="https://github.com/user-attachments/assets/fbf4d4d9-fb88-4b60-9e68-19939e3cce79" />  
Les encodeurs à utiliser sont donc :  
- ENC_A => situé en `PA6`
- ENC_B => situé en `PA4`
- ENC_Z => situé en `PC8` 

Dans notre cas à nous, nous n'utiliserons que les signaux ENC_A et ENC_B.

En entrant dans le bloc `Feedback motor enc`, nous observons alors :  
<img width="390" height="457" alt="image" src="https://github.com/user-attachments/assets/9babda1d-0c41-4819-a1f5-c4351bb5346f" />

Ainsi, le module permettant de réceptionner le signal reçu via le connecteur RS232 et de le transmettre et le mettre en forme pour notre carte STM32 est donc le ```MAX3097ECSE+```.  
Sa datasheet est fournie dans le dossier _Ressources_.  

D'après le fichier `.ioc` de notre projet, nous obtenons :  
<img width="549" height="525" alt="image" src="https://github.com/user-attachments/assets/60a170a9-6fc1-40c1-8142-30199defa5a4" />  
Ainsi, le TIMER à activer en mode encodeur est le `TIMER 3 -> CH1, CH2 & CH3`.  

On configure les trois **CHANNELS** de la manière suivante :  
<img width="692" height="178" alt="image" src="https://github.com/user-attachments/assets/38e46ec0-602d-4c19-9004-cad6b76391af" />  
Et pour le **TIMER** :  
<img width="700" height="446" alt="image" src="https://github.com/user-attachments/assets/7462f3f2-0806-4d58-922e-396e56a67f3d" />   
<img width="534" height="50" alt="image" src="https://github.com/user-attachments/assets/6049fddd-631d-4d51-88c3-aa040a97dce1" />  

##### Configuration d'un TIMER pour mesures à intervalle de temps régulier    

Comme vu durant les séances de TD, la fréquence de mesure idéale au vu des constantes de temps mécaniques de notre moteur est de `f = 100 Hz`, ce qui impose donc une `mesure de vitesse toutes les 10 ms`.  

Afin de faire des mesures de vitesse à intervalle de temps régulier, nous utilisons alors le `TIMER 7` et déclenchons une `mesure de vitesse via sa fonction de Callback`.  

Nous configurons le TIMER 7 comme suit :  
<img width="711" height="540" alt="image" src="https://github.com/user-attachments/assets/bcea3c50-00b1-4bb2-bcc1-d2c71be4c4a6" />  
Cela nous permet donc de déclencher la mesure de la vitesse de rotation de notre moteur toutes les 10 ms.  

##### Mesure de la vitesse de rotation de notre moteur      

Suite à cela, nous implémentons alors la fonction `input_encoder_init` permettant d'initialiser le TIMER 3 en mode encodeur, de lancer les interruptions via le TIMER 7 et d'ajouter au shell la fonction `input_encoder_get_speed` permettant d'obtenir la vitesse de rotation de notre moteur.  
Voici son contenu :  
```C
int input_encoder_init (void)
{
	if (HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL) != HAL_OK)
	{
		return HAL_ERROR;
	}
    __HAL_TIM_SET_COUNTER(&htim3, 0);
    count_prev = 0;
    HAL_TIM_Base_Start_IT(&htim7);
    shell_add(&hshell1, "getspeed", input_encoder_get_speed, "Get motor speed");
    return HAL_OK;
}
```

Suite à cela, nous implémentons alors la fonction `input_encoder_get_speed` appelée par le shell lorsque l'utilisateur souhaite obtenir la vitesse de rotation du moteur.  
Voici son contenu :  
```C
int input_encoder_get_speed(h_shell_t* h_shell, int argc, char** argv)
{
	int size;

	if(argc!=1)
	{
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Need 1 argument : getspeedpolling\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		return HAL_ERROR;
	}

	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "measured_speed: %f \n\r", speed_rps);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return HAL_OK;
}
```

Pour finir, nous implémentons alors la fonction `measure_speed` appelée au sein de la fonction de mesure de vitesse de rotation du shell.  
Voici son contenu :  
```C
void measure_speed (void)
{
    count_now = (int16_t)__HAL_TIM_GET_COUNTER(&htim3);

    int16_t delta = count_now - count_prev;

    /* Gestion wrap-around 16 bits */
    if (delta > ENCODER_WRAP_DELTA)
    {
        delta -= ENCODER_COUNTER_MAX;
    }
    else if (delta < -ENCODER_WRAP_DELTA)
    {
        delta += ENCODER_COUNTER_MAX;
    }

    count_prev = count_now;

    // tours par seconde
    speed_rps = ((float)delta * 2.0f * PI) / (ENCODER_CPR * DT_SEC);

    speed_rpm = speed_rps / 60.0f;
}
```

> [!IMPORTANT]
> Une fois encore, à des fins de practicité de notre code, nous avons décider de rendre les variables `int16_t count_now`, `int16_t count_prev`, `float speed_rpm` et `float speed_rps` globales afin de s'assurer du bon fonctionnment de l'ensemble de notre code.

> [!IMPORTANT]
> La ligne de code suivante : ```C speed_rps = ((float)delta * 2.0f * PI) / (ENCODER_CPR * DT_SEC);``` mérite, selon moi, une attention particulière.
> L'idée globale est en fait de multiplier ω = 2*π*f avec f = 1/DT_SEC = 1/10ms par delta/ENCODER_CPR avec delta = count_now - count_prev (correspondant à la différence de valeur du compteur en l'espace de 10 ms) et ENCODER_CPR = 1024 (correspondant au nombre d'impulsions pour un tour complet de l'axe de l'arbre moteur).

Nous téléversons alors le fichier projet sur notre carte STM32 et obtenons alors le résultat suivant :  
<img width="221" height="181" alt="image" src="https://github.com/user-attachments/assets/2b9e5cf9-eacd-4d2e-a477-c5076be6c9dc" />  
> [!IMPORTANT]
> La valeur fournie correspond à la vitesse angulaire en rad/s.  

Le code fonctionne donc bel et bien 😁  

# FIN 

