//Rute Cartaxo Lima, 116111439

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1
#include <avr/io.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "nokia5110.h"

//variável global
int FreqRespiracao = 5, TempoResp, tempo_ms = 0, bpm = 0, temperatura, SatOxigenio, contador = 0, sel = 0, oxigenio = 0, volume = 0, max = 4000, AjusteVol = 16, selSangueDireita=0;
float tensao;
uint16_t leitura_ADC = 0;
uint8_t a = 1;
char NumResp[20], FreqCardiaca[20], Temp[20], SpO2[20], Pressao[20], O2[20], Vol[20];

void valor_sensor_temp() //Determina valor de temperatura.
{
	ADMUX = 0b01000001; //Tensão interna de 5V no canal 1.
	tensao = (leitura_ADC*5.0/1023.0); //conversão da palavra binária para tensão.
	temperatura = (tensao+1.0)*10.0; //Eq. da reta de tensão vs temperatura.
	if((temperatura<35)||(temperatura>41)) //ativa buzzer para temperatura>41 e temperatura<35.
	{
		PORTD ^= 0b10000000;
	}
}

void valor_sensor_oxig() //Determina valor de %SpO2.
{
	ADMUX = 0b01000000; //Tensão interna de 5V no canal 0.
	tensao = (leitura_ADC*5.0/1023.0); //conversão da palavra binária para tensão.
	SatOxigenio = tensao*25.0; //Eq. da reta de tensão vs SpO2.
	if(SatOxigenio<60) //ativa buzzer para SpO2<60%.
	{
		PORTD ^= 0b10000000;
	}
}

void texto_LCD1 () //Mostra texto 1 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Sinais Vitais", 1);
	nokia_lcd_set_cursor(37,10);
	nokia_lcd_write_string("bpm", 1);
	nokia_lcd_set_cursor(37,20);
	nokia_lcd_write_string("%SpO2", 1);
	nokia_lcd_set_cursor(37,30);
	nokia_lcd_write_string("ºC", 1);
	nokia_lcd_set_cursor(0,40);
	nokia_lcd_write_string(";HxL:", 1);
	nokia_lcd_set_cursor(55,40);
	nokia_lcd_write_string("mmHg", 1);
	
	sprintf(FreqCardiaca, "%3d", bpm); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(FreqCardiaca, 1); //mostra no LCD valor de bpm.
	sprintf(SpO2, " %2d", SatOxigenio); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string(SpO2, 1); //mostra no LCD valor de SpO2.
	sprintf(Temp, "%3d", temperatura); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string(Temp, 1);//mostra no LCD valor da temperatura.
	nokia_lcd_set_cursor(0,40);
	nokia_lcd_write_string(Pressao, 1); //mostra no LCD valor da pressão.
	
	nokia_lcd_render();
}

void texto_LCD2A () //Mostra texto 2A no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Parametros", 1);
	nokia_lcd_set_cursor(25,10);
	nokia_lcd_write_string("* resp/min", 1);
	nokia_lcd_set_cursor(37,20);
	nokia_lcd_write_string("%O2", 1);
	nokia_lcd_set_cursor(37,30);
	nokia_lcd_write_string("vol", 1);

	sprintf(NumResp, " %2d", FreqRespiracao); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(NumResp, 1); //mostra no LCD valor de resp/min.
	sprintf(O2, "%3d", oxigenio); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string(O2, 1); //mostra no LCD valor de oxigenio.
	sprintf(Vol, "%3d", volume); //converte volume de int para string.
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string(Vol, 1); //mostra no LCD valor do volume.
	
	nokia_lcd_render();
}

void texto_LCD2B () //Mostra texto 2B no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Parametros", 1);
	nokia_lcd_set_cursor(37,10);
	nokia_lcd_write_string("resp/min", 1);
	nokia_lcd_set_cursor(25,20);
	nokia_lcd_write_string("* %O2", 1);
	nokia_lcd_set_cursor(37,30);
	nokia_lcd_write_string("vol", 1);

	sprintf(NumResp, " %2d", FreqRespiracao); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(NumResp, 1); //mostra no LCD valor de resp/min.
	sprintf(O2, "%3d", oxigenio); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string(O2, 1); //mostra no LCD valor de oxigenio.
	sprintf(Vol, "%3d", volume); //converte volume de int para string.
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string(Vol, 1); //mostra no LCD valor do volume.
	
	nokia_lcd_render();
}

