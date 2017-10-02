#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <pthread.h>
#include <stdlib.h>

#include "myLists.h"

pthread_t localTid[3];
int tid;
int size;
int numberOfJudges = 5;
int numberOfDoctors = 5;
int rings[5] = {1};

MPI_Datatype MPI_MESSAGE_STRUCT;

typedef struct MessageStruct {
    int tid;
    int state;
    int request;
    int message;
} messageSend, messageReceive;

// thread semafors
//pthread_mutex_t printf_mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_mutex_init(&printf_mutex, NULL);
//pthread_mutex_lock(&printf_mutex);
//pthread_mutex_unlock(&printf_mutex);

// STATES:
#define FREE 0
#define FREE_CONNECTING 1
#define BUSY 2
#define BUSY_CONFIRMED 3

int threadState;

// MESSAGES:
// 1 - request for state
// 2 - request for connection
// 3 - request acceptation
// 4 - acceptation confirmation

// 3 - sending elder number
// 4 - sending elder hp to subtract
// 5 - end of fight

// todo wyświetlanie komunikatów o stanie
// todo zmienić wysyłanie wyników walk na pętlę for ???

void *addNewElders() {
    // todo dodawanie co jakiś czas nowych staruszków
}

void *receiveLoop() {
    int number;
    MPI_Status status;
    int i = 0;

    messageSend rcv;
    while(1)
    {
        MPI_Recv(&rcv, 1, MPI_MESSAGE_STRUCT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        printf("receive in %d\n", rcv.tid);
        i++;
        if(i == size-1) break;
    }
    threadState = BUSY;
}

int main(int argc, char** argv) {
    MPI_Status status;
    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);

    // struct init
    //const int nitems=2;
    int blocklengths[4] = {1,1,1,1};
    MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Aint     offsets[4];
    offsets[0] = offsetof(messageSend, tid);
    offsets[1] = offsetof(messageSend, state);
    offsets[2] = offsetof(messageSend, request);
    offsets[3] = offsetof(messageSend, message);
    MPI_Type_create_struct(4, blocklengths, offsets, types, &MPI_MESSAGE_STRUCT);
    MPI_Type_commit(&MPI_MESSAGE_STRUCT);

    //TODO nie wiemy ilu będzie emerytów, więc nie wiem czy nie powinniśmy przechowywać ich w liście
    int eldersCount = 10 + tid;
    int elders[eldersCount];

    int fightSubstractHP = 25;
    int receiverTid = tid;
    int i;
    for(i = 0; i < eldersCount; i++) elders[i] = 100;

    pthread_create(&(localTid[1]), NULL, receiveLoop, NULL);
    pthread_create(&(localTid[2]), NULL, addNewElders, NULL);



    threadState = FREE;








    for (i = 0; i < size; i++) {
        if (i != tid) {
            messageSend send;
            send.tid = tid;
            send.request = 1;
            MPI_Send(&send, 1, MPI_MESSAGE_STRUCT, i, 100, MPI_COMM_WORLD);
            printf("send from %d to %d\n", tid, i);
        }
    }

    while(1) {
        if (threadState == BUSY) {
            break;
        }
    }

//        while (1) {
//            while(1) {
//                threadState = FREE;
//                for (i = 0; i < size; i++) {
//                    int sendValue = 1;
//                    if (i != tid) MPI_Send(&sendValue, 1, MPI_INT, i, 100, MPI_COMM_WORLD);
//                }
//                while (1) if (threadState != FREE) break;
//                if (threadState == FREE_CONNECTING) MPI_Send(2, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//                while (1) if (threadState != FREE_CONNECTING) break;
//                if (threadState == BUSY) break;
//            }
//
//            // setting amount of elders
//            MPI_Send(&eldersCount, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//            int receiverEldersCount;
//            MPI_Recv(&receiverEldersCount, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//            int chosenEldersCount = (eldersCount < receiverEldersCount) ? eldersCount : receiverEldersCount;
//
//            //reservation
//            if (tid > receiverTid) { // other thread deals with reservation and fights
//                int receivedCommand;
//                while(1) {
//                    MPI_Recv(&receivedCommand, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//                    if (receivedCommand == 5) break;
//                    else {
//                        int elderNumber, hpToSubstract;
//                        MPI_Recv(&elderNumber, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//                        MPI_Recv(&hpToSubstract, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
//                        elders[elderNumber] -= hpToSubstract;
//                    }
//                }
//            }
//            else { // this thread deals with reservation and fights
//                //todo rezerwacja ringu, i liczby sędziów
//
//                for(i = 0; i < chosenEldersCount; i++) {
//                    sleep(5000); // todo czy to jest ok?
//                    if(rand()&2 == 0) { // our elder won
//                        elders[i] -= fightSubstractHP;
//                        MPI_Send(3, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//                        MPI_Send(fightSubstractHP*2, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//                    }
//                    else { // our elder lose
//                        elders[i] -= 2 * fightSubstractHP;
//                        MPI_Send(3, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//                        MPI_Send(fightSubstractHP, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//                    }
//                }
//
//                //todo po walce zarezerwuj lekarza
//                //todo zwolnij wszystkie zasoby
//
//
//                MPI_Send(5, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
//            }
//
//            //todo sprzątanie w liście staruszków
//        }


    pthread_join(localTid[1], NULL);
    pthread_join(localTid[2], NULL);
    MPI_Finalize();

    return 0;
}
