#include <stdio.h>
#include <unistd.h>
#include <mpi.h>
#include <stdlib.h>

#include "myLists.h"

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

void addNewElders() {
    // todo dodawanie co jakiś czas nowych staruszków
}

void receiveLoop() {
    while(1)
    {
        int number;
        MPI_Status status;
        MPI_Recv(&number, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        switch (number)
        {
            case 0:
                break;

        }



    }
}

int main(int argc, char** argv) {
    int tid, size;
    MPI_Status status;
    MPI_Init(&argc, argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);

    //todo zabezpieczyć??
    int numberOfJudges = atoi(argv[1]);
    int numberOfDoctors = atoi(argv[2]);
    int rings[atoi(argv[3])];

    //TODO nie wiemy ilu będzie emerytów, więc nie wiem czy nie powinniśmy przechowywać ich w liście
    int eldersCount = 10;
    int elders[eldersCount];

    int fightSubstractHP = 25; // todo czy 25 jest ok?
    int receiverTid = tid;
    int i;
    for(i = 0; i < eldersCount; i++) elders[i] = 100;


    if(fork() == 0) {
        if (fork() == 0) {
            addNewElders();
        }
        else {
            receiveLoop();
        }
    }
    else {
        while (1) {
            while(1) {
                threadState = FREE;
                for (i = 0; i < size; i++) {
                    if (i != tid) MPI_Send(1, 1, MPI_INT, i, MPI_ANY_TAG, MPI_COMM_WORLD);
                }
                while (1) if (threadState != FREE) break;
                if (threadState == FREE_CONNECTING) MPI_Send(2, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
                while (1) if (threadState != FREE_CONNECTING) break;
                if (threadState == BUSY) break;
            }

            // setting amount of elders
            MPI_Send(&eldersCount, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
            int receiverEldersCount;
            MPI_Recv(&receiverEldersCount, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            int chosenEldersCount = (eldersCount < receiverEldersCount) ? eldersCount : receiverEldersCount;

            //reservation
            if (tid > receiverTid) { // other thread deals with reservation and fights
                int receivedCommand;
                while(1) {
                    MPI_Recv(&receivedCommand, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                    if (receivedCommand == 5) break;
                    else {
                        int elderNumber, hpToSubstract;
                        MPI_Recv(&elderNumber, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        MPI_Recv(&hpToSubstract, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
                        elders[elderNumber] -= hpToSubstract;
                    }
                }
            }
            else { // this thread deals with reservation and fights
                //todo rezerwacja ringu, i liczby sędziów

                for(i = 0; i < chosenEldersCount; i++) {
                    sleep(5000); // todo czy to jest ok?
                    if(rand()&2 == 0) { // our elder won
                        elders[i] -= fightSubstractHP;
                        MPI_Send(3, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
                        MPI_Send(fightSubstractHP*2, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
                    }
                    else { // our elder lose
                        elders[i] -= 2 * fightSubstractHP;
                        MPI_Send(3, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
                        MPI_Send(fightSubstractHP, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
                    }
                }

                //todo po walce zarezerwuj lekarza
                //todo zwolnij wszystkie zasoby


                MPI_Send(5, 1, MPI_INT, receiverTid, MPI_ANY_TAG, MPI_COMM_WORLD);
            }

            //todo sprzątanie w liście staruszków
        }
    }

    MPI_Finalize();

    return 0;
}
