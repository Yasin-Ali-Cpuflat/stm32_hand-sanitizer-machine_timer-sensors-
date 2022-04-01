# stm32_hand-sanitizer-machine_timer-sensors

#include "main.h"

// MAIN
GPIO_PinState ir_durumu ;
uint8_t Durum=0 ;

TIM_HandleTypeDef timer_ozellikler ;
GPIO_InitTypeDef IR_ozellikler;  				
GPIO_InitTypeDef GPIO_RELAY_ozellikler ;
GPIO_InitTypeDef GOSTERGE_ozellikler;

uint8_t sayici = 0 ;
/*
A	B3
B	B4
C	B5
D	B6
E	B7
F	B8
G	B9
*/

void IR_TANIM (void){


	__HAL_RCC_GPIOD_CLK_ENABLE();		          // GIOD clocgu enable yap 
						 	
	IR_ozellikler.Mode = GPIO_MODE_INPUT;	 	//PD14 input olarak aldim
	IR_ozellikler.Pin = GPIO_PIN_14;			  	//pin PD14 ayarladik
	IR_ozellikler.Pull = GPIO_NOPULL; 			// pull-up 
	
	HAL_GPIO_Init(GPIOD, &IR_ozellikler);		// PD14 input olarak aldim

}


void RELAY_TANIM () {
  
	
	__HAL_RCC_GPIOD_CLK_ENABLE();		
	
					// stmden relay'a PD13 Pini output olarak aldim	 	
	GPIO_RELAY_ozellikler.Pin 	= GPIO_PIN_13; 
	
  
	//OD
		// disarda bir direnc kullandigim için Push Pull olarak ayarladim .
GPIO_RELAY_ozellikler.Mode 	= GPIO_MODE_OUTPUT_PP; 
	
  GPIO_RELAY_ozellikler.Pull 	= GPIO_NOPULL;
  GPIO_RELAY_ozellikler.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_RELAY_ozellikler);


}
void GOSTERGE_TANIM () {
  
	
	__HAL_RCC_GPIOB_CLK_ENABLE();		// GPIOB clogunu enable yaptim
	
					//GPIOB'deki 7-segmentte kullanacagimiz pinleri tanimladim 
	GOSTERGE_ozellikler.Pin 	=  GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9; 
	
  
 GOSTERGE_ozellikler.Mode 	= GPIO_MODE_OUTPUT_PP; //inlerin modunu çikis olarak tanimladim 
  GOSTERGE_ozellikler.Pull 	= GPIO_NOPULL;
  GOSTERGE_ozellikler.Speed = GPIO_SPEED_FREQ_LOW;// frekans hizini ayarladim . 
  HAL_GPIO_Init(GPIOB, &GOSTERGE_ozellikler);


}

void timer_TANIM (void) {
		__HAL_RCC_TIM2_CLK_ENABLE();	
	
	timer_ozellikler.Instance 						= TIM2; //TIM2 ayarladim
	timer_ozellikler.Init.Prescaler   		= 1000;//160000000/1000=16000
	timer_ozellikler.Init.CounterMode 		= TIM_COUNTERMODE_UP;// artan sayma 
	timer_ozellikler.Init.Period 					= 8000;
																//0,5s sadece 0,5 saniyeye kadar PomPa calisin 
																//2 hz  1/2 = 0,5 saniye    
																
	while (HAL_TIM_Base_Init(&timer_ozellikler)!= HAL_OK); // timer çalisacagina kadar bekle 

	HAL_TIM_Base_Start_IT(&timer_ozellikler);
	HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_Base_Start(&timer_ozellikler);// timer baslat
	
	}



//
int main(void)
{
  HAL_Init();
  SystemClock_Config();
	
	RELAY_TANIM ();
	GOSTERGE_TANIM();
	timer_TANIM () ;
	IR_TANIM();
	Durum = 0 ;
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 , GPIO_PIN_SET);
	while (1) {

		
	
	
	}	
}
//
void TIM2_IRQHandler(void){
	HAL_TIM_IRQHandler ( &timer_ozellikler );
// her 0,5sn   bu fonksoyn calisir

	
	
	ir_durumu = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_14); // 14 pinin durumunu oku 
	
	
	if(ir_durumu==GPIO_PIN_RESET  &&	Durum== 0){//ir sensoru  durumu RESET ve Durum==0 ise  
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET); //relaye RESET ver yani çalistir
	Durum = 1 ;// Durum 1 yap
}

else if(ir_durumu==GPIO_PIN_RESET  &&	Durum== 1) {//ir sensoru  durumu RESET ve Durum==0 ise 
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);//relayi SET yap yani kapat 
}

else if(ir_durumu==GPIO_PIN_SET ) // ir sensoru  durumu SET ise 
{
	Durum = 0 ;//Durum 0 yap
	

//A(B3) - B(B4) - C(B5) -  D(B6) - E(B7) - F(B8) - G(B9)

sayici ++ ;
	if ( sayici == 1 ) {
		//GPIOB PIN3'u çalistir yani gödtergede a çalistir
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET); 
		//GPIOB PIN 4,5,6,7,8,9 kapat
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4 |GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 |GPIO_PIN_8 |GPIO_PIN_9, GPIO_PIN_RESET);
	}
	else if ( sayici == 2 ) {
				//GPIOB PIN 3,4 çalistir yani gödtergede a , b çalistir
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 , GPIO_PIN_SET);
				//GPIOB PIN 5,6,7,8,9 kapat

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 |GPIO_PIN_8 |GPIO_PIN_9, GPIO_PIN_RESET);
	
	}
	else if ( sayici == 3 ) {
						//GPIOB PIN 3,4 ,5 çalistir yani gödtergede a , b , c çalistir
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 , GPIO_PIN_SET);
		//GPIOB PIN 6,7,8,9 kapat
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6 |GPIO_PIN_7 |GPIO_PIN_8 |GPIO_PIN_9, GPIO_PIN_RESET);
	
	}
	else if ( sayici == 4 ) {
								//GPIOB PIN 3,4 ,5 ,6 çalistir yani gödtergede a , b , c ,d çalistir

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 |  GPIO_PIN_5 |GPIO_PIN_6, GPIO_PIN_SET);
		//GPIOB PIN 7,8,9 kapat
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7 |GPIO_PIN_8 |GPIO_PIN_9, GPIO_PIN_RESET);
	
	}
	else if ( sayici == 5 ) {
										//GPIOB PIN 3,4 ,5 ,6 ,7 çalistir yani gödtergede a , b , c ,d ,e çalistir

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 , GPIO_PIN_SET);
		//GPIOB PIN 8,9 kapat
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8 |GPIO_PIN_9, GPIO_PIN_RESET);
	
	}
	else if ( sayici == 6 ) {
												//GPIOB PIN 3,4 ,5 ,6 ,7,8 çalistir yani gödtergede a , b , c ,d ,e ,f çalistir

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 |GPIO_PIN_8 , GPIO_PIN_SET);
				//GPIOB PIN 9 kapat

		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
	
	}
	
		else if ( sayici == 7 ) {
															//GPIOB PIN 3,4 ,5 ,6 ,7,8 ,9 çalistir yani gödtergede a , b , c ,d ,e ,f ,g çalistir

	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 |GPIO_PIN_6 |GPIO_PIN_7 |GPIO_PIN_8 | GPIO_PIN_9 , GPIO_PIN_SET);
	sayici = 0 ;//sayici sifirla 
	}

}

}