void texto_LCD2C () //Mostra texto 2C no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Parametros", 1);
	nokia_lcd_set_cursor(37,10);
	nokia_lcd_write_string("resp/min", 1);
	nokia_lcd_set_cursor(37,20);
	nokia_lcd_write_string("%O2", 1);
	nokia_lcd_set_cursor(25,30);
	nokia_lcd_write_string("* vol", 1);

	sprintf(NumResp, " %2d", FreqRespiracao); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string(NumResp, 1); //mostra no LCD valor de resp/min.
	sprintf(O2, "%3d", oxigenio); //converte FreqRespiracao de int para string.
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string(O2, 1); //mostra no LCD valor de oxigenio.
	sprintf(Vol, "%3d", volume); //converte volume de int para string.
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string(Vol, 1); //mostra no LCD valor do volume.
	
	nokia_lcd_render();
}

void texto_LCD3 () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	
	nokia_lcd_render();
}

void texto_LCD3APos () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(23,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(43,40);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3ANeg () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(23,20);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3BPos () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(43,20);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(63,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(43,40);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3BNeg () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(63,20);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3ABPos () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(0,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(23,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(43,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(63,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(3,40);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(23,40);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(43,40);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3ABNeg () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(23,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(63,20);
	nokia_lcd_write_string("^", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(23,40);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3OPos () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(43,40);
	nokia_lcd_write_string("*", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("^", 1);
	
	nokia_lcd_render();
}

void texto_LCD3ONeg () //Mostra texto 3 no LCD.
{
	nokia_lcd_clear();
	nokia_lcd_set_cursor(0,0);
	nokia_lcd_write_string("Tipo sanguineo", 1);

	nokia_lcd_set_cursor(0,10);
	nokia_lcd_write_string("A+", 1);
	nokia_lcd_set_cursor(20,10);
	nokia_lcd_write_string("A-", 1);
	nokia_lcd_set_cursor(40,10);
	nokia_lcd_write_string("B+", 1);
	nokia_lcd_set_cursor(60,10);
	nokia_lcd_write_string("B-", 1);
	nokia_lcd_set_cursor(0,30);
	nokia_lcd_write_string("AB+", 1);
	nokia_lcd_set_cursor(20,30);
	nokia_lcd_write_string("AB-", 1);
	nokia_lcd_set_cursor(40,30);
	nokia_lcd_write_string("O+", 1);
	nokia_lcd_set_cursor(60,30);
	nokia_lcd_write_string("O-", 1);
	nokia_lcd_set_cursor(63,40);
	nokia_lcd_write_string("*", 1);
	
	nokia_lcd_render();
}

void Servo_FreqCard() //função desloca servo da frequência cardíaca de 0º - max - 0º.
{
	static uint8_t flag = 1;
	if(flag)
	{
		if(OCR1A >= max) //compara valor de OCR1A ao valor max, definido nas interrupções INT0 e INT1.
		{
			flag = 0;
			OCR1A -= 250;
		}
		else
		{
			OCR1A += 250;
		}
	}
	else
	{
		if(OCR1A == 2000)
		{
			PORTD ^= 0b10000000; //ativa buzzer para servo de FreqCard na posição 0º.
			flag = 1;
			OCR1A += 250;
		}
		else OCR1A -= 250;
	}
}

void Timer_200ms (uint32_t tempoms) //Executa o texto do LCD a cada 200ms.
{
	static uint32_t tempo_anterior = 0;
	if((tempoms - tempo_anterior) >= 200)
	{
		//Seleciona uma das telas do LCD a cada 200ms
		if((sel == 0)||(sel == 1))
		texto_LCD1();
		else if((sel == 2)||(sel == 3))
		texto_LCD2A();
		else if ((sel == 4)||(sel == 5))
		texto_LCD2B();
		else if ((sel == 6)||(sel == 7))
		texto_LCD2C();
		else if ((sel == 8)||(sel == 9))
		{
			if (selSangueDireita == 0)
			texto_LCD3();
			else if (selSangueDireita == 1)
			texto_LCD3APos();
			else if (selSangueDireita == 2)
			texto_LCD3ANeg();
			else if (selSangueDireita == 3)
			texto_LCD3BPos();
			else if (selSangueDireita == 4)
			texto_LCD3BNeg();
			else if (selSangueDireita == 5)
			texto_LCD3ABPos();
			else if (selSangueDireita == 6)
			texto_LCD3ABNeg();
			else if (selSangueDireita == 7)
			texto_LCD3OPos();
			else if (selSangueDireita == 8)
			texto_LCD3ONeg();
		}
		
		tempo_anterior = tempoms;
	}
}

void Timer_150ms (uint32_t tempoms) //Mede cada sensor alternadamente a cada 150ms.
{
	static uint32_t tempo_anterior = 0, a = 0;

	if((tempoms - tempo_anterior) >= 150)
	{
		//Executa a cada 150ms
		if(a%2==0) //valor de a par.
		{
			valor_sensor_temp();
		}
		else //valor de a ímpar.
		{
			valor_sensor_oxig();
		}
		a++;
		tempo_anterior = tempoms;
	}
}

void USART_iniciar (unsigned int ubrr) //função de inicialização do USART.
{
	UBRR0H = (unsigned char)(ubrr>>8); //ajusta a taxa de transmissão.
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0); //habilita interrupção do transmissor e receptor.
	UCSR0C = (0<<USBS0)|(3<<UCSZ00); //ajusta para 8 bits de dados, 1 parada e modo de paridade desativado.
}

void USART_transmite (unsigned char dado)
{
	while(!(UCSR0A&(1<<UDRE0))); //aguarda limpeza do registrador.
	UDR0 = dado; //insere o dado no registrador e o envia.
}

unsigned char USART_recebe (void)
{
	while(!(UCSR0A&(1<<RXC0))); //aguarda o dado ser recebido no registrador.
	return UDR0; //lê dado recebido e retorna.
}

void USART_Erro(void) //função que chama ERRO!.
{
	Pressao[0] = 'E';
	Pressao[1] = 'R';
	Pressao[2] = 'R';
	Pressao[3] = 'O';
	Pressao[4] = '!';
	Pressao[5] = 0;
	Pressao[6] = 0;
	Pressao[7] = 0;
	Pressao[8] = 0;
	
	contador = 0;
}

void USART_Formato (void) //função que configura formato do USART para ;HHHxLLL:.
{
	char udr;
	udr = UDR0;
	if((Pressao[0] == 59)||(udr == ';')) //59 = ';' em ASCII
	{
		Pressao[contador] = udr;
		contador++;

		if(contador == 9)
		{
			contador = 0;
		}
	}
	if((contador == 0)&&(Pressao[8] != 58)) //58 = ':' em ASCII.
	{
		USART_Erro();
	}
	if((contador == 0)&&(Pressao[4] != 120)) //120 = 'x' em ASCII.
	{
		USART_Erro();
	}
	//condição para bits referentes aos números HHH e LLL.
	if((contador == 0)&&(((Pressao[1]<=47)||(Pressao[1]>=58))||((Pressao[2]<=47)||(Pressao[2]>=58))||((Pressao[3]<=47)||(Pressao[3]>=58))||((Pressao[5]<=47)||(Pressao[5]>=58))||((Pressao[6]<=47)||(Pressao[6]>=58))||((Pressao[7]<=47)||(Pressao[7]>=58))))
	{
		USART_Erro();
	}
	if((contador != 0)&&(contador < 8)&&(udr == ':'))
	{
		USART_Erro();
	}
}

ISR(INT0_vect) //interrupção por acionamento do pino PD2.
{
	if((sel == 2)||(sel == 3)) //Seleciona opção alterar frequência resp/min.
	{
		FreqRespiracao ++;
		if(FreqRespiracao==31)
		FreqRespiracao=5;
	}
	else if((sel == 4)||(sel == 5)) //Seleciona opção alterar %O2.
	{
		OCR1B += 200;
		oxigenio = (0.05*OCR1B)-100; //converte valor de OCR1B em ms para porcentagem de oxigênio.
		if(oxigenio>=100)
		{
			oxigenio = 100;
			OCR1B = 4000;
		}
	}
	else if((sel == 6)||(sel == 7)) //Seleciona opção alterar o volume.
	{
		volume ++;
		//Determina valor max a partir do volume determinado.
		if(volume>=8)
		{
			volume = 8;
			max = 4000;
			AjusteVol = 16;
		}
		else if(volume==0)
		{
			max = 4000;
			AjusteVol = 16;
		}
		else if(volume==1)
		{
			max = 2250;
			AjusteVol = 2;
		}
		else if(volume==2)
		{
			max = 2500;
			AjusteVol = 4;
		}
		else if(volume==3)
		{
			max = 2750;
			AjusteVol = 6;
		}
		else if(volume==4)
		{
			max = 3000;
			AjusteVol = 8;
		}
		else if(volume==5)
		{
			max = 3250;
			AjusteVol = 10;
		}
		else if(volume==6)
		{
			max = 3500;
			AjusteVol = 12;
		}
		else if(volume==7)
		{
			max = 3750;
			AjusteVol = 14;
		}
	}
	else if((sel == 8)||(sel == 9)) //Seleciona opção do tipo sanguíneo.
	{
		selSangueDireita++;
		if (selSangueDireita == 9)
		selSangueDireita = 0;
	}
}

ISR(INT1_vect) //interrupção por acionamento do pino PD3.
{
	if((sel == 2)||(sel == 3)) //Seleciona opção alterar frequência resp/min.
	{
		FreqRespiracao --;
		if(FreqRespiracao==4)
		FreqRespiracao=30;
	}
	else if((sel == 4)||(sel == 5)) //Seleciona opção alterar %O2.
	{
		OCR1B -= 200;
		oxigenio = (0.05*OCR1B)-100; //converte valor de OCR1B em ms para porcentagem de oxigênio.
		if(oxigenio<=0)
		{
			oxigenio = 0;
			OCR1B = 2000;
		}
	}
	else if((sel == 6)||(sel == 7)) //Seleciona opção alterar o volume.
	{
		volume --;
		//Determina valor max a partir do volume determinado.
		if(volume<=0)
		{
			volume = 0;
			max = 4000;
			AjusteVol = 16;
		}
		else if(volume==1)
		{
			max = 2250;
			AjusteVol = 2;
		}
		else if(volume==2)
		{
			max = 2500;
			AjusteVol = 4;
		}
		else if(volume==3)
		{
			max = 2750;
			AjusteVol = 6;
		}
		else if(volume==4)
		{
			max = 3000;
			AjusteVol = 8;
		}
		else if(volume==5)
		{
			max = 3250;
			AjusteVol = 10;
		}
		else if(volume==6)
		{
			max = 3500;
			AjusteVol = 12;
		}
		else if(volume==7)
		{
			max = 3750;
			AjusteVol = 14;
		}
		else if(volume==8)
		{
			max = 4000;
			AjusteVol = 16;
		}
	}
}

ISR(PCINT0_vect) //interrupção do pino PB.
{
	sel++; //incrementa valor do botão em PB6
	if(sel == 10)
	sel = 0;
}

ISR(PCINT2_vect) //interrupção do pino PD.
{
	static uint32_t Tempo_FreqCard = 0;
	bpm = 60000/((tempo_ms-Tempo_FreqCard)*2);
	Tempo_FreqCard = tempo_ms;
}

ISR(TIMER0_COMPA_vect) //interrupção a cada 1ms.
{
	tempo_ms ++;
	if(tempo_ms%TempoResp == 0) //chama a função que varia a frequência resp/min no servo 2.
	{
		Servo_FreqCard();
	}
}

ISR(ADC_vect)
{
	leitura_ADC = ADC;
}

ISR(USART_RX_vect)
{
	USART_Formato();
}

int main(void)
{
	//GPIO
	DDRD = 0b10000000; //habilita PD0 a PD6 como entrada e PD7 como saída.
	PORTD = 0b01111110; //habilita resistor pull-up em PD1 a PD6.
	DDRB = 0b00000110; //habilita PB1(OC1A) e PB2(OC1B) como saída.
	PORTB ^= 0b01000000; //habilita resistor pull-up em PB6.
	DDRC = 0b11111100; //habilita PC2 a PC6 como saída e PC0 e PC1 como entrada.
	
	//TIMER
	TCCR0A = 0b00000010; //habilita modo CTC do TC0.
	TCCR0B = 0b00000011; //liga TC0 com prescaler=64.
	OCR0A = 249; //ajusta o comparador para TC0 até 249.
	TIMSK0 = 0b00000010; //habilita interrupção a cada 1ms=(64*(249+1))/16MHz.
	
	ICR1 = 39999; //configura período do PWM para 20ms.
	TCCR1A = 0b10100010; //modo PWM rápido, ativa PWM no OC1B não invertido.
	TCCR1B = 0b00011010; //prescaler = 8.
	
	OCR1A = 2000; //1ms.
	OCR1B = 2000; //2ms.
	
	//INTERRUPÇÃO
	PCICR = 0b00000101; //habilita interrupção externa em PD e PB.
	PCMSK0 = 0b01000000; //habilita interrupção externa somente em PB6.
	PCMSK2 = 0b00110000; //habilita interrupção externa somente em PD4 e em PD5.
	EICRA = 0b00001010; //interrupção externa em INT0 e INT1 na borda de descida.
	EIMSK = 0b00000011; //habilita interrupção externa em INT0 e INT1.
	
	//ADC
	ADCSRA = 0b11101111; //habilita o AD, interrupção, conversão e prescaler = 128
	ADCSRB = 0b00000000; //conversão contínua.
	DIDR0 = 0b00111100; //habilita PC0 e PC1 como entrada de ADC0 e ADC1.
	
	//USART
	USART_iniciar(MYUBRR);
	
	sei(); //habilita interrupções globais.
	
	nokia_lcd_init();
	nokia_lcd_clear();
	nokia_lcd_render();

	while (1)
	{
		Timer_150ms(tempo_ms);
		Timer_200ms(tempo_ms);
		TempoResp = 60000/(FreqRespiracao*AjusteVol);
	}
}