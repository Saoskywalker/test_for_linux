#ifdef __WINDOWS__
#include <windows.h>
#endif

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include "ts_calibrate.h"
#include "MTF_file.h"

typedef struct
{
    char i[4];
    int j;
    char *c;
    void *next;
} test_struct;

char *jjiii(void)
{
    static char j[] = "asdf";
    return j;
}

#include "cJSON.h"

/* Used by some code below as an example datatype. */
struct record
{
    const char *precision;
    double lat;
    double lon;
    const char *address;
    const char *city;
    const char *state;
    const char *zip;
    const char *country;
};

/* Create a bunch of objects as demonstration. */
static int print_preallocated(cJSON *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;

    /* formatted print */
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char *)malloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char *)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1))
    {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0)
        {
            printf("cJSON_PrintPreallocated not the same as cJSON_Print!\n");
            printf("cJSON_Print result:\n%s\n", out);
            printf("cJSON_PrintPreallocated result:\n%s\n", buf);
        }
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    /* success */
    printf("%s\n", buf);

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1))
    {
        printf("cJSON_PrintPreallocated failed to show error with insufficient memory!\n");
        printf("cJSON_Print result:\n%s\n", out);
        printf("cJSON_PrintPreallocated result:\n%s\n", buf_fail);
        free(out);
        free(buf_fail);
        free(buf);
        return -1;
    }

    free(out);
    free(buf_fail);
    free(buf);
    return 0;
}

/* Create a bunch of objects as demonstration. */
static void create_objects(void)
{
    /* declare a few. */
    cJSON *root = NULL;
    cJSON *fmt = NULL;
    cJSON *img = NULL;
    cJSON *thm = NULL;
    cJSON *fld = NULL;
    int i = 0;

    /* Our "days of the week" array: */
    const char *strings[7] =
        {
            "Sunday",
            "Monday",
            "Tuesday",
            "Wednesday",
            "Thursday",
            "Friday",
            "Saturday"};
    /* Our matrix: */
    int numbers[3][3] =
        {
            {0, -1, 0},
            {1, 0, 0},
            {0, 0, 1}};
    /* Our "gallery" item: */
    int ids[4] = {116, 943, 234, 38793};
    /* Our array of "records": */
    struct record fields[2] =
        {
            {"zip",
             37.7668,
             -1.223959e+2,
             "",
             "SAN FRANCISCO",
             "CA",
             "94107",
             "US"},
            {"zip",
             37.371991,
             -1.22026e+2,
             "",
             "SUNNYVALE",
             "CA",
             "94085",
             "US"}};
    volatile double zero = 0.0;

    /* Here we construct some JSON standards, from the JSON site. */

    /* Our "Video" datatype: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "name", cJSON_CreateString("Jack (\"Bee\") Nimble"));
    cJSON_AddItemToObject(root, "format", fmt = cJSON_CreateObject());
    cJSON_AddStringToObject(fmt, "type", "rect");
    cJSON_AddNumberToObject(fmt, "width", 1920);
    cJSON_AddNumberToObject(fmt, "height", 1080);
    cJSON_AddFalseToObject(fmt, "interlace");
    cJSON_AddNumberToObject(fmt, "frame rate", 24);

    /* Print to text */
    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our "days of the week" array: */
    root = cJSON_CreateStringArray(strings, 7);

    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our matrix: */
    root = cJSON_CreateArray();
    for (i = 0; i < 3; i++)
    {
        cJSON_AddItemToArray(root, cJSON_CreateIntArray(numbers[i], 3));
    }

    /* cJSON_ReplaceItemInArray(root, 1, cJSON_CreateString("Replacement")); */

    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our "gallery" item: */
    root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "Image", img = cJSON_CreateObject());
    cJSON_AddNumberToObject(img, "Width", 800);
    cJSON_AddNumberToObject(img, "Height", 600);
    cJSON_AddStringToObject(img, "Title", "View from 15th Floor");
    cJSON_AddItemToObject(img, "Thumbnail", thm = cJSON_CreateObject());
    cJSON_AddStringToObject(thm, "Url", "http:/*www.example.com/image/481989943");
    cJSON_AddNumberToObject(thm, "Height", 125);
    cJSON_AddStringToObject(thm, "Width", "100");
    cJSON_AddItemToObject(img, "IDs", cJSON_CreateIntArray(ids, 4));

    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our array of "records": */
    root = cJSON_CreateArray();
    for (i = 0; i < 2; i++)
    {
        cJSON_AddItemToArray(root, fld = cJSON_CreateObject());
        cJSON_AddStringToObject(fld, "precision", fields[i].precision);
        cJSON_AddNumberToObject(fld, "Latitude", fields[i].lat);
        cJSON_AddNumberToObject(fld, "Longitude", fields[i].lon);
        cJSON_AddStringToObject(fld, "Address", fields[i].address);
        cJSON_AddStringToObject(fld, "City", fields[i].city);
        cJSON_AddStringToObject(fld, "State", fields[i].state);
        cJSON_AddStringToObject(fld, "Zip", fields[i].zip);
        cJSON_AddStringToObject(fld, "Country", fields[i].country);
    }

    /* cJSON_ReplaceItemInObject(cJSON_GetArrayItem(root, 1), "City", cJSON_CreateIntArray(ids, 4)); */

    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "number", 1.0 / zero);

    if (print_preallocated(root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);
}

