#include "hmm.h"
#include <math.h>
#define o(a) (a - 'A')

HMM hmm_model[8];
char seq[2500][MAX_SEQ];

double cal_delta(HMM hmm, double delta[MAX_SEQ][6], char s[MAX_SEQ]) {
    for(int t = 0; t < 50; t++) {
        for(int j = 0; j < 6; j++) {
            if(t == 0) {
                delta[t][j] = hmm.initial[j] * hmm.observation[o(s[t])][j];
            }
            else {
                double max = 0;
                for(int i = 0; i < 6; i++) {
                    double tmp = delta[t-1][i] * hmm.transition[i][j];
                    if(tmp > max)
                        max = tmp;
                }
                delta[t][j] = max * hmm.observation[o(s[t])][j];
            }
        }
    }
    double max = 0;
    for(int i = 0; i < 6; i++) {
        if(max < delta[49][i])
            max = delta[49][i];
    }
    return max;
}

int main(int argc, char *argv[]) {
    char *modellist = argv[1];
    char *test_seq = argv[2];
    char *result = argv[3];
    char model_name[MAX_LINE];
    FILE *modelfd = open_or_die(modellist, "r");
    int n_model;
    for(;fscanf(modelfd, "%s", model_name) != EOF; n_model++) {
        loadHMM(&hmm_model[n_model], model_name);
    }
    fclose(modelfd);
    FILE *testfd = open_or_die(test_seq, "r");
    int n_seq = 0;
    for(;fscanf(testfd, "%s", seq[n_seq]) != EOF; n_seq++);
    fclose(testfd);
    FILE *resultfd = open_or_die(result, "w");
    for(int n = 0; n < n_seq; n++) {
        double delta[MAX_SEQ][6];
        double max = 0;
        int max_i = 0;
        for(int i = 0; i < n_model; i++) {
            double tmp = cal_delta(hmm_model[i], delta, seq[n]);
            if(tmp > max) {
                max = tmp;
                max_i = i;
            }
        }
        fprintf(resultfd,"%s %e\n", hmm_model[max_i].model_name, max);
        printf("%s\n", hmm_model[max_i].model_name);
    }
    fclose(resultfd);
}
