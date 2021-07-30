#include <stdio.h>
#include <stdint.h>
#include <malloc.h>
#include <string.h>
#include "MTF_file.h"
#include "DW_FileDecodeLib.h"
#include "cJSON.h"
// #include "cJSON_extend.h"
int print_preallocated(char *name, cJSON *root);
#include "dwDriver.h"
#include "dwDriveD2.h"

#define DW_MAX_PAGE 256
#define STRING_TEMP_SIZE 256*3 //256Ϊ��ת������ֽ���, ת����, 1�ֽڱ������ַ���һ���ո�(�ַ���ĩβ�ո��Ϊ0), ����Ϊ3

static size_t _old_num = 65535; //�����¾ɶԱ�������ҳ, ��ֹͷΪ65535, ��Ȼ��ҳ���ɳ���
static size_t hmiCmdCnt = 0;
static uint8_t *string_temp = NULL;
static cJSON *HMIConfig = NULL, *HmiCfg_display_number = NULL, *HmiCfg_array = NULL, *HmiCfg_control = NULL,
        *HmiCfg_page_run = NULL;

//���ʼ��
int HMI_JSON_create_init(void)
{
    _old_num = 65535;
    hmiCmdCnt = 0;
    string_temp = (uint8_t *)malloc(STRING_TEMP_SIZE); 

    if(string_temp==NULL)
        return 1; //ʧ��
    else
        return 0; //�ɹ�
}

//���ͷ�
int HMI_JSON_remove(void)
{
    free(string_temp);
    return 0; //�ɹ� 
}

/*********���ڰ�������������JSON�ַ�����**************/
static uint8_t *HmiJsonBtos(uint8_t *b, size_t count)
{
    size_t j = 0, i = 0;
    uint8_t t = 0;

    if (b == NULL || count == 0)
        return NULL;

    for (i = 0, j = 0; j < count; i += 3, j++)
    {
        t = (b[j] & 0XF0) >> 4;
        if (t > 9)
            string_temp[i] = t + '7'; //A~F
        else
            string_temp[i] = t + '0'; //0~9

        t = b[j] & 0X0F;

        if (t > 9)
            string_temp[i + 1] = t + '7'; //A~F
        else
            string_temp[i + 1] = t + '0'; //0~9

        string_temp[i + 2] = ' ';
    }
    string_temp[i - 1] = 0; //�ַ�����β

    return string_temp;
} 

static size_t HmiRunCodeNumCreate(void)
{
    //���������д����
    return ++hmiCmdCnt;
}

static cJSON *HmiArray(cJSON *object, char *sss)
{
    //����ĳ������
    cJSON *_type = NULL;
    
    _type = cJSON_GetObjectItem(object, sss); //����Ƿ����ɹ�
    if (_type == NULL)                           //��������
    {
        _type = cJSON_CreateArray();
        cJSON_AddItemToObject(object, sss, _type);
    }
    return _type;
}

static int HmiRunCodeGenerate(char *sss)
{
    uint8_t *data = NULL, *ss = NULL;
    unsigned int *cnt = NULL;

	dwGetSendData(&data, &cnt); //������������
    if (*cnt * 3 > STRING_TEMP_SIZE)
        return 0;                //����
    ss = HmiJsonBtos(data, (size_t)(*cnt)); //��������ת��Ϊ�ַ���
    if(ss==NULL)
    {
        return 0; //ʧ��
    }
    else
    {
        return cJSON_AddItemToArray(HmiArray(HMIConfig, sss), 
                                    cJSON_CreateString((const char *)ss)); //����JSON
    }
}
/***********************************************/

