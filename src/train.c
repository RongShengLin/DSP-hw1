#include "hmm.h"
#include <math.h>
#define f(a) (a - 'A')
int it;
char *model_init;
char *train_data;
char *out_model;
HMM hmm_initial;
double a[MAX_SEQ][6];
double b[MAX_SEQ][6];
double ga[MAX_SEQ][6];
double e[MAX_SEQ][6][6];
double A[6][6][2];
double B[6][6][2];
char seq[10000][MAX_SEQ];


void cal_a(char s[MAX_SEQ]) {
    for(int i = 0; i < 50; i++) {
        for(int j = 0; j < 6; j++) {
            if(i == 0) {
                a[i][j] = hmm_initial.initial[j] * hmm_initial.observation[f(s[i])][j];
            }
            else {
                a[i][j] = 0;
                for(int k = 0; k < 6; k++) {
                    a[i][j] += (a[i-1][k] * hmm_initial.transition[k][j]
                                * hmm_initial.observation[f(s[i])][j]);
                }
            }
        }
    }
    return;
}

void cal_b(char s[MAX_SEQ]) {
    for(int i = 49; i >= 0; i--) {
        for(int j = 0; j < 6; j++) {
            if(i == 49) {
                b[i][j] = 1;
            }
            else {
                b[i][j] = 0;
                for(int k = 0; k < 6; k++) {
                    b[i][j] += (b[i+1][k] * hmm_initial.transition[j][k]
                                * hmm_initial.observation[f(s[i+1])][k]);
                }
            }
        }
    }
    return;
}

void cal_ga() {
    for(int t = 0; t < 50; t++) {
        double tmp = 0;
        for(int j = 0; j < 6; j++) {
            tmp += (a[t][j] * b[t][j]);
        }
        for(int i = 0; i < 6; i++) {
            ga[t][i] = (a[t][i] * b[t][i]) / tmp;
        }
    }
    return;
}

void cal_e(char s[MAX_SEQ]) {
    for(int t = 0; t < 49; t++) {
        double tmp = 0;
        for(int k = 0; k < 6; k++) {
            for(int l = 0; l < 6; l++) {
                tmp += (a[t][k] * b[t+1][l] * hmm_initial.transition[k][l]
                            * hmm_initial.observation[f(s[t+1])][l]);
            }
        }
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 6; j++) {
                e[t][i][j] = (a[t][i] * b[t+1][j] * hmm_initial.transition[i][j]
                              * hmm_initial.observation[f(s[t+1])][j]) / tmp;
            }
        }
    }
}

void update_pi(double pi[6], int N) {
    for(int i = 0; i < 6; i++) {
        pi[i] += (ga[0][i] / N);
    }
}

void set_AB() {
    for(int i = 0; i < 6; i++)
        for(int j = 0; j < 6; j++)
            for(int k = 0; k < 2; k++) {
                A[i][j][k] = 0;
                B[i][j][k] = 0;
            }
}

void update_A() {
    for(int t = 0; t < 49; t++) {
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < 6; j++) {
                A[i][j][0] += e[t][i][j];
                A[i][j][1] += ga[t][i];
            }
        }
    }
}

void update_B(char s[MAX_SEQ]) {
    for(int t = 0; t < 50; t++) {
        for(int i = 0; i < 6; i++) {
            for(int k = 0; k < 6; k++) {
                B[k][i][1] += ga[t][i];
                if(k == f(s[t])) {
                    B[k][i][0] += ga[t][i];
                }
            }
        }
    }
}

double cal_acc() {
    double acc = 0;
    for(int i = 0; i < 6; i++)
        acc += (a[49][i]);
    return acc;
}

int main(int argc, char *argv[])
{
    //load arguments and model
    it = atoi(argv[1]);
    model_init = argv[2];
    train_data = argv[3];
    out_model = argv[4];
    loadHMM( &hmm_initial, model_init);
    hmm_initial.model_name = out_model;
    FILE* tr = open_or_die(train_data, "r");
    int seq_n = 0;
    for(; fscanf(tr, "%s", seq[seq_n]) != EOF; seq_n++);
    for(int i = 0; i < it; i++) {
        double pi[6] = {0, 0, 0, 0, 0, 0};
        double acc = 0;
        set_AB();
        for(int j = 0; j < seq_n; j++) {
            cal_a(seq[j]);
            cal_b(seq[j]);
            cal_ga();
            cal_e(seq[j]);
            update_pi(pi, seq_n);
            update_A();
            update_B(seq[j]);
            acc += cal_acc();
        }
        double total_pi = 0;
        for(int j = 0; j < 6; j++) {
            hmm_initial.initial[j] = pi[j];
            total_pi += pi[j];
            double t_tran = 0, t_ob = 0;
            for(int k = 0; k < 6; k++) {
                t_tran += A[j][k][0] / A[j][k][1];
                t_ob += B[k][j][0] / B[k][j][1];
                hmm_initial.transition[j][k] = A[j][k][0] / A[j][k][1];
                hmm_initial.observation[j][k] = B[j][k][0] / B[j][k][1];
            }
            //printf("tran ob %lf %lf\n", t_tran, t_ob);
        }
        //printf("acc: %e\n", acc);
        //printf("pi %lf\n", total_pi);
    }

    FILE* fd = open_or_die(out_model, "w");
	dumpHMM(fd, &hmm_initial );
    fclose(fd);
    fclose(tr);
	return 0;
}
