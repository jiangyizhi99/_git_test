#include <iostream>
#include "_test_data.h"

int main()
{
    printf("printf:\n");
    printf("%c\n", ch);
    printf("%u\n", ui);
    printf("%d\n", i);
    printf("%ld\n", l);
    printf("%1.2f\n", f);
    printf("%lf\n", d);
    for (int i = 0; i < sizeof chs; i++) {
        printf("%c", chs[i]);
    }
    printf("\n");
    printf("%ls\n", wchs);
    printf("%s\n\n", chinese);

    printf("snprintf:\n");
    snprintf(ch_temp, sizeof(ch_temp),  "abcd");
    std::cout << ch_temp << std::endl;
    snprintf(ch_temp, sizeof ui, ui_p);
    std::cout << ch_temp;
    snprintf(ch_temp, sizeof i, i_p);
    std::cout << ch_temp;
    snprintf(ch_temp, sizeof l, l_p);
    std::cout << ch_temp;
    snprintf(ch_temp, sizeof f, f_p);
    std::cout << ch_temp;
    snprintf(ch_temp, sizeof d, d_p);
    std::cout << ch_temp;

    return 0;
}