//ҳ����, ����ʼ���ؼ�����
static void HmiPageCreate(size_t new_num)
{
    char sss[32] = {0};

    if (_old_num != new_num) //�¾�ҳ�Ų�һ��
    {
        sprintf(sss, "display-number@%d", (int)new_num);
        HmiCfg_display_number = cJSON_GetObjectItem(HMIConfig, sss); //���ҳ���Ƿ����ɹ�
        if (HmiCfg_display_number == NULL)
        {
            HmiCfg_display_number = cJSON_AddObjectToObject(HMIConfig, sss); //����ҳ��

            memset(sss, 0, sizeof(sss)); //����ҳʱ�Զ����е�����
            sprintf(sss, "code-group@%d", (int)HmiRunCodeNumCreate());
            HmiCfg_page_run = cJSON_AddStringToObject(HmiCfg_display_number, "run", sss);
        }
        else
        {
            HmiCfg_page_run = cJSON_GetObjectItem(HmiCfg_display_number, "run");
        }
        _old_num = new_num;
    }
}

//����ֵ����
static void HmiTouchKey(uint8_t *_fptr, size_t *afterOffset)
{
    //�����ؼ�
    HmiCfg_control = cJSON_CreateObject();
    HmiCfg_array = HmiArray(HmiCfg_display_number, "key"); //��鲢��������
    cJSON_AddItemToArray(HmiCfg_array, HmiCfg_control); //�ŵ�key��

    cJSON_AddStringToObject(HmiCfg_control, "name", ""); 
    cJSON_AddStringToObject(HmiCfg_control, "type", "click");
    cJSON_AddBoolToObject(HmiCfg_control, "storage-bool", false);

    if (_fptr[14] != 0XFF) //�ж��Ƿ��ͼ�ֵ
    {
        if (_fptr[14] == 0XFE) //�ϴ�
        {
            if (_fptr[16] == 0XFE) //DGUSÿ�����ص���չ��־(��16���ֽ�Ϊһ��, �ڶ��鿪ʼ, ÿ�鿪ͷ0XFE)
            {
                cJSON_AddNumberToObject(HmiCfg_control, "key-code", 
                                            ((uint16_t)_fptr[20] << 8) | (_fptr[21])); //��ֵ
                cJSON_AddNumberToObject(HmiCfg_control, "address",
                                        ((uint16_t)_fptr[17] << 8) + _fptr[18]); //�ؼ���
                cJSON_AddBoolToObject(HmiCfg_control, "auto-send", true); //�Զ��ϴ���ֵ
            }
            *afterOffset = 32; //��һ�ؼ��洢ƫ��ֵ
        }
        else 
        {
            if (_fptr[14] == 0XFD) //���ϴ�
            {
                if (_fptr[16] == 0XFE) //DGUSÿ�����ص���չ��־(��16���ֽ�Ϊһ��, �ڶ��鿪ʼ, ÿ�鿪ͷ0XFE)
                {
                    cJSON_AddNumberToObject(HmiCfg_control, "key-code", 
                                            ((uint16_t)_fptr[20] << 8) | (_fptr[21])); //��ֵ
                    cJSON_AddNumberToObject(HmiCfg_control, "address",
                                            ((uint16_t)_fptr[17] << 8) + _fptr[18]); //�ؼ���
                    cJSON_AddBoolToObject(HmiCfg_control, "auto-send", false);    //���ϴ���ֵ
                }
                *afterOffset = 32; //��һ�ؼ��洢ƫ��ֵ
            }
        }
    }
}

//ָ��汾, ����ֵ����
static void HmiTouchKey_Command(uint8_t *_fptr, size_t *afterOffset)
{
    //�����ؼ�
    HmiCfg_control = cJSON_CreateObject();
    HmiCfg_array = HmiArray(HmiCfg_display_number, "key"); //��鲢��������
    cJSON_AddItemToArray(HmiCfg_array, HmiCfg_control); //�ŵ�key��
    
    cJSON_AddStringToObject(HmiCfg_control, "name", "");
    cJSON_AddStringToObject(HmiCfg_control, "type", "click");
    cJSON_AddBoolToObject(HmiCfg_control, "storage-bool", false);

    if (_fptr[14] != 0XFF) //�ж��Ƿ��ͼ�ֵ
    {
        cJSON_AddNumberToObject(HmiCfg_control, "key-code", 
                                ((uint16_t)_fptr[14] << 8) | (_fptr[15])); //��ֵ
        cJSON_AddNumberToObject(HmiCfg_control, "address", 4224); //�̶��ؼ���: 0X1080
        cJSON_AddBoolToObject(HmiCfg_control, "auto-send", true); //�Զ��ϴ���ֵ
    }
    *afterOffset = 16; //��һ�ؼ��洢ƫ��ֵ
}