int test_ts_calibrate(void)
{
    ts_calibration cal;
    int xi = 50, yi = 80, xo = 0, yo = 0;

    /*物理坐标*/
    cal.x[0] = 100;
    cal.y[0] = 100;
    cal.x[1] = 1600 - 100;
    cal.y[1] = 50;
    cal.x[2] = 1600 - 100;
    cal.x[2] = 960 - 100;
    cal.x[3] = 100;
    cal.y[3] = 960 - 100;
    cal.x[4] = 800;
    cal.y[4] = 400;

    /*逻辑坐标*/
    cal.xfb[0] = 50;
    cal.yfb[0] = 50;
    cal.xfb[1] = 800 - 50;
    cal.yfb[1] = 50;
    cal.xfb[2] = 800 - 50;
    cal.xfb[2] = 480 - 50;
    cal.xfb[3] = 50;
    cal.yfb[3] = 480 - 50;
    cal.xfb[4] = 400;
    cal.yfb[4] = 240;

    perform_calibration(&cal);

    printf("%d %d %d %d %d %d %d\r\n",
           cal.a[0], cal.a[1], cal.a[2], cal.a[3],
           cal.a[4], cal.a[5], cal.a[6]);

    /*检验校准 x=100 y=150*/
    coords_get(&cal, &xi, &yi, &xo, &yo);
    printf("x %d\r\n", xo);
    printf("y %d\r\n", yo);

    return 0;
}

int test(int argc, char *argv[])
{
    int j = 123;

    char *cmd[] = {
        "sds",
        "ere",
        "1as",
        ""};
    char *uu = NULL;
    char op[32] = {0};
    test_struct *pp, *cc, *jj, *kk;
    int ret;
    char *num = "123";
    ret = atoi(num);
    printf("ret: %d\r\n", ret);

    cc = malloc(sizeof(test_struct));
    jj = malloc(sizeof(test_struct));
    pp = malloc(sizeof(test_struct));
    kk = pp;
    cc->c = "ds";
    jj->c = "eer";
    pp->c = "fdsp";

    pp->next = jj;
    jj->next = cc;
    cc->next = pp;
    pp = pp->next;
    printf("pp: %s\r\n", pp->c);
    free(cc);
    free(jj);
    free(kk);
    strcpy(op, cmd[2]);
    printf("cmd: %s\r\n", op);
    uu = jjiii();
    printf("uu: %s\r\n", uu);
    printf("hello world O(∩_∩)O %d\r\n", argc);
    printf("%d\r\n", j);
    for (char i = 0; i < argc; i++)
    {
        printf("%s\r\n", argv[i]);
    }

    return 0;
}

/* #include <SDL2/SDL.h>
void test_sdl(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) == -1) //SDL_初始化
    {
        printf("Could not initialize SDL!\n");
    }
    else
    {
        printf("SDL initialized.\n");
        SDL_Quit(); //退出SDL调用
    }
} */

int test_sdl_bmp(int argc, char **argv);

#if 1

typedef enum 
{
    ctrl_key = 0,
    ctrl_text,
    ctrl_picture
}ctrl_type;

#define DW_MAX_PAGE 256

int hmiPage = 0;
cJSON *HMIConfig = NULL, *display_number[DW_MAX_PAGE] = NULL, *HMI_key[DW_MAX_PAGE] = NULL,
        *HMI_key_branch[DW_MAX_PAGE] = NUL;

