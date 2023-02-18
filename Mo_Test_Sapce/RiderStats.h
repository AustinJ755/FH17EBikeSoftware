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
        int Index;
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

#endif //UNTITLED1_RiderStats_H



rideStats* createRideArr();

rideStats* updateRideArr(float avgRidSpd, float assisMiles, float unassisMiles,
                    float curBattPer, float time, rideStats *stats);

rideAvg avgStruct(rideStats *stats);
