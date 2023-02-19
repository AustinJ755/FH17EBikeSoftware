// RiderStats.h
#ifndef UNTITLED1_RiderStats_H
#define UNTITLED1_RiderStats_H

#define SENTI -1f

typedef struct RideStats {
        float AvgRidSpd;
        float AssisMiles;
        float UnassisMiles;
        float CurBattPer;
        float Time;
} rideStats;

typedef struct RideAvg {
    float AvgSpd;
    float TvlSpd;
    float TotAssisMiles;
    float TotUnassisMiles;
    float percAssisMiles;
    float perUnassisMiles;
    float totRideTime;
    float battLoss;
    float perBattMile;
} rideAvg;

rideStats logStats[50];

int statsIndex;

#endif //UNTITLED1_RiderStats_H



void statsInitialize();

void updateRideArr(float avgRidSpd, float assisMiles, float unassisMiles,
                    float curBattPer, float time);

rideAvg avgStruct();