int DW_TouchFileDecode(uint8_t *_fptr, size_t file_size)
{
    char sss[16] = {0};
    size_t dest = 0, afterOffset = 0;
    int x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    uint8_t version = 0;

    dest = 0;
    while (_fptr[dest] != 0XFF && _fptr[dest + 1] != 0XFF)
    {
        if(dest >= file_size)
            return 0;
        
        HmiPageCreate(_fptr[dest + 1]); //��Ҫ�ж��Ƿ�ͬһҳ

        //ʶ�𴥿ع�������
        if(_fptr[dest + 14]==0XFD||_fptr[dest + 14]==0XFE) //����ָ��汾��DGUS�汾
        { //DGUS�汾
            if (_fptr[dest + 15] == 0X05) //����ֵ����
                HmiTouchKey(&_fptr[dest], &afterOffset);
            //�ڴ��������
            else
                break; //�������������;��˳�����
        }
        else
        { //ָ��汾
            version = 1;
            HmiTouchKey_Command(&_fptr[dest], &afterOffset);
        }

        //������Χ
        x1 = ((int)_fptr[dest + 2] << 8) + _fptr[dest + 3];
        y1 = ((int)_fptr[dest + 4] << 8) + _fptr[dest + 5];
        x2 = ((int)_fptr[dest + 6] << 8) + _fptr[dest + 7];
        y2 = ((int)_fptr[dest + 8] << 8) + _fptr[dest + 9];
        cJSON_AddNumberToObject(HmiCfg_control, "x1", x1); 
        cJSON_AddNumberToObject(HmiCfg_control, "y1", y1); 
        cJSON_AddNumberToObject(HmiCfg_control, "x2", x2); 
        cJSON_AddNumberToObject(HmiCfg_control, "y2", y2);  

        //����
        if (((uint16_t)_fptr[dest + 12] << 8) + _fptr[dest + 13] != 0XFF00)
        { //�а���Ч��
            dwCutPic(_fptr[dest + 13], x1, y1, x2, y2, x1, y1);
            //���ɰ�����������
            memset(sss, 0, sizeof(sss));
            sprintf(sss, "code-group@%d", (int)HmiRunCodeNumCreate());
            cJSON_AddStringToObject(HmiCfg_control, "run-down", sss);
            HmiRunCodeGenerate(sss);
        }

        //�ɿ�
        if (((uint16_t)_fptr[dest + 10] << 8) + _fptr[dest + 11] != 0XFF00)
        { //��תҳ��
            dwDisPicNoL(_fptr[dest + 11]);
            memset(sss, 0, sizeof(sss));
            sprintf(sss, "code-group@%d", (int)HmiRunCodeNumCreate());
            cJSON_AddStringToObject(HmiCfg_control, "run-up", sss);
            HmiRunCodeGenerate(sss);
        }
        else if (((uint16_t)_fptr[dest + 12] << 8) + _fptr[dest + 13] != 0XFF00)
        { //�ָ���ť
            dwCutPic(_fptr[dest + 1], x1, y1, x2, y2, x1, y1);
            memset(sss, 0, sizeof(sss));
            sprintf(sss, "code-group@%d", (int)HmiRunCodeNumCreate());
            cJSON_AddStringToObject(HmiCfg_control, "run-up", sss);
            HmiRunCodeGenerate(sss);
        }

        dest = dest + afterOffset; //��һ���ؼ�ƫ��
    }

    if (version == 1)
    {
        //���°汾��: ָ�HMI�汾��
        cJSON_ReplaceItemInObject(HMIConfig, "version", cJSON_CreateString("MTF_HMI_COMMMAND"));
    }

    return 0; //success
}