static unsigned char touch_switch_data[8*1024] = {0}; //HMIConfig.bin文件缓存
static u8 key_code[3], coord_code[5];
void touch_switch_check(int *Xin, int *Yin, u8 state)
{
    static u8 s = 0;
    static u16 dest = 0;
    int x1, y1, x2, y2;
    char pic_path[PATH_LEN];
    int com_len = 3;

    RectInfo pic;
    pic.pixelByte = 4;
    pic.crossWay = 0;
    pic.alpha = 255;

    HMIConfig = cJSON_CreateObject();
    cJSON_AddStringToObject(HMIConfig, "version", "MTF_HMI"); //HMI版本号
    cJSON_AddStringToObject(HMIConfig, "run", "code-group@1"); //开机运行代码


    
    cJSON_AddNumberToObject(display_number[hmiPage], "width", 1920);
    cJSON_AddNumberToObject(display_number[hmiPage], "height", 1080);
    cJSON_AddFalseToObject(display_number[hmiPage], "interlace");
    cJSON_AddNumberToObject(display_number[hmiPage], "frame rate", 24);

    if (state) //按下
    {

        for (dest = 0; dest < 8 * 1024 && touch_switch_data[dest] != 0XFF && touch_switch_data[dest + 1] != 0XFF; dest += 16)
        {
                char sss[32] = {0};
    sprintf(sss, "display-number@%d", touch_switch_data[dest + 1]);
    cJSON_AddItemToObject(HMIConfig, sss, display_number[hmiPage] = cJSON_CreateObject());
    cJSON_AddItemToObject(display_number[hmiPage], "key", HMI_key[hmiPage] = cJSON_CreateArray());
    cJSON_AddItemToArray(HMI_key[hmiPage], HMI_key_branch[key_branch] = cJSON_CreateObject());
   
    cJSON_AddStringToObject(HMI_key_branch[key_branch], "name", "");
    cJSON_AddNumberToObject(HMI_key_branch[key_branch], "x1", x1); 
    cJSON_AddNumberToObject(HMI_key_branch[key_branch], "y1", y1); 
    cJSON_AddNumberToObject(HMI_key_branch[key_branch], "x2", x2); 
    cJSON_AddNumberToObject(HMI_key_branch[key_branch], "y2", y2);       
            {
                x1 = ((int)touch_switch_data[dest + 2] << 8) + touch_switch_data[dest + 3];
                y1 = ((int)touch_switch_data[dest + 4] << 8) + touch_switch_data[dest + 5];
                x2 = ((int)touch_switch_data[dest + 6] << 8) + touch_switch_data[dest + 7];
                y2 = ((int)touch_switch_data[dest + 8] << 8) + touch_switch_data[dest + 9];
                if (*Xin >= x1 && *Xin <= x2 &&
                    *Yin >= y1 && *Yin <= y2)
                {
                    if (((u16)touch_switch_data[dest + 12] << 8) + touch_switch_data[dest + 13] != 0XFF00)
                    { //有按下效果
                        pic_type_adapt(dirPath, &touch_switch_data[dest + 13], NULL, pic_path);
                        if (UI_pic_cut(pic_path, &pic, x1, y1, x2, y2) == 0)
                        {
                            UI_disRegionCrossAdapt(&pic, x1, y1);
                            LCD_Exec();
                            free(pic.pixelDatas);
                        }
                    }
                    if (touch_switch_data[dest + 14] != 0XFF) //判断是否发送键值
                    {
                        ///////兼容DGUSII按键返回///////
                        // if (touch_switch_data[dest + 14] == 0XFE) //上传
                        // {
                        //     if (touch_switch_data[dest + 16] == 0XFE)
                        //     {
                        //         // controlerTable[controlerCnt] = (touch_switch_data[dest + 17]<<8)+ \
                        //         //                                 touch_switch_data[dest + 18];
                        //         // controlerCnt++;
                        //     }
                        // }
                        // else if (touch_switch_data[dest + 14] == 0XFD) //不上传
                        // {
                        //     if (touch_switch_data[dest + 16] == 0XFE)
                        //     {
                        //         // controlerTable = 
                        //     }
                        // }
                        // //////////////////////////////////
                        // else
                        {
                            key_code[0] = KEY_DOWN;
                            key_code[1] = touch_switch_data[dest + 14];
                            key_code[2] = touch_switch_data[dest + 15];
                            ComModel.send(&key_code[0], &com_len);
                        }
                    }
                    s = 1;
                    break;
                }
            }
        }
    }
    else //松开
    {
        if (s == 1) //之前已按下按钮
        {
            s = 0;
            if (((u16)touch_switch_data[dest + 10] << 8) + touch_switch_data[dest + 11] != 0XFF00)
            { //跳转页面
                if (pic_type_adapt(dirPath, &touch_switch_data[dest + 11], NULL, pic_path) == T_GIF)
                {
                    gif_decode((u8 *)pic_path, 0, 0, &pic);
                    UI_LCDBackupRenew();
                    page_num = touch_switch_data[dest + 11];
                }
                else if (UI_pic(pic_path, &pic) == 0)
                {
                    UI_disRegionCrossAdapt(&pic, 0, 0);
                    free(pic.pixelDatas);
                    LCD_Exec();
                    UI_LCDBackupRenew();
                    page_num = touch_switch_data[dest + 11];
                }
            }
            else if (((u16)touch_switch_data[dest + 12] << 8) + touch_switch_data[dest + 13] != 0XFF00)
            { //恢复按钮
                pic_type_adapt(dirPath, &page_num, NULL, pic_path);
                x1 = ((u16)touch_switch_data[dest + 2] << 8) + touch_switch_data[dest + 3];
                y1 = ((u16)touch_switch_data[dest + 4] << 8) + touch_switch_data[dest + 5];
                x2 = ((u16)touch_switch_data[dest + 6] << 8) + touch_switch_data[dest + 7];
                y2 = ((u16)touch_switch_data[dest + 8] << 8) + touch_switch_data[dest + 9];
                if (UI_pic_cut(pic_path, &pic, x1, y1, x2, y2) == 0)
                {
                    UI_disRegionCrossAdapt(&pic, x1, y1);
                    LCD_Exec();
                    free(pic.pixelDatas);
                }
            }
            if (touch_switch_data[dest + 14] != 0XFF) //判断是否发送键值
            {
                key_code[0] = KEY_UP;
                key_code[1] = touch_switch_data[dest + 14];
                key_code[2] = touch_switch_data[dest + 15];
                ComModel.send(&key_code[0], &com_len);
            }
        }
    }
}

