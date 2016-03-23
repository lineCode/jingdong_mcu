#pragma sfr
#pragma nop
#pragma di
#pragma ei

#define SDA P6.1
#define SCL P6.0
#define io_SDA PM6.1
#define io_SCL PM6.0
#define output 0
#define input 1

void IIC_Init(void)
{
	io_SCL = output;
	SCL = 1;
	io_SDA = input;
}

void IIC_Delay()
{
	unsigned char register C_Delay;
	for(C_Delay=0;C_Delay<2;C_Delay++)   // 3 X 1 nop --> 86khz;  3 x 2 nop 16.03  68.6KHZ
	{
		NOP();
		NOP();
		NOP();
	} 	
}

void IIC_start()
{
//	DI();
	SCL = 0;
	IIC_Delay();
	io_SDA = input;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	io_SDA = output;
	SDA = 0;
	IIC_Delay();
	IIC_Delay();
	SCL = 0;
}

void IIC_stop()
{
	SCL = 0;
	IIC_Delay();
	io_SDA = output;
	SDA = 0;
	IIC_Delay();
	SCL = 1;
	IIC_Delay();
	io_SDA = input;
//	EI();
}

void IIC_WritByte(unsigned char byte)
{
	unsigned char data,IIC_cnt_W;
	io_SDA = output;
	data = byte;
	for(IIC_cnt_W=0;IIC_cnt_W<8;IIC_cnt_W++)
	{
		SCL = 0;
		IIC_Delay();
		if((data & 0x80)!=0){
			io_SDA = input;
		}
		else{
			io_SDA = output;
			SDA = 0;
		}
		IIC_Delay();
		SCL = 1;
		IIC_Delay();
		SCL = 0;
		data = data<<1;
	}	
}

unsigned char IIC_ReadByte()
{
	unsigned char data=0,IIC_cnt_R,bit0;
	io_SDA = input;
	for(IIC_cnt_R=0;IIC_cnt_R<8;IIC_cnt_R++)
	{
		SCL = 0;
		IIC_Delay();
		SCL = 1;
		IIC_Delay();
		if(SDA==1)
			bit0 = 1;
		else
			bit0 = 0;
			
		SCL = 0;
		data = data<<1;
		data = data|bit0;
	}
	return(data);
}

unsigned char GetACK()  
{
	unsigned char ack;
	io_SDA = input;
	SCL = 0; 
	IIC_Delay();
	SCL = 1;
	IIC_Delay();			
	ack = SDA;
	IIC_Delay();	
	return(ack);
}

void NoACK(void)  
{
	SCL = 0; 
	IIC_Delay();
	io_SDA = input;		//2014/9/12 10:20:42 spark ch
	IIC_Delay();
	SCL = 1;			
	IIC_Delay();	
	SCL = 0; 
}

void ACK(void)  //2014/9/12 17:12:56 spark ch
{
		
	SCL = 0; 
	IIC_Delay();
	io_SDA = output;
	SDA = 0;		
	IIC_Delay();
	SCL = 1;			
	IIC_Delay();	
	SCL = 0; 
}

unsigned char IIC_Read(unsigned char id,unsigned char address,unsigned char num,unsigned char *ReadData)
{

	unsigned char i;
	
	IIC_start();
	
	i = id&0xFE;
	
	IIC_WritByte(i);
	
	i=GetACK();
	
	if(i==1)
	{
		IIC_stop();
		return(1);
	}	
	
	IIC_WritByte(address);
	
	i=GetACK();
	
	if(i==1)
	{
		IIC_stop();
		return(1);
	}	
	
	IIC_start();
	
	i = id|0x01;
	
	IIC_WritByte(i);
	
	i=GetACK();
	
	if(i==1)
	{
		IIC_stop();
		return(1);
	}	
	
	for(i=0;i<num;i++)
	{
		*(ReadData+i) = IIC_ReadByte();
		if(i>=num-1){
			NoACK();
		}
		else{
			ACK();
		}		
	}
	
	IIC_stop();
	return(0);
}

unsigned char IIC_Write(unsigned char id,unsigned char address,unsigned char num,unsigned char *WriteData)
{
	unsigned char state,cnt;
	
	IIC_start();
	
	state = id&0xFE;
	
	IIC_WritByte(state);
	
	state=GetACK();
	
	if(state==1)
	{
		IIC_stop();
		return(1);
	}	
	
	IIC_WritByte(address);
	
	state=GetACK();
	
	if(state==1)
	{
		IIC_stop();
		return(1);
	}	
	
	for(cnt=0;cnt<num;cnt++)
	{
		IIC_WritByte(*(WriteData+cnt));
		state=GetACK();
		if(state==1)
		{
			IIC_stop();
			return(2);
		}	
	}
	
	IIC_stop();
	return(0);
}

