/**********************************************
 * name: command application v1.0
 * email:support@mtf123.club
 * Copyright(c) MTF 2016~2030. www.mtf123.club
 * All Rights Reserved
 * ******************************************/

#include "dwDriver.h"

u16  locaX, locaY;
u8 language; 
u8 keyNum = 0;
u16 fColor = DW_COL_WHITE;
u16 bcolor = DW_COL_BLACK;

DwKeyListen dwKeyListen[30];

void(*dwSend)(u8);

union CharInt{
	u8 b[2];
	u16 a;
};

#include <malloc.h>

static unsigned int _data_cnt = 0;
static u8 *_data = NULL;
#define BUF_MAX 256

char dwMount(void)
{
	_data_cnt = 0;
	_data = malloc(BUF_MAX); //�������256�ֽ�
	if (_data == NULL)
		return 1;
	else
		return 0;
}

char dwRemove(void)
{
	free(_data);
	return 0;
}

void dwGetSendData(u8 **i, unsigned int **j)
{
	*i = _data;
	*j = &_data_cnt;
}

/*******����֡ͷ*********/
static __INLINE void dwSendStart(void)
{
	_data_cnt = 0;
}

/*******����֡β*********/
static __INLINE void dwSendOver(void)
{
	// MTF_Com_sendTail();
}

/***����֡�û�����***/
static __INLINE void dwSendBuf(u8 byte)
{
	if (_data_cnt < BUF_MAX)
	{
		_data[_data_cnt] = byte;
		_data_cnt++;
	}
}

/*******************about key********************/
// static u8 keyFlag = 0;
void dwCancelKey(void){
	keyNum = 0;
}

//****************����ģʽ*****************
void dwListenCoord(void(*press)(void), void(*free)(void), const Button* btn){
	if(keyNum < 30){
		dwKeyListen[keyNum].pressHandle = press;
		dwKeyListen[keyNum].freeHandle = free;
		dwKeyListen[keyNum].button = (Button*)btn;
		keyNum++;
	}
}

/**����������ת����int ��**/
/* static void dwConvertLocation(void){
	locaX = 0;
	locaX = frameData[1];
	locaX <<= 8;
	locaX += frameData[2];
	
	locaY = 0;
	locaY = frameData[3];
	locaY <<= 8;
	locaY += frameData[4];
}

static u8 dwGetKey(void){
	u8 i;
	dwConvertLocation();
	for(i=0; i<keyNum; i++){	
		if((locaX >= dwKeyListen[i].button->xs) && (locaX <= dwKeyListen[i].button->xe)
			&& (locaY >= dwKeyListen[i].button->ys) && (locaY <= dwKeyListen[i].button->ye)){
			break;
		}
	}
	return i;
}

void dwHandlerCoord(void){
	static u8 i;

	if (keyFlag)
	{
		// ���ִ���
		if (dwQueryCmd() == DW_CMD_RELEASE)
			{
				keyFlag = 0;
				//		if((i < keyNum)&&(i==dwGetKey())){ // ��Ч����
				if ((i < keyNum))
				{ // ��Ч����
					if (dwKeyListen[i].freeHandle != 0)
					{ // ��Ϊ��
						dwKeyListen[i].freeHandle();
					}
				}
			}
	}
	else
	{
		// ���´���
		if (dwQueryCmd() == DW_CMD_PRESSED)
		{
			keyFlag = 1;
			i = dwGetKey();
			if (i < keyNum)
			{ // ��Ч����
				if (dwKeyListen[i].pressHandle != 0)
				{ // ��Ϊ��
					dwKeyListen[i].pressHandle();
				}
			}
		}
	}
} */

//***************����ģʽ***********************
/* void dwListenButton(void(*press)(void), void(*free)(void), u16 Command){
	if(keyNum < 30){
		dwKeyListen[keyNum].pressHandle = press;
		dwKeyListen[keyNum].freeHandle = free;
		dwKeyListen[keyNum].command = Command;
		keyNum++;
	}
} */

/* void dwHandlerButton(void){
	static u8 i;
	u16 cmd = 0;

	if (keyFlag)
	{
		// ���ִ���
		if (dwQueryCmd() == DW_KEY_RELEASE)
		{
			keyFlag = 0;
			//		if((i < keyNum)&&(i==dwGetKey())){ // ��Ч����
			if ((i < keyNum))
			{ // ��Ч����
				if (dwKeyListen[i].freeHandle != 0)
				{ // ��Ϊ��
					dwKeyListen[i].freeHandle();
				}
			}
		}
	}
	else
	{
		// ���´���
		if (dwQueryCmd() == DW_KEY_PRESSED)
		{
			keyFlag = 1;
			cmd = ((u16)frameData[1] << 8) + frameData[2];
			for (i = 0; i < keyNum; i++)
			{
				if (cmd == dwKeyListen[i].command)
					break;
			}
			if (i < keyNum)
			{ // ��Ч����
				if (dwKeyListen[i].pressHandle != 0)
				{ // ��Ϊ��
					dwKeyListen[i].pressHandle();
				}
			}
		}
	}
} */

