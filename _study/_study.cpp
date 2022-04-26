#include <iostream>
#include "_test_data.h"

//char ch = 'a';
//unsigned int ui = 205;
//int i = -205;
//long l = 200000;
//float f = 3.14;
//double d = 3.1415926;
//char chs[] = "abcdefg";
//char* chs_p = chs;
//wchar_t wchs[] = L"abcdefghijklmnopqrstuvwxyz";
//wchar_t* wchs_p = wchs;

int main()
{
    printf("printf:\n");
    printf("%c\n", ch);
    printf("%u\n",ui);
    printf("%d\n", i);
    printf("%ld\n", l);
    printf("%f\n", f);
    printf("%g\n", d);
    for (int i = 0; i < sizeof chs; i++) {
        printf("%c", chs[i]);
    }
    printf("\n");
    printf("%ls\n", wchs);
    printf("%c\n\n", chinese);

    printf("snprintf:\n");
    snprintf(ch_temp,sizeof ch,ch_p);
    snprintf(ch_temp, sizeof ui, ui_p);


    return 0;
}

