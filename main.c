#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>



#include "headers/ringBufferAPI.h"
#include "headers/patient.h"

char* randomMessage() {
    unsigned int seed;
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, &seed, sizeof(seed));
    close(fd);

    srand(seed);

    char * randMessage = malloc(8);
    int randID = rand()%256;
    int randSensor = rand()%5+1;
    int randValue;
    double randDbl;
    switch (randSensor) {
        case 1:
            // mkes it harder to die...
            randDbl = rand()/6.00;
            if (randDbl == 1) {
                randValue = 6;
            }
            else {
                randValue = ((int)randDbl)%6;
            }
            break;
        case 2: randValue = (rand()%(250-40))+40; break;
        case 3: randValue = (rand()%(100-80))+80; break;
        case 4: randValue = (rand()%(40-12))+12; break;
        case 5: randValue = (rand()%(40-35))+35; break;
        default: randValue = rand()%7; randSensor = 1; break;
    }
    snprintf(randMessage, 8, "%02X;%X;%02X", randID, randSensor, randValue);
    // printf("%s\n", randMessage);
    return randMessage;
}

int main() {
    // Init variables
    clock_t start = clock();
    clock_t end;
    RingBuffer *ringBuffer = initBuffer();
    Patient patient[256] = {};
    char *temp = NULL;
    double init = 0, write = 0, read = 0, print = 0;
    initPatient(patient);
    end = clock();

    init = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    // Writing to the buffer
    for (int i = 0; i<1000000; i++) {
        temp = randomMessage();
        bufferWrite(ringBuffer, temp);
        free(temp);
    }
    end = clock();
    write = (double)(end - start) / CLOCKS_PER_SEC;
    // reading and processing the data
    start = clock();
    for (int i = 0; i < 1000000; i++){
        temp = bufferRead(ringBuffer);
        if (temp != NULL) {
            processData(patient, temp);
            free(temp);
        }
    }
    end = clock();

    read = (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();
    // Display Patient Data
    for (int i = 0; i<256; i++) {
        sendData(i);
        printPatient(&patient[i]);
    }
    end = clock();
    print = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Elapsed Time:\n");
    printf("  Init: %f seconds\n", init);
    printf("  Write: %f seconds\n", write);
    printf("  Read: %f seconds\n", read);
    printf("  Print: %f seconds\n", print);
    freeRingBuffer(&ringBuffer);
}