// ���õ�ɫ��
void dwSetColor(u16 f,u16 b){
	fColor = f;
	bcolor = b;
	dwSendStart();
	dwSendBuf(0x40);
	dwSendBuf((u8)(f>>8));
	dwSendBuf((u8)f);
	dwSendBuf((u8)(b>>8));
	dwSendBuf((u8)b);
	dwSendOver();
}

/***ȡָ��λ����Ϊ����ɫ***/
void dwSetBackColor(u16 x,u16 y){
	dwSendStart();
	dwSendBuf(0x42);
	dwSendBuf((u8)(x>>8));
	dwSendBuf((u8)x);
	dwSendBuf((u8)(y>>8));
	dwSendBuf((u8)y);
	dwSendOver();
}

/***ȡָ��λ����Ϊǰ��ɫ***/
void dwSetForeColor(u16 x,u16 y){
	dwSendStart();
	dwSendBuf(0x43);
	dwSendBuf((u8)(x>>8));
	dwSendBuf((u8)x);
	dwSendBuf((u8)(y>>8));
	dwSendBuf((u8)y);
	dwSendOver();
}

/*****ȫ����ʾָ��λ��ͼƬ����������*****/
//para: Num Ϊλ��������
void dwDisPicNoL(u8 picNum){
	dwSendStart();
	dwSendBuf(0x70); //��ʾͼƬ����
	dwSendBuf(picNum);
	dwSendOver();
}

/*****ȫ����ʾָ��λ��ͼƬ��������*****/
//para: Num Ϊλ��������
void dwDisPicWithL(u8 picNum){
	dwDisPicNoL(picNum + language);
}

// L = 1�������ԣ�l= 0 ����������
void dwDisButton(const Button* button,u8 l,u16 x,u16 y){
	if(l){
		dwCutPicL(button->picNum,button->xs,button->ys,button->xe,button->ye,x,y);
	}else{
		dwCutPic(button->picNum,button->xs,button->ys,button->xe,button->ye,x,y);
	}
}

// L = 1�������ԣ�l= 0 ����������
void dwDisImage(const Image* image,u8 l,u16 x,u16 y){
	if(l){
		dwCutPicL(image->picNum,image->xs,image->ys,image->xe,image->ye,x,y);
	}else{
		dwCutPic(image->picNum,image->xs,image->ys,image->xe,image->ye,x,y);
	}
}

/****����ͼƬ��ʾ��ָ��λ�ò���������****/
void dwCutPic(u8 picNum,u16 xs,u16 ys,u16 xe,u16 ye,u16 x,u16 y){
	dwSendStart();

	dwSendBuf(0x71);//����ͼƬ����
	dwSendBuf(picNum);
	
	dwSendBuf((u8)(xs>>8));
	dwSendBuf((u8)xs);
	dwSendBuf((u8)(ys>>8));
	dwSendBuf((u8)ys);
	
	dwSendBuf((u8)(xe>>8));
	dwSendBuf((u8)xe);
	dwSendBuf((u8)(ye>>8));
	dwSendBuf((u8)ye);

	dwSendBuf((u8)(x>>8));
	dwSendBuf((u8)x);
	dwSendBuf((u8)(y>>8));
	dwSendBuf((u8)y);

	dwSendOver();
}

/****����ͼƬ��ʾ��ָ��λ����������****/
void dwCutPicL(u8 picNum,u16 xs,u16 ys,u16 xe,u16 ye,u16 x,u16 y){
	dwCutPic(picNum+language,xs,ys,xe,ye,x,y);
}

/*****ָʾ��Сָ��λ����ʾһ���ַ�*****/
//prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
void dwDisChar(TextSize size, u16 x, u16 y, u8 c){
	u8 temp[2]= {0,0};
	temp[0] = c;
	dwDisString(size,x,y,temp);
}

/*****ָʾ��Сָ��λ����ʾһ������*****/
//prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
void dwDisNum(TextSize size, u16 x, u16 y, u8 c){
	if(c < 10){
		u8 temp[2]= {0,0};
		temp[0] = c + 0x30;
		dwDisString(size,x,y,temp);
	}	
}