int DW_TouchFileDecode(char *_fptr, size_t file_size)
{
    if() //command version
    {

    }
    else //DGUS version
    {

    }

    return 0; //success
}

int DW_KeyCtrlDecode(char *ctrl)
{
    return 0; //success
}

int DW_TextCtrlDecode(char *ctrl)
{
        if(*ctrl!=0X5A)
        return 1; //fail

        return 0; //success
}

int DW_PicCtrlDecode(char *ctrl)
{
        if(*ctrl!=0X5A)
        return 1; //fail

        return 0; //success
}

static int DW_DisCtrlDecode(char *ctrl)
{ //control decode and put to json
    
    if(*ctrl!=0X5A)
        return 1; //fail
    
    switch (*ctrl) //check ctrl type
    {
    case ctrl_text:
        /* run */
        break;
    
    default:
        return 2; //fail
        break;
    }

    return 0; //success
}

int DW_DisFileDecode(char *_fptr, size_t file_size)
{
    size_t i = 0, j = 0;
    const char head_code[] = {0X14, 0X44, 0X47, 0X55, 0X53, 0X5F, 0X32};
    char *fptr_page = _fptr, fptr_control = _fptr+0X4000;

    //check head
    for (i = 0; i < sizeof(head_code); i++)
    {
        if (*_fptr!=head_code[i])
            return 1; //fail
    }
    fptr_page += 18;

    for (i = 0; i < 256; i+=4) //here page number max 256
    {
        if (*fptr_page & 0XF0 == 0X40) //page start symbol
        {
            //this page has control number
            for (j = 0; j < *(fptr_page+2); j++)
            {
                // if(fptr_control-_fptr>=file_size)
                //     return 2; //fail
                if(*fptr_control==0XFF) //file tail is 0XFF
                    return 0;
                if(*fptr_control==0X5A) //control start is 0X5A
                    DW_DisCtrlDecode(fptr_control);
                fptr_control+=32; //next controll
            }
        }
        else
        {
            break;
        }
    }

    return 0; //success
}

void test_DW_file_decode(char *nameDest, char *nameSrc)
{
    int error = 0;
    unsigned char* _fptr = NULL;
    size_t file_size = 0;

    error = MTF_load_file(&_fptr, &file_size, nameSrc);
    if(!error)
        error = DW_DisFileDecode(_fptr, file_size);
        // error = DW_TouchFileDecode(_fptr, file_size);

    free(_fptr);
}

#endif

int main(int argc, char *argv[])
{
    /* print the version */
    printf("Version: %s\n", cJSON_Version());

    /* Now some samplecode for building objects concisely: */
    create_objects();

    FILE *fp = NULL, *fp2 = NULL;
    char *tempData = NULL;
    int total = 0;

    tempData = (char *)malloc(1024 * 1024);
    fp = fopen("./touch.bin", "rb");              //只读存在打开
    fp2 = fopen("./OUTPUT/touch_out.bin", "wb+"); //读写生成打开
    if (fp != NULL && fp2 != NULL)
    {
        // char str[] = "ssgerhbf";
        // printf("%d\r\n",fwrite(str, 1, sizeof(str) , fp));

        total = fread(tempData, 1, 1024 * 1024, fp);
        printf("%d\r\n", total);
        printf("%d\r\n", fwrite(tempData, 1, total, fp2));

        free(tempData);
        fclose(fp);
        fclose(fp2);
    }
    else
    {
        if (fp == NULL)
            printf("file1 error\r\n");
        if (fp2 == NULL)
            printf("file2 error\r\n");
    }

#ifdef __WINDOWS__
    system("pause");
#endif

    return 0;
}
