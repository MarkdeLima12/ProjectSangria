/*
* Purpose:
*   Drives a full end-to-end simulation of patient sensor traffic using the ring buffer API.
* How it works:
*   - Initializes the ring buffer and patient table.
*   - For 1,000,000 iterations, generates a random message, writes it, reads one message,
*     and immediately processes it.
*   - Prints all patient data and reports a single total elapsed time.
* How this differs from main2.c:
*   - This file interleaves write and read/processing in a single loop and reports only
*     one aggregate timing value. main2.c splits init/write/read/print into separate phases
*     and times each phase independently.
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>


#include "headers/ringBufferAPI.h"
#include "headers/patient.h"

char *randomMessage() {
    unsigned int seed;
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &seed, sizeof(seed));
    close(fd);

    srand(seed);

    char *randMessage = malloc(8);
    int randID = rand() % 256;
    int randSensor = rand() % 5 + 1;
    int randValue;
    double randDbl;
    switch (randSensor) {
        case 1:
            // mkes it harder to die...
            randDbl = rand() / 6.00;
            if (randDbl == 1) {
                randValue = 6;
            } else {
                randValue = ((int) randDbl) % 5;
            }
            break;
        case 2: randValue = (rand() % (250 - 40)) + 40;
            break;
        case 3: randValue = (rand() % (100 - 80)) + 80;
            break;
        case 4: randValue = (rand() % (40 - 12)) + 12;
            break;
        case 5: randValue = (rand() % (40 - 35)) + 35;
            break;
        default: randValue = rand() % 5;
            randSensor = 1;
            break;
    }
    snprintf(randMessage, 8, "%02X/%X/%02X", randID, randSensor, randValue);
    return randMessage;
}

int main() {
    // Init variables
    clock_t start;
    clock_t end;
    RingBuffer *ringBuffer = initBuffer();
    Patient patient[256] = {};
    char *temp = NULL;
    double time;
    initPatient(patient);
    start = clock();
    for (int i = 0; i < 1000000; i++) {
        temp = randomMessage();
        bufferWrite(ringBuffer, temp);
        free(temp);

        temp = bufferRead(ringBuffer);
        if (temp != NULL) {
            processData(patient, temp);
            free(temp);
        }
    }
    end = clock();

    time = (double) (end - start) / CLOCKS_PER_SEC;

    // Display Patient Data
    for (int i = 0; i < 256; i++) {
        sendData(i);
        printPatient(&patient[i]);
    }

    printf("Elapsed Time: %f \n", time);
    freeRingBuffer(&ringBuffer);
}