/*****ָʾ��Сָ��λ����ʾ����*****/
//prama: size=8 8*8; 16 16*16; 24 24*24; 32 32*32
void dwDisString(TextSize size, u16 x, u16 y, u8 string[]){
	u8 i = 0;
	dwSendStart();
	switch(size){
		case DW_SIZE_8:
			dwSendBuf(0x53);
			break;
		case DW_SIZE_12:
			dwSendBuf(0x6E);
			break;
		case DW_SIZE_16:
			dwSendBuf(0x54);
			break;
		case DW_SIZE_24:
			dwSendBuf(0x6F);
			break;
		case DW_SIZE_32:
			dwSendBuf(0x55);
			break;	
		case DW_SIZE_48:
			dwSendBuf(0x98);
			break;
	}
	dwSendBuf((u8)(x>>8));
	dwSendBuf((u8)x);
	dwSendBuf((u8)(y>>8));
	dwSendBuf((u8)y);
	if(size == DW_SIZE_48){
		dwSendBuf(0x24);    // lib_ID
		dwSendBuf(0x81);    // C_Mode
		dwSendBuf(0x05);    // C_Dots
		dwSendBuf((u8)(fColor>>8));
		dwSendBuf((u8)fColor);
		dwSendBuf((u8)(bcolor>>8));
		dwSendBuf((u8)bcolor);
	}
	while(string[i]){
		dwSendBuf(string[i]);
		i++;
	}
	dwSendOver();
}

/****** ������Ļ����ģʽ ******/
void dwSetMode(void){
	dwSendStart();
	dwSendBuf(0xE0);
	dwSendBuf(0x55);
	dwSendBuf(0x5A);
	dwSendBuf(0xA5);
	dwSendBuf(0x00);  // ��ʾģʽ
	dwSendBuf(0x07);  // ������115200
	dwSendBuf(0x48);  // ����ģʽ ֻ�ϴ�һ��0x73
	dwSendBuf(0xEF);  // �ı���ʾģʽ�˳�����
	dwSendOver();
}

/**** ��Ļ�Դ���������һ�� *******/
void dwSound(u8 duration){  // duration*10ms
	dwSendStart();
	dwSendBuf(0x79);
	dwSendBuf(duration);
	dwSendOver();
}

 /*****�ȴ�����*****/
/* void dwWaitRelease(void){
	while(frameData[0] != 0x72 && frameData[0] != 0x78);
} */

 /*****�ȴ�����*****/
/* void dwWaitPress(void){
	while(frameData[0] != 0x73 && frameData[0] != 0x79);
} */

/***��ѯ����״̬****/
// DW_CMD_PRESSED= 0x73  ����
// DW_CMD_RELEASE= 0x72  ����
// DW_CMD_DONE   = 0xE4  ����У�����

/****��ѯ���յ�����***/
// u8 dwQueryCmd(void)
// {
// 	if(frameFlag) //�Ƿ�������֡
// 	{
// 		frameFlag = 0;
// 		return frameData[0];
// 	}
// 	else
// 	{
// 		return 0XFF;
// 	}
// }

/****����У׼***/
/* void dwCalibration(void){ 
	dwSendStart();
	dwSendBuf(0xE4); 
	dwSendBuf(0x55);
	dwSendBuf(0x5A);
	dwSendBuf(0xA5);
	dwSendOver();
	while(dwQueryCmd() != 0xE4);  // �ȴ��յ�У׼�ɹ�ָ��
} */

// ���������������
// param id:  0--120;
// param num: 1--120;
// extern u8 muteFlag;
void dwPlayMusic(u8 id, u8 num){
	// if(muteFlag) return; //����
	dwSendStart();
	dwSendBuf(0x30);
	dwSendBuf(id);
	dwSendBuf(num);
	dwSendBuf(0xA0); 	//����0x00~0xff
	dwSendOver();
}

// ֹͣ����
void dwStopMusic(void){
	dwSendStart();
	dwSendBuf(0x30);
	dwSendBuf(0);
	dwSendBuf(0);
	dwSendBuf(0);   
	dwSendOver();
}

//��������
void dwPlayVol(u8 i){
	dwSendStart();
	dwSendBuf(0x32);
	dwSendBuf(i);
	dwSendOver();
}

//AV����
//i: 0�ر�, 1����
//contrast(0~255): �Աȶ�(Ĭ��128), bright(0~255): ����(Ĭ��32) 
//saturation(0~255): ���Ͷ�(Ĭ��128), hue(0~255): ɫ��(Ĭ��0) 
void AVinput(u8 i, u8 contrast, u8 bright, u8 saturation, u8 hue)
{
	dwSendStart();
	dwSendBuf(0x8A);
	dwSendBuf(i);
	dwSendBuf(contrast);
	dwSendBuf(bright);
	dwSendBuf(saturation);
	dwSendBuf(hue);
	dwSendOver();
}
