#include "hmm.h"
#include <math.h>


int main(int argc, char *argv[]) {
    char *file1 = argv[1];
    char *file2 = argv[2];
    FILE *fd1 = open_or_die(file1, "r");
    FILE *fd2 = open_or_die(file2, "r");
    char s1[MAX_LINE], s2[MAX_LINE];
    double fail = 0;
    for(int i = 0; i < 2500; i++) {
        fscanf(fd1,"%s", s1);
        fscanf(fd2,"%s", s2);
        printf("%s ", s1);
        printf("%s %d\n", s2, strcmp(s1, s2));
        if(strcmp(s1, s2) == 0)
            fail += 1;
    }
    printf("%lf\n", fail/2500);
    fclose(fd1);
    fclose(fd2);
}