/************��ʾ�ؼ����***********/
//ctrl[0]==0X5A //ͷ, �̶�
//ctrl[1] //�ؼ�����
//((uint16_t)ctrl[2]<<8)|ctrl[3] //SPָ��: ָ���ؼ��������ݵĴ��λ��(��ROM���غ�洢��RAM�ĵ�ַָ��, һ��0XFFFF, ϵͳ�Զ�)
//(((uint16_t)ctrl[4]<<8)|ctrl[5])*2 //�ؼ��������ݳ���(�ֽ�)
//((uint16_t)ctrl[6]<<8)|ctrl[7] //VPָ��: �ؼ���0x0000-0xFFFF����Щ����ָ����ַ�ģ�д0x0000���ɡ�
//����Ϊ��ϸ��������
//�ؼ���������(���: �ؼ������ؼ��������ݳ�������ֽ�)
/********************************************/

static uint32_t _RGB565to888(uint16_t pencolor) //rgb565תrgb888,͸��ͨ��Ϊ��͸
{
	uint32_t tempColor = 0;
	tempColor = (pencolor & 0X0000001F) << 3;  //B
	tempColor |= (pencolor & 0X000007E0) << 5; //G
	tempColor |= ((pencolor & 0X0000F800) << 8) | 0XFF000000; //R & alpha
	return tempColor;
}

//�ı���ʾ
int HmiDisText(uint8_t *_fptr)
{
    uint8_t *ss = NULL;
    uint8_t temp[3] = {0};
    int x1 = 0, y1 = 0;
    uint32_t color = 0;

    if(_fptr[0]!=0X5A) //ͷ
        return 1; //fail

    _fptr += 6; //�ؼ���������ƫ��

    //�����ؼ�
    HmiCfg_control = cJSON_CreateObject();
    HmiCfg_array = HmiArray(HmiCfg_display_number, "text"); //��鲢��������
    cJSON_AddItemToArray(HmiCfg_array, HmiCfg_control); //�ŵ�text��

    cJSON_AddStringToObject(HmiCfg_control, "name", ""); 
    cJSON_AddNumberToObject(HmiCfg_control, "address", ((uint16_t)_fptr[0] << 8) + _fptr[1]); //�ؼ���

    x1 = ((int)_fptr[2]<<8)|_fptr[3];
    y1 = ((int)_fptr[4]<<8)|_fptr[5];
    cJSON_AddNumberToObject(HmiCfg_control, "x1", x1); 
    cJSON_AddNumberToObject(HmiCfg_control, "y1", y1);

    color = _RGB565to888(((uint16_t)_fptr[6]<<8)|_fptr[7]);
    temp[0] = (color&0X00FF0000)>>16; //R
    temp[1] = (color&0X0000FF00)>>8; //G
    temp[2] = (color&0X000000FF); //B
    ss = HmiJsonBtos(temp, 3);
    cJSON_AddStringToObject(HmiCfg_control, "color", (char *)ss);

    cJSON_AddNumberToObject(HmiCfg_control, "font", _fptr[19]);
    cJSON_AddNumberToObject(HmiCfg_control, "size", _fptr[21]); 

    //ÿҳ����ʱ�Զ���������, ��ʾ
    dwD2DisString(((uint16_t)_fptr[0] << 8) + _fptr[1], (unsigned char *)"");
    HmiRunCodeGenerate(HmiCfg_page_run->valuestring);

    return 0; //success
}

//����ͼ����ʾ
int HmiDisPic(uint8_t *_fptr)
{
    int x1 = 0, y1 = 0;

    if (_fptr[0] != 0X5A) //ͷ
        return 1;         //fail

    _fptr += 6; //�ؼ���������ƫ��

    //�����ؼ�
    HmiCfg_control = cJSON_CreateObject();
    HmiCfg_array = HmiArray(HmiCfg_display_number, "picture"); //��鲢��������
    cJSON_AddItemToArray(HmiCfg_array, HmiCfg_control);     //�ŵ�text��

    cJSON_AddStringToObject(HmiCfg_control, "name", "");
    cJSON_AddNumberToObject(HmiCfg_control, "address", ((uint16_t)_fptr[0] << 8) + _fptr[1]); //�ؼ���

    x1 = ((int)_fptr[2] << 8) | _fptr[3];
    y1 = ((int)_fptr[4] << 8) | _fptr[5];
    cJSON_AddNumberToObject(HmiCfg_control, "x1", x1);
    cJSON_AddNumberToObject(HmiCfg_control, "y1", y1);

    cJSON_AddNumberToObject(HmiCfg_control, "photo", _fptr[14]);
    cJSON_AddNumberToObject(HmiCfg_control, "mode", _fptr[15]);
    
    //ÿҳ����ʱ�Զ���������, ��ʾ
    dwD2DisICO(((uint16_t)_fptr[0] << 8) + _fptr[1], 0);
    HmiRunCodeGenerate(HmiCfg_page_run->valuestring);

    return 0; //success
}

