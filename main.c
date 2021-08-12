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
#include "DW_FileDecodeLib.h"

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
int print_preallocated(char *name, cJSON *root)
{
    /* declarations */
    char *out = NULL;
    char *buf = NULL;
    char *buf_fail = NULL;
    size_t len = 0;
    size_t len_fail = 0;
    FILE *fp = NULL;
    size_t total = 1;

    /* formatted print 注意释放out内存*/
    out = cJSON_Print(root);

    /* create buffer to succeed */
    /* the extra 5 bytes are because of inaccuracies when reserving memory */
    len = strlen(out) + 5;
    buf = (char*)malloc(len);
    if (buf == NULL)
    {
        printf("Failed to allocate memory.\n");
        return 1;
    }

    /* create buffer to fail */
    len_fail = strlen(out);
    buf_fail = (char*)malloc(len_fail);
    if (buf_fail == NULL)
    {
        printf("Failed to allocate memory.\n");
        return 1;
    }

    /* Print to buffer */
    if (!cJSON_PrintPreallocated(root, buf, (int)len, 1)) {
        printf("cJSON_PrintPreallocated failed!\n");
        if (strcmp(out, buf) != 0) {
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
    printf("%s\n", buf); //打印输出JSON

    /*输出JSON文件*/
    if (name != NULL)
    {
        while (buf[total]) //检查字节数
        {
            total++;
        }
        fp = fopen(name, "wb+"); //读写生成打开
        fwrite(buf, 1, total, fp);
        fclose(fp);
    }

    /* force it to fail */
    if (cJSON_PrintPreallocated(root, buf_fail, (int)len_fail, 1)) {
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
    if (print_preallocated(NULL, root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    /* Our "days of the week" array: */
    root = cJSON_CreateStringArray(strings, 7);

    if (print_preallocated(NULL, root) != 0)
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

    if (print_preallocated(NULL, root) != 0)
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

    if (print_preallocated(NULL, root) != 0)
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

    if (print_preallocated(NULL, root) != 0)
    {
        cJSON_Delete(root);
        exit(EXIT_FAILURE);
    }
    cJSON_Delete(root);

    root = cJSON_CreateObject();
    cJSON_AddNumberToObject(root, "number", 1.0 / zero);

    if (print_preallocated(NULL, root) != 0)
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

int main(int argc, char *argv[])
{
    /* print the version */
    printf("Version: %s\n", cJSON_Version());

    /* Now some samplecode for building objects concisely: */
    // create_objects();
    DW_FileDecode("./HMIConfig.json", "./touch.bin", "./display.bin");

    FILE *fp = NULL, *fp2 = NULL;
    char *tempData = NULL;
    int total = 0;

    tempData = (char *)malloc(1024 * 1024);
    fp = fopen("./touch.bin", "rb");              //只读存在打开
    fp2 = fopen("./build/touch_out.bin", "wb+"); //读写生成打开
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
