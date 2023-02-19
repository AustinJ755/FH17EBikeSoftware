// RiderStats.c

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "RiderStats.h"
#include <string.h>


/*
 * create ride stats array
 */
void statsInitialize() {
    statsIndex = 0;
}


/*
 * ridersArray() defines a struct with the following float items:
 * avg riding speed (only consist of speed recordings from when the bike isn't stopped),
 * Assisted miles, unassisted miles, current battery percentage, total miles,
 * times in milliseconds since last log and logs it into  array
 */
void updateRideArr(float avgRidSpd, float assisMiles, float unassisMiles,
                    float curBattPer, float time) {

    //add new data
    logStats[statsIndex].AvgRidSpd = avgRidSpd;
    logStats[statsIndex].AssisMiles = assisMiles;
    logStats[statsIndex].UnassisMiles = unassisMiles;
    logStats[statsIndex].CurBattPer = curBattPer;
    logStats[statsIndex].Time = time;

    //increment stats index
    statsIndex++;
}


/*
 * calculate all the averages and values
 * @param: stats is the array of rider statistics
 */
rideAvg avgStruct() {
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
    for(int i = 0; i <= statsIndex; i++) {
        averages.AvgSpd += logStats[i].AvgRidSpd; // first add up total speed before divide
        averages.TotAssisMiles += logStats[i].AssisMiles;
        averages.TotUnassisMiles += logStats[i].UnassisMiles;
        averages.totRideTime += logStats[i].Time;
    }

    //returns if there's no logged data
    if(statsIndex == 0) {
        return averages;
    }

    //total miles
    totMiles = averages.TotAssisMiles + averages.TotUnassisMiles;

    //calculate averages values
    averages.AvgSpd /= (statsIndex + 1);  //official averege speed
    averages.TvlSpd = totMiles / averages.totRideTime;
    averages.percAssisMiles = averages.TotAssisMiles / totMiles;
    averages.perUnassisMiles = averages.TotUnassisMiles / totMiles;
    averages.battLoss = logStats[0].CurBattPer - logStats[statsIndex].CurBattPer; //initial battery - last logged battery is battery loss
    averages.perBattMile = averages.battLoss / totMiles;

    return averages;
}

