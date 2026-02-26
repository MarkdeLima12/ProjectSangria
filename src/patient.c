//
// Source file for patient.h
//
#include "../headers/patient.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void initPatient(Patient *arr) {
    srand(time(NULL));
    if (!arr) {
        printf("Array NULL");
        return;
    }

    for (int i = 0; i < 256; i++) {
        arr[i].age = rand() % 101; // random number between 0 and 100
        arr[i].status = STATUS_UNKNOWN;
        arr[i].HR = 255;
        arr[i].O2 = 255;
        arr[i].respRate = 255;
        arr[i].temp = 255;
    }
}

void processData(Patient *arr, char *message) {
    if (!message) {
        printf("NULL message");
        return;
    }

    char *ID = strtok(message, "/");
    char *type = strtok(NULL, "/");
    char *value = strtok(NULL, "/");

    // Converts the hex strings into numbers
    int patientID = strtol(ID, NULL, 16);
    // Assume if they die, we ain't bringing em back
    if (arr[patientID].status != STATUS_DECEASED) {
        SensorType sensType = (SensorType) strtol(type, NULL, 16);
        int val = strtol(value, NULL, 16);

        addData(&arr[patientID], sensType, &val);
        // sendData(patientID);
    }
}

void addData(Patient *patient, SensorType type, void *data) {
    if (!patient) {
        printf("Patient NULL");
        return;
    }
    if (!data) {
        printf("Data NULL");
        return;
    }
    switch (type) {
        case STYPE_STATUS:
            patient->status = (PatientStatus) (*(const unsigned char *) data);
            break;
        case STYPE_HR:
            patient->HR = *(const unsigned char *) data;
            break;
        case STYPE_O2:
            patient->O2 = *(const unsigned char *) data;
            break;
        case STYPE_RESP_RATE:
            patient->respRate = *(const unsigned char *) data;
            break;
        case STYPE_TEMPERATURE:
            patient->temp = *(const unsigned char *) data;
            break;
        default:
            break;
    }
}

void sendData(int patientID) {
    //Symbolic, but really does nothing
    printf("Sending Patient %d Data...\n", patientID);
}

void printPatient(Patient *patient) {
    static const char *statusText[] = {
        "GOOD",
        "STABLE",
        "FAIR",
        "SERIOUS",
        "CRITICAL",
        "UNKNOWN",
        "DECEASED"

    };
    const char *statusLabel = "INVALID";

    if (patient == NULL) {
        printf("Patient: NULL\n");
        return;
    }

    if (patient->status >= STATUS_GOOD && patient->status <= STATUS_UNKNOWN) {
        statusLabel = statusText[patient->status];
    }

    if (patient->status == STATUS_DECEASED) {
        patient->HR = 0;
        patient->O2 = 0;
        patient->respRate = 0;
        patient->temp = 21;
    }

    printf("Patient Data\n");

    if (patient->age == 255) printf("  Age: UNKNOWN\t\t");
    else printf("  Age: %u\t\t", (unsigned int) patient->age);

    printf("  Status: %s (%d)\n", statusLabel, patient->status);

    if (patient->HR == 255) printf("  HR: UNKNOWN\t\t");
    else printf("  HR: %u\t\t", (unsigned int) patient->HR);

    if (patient->O2 == 255) printf("  O2: UNKNOWN\n");
    else printf("  O2: %u\n", (unsigned int) patient->O2);\

    if (patient->respRate == 255) printf("  Resp Rate: UNKNOWN\t");
    else printf("  Resp Rate: %u\t", (unsigned int) patient->respRate);

    if (patient->temp == 255) printf("  Temperature: UNKNOWN\n");
    else printf("  Temperature: %u\n", (unsigned int) patient->temp);
}
