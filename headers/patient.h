//
// Created by markd on 2026-02-22.
//

#ifndef LIQUIDANALYTICSTECHNICAL_PATIENTDATA_H
#define LIQUIDANALYTICSTECHNICAL_PATIENTDATA_H
#include <stdbool.h>

// Enum for the status of a patient
typedef enum PatientStatus{
    STATUS_GOOD = 0,
    STATUS_STABLE = 1,
    STATUS_FAIR = 2,
    STATUS_SERIOUS = 3,
    STATUS_CRITICAL = 4,
    STATUS_DECEASED = 5
} PatientStatus;

// Enum for the type of Data coming in
typedef enum SensorType{
    STYPE_AGE = 0,
    STYPE_STATUS = 1,
    STYPE_HR = 2,
    STYPE_O2 = 3,
    STYPE_BLOOD_PRESSURE = 4,
    STYPE_HEART_RHYTHM = 5,
    STYPE_RESP_RATE = 6,
    STYPE_TEMPERATURE = 7
} SensorType;

// Struct for the patient and its data
typedef struct Patient {
    unsigned char ID;
    unsigned char age;
    PatientStatus status;
    int HR;
    int O2;
    int bloodPressure;
    int heartRhythm;
    int respRate;
    int temp;
    struct Patient *next; // so they can be connected as a DLL
    struct Patient *prev;
} Patient;

void initPatient();
void processData();
void addData(SensorType type, int data);
bool isFull();



#endif //LIQUIDANALYTICSTECHNICAL_PATIENTDATA_H