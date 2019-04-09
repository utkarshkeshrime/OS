#include <pthread.h>

struct station
{
    pthread_mutex_t pc_mutex;//mutex for the critical sections in code
    pthread_cond_t arrival_train;//codition variable for all users that the train reach to the station
    pthread_cond_t giong_away ;//condition variable for the train to leave the station
    int passengers;//number of robots reach to station wait the train
    int indicate;// to indicate that there is atrain  on the board and it equal the number of sets in it
    int available_seats;//the number of free sets on the train
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);