static int DW_DisCtrlDecode(uint8_t *_fptr)
{ //control decode and put to json
    int res = 0;
    
    switch (_fptr[1]) //check ctrl type
    {
    case 0X11: res = HmiDisText(_fptr); break; //�ı���ʾ
    case 0X00: res = HmiDisPic(_fptr); break; //����ͼ����ʾ
    default: res = 2; break; //fail 
    }

    return res; //success
}

int DW_DisFileDecode(uint8_t *_fptr, size_t file_size)
{
    size_t i = 0, j = 0;
    //this file composition: 1.file head; 2. every page control number; 3.control information
    const char head_code[] = {0X14, 0X44, 0X47, 0X55, 0X53, 0X5F, 0X32}; //file head
    uint8_t *fptr_page = _fptr+18; //every page control number
    uint8_t *fptr_control = _fptr+0X4000; //control information start

    //check head 
    for (i = 0; i < sizeof(head_code); i++)
    {
        if (_fptr[i]!=head_code[i])
            return 1; //fail
    }

    for (i = 0; i < 256; i++) //here page number max 256
    {
        if ((*fptr_page & 0XF0) == 0X40) //page tail symbol
        {
            //this page has control number
            if (*(fptr_page - 2) != 0) //��ҳ�пؼ�
            {
                HmiPageCreate(i); //���ɴ�ҳJSON

                for (j = 0; j < *(fptr_page - 2); j++)
                {
                    if (fptr_control - _fptr >= file_size)
                        return 2;              //fail
                    if (*fptr_control == 0XFF) //file tail is 0XFF
                        return 0;
                    if (*fptr_control == 0X5A) //control start is 0X5A
                        DW_DisCtrlDecode(fptr_control);
                    fptr_control += 32; //next controll info offset is 32
                }
            }

            fptr_page += 4; //next number info offset is 4
        }
        else
        {
            return 2; //fail;
        }
    }

    return 0; //success
}

void DW_FileDecode(char *nameDest, char *nameTouchSrc, char *nameDisSrc)
{
    int error = 0;
    unsigned char *_fptr = NULL;
    size_t file_size = 0;
    unsigned char _ok = 0;

    HMI_JSON_create_init();
    dwMount();

    HMIConfig = cJSON_CreateObject();
    cJSON_AddStringToObject(HMIConfig, "version", "MTF_HMI"); //HMI�汾��

    dwD2DisPicNoL(0); //Ĭ�Ͽ���0��ͼƬ
    HmiRunCodeNumCreate();
    cJSON_AddStringToObject(HMIConfig, "run", "code-group@1"); //��������������Ϊ1 
    HmiRunCodeGenerate("code-group@1");               
    
    error = MTF_load_file(&_fptr, &file_size, nameTouchSrc);
    if(!error)
    {
        error = DW_TouchFileDecode(_fptr, file_size);
        if (!error)
            _ok = 1;
        free(_fptr);
    }

    error = MTF_load_file(&_fptr, &file_size, nameDisSrc);
    if(!error)
    {
        error = DW_DisFileDecode(_fptr, file_size);
        if (!error)
            _ok = 1;
        free(_fptr);
    }

    if (_ok)
    {
        /* Print to text */
        if (print_preallocated(nameDest, HMIConfig) != 0)
            printf("output error\r\n");
        cJSON_Delete(HMIConfig);
    }
    else
    {
        printf("decode error: %d\r\n", error);
    }

    dwRemove();
    HMI_JSON_remove();
}
