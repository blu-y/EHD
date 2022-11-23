#include <stdio.h>
#include "address_map_nios2.h"

extern volatile int *KEY_Ptr;		//�ܺο��� �����Ͽ��� KEY_ptr������ ����
extern volatile int *TIMER_Ptr;		//�ܺο��� �����Ͽ��� TIMER_ptr������ ����
extern int run;				//�ܺο��� �����Ͽ��� run������ ����
extern int period;  //���� �ڵ忡 ���� ������ �����ߴ� period �Լ� ����



void pushbutton_ISR()
{
	int press;		//press ���� ����
	press=*(KEY_Ptr+3);	//edge capture register���� ���� press�� ����
    *(KEY_Ptr+3)= press;	//edge capture register�� � ���� �ٽ� �Ἥ �ʱ�ȭ ��

    if(press == 0b10){  //������ ���� KEY1�̶��
        period = period / 2; //�ֱⰡ �� �����ؾ��ϹǷ� /2�� �ؾ���
        *(TIMER_Ptr+1)=0b1011;		//control register�� STOP, CONT, ITO = 1
        config_timer(); //config_timer ȣ��
        
    }

    else if(press == 0b100){ //������ ���� KEY2���
         period = period * 2; //�ֱⰡ �� �� �����ؾ��ϹǷ� *2�� �ؾ���
        *(TIMER_Ptr+1)=0b1011;		//control register�� STOP, CONT, ITO = 1
        config_timer(); //config_timer ȣ��
    }

	else if(press==0b1000)		//������ ���� KEY3���
	{
		if(run==1)		//run=1�̶�� ��, �̴� Ÿ�̸Ӱ� �۵� ���̶�� �ǹ�
		{
			*(TIMER_Ptr+1)=0b1011;	//control register�� STOP, CONT, ITO�� 1�� �����Ͽ� Ÿ�̸Ӹ� stop��
			run=0;		//run�� 0�� �־� stop �������� �˸�
		}
		else if (run==0)			//run�� 1�� �ƴ϶�� 0�̹Ƿ� stop ������
		{
			*(TIMER_Ptr+1)=0b0111;	//control register�� start, cont, ito�� 1�� �����Ͽ� timer�� �۵� ��Ŵ
			run=1;		//run�� 1�� �־� Ÿ�̸Ӱ� �۵� �������� �˸�
		}
	}
    
	return;
}