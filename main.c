#include <stdio.h>
#include <stdlib.h>

#include "headers/ringBufferAPI.h"
#include "headers/hashMap.h"

int main() {

    // Init variables
    RingBuffer *ringBuffer = initBuffer();
    PatientHashMap *patientHashMap = initPatientHashMap();
    char *temp = NULL;

    // This will just go on repeat essentially
    // Maybe abstract this so there is a random function that makes random values for testing
    bufferWrite(ringBuffer, "This is the message");
    sendData();

    // Clean up variables
    free(temp);
    freeHashMap(&patientHashMap);
    freeRingBuffer(&ringBuffer);
}