//
// Created by markd on 2026-02-22.
//

#ifndef LIQUIDANALYTICSTECHNICAL_PATIENTDATA_H
#define LIQUIDANALYTICSTECHNICAL_PATIENTDATA_H

// Enum for the status of a patient
typedef enum PatientStatus{
    STATUS_GOOD = 0,
    STATUS_STABLE = 1,
    STATUS_FAIR = 2,
    STATUS_SERIOUS = 3,
    STATUS_CRITICAL = 4,
    STATUS_DECEASED = 6,
    STATUS_UNKNOWN = 5,
} PatientStatus;

// Enum for the type of Data coming in
typedef enum SensorType{
    STYPE_STATUS = 1,
    STYPE_HR = 2,
    STYPE_O2 = 3,
    STYPE_RESP_RATE = 4,
    STYPE_TEMPERATURE = 5
} SensorType;

// Struct for the patient and its data
typedef struct Patient {
    unsigned char age;
    PatientStatus status;
    unsigned char HR;
    unsigned char O2;
    unsigned char respRate;
    unsigned char temp;
} Patient;

/*
 * Creates a new patient in memory
 * All chars = 255
 * All pointers = NULL
 */
void initPatient(Patient *patient);

/*
 * Processes data and calls addData with correct type
 * Message structure XX;X;XX\0 => PatientID;SensorID;SensorVal\0
 * @param message | raw message data to be processed
 * @param arr | patient array that we use for addData
 */
void processData(Patient *arr, char *message);

/*
 * memcpys the data in the correct spot based on the SensorType
 * @param patient | patient struct to update
 * @param type | Sensor the data was collected from
 * @param data | pointer to beginning of correct chunk of raw message
 */
void addData(Patient *patient, SensorType type, void* data);

/*
 * Prints all patient fields in a readable format.
 * @param patient | patient to print
 */
void printPatient(Patient *patient);

/*
 * Sends Data to the LLM
 * @param patientID | bs int variable used for making the print statement nice
 */
void sendData(int patientID);

#endif //LIQUIDANALYTICSTECHNICAL_PATIENTDATA_H
