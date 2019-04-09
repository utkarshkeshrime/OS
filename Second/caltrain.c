#include <pthread.h>
#include "caltrain.h"

/**
*insialize all the paramters in the structure with it's insiale state
*/

void
station_init(struct station *station)
{
    pthread_mutex_init(&station->pc_mutex, NULL);///insialize mutex to null there is no passengers or trains on the tation
    pthread_cond_init(&station->giong_away, NULL);/// insialize null because there is no train on the station
    pthread_cond_init(&station->arrival_train, NULL);///intsalize null because there is no train arrive the station
    station->indicate=0;/// no train in the station
    station->passengers=0;///there is no one in the statin
    station->available_seats=0;///no train on the station an there is no free sets for waiting passengers
}


/**
*this function when the train arrive in the station call it
*it sets all waiting passengers free so they can get in the train
*it also wait until signal from function on board to leave the station
*/

void
station_load_train(struct station *station, int count)
{
    station->indicate=count;///indicate that the there is a train on the station set the number of free sets for passengers equal to the count
    station->available_seats=count;///set the number of available sets for passengers equal to the count

    /**
    *this condition check if there is no waiting passenger on the station
    *or the train come to the station is full so no on can get in it
    *in this two cases the train leave the station
    */
    if(station->passengers==0||count==0)
    {
        station->indicate=0;///set indicate equal zero so there is no train in the station
        ///leave the station.....
        return;
    }
    ///start critical section
    pthread_mutex_lock(&station->pc_mutex);
    while(station->passengers>0&&station->indicate>0)
    {
        ///set all waiting passengers free to get in the train
        pthread_cond_broadcast (&station->arrival_train);
        ///wait until all the free sets in the train is full to leave the station
        pthread_cond_wait (&station->giong_away,&station->pc_mutex);
    }
    pthread_mutex_unlock(&station->pc_mutex);
}


/**
*this function is called if there is an passenger come to the station and make it wait
*if there is a train and avaliabe space in it fo theis passenger set this passenger free
*/

void
station_wait_for_train(struct station *station)
{
    ///start the critical section..
    pthread_mutex_lock(&station->pc_mutex);
    ///increament the number of waiting passenger on the station
    station->passengers=station->passengers+1;
    ///check if there is a train or there is any free spaces in the train...
    while(station->indicate==0)
    {
        ///wait until the train arrives in the train...
        pthread_cond_wait (&station->arrival_train,&station->pc_mutex);
    }
    station->indicate--;///decrement the number of free spaces to stop othe passengers and catch them in wait if there is no free spaces
    ///close critical section
    pthread_mutex_unlock(&station->pc_mutex);
}


/**
*this function indicato to the train to leave the station
*fill all free sets in it and derrement the waiting passengers on the station
*it gives signal to train to leave in any on of both condition occure
*first : the number of passenger finished so there is no on in the sy=tation in this case the train leave
*second : the number of free sets in the train equal zero
*/

void
station_on_board(struct station *station)
{

    ///start critical section....
    pthread_mutex_lock(&station->pc_mutex);
    station->available_seats--;///decrement the avaliable spaces in the train
    station->passengers--;///decremnet the number of waiting passenger on the station
    ///if there is no one wait the train on the station
    if(station->passengers==0)
    {
        ///send signal to the train to leave the station
        pthread_cond_signal (&station->giong_away);
    }
    ///if there is the number of sets in the train equal zero and no space for more on passenger
    else if(station->indicate==station->available_seats&&station->available_seats==0)
    {
        ///send signal to the train to leave the station
        pthread_cond_signal (&station->giong_away);
    }
    ///close critical section...
    pthread_mutex_unlock(&station->pc_mutex);
}
