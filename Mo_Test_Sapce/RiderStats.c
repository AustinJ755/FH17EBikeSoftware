// RiderStats.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "RiderStats.h"
#include <string.h>


/*
 * create ride stats array
 * */
rideStats* createRideArr() {
    rideStats stats[1];

    stats[0].AvgRidSpd = SENTI;
    stats[0].AssisMiles = SENTI;
    stats[0].UnassisMiles = SENTI;
    stats[0].CurBattPer = SENTI;
    stats[0].Time = SENTI;
    stats[0].Index = SENTI;

    return stats;
}


/*
 * ridersArray() defines a struct with the following float items:
 * avg riding speed (only consist of speed recordings from when the bike isn't stopped),
 * Assisted miles, unassisted miles, current battery percentage, total miles,
 * times in milliseconds since last log
 */
rideStats* updateRideArr(float avgRidSpd, float assisMiles, float unassisMiles,
                    float curBattPer, float time, rideStats *stats) {

    int count = 0;
    rideStats *statsInc = stats; //pointer to increment

    //count stats elements
    while((statsInc->Index) != SENTI) {
        count++;
        statsInc++;
    }


    //make new array
    rideStats statsUp[count + 2]; //1 for new data, 1 for sentinel

    //reset
    statsInc = stats;

    //start copy
    for(int i = 0; i < count; i++) {
        statsUp[i].AvgRidSpd = statsInc->AvgRidSpd;
        statsUp[i].AssisMiles = statsInc->AssisMiles;
        statsUp[i].UnassisMiles = statsInc->UnassisMiles;
        statsUp[i].CurBattPer = statsInc->CurBattPer;
        statsUp[i].Time = statsInc->Time;
        statsUp[i].Index = statsInc->Index;

        statsInc++ //increment pointer
    }

    //add new data
    statsUp[count].AvgRidSpd = avgRidSpd;
    statsUp[count].AssisMiles = assisMiles;
    statsUp[count].UnassisMiles = unassisMiles;
    statsUp[count].CurBattPer = curBattPer;
    statsUp[count].Time = time;
    statsUp[count].Index = count; //index of new data is count

    //add sentinel
    statsUp[count + 1].AvgRidSpd = SENTI;
    statsUp[count + 1].AssisMiles = SENTI;
    statsUp[count + 1].UnassisMiles = SENTI;
    statsUp[count + 1].CurBattPer = SENTI;
    statsUp[count + 1].Time = SENTI;
    statsUp[count + 1].Index = SENTI; //index of new data is count

    //return new array
    return statsUp
}


/*
 * calculate all the averages and values
 * @param: stats is the array of rider statistics
 */
rideAvg avgStruct(rideStats *stats) {
    int count = 0;
    rideStats *statsInc = stats; //pointer to increment
    rideAvg averages;
    float totMiles = 0;

    //initialize struct
    averages.AvgSpd = 0;
    averages.TvlSpd = 0;
    averages.TotAssisMiles = 0;
    averages.TotUnassisMiles = 0;
    averages.percAssisMiles = 0;
    averages.perUnassisMiles = 0;
    averages.totRideTime = 0;
    averages.battLoss = 0;
    averages.perBattMile = 0;


    //count stats elements and add the totals
    while((statsInc->Index) != SENTI) {
        averages.AvgSpd += (statsInc->AvgRidSpd); // first add up total speed before divide
        averages.TotAssisMiles += (statsInc->AssisMiles);
        averages.TotUnassisMiles += (statsInc->UnassisMiles);
        averages.totRideTime += (statsInc->Time);

        count++;
        statsInc++;
    }

    //returns if there's no logged data
    if(count == 0) {
        return averages;
    }

    //total miles
    totMiles = averages.TotAssisMiles + averages.TotUnassisMiles;

    //calculate averages values
    averages.AvgSpd /= count;  //official averege speed
    averages.TvlSpd = totMiles / averages.totRideTime;
    averages.percAssisMiles = averages.TotAssisMiles / totMiles;
    averages.perUnassisMiles = averages.TotUnassisMiles / totMiles;
    averages.battLoss = (stats->CurBattPer) - ((statsInc--)->CurBattPer); //initial battery - last logged battery is battery loss
    averages.perBattMile = averages.battLoss / totMiles;

    return averages;
}

