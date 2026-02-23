//
// Created by markd on 2026-02-22.
//

#ifndef LIQUIDANALYTICSTECHNICAL_HASHMAP_H
#define LIQUIDANALYTICSTECHNICAL_HASHMAP_H

typedef struct PatientHashMap {
    int size;
    struct Patient *data[16]; // Fixed
} PatientHashMap;

PatientHashMap *initPatientHashMap();
void addPatient();
void removePatient();
void sendData();
void freeHashMap(PatientHashMap **hashMap);
#endif //LIQUIDANALYTICSTECHNICAL_HASHMAP_H