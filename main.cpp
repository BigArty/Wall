#include <pthread.h>
#include <vector>
#include <stdio.h>
#include <random>
#include <unistd.h>

using namespace std;

struct data {
    int min;
    int max;
    int n, m;
    int seed;
    pthread_mutex_t *mutex;
    int *bricks;
    int startRow;
    bool *isWorking;
};

void printWall(int *bricks, int m, int br) {
    int n = 0;
    for (int i = m / 2-1; i >= 0; --i) {
        n = bricks[i*2 + 1];
        if (n >= 1) {
            printf("0]");
            for (int j = 1; j < n-((n-1)/br); j++) {
                printf("[00]");
            }
            if (n == br+1) {
                printf("[0");
            }
        }
        printf("\n");
        n = bricks[i*2];
        for (int j = 0; j < n; j++) {
            printf("[00]");
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);
}

void *prntthrd(void *arg) {
    data *d = (data *) arg;
    pthread_mutex_lock(d->mutex);
    while (*(d->isWorking)) {
        printWall(d->bricks, d->m, d->n);
        pthread_mutex_unlock(d->mutex);
        sleep(1);
        pthread_mutex_lock(d->mutex);
    }
    printWall(d->bricks, d->m, d->n);
    pthread_mutex_unlock(d->mutex);
}

void *thread(void *arg) {
    data *d = (data *) arg;
    pthread_mutex_lock(d->mutex);
    int n = d->n+d->startRow;
    int m = d->m;
    mt19937 gen(d->seed);
    uniform_int_distribution<> uid(d->min, d->max);//генерация случайных нормальнораспределённых чисел
    //int a = uid(gen);
    //printf("%i\n",d->startRow);
    pthread_mutex_unlock(d->mutex);
    for(int j=0;j<m/2;++j){
        for(int i=0;i<n;++i){
            pthread_mutex_lock(d->mutex);
            while(((j+d->startRow)!=0)&&(d->bricks[j*2+d->startRow-1]<i+2-d->startRow)&&((d->n==n)||(i!=n-1))){
                pthread_mutex_unlock(d->mutex);
                usleep(100);
                pthread_mutex_lock(d->mutex);
            }
            pthread_mutex_unlock(d->mutex);
            sleep((unsigned int)uid(gen));
            pthread_mutex_lock(d->mutex);
            d->bricks[j*2+d->startRow]++;
            pthread_mutex_unlock(d->mutex);
        }
    }
    pthread_mutex_lock(d->mutex);
    if(d->startRow==1){
        *(d->isWorking)=false;
    }
    pthread_mutex_unlock(d->mutex);
}

int main() {
    int n, m, pmin, pmax, smin, smax;
    scanf("%i %i", &n, &m);
    scanf("%i %i", &pmin, &pmax);
    scanf("%i %i", &smin, &smax);
    int *brick = (int *) malloc(sizeof(int) * m);
    for (int i = 0; i < m; ++i) {
        brick[i] = 0;
    }
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    bool b = true;
    srand(time(NULL));
    data *datap = new data();
    data *datas = new data();
    datap->m = datas->m = m;
    datap->n = datas->n = n;
    datap->seed = rand();
    datas->seed = rand();
    datap->mutex = datas->mutex = &mutex;
    datap->bricks = datas->bricks = brick;
    datap->min = pmin;
    datas->min = smin;
    datap->max = pmax;
    datas->max = smax;
    datap->startRow = 0;
    datas->startRow = 1;
    datas->isWorking = datap->isWorking = &b;
    pthread_t p, s, prnt;
    if (pthread_create(&p, NULL, thread, datap) != 0) {
        perror("thread create");
        exit(3);
    }
    if (pthread_create(&s, NULL, thread, datas) != 0) {
        perror("thread create");
        exit(3);
    }
    if (pthread_create(&prnt, NULL, prntthrd, datas) != 0) {
        perror("thread create");
        exit(3);
    }
    pthread_join(p, NULL);
    pthread_join(s, NULL);
    pthread_join(prnt, NULL);
}