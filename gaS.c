
/*=================================================
              Genetic Algorithm - gaS.c
              Originally Developed For
    CS 1783 - Artificial Intelligence Programming
         Assignment III - Genetic Algorithms
           Evan William Gretok - 20180904
  =================================================*/


// Inclusions
#include <math.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "datatypes.h"


// Definitions
typedef int bool;
#define true 1
#define false 0

// Global Variable Declaration and Initialization

// Sample Code for Additional Definitions
// Provided by Dr. James Bilitski of the University of Pittsburgh at Johnstown
// Used and Modified with Permission

typedef enum days { MWF, TH, LAST_DAY } days;

char* dayNames[LAST_DAY] = { "MWF", "TH" };

TimePeriod timePeriods[NUM_TIME_PERIODS] = {
  // ID Day StartTime
  {  0, MWF, 8    },
  {  1, MWF, 9    },
  {  2, MWF, 10   },
  {  3, MWF, 11   },
  {  4, MWF, 12   },
  {  5, MWF, 1    },
  {  6, MWF, 2    },
  {  7, MWF, 3    },
  {  8, TH,  8    },
  {  9, TH,  930  },
  { 10, TH,  11   },
  { 11, TH,  1230 },
  { 12, TH,  2    },
  { 13, TH,  330  }
};

Room rooms[NUM_ROOMS] = {
  { 0, "BL134",     30,  true  },
  { 1, "BL138",     50,  true  },
  { 2, "KR224",     40,  false },
  { 3, "KR124",     20,  false },
  { 4, "KR206",     30,  true  },
  { 5, "Biddle123", 35,  false },
  { 6, "Biddle125", 40,  false },
  { 7, "Biddle205", 45,  false },
  { 8, "ES100",     100, true }
};

Professors professors[NUM_PROFS] = {
  { 0, "Hagerich"    },
  { 1, "Bilitski"    },
  { 2, "Smigla"      },
  { 3, "Im"          },
  { 4, "Frederick"   },
  { 5, "Thomson"     },
  { 6, "Darling"     },
  { 7, "Hinderliter" },
  { 8, "Ferencek"    }
};

// The definition of a course.  Excludes room and time.  Just a helper to fill in default values.
Course coursesStub[NUM_COURSES] = {
  {  0, "cs015",   0, 20, false, 0, 0 },
  {  1, "cs456",   1, 20, true,  0, 0 },
  {  2, "cs456",   1, 20, true,  0, 0 },
  {  3, "cs456",   1, 20, true,  0, 0 },
  {  4, "cs1783",  1, 15, true,  0, 0 },
  {  5, "cs455",   0, 20, true,  0, 0 },
  {  6, "cs015",   2, 35, true,  0, 0 },
  {  7, "cs015",   3, 35, false, 0, 0 },
  {  8, "cs015",   3, 35, false, 0, 0 },
  {  9, "cs015",   4, 35, false, 0, 0 },
  { 10, "math001", 8, 40, false, 0, 0 },
  { 11, "math001", 8, 50, false, 0, 0 },
  { 12, "math001", 8, 60, false, 0, 0 },
  { 13, "math002", 5, 40, false, 0, 0 },
  { 14, "math002", 5, 50, false, 0, 0 },
  { 15, "math002", 5, 60, false, 0, 0 },
  { 16, "soc100",  6, 45, true,  0, 0 },
  { 17, "soc100",  6, 40, true,  0, 0 },
  { 18, "soc100",  6, 35, true,  0, 0 },
  { 19, "cs047",   1, 15, true,  0, 0 },
  { 20, "cs047",   1, 15, true,  0, 0 },
  { 21, "psy200",  7, 30, false, 0, 0 },
  { 22, "psy200",  7, 35, false, 0, 0 },
  { 23, "psy200",  7, 30, false, 0, 0 },
  { 24, "cs045",   0, 20, true,  0, 0 },
  { 25, "cs045",   0, 20, true,  0, 0 },
  { 26, "cs015",   2, 20, true,  0, 0 }
};


// FUNCTION PROTOTYPES //////////////////////////////////////////////////////////////////

char* getProfessorName( int pid );
char* getRoomName( int rid );
int   getRoomSize( int rid );
void  printCourse( Course* cptr );
void  printPopulation( Schedule* population, int populationSize );
void  initializePopulation( Schedule* population, int populationSize );
void  scorePopulation( Schedule* population, int populationSize );
void  selectAndClone( Schedule* population, int populationSize );
void  crossover( Schedule* population, int populationSize, float probability );
void  mutation( Schedule* population, int populationSize, float probability );
int   averageFitness( Schedule* population, int populationSize );
int   maxFitness( Schedule* population, int populationSize );
int   minFitness( Schedule* population, int populationSize );


// MAIN /////////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] ) {

  int   display        = 0;    // Enable/Disable Terminal Messages
  int   maxGen         = 0;    // Maximum Generations to Run
  int   populationSize = 0;    // Population Size
  float pc             = 0.0;  // Probability of Crossover - Pc
  float pm             = 0.0;  // Probability of Mutation  - Pm

  // Create file (pointer) to save raw data to.
  FILE *log;
  FILE *timing;

  // Parse Input Parameters
  if( argc != 6 ) {
    printf( "Usage: %s <populationSize> <generations> <crossoverProbability> <mutationProbability> <display>\n", argv[0] );
    printf( "  Population Size       - number of members in the whole population.\n" );
    printf( "  Generations           - number of iterations to process the population through the genetic algorithm.\n" );
    printf( "  Crossover Probability - likelihood of population member data crossover.\n" );
    printf( "  Mutation Probability  - likelihood of population member data mutation.\n" );
    printf( "  Display               - whether or not (1/0) to display status and population members to terminal.\n" );
    return 1;
  } else {
    populationSize = atoi( argv[1] );
    maxGen         = atoi( argv[2] );
    pc             = atof( argv[3] );
    pm             = atof( argv[4] );
    display        = atoi( argv[5] );
  }

  // Open File to Log Results
  log    = fopen( "log.txt",   "w+" );
  timing = fopen( "gaOut.txt", "w+" );

  // Initialize Population of Required Size
  Schedule population[populationSize];  // An array of schedules.
  // Don't need a pointer!  We can pass array name as starting address!

  // Initialize Random Population
  initializePopulation( population, populationSize );

  // Initial Score (Test, may remove later.)
  scorePopulation( population, populationSize );

  // Log Score Data to File
  fprintf( log, "%i,%i,%i,%i\n", 0,
          minFitness( population, populationSize ),
          averageFitness( population, populationSize ),
          maxFitness( population, populationSize ) );

  // Display Initial Population
  printPopulation( population, populationSize );

  // Primary Loop - Score and Develop Population - can set to terminate when the
  //   best fitness is achieved, or just set it to run ~10,000 times and use the
  //   schedule with the best fitness in the final gen.
  int mainLoopIterator = 0;

  for( mainLoopIterator = 0; mainLoopIterator < maxGen; mainLoopIterator++ ) {

    // Select Parents and Clone to Next Generation
    selectAndClone( population, populationSize );

    // Perform Crossover
    crossover( population, populationSize, pc );

    // Perform Mutation
    mutation( population, populationSize, pm );

    // Score New Population
    scorePopulation( population, populationSize );

    // Log Score Data to File
    fprintf( log, "%i,%i,%i,%i\n", mainLoopIterator + 1,
            minFitness( population, populationSize ),
            averageFitness( population, populationSize ),
            maxFitness( population, populationSize ) );

    // Print Entire Population (Every 100 Generations)
    if( !( mainLoopIterator % 100) && display ) {
      printf( "Generation %i - Printing Current Population - ", mainLoopIterator );
      printPopulation( population, populationSize );
      printf( "Average Fitness of this Generation is %i\n", averageFitness( population, populationSize ) );
    }

  } // END Primary Loop

  // Print Final Population
  printf( "\n\n = = =  Final Population As Follows  = = =  \n\n" );
  printPopulation( population, populationSize );

  // Select and Display the Best Scoring Member
  int bestIter, bestScore = -999999, bestIndex = -1;
  for( bestIter = 0; bestIter < populationSize; bestIter++ ) {
    if( population[bestIter].score > bestScore ) {
      bestScore = population[bestIter].score;
      bestIndex = bestIter;
    }
  }
  if( display ) {
    printf( "\n\nThe best scoring schedule developed is schedule %i with score %i.\n", bestIndex, population[bestIndex].score );
    printf( "\n\n === SCHEDULE %i ==================================================================================================\n", bestIndex );
    printf ("CRN\tCourseName\tProf\tCrsSize\tNeedsMultimedia \tRmName\tRmSize\tRmHasMM\t    Days\tTime\n");
    int j = 0;
    for( j = 0; j < NUM_COURSES; ++j) {
      printCourse( &population[bestIndex].schedule[j] );
    }
    printf( " === SCORE %5i ==================================================================================================\n\n\n", population[bestIndex].score );
  }

  // Close File
  fclose( log );
  fclose( timing );

  // Conclusion Message
  if( display ) {
    printf( "  This concludes the functionality of this genetic algorithm.\n\n  Have a remarkably pleasant day!  - EWG SDG  \n\n" );
  }

  return 0;
}


// Function Implementation

// getProfessorName - return a pointer to the proper professor name string from
//   the predefined structure based on ID.
char* getProfessorName( int pid ) {
  return professors[pid].professorName;
}

// getRoomName - return a pointer to the proper room name string from the
//   predefined structure based on ID.
char* getRoomName( int rid ) {
  return rooms[rid].roomName;
}

// getRoomName - return the room size from the predefined structure based on ID.
int getRoomSize( int rid ) {
  return rooms[rid].roomSize;
}

// printCourse - display the relevant data from a single course passed in.
void printCourse( Course* cptr ) {
  char *profName = NULL;
  char *rmName   = NULL;
  int   roomSize = 0;
  profName       = getProfessorName( cptr -> professorID );
  rmName         = getRoomName( cptr -> roomID );
  roomSize       = getRoomSize( cptr -> roomID );
  printf("%i\t%.10s\t%10s\t%5i\t%i\t            %10s\t%i\t     %i\t\t  %5s\t   %5i\n",
    cptr -> CRN,
    cptr -> courseName,
    profName,
    cptr -> courseSize,
    cptr -> multimedia,
    rmName,
    roomSize,
    rooms[ cptr -> roomID ].multimedia,
    dayNames[ timePeriods[ cptr -> timePeriodID ].days ],
    timePeriods[ cptr -> timePeriodID ].startTime
  );
}

// printPopulation - display the entire population of schedules and scores.
void printPopulation( Schedule* population, int populationSize ) {
  int i = 0;
  for( i = 0; i < populationSize; i++ ) {
    printf( "\n\n === SCHEDULE %i ==================================================================================================\n", i );
    printf( "CRN\tCourseName\tProf\tCrsSize\tNeedsMultimedia \tRmName\tRmSize\tRmHasMM\t    Days\tTime\n" );
    int j = 0;
    for( j = 0; j < NUM_COURSES; j++ ) {
      printCourse( &population[i].schedule[j] );
    }
    printf( " === SCORE %5i ==================================================================================================\n", population[i].score );
  }
  printf( "\n\n" );
}

// initializePopulation - generate empty population from predefined course
//   listing as well as the random room and time IDs.
void initializePopulation( Schedule* population, int populationSize ) {
  srand( time( NULL ) );
  // Start by just copying the definition data in.
  int i = 0;
  for( i = 0; i < populationSize; i++ ) {
    memcpy( &population[i].schedule, coursesStub, sizeof( coursesStub ) );
	  // Put in a random room and time.
    int j = 0;
    for( j = 0; j < NUM_COURSES; j++ ) {
      population[i].schedule[j].roomID       = rand() % NUM_ROOMS;
      population[i].schedule[j].timePeriodID = rand() % NUM_TIME_PERIODS;
      population[i].score                    = 0;
    }
  }
}

// scorePopulation - loop through the population scoring each schedule per the
//   instructions provided.
void scorePopulation( Schedule* population, int populationSize ) {
  // Loop through all of the schedules, scoring each.
  int i                      = 0;
  for( i = 0; i < populationSize; i++ ) {
    // Loop through each class.
    // All start at zero in initialization, but zero again for certainty.
    population[i].score        = 0;
    int j                      = 0;
    for( j = 0; j < NUM_COURSES; j++ ) {
      // Check simple things, like multimedia and size.
      // For multimedia, 20 if successful, -50 if failed.
      // If the need for multimedia of this course in this population meets its original spec...
      if( ( population[i].schedule[j].multimedia == 1 ) &&
          ( rooms[ population[i].schedule[j].roomID ].multimedia == 1 ) ) {
        // Add 20 to score if it needs and gets multimedia.
        population[i].score   += 20;
      } else if( ( population[i].schedule[j].multimedia == 1 ) &&
                 ( rooms[ population[i].schedule[j].roomID ].multimedia == 0 ) ) {
        // Subtract 50 from score if it needs one but does not get one.
        population[i].score   -= 50;
      }
      // Otherwise, no change to score if it doesn't need it but has it anyway or doesn't get it.
      // For size, 20 if successful, -70 if failed.
      // If the size of the room for this course in this population meets or exceeds its original spec...
      if( population[i].schedule[j].courseSize <= rooms[ population[i].schedule[j].roomID ].roomSize ) {
        // ...pass it by adding 20 to the score...
        population[i].score   += 20;
      } else {
        // ...otherwise fail it and subtract 50.
        population[i].score   -= 70;
      }
      // For the more complex shared teachers and times, need to look through classes BELOW this one (credit to Kevin Eshelman for algorithm).
      int k                    = 0;
      for( k = j+1; k < NUM_COURSES; k++ ) {
        // Look for same teacher at same time.
        if( ( population[i].schedule[k].professorID == population[i].schedule[j].professorID ) && ( population[i].schedule[k].timePeriodID == population[i].schedule[j].timePeriodID ) ) {
          // If detected, subtract 300 points.
          population[i].score -= 300;
	        // And break this innermost loop to prevent over-punishment.
	        k                    = NUM_COURSES;
        }
      }
      // Implemented in two separate loops to prevent over-punishment for each.
      for( k = ( j + 1 ); k < NUM_COURSES; k++ ) {
        // Look for same room at same time.
        if( ( population[i].schedule[k].roomID == population[i].schedule[j].roomID ) &&
            ( population[i].schedule[k].timePeriodID == population[i].schedule[j].timePeriodID ) ) {
          // If detected, subtract 300 points.
          population[i].score -= 300;
	        // And break this innermost loop to prevent over-punishment.
	        k                    = NUM_COURSES;
        }    //
      }      // End Inner Course (K) For
    }        // End Outer Course (J) For
  }          // End Entire Population (I) For
}            // End scorePopulation

// selectAndClone - perform the selection and assembly process for a new generation of
//   schedules.
void selectAndClone( Schedule* population, int populationSize ) {
  // Make a copy of the current generation so that we can make a new one.
  Schedule lastGeneration[populationSize];
  int      i = 0;
  for( i = 0; i < populationSize; i++ ) {
    memcpy( &lastGeneration, population, populationSize * sizeof( *population ) );
  }
  // Run Selection - clone parents: one by one, twos, whatever.  Here chose one
  //   by one, tournament method.
  for( i = 0; i < populationSize; i++ ) {
    // Randomly select two.
    int competitorOne = 0;
    int competitorTwo = 0;
    competitorOne     = rand( ) % populationSize;
    competitorTwo     = rand( ) % populationSize;
    // Not sure if needed, but make sure they're not the same.
    if( competitorOne == competitorTwo ) {
      i--;
    } else {
      // If competitorOne has a bigger score (or they're the same), clone it to the new population.
      if( lastGeneration[competitorOne].score >= lastGeneration[competitorTwo].score ){
        memcpy( &population[i].schedule, &lastGeneration[competitorOne].schedule, sizeof( population[i].schedule ) );
        // Otherwise, clone competitorTwo.
      } else {
        memcpy( &population[i].schedule, &lastGeneration[competitorTwo].schedule, sizeof( population[i].schedule ) );
      }  // End Inner If
    }    // End Outer If
  }      // End Population Loop
}        // End selectAndClone - we finish with a new population of tournament selected clones from the old one.

// crossover - perform the operations associated with crossover of population
//   schedule data, specifically the swapping of room and time IDs between
//   crossed schedules.
void crossover( Schedule* population, int populationSize, float probability ) {
  // Calculate how many will be crossed over.
  float calcCross      = populationSize * probability;
  int   numCross       = ceil( calcCross );
  // Crossover requires two schedules.  Needs even number.  Address this.
  if( numCross % 2 ) {
    numCross--;
  }
  // System required for tracking already crossed entries.
  int alreadyCrossed[numCross];  // Array of previously mutated indices.
  int lastCrossedIndex = 0;      // Last index of that array filled.
  int i                = 0;
  for( i = 0; i < numCross; i++ ) {
    alreadyCrossed[i]  = -1;
  }
  // Crossover loop runs half that many times.
  int crossTimes       = numCross / 2;
  int c                = 0;
  for( c = 0; c < crossTimes; c++ ) {
    // Randomly select which two to cross.
    int crossOne       = rand( ) % populationSize;
    int crossTwo       = rand( ) % populationSize;
    // If these are the same, stop, and subtract from loop index to get another shot.
    if( crossOne == crossTwo ) {
      c--;
    } else {
      // Search alreadyCrossed for crossOne.
      int searchDupIter  = 0;
      bool foundDup      = false;
      for( searchDupIter = 0; searchDupIter < lastCrossedIndex; searchDupIter++ ) {
        if( alreadyCrossed[searchDupIter] == crossOne ) {
          foundDup       = true;
        }
      }
      // If this index is not found, check for crossTwo.
      if( foundDup ) {
        c--;
      } else {
        // Search alreadyCrossed for crossTwo.
        int searchDupIter  = 0;
        bool foundDup      = false;
        for( searchDupIter = 0; searchDupIter < lastCrossedIndex; searchDupIter++ ) {
          if( alreadyCrossed[searchDupIter] == crossTwo ) {
            foundDup       = true;
          }
        }
        // If this index is not found, all checks complete.  Add the two indexes to alreadyCrossed and perform crossover of two full schedules.
        if( foundDup ) {
          c--;
        } else {
          alreadyCrossed[lastCrossedIndex++] = crossOne;
          alreadyCrossed[lastCrossedIndex++] = crossTwo;
          // And perform crossover on those courses.
          // Pick a random spot in the middle of the schedule.
          int divisionSpot     = rand( ) % NUM_COURSES;
          // Pick whether we're shifting above or below.
          int shiftAbove       = rand( ) % 2;
          // Transfer the times and rooms above or below that point.
          if( shiftAbove ) {
            int crossLoopIter  = 0;
            for( crossLoopIter = 0; crossLoopIter < divisionSpot; crossLoopIter++ ) {
              int swapper      = 0;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].timePeriodID;
              population[crossOne].schedule[crossLoopIter].timePeriodID = population[crossTwo].schedule[crossLoopIter].timePeriodID;
              population[crossTwo].schedule[crossLoopIter].timePeriodID = swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].roomID;
              population[crossOne].schedule[crossLoopIter].roomID       = population[crossTwo].schedule[crossLoopIter].roomID;
              population[crossTwo].schedule[crossLoopIter].roomID       = swapper;
            }
          } else {
            int crossLoopIter  = divisionSpot;
            for( crossLoopIter = divisionSpot; crossLoopIter < NUM_COURSES; crossLoopIter++ ) {
              int swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].timePeriodID;
              population[crossOne].schedule[crossLoopIter].timePeriodID = population[crossTwo].schedule[crossLoopIter].timePeriodID;
              population[crossTwo].schedule[crossLoopIter].timePeriodID = swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].roomID;
              population[crossOne].schedule[crossLoopIter].roomID       = population[crossTwo].schedule[crossLoopIter].roomID;
              population[crossTwo].schedule[crossLoopIter].roomID       = swapper;
            }  // End Crossover For
          }    // End Shifting Position If
        }      // End Else for crossTwo Duplicate Check
      }        // End Else for crossOne Duplicate Check
    }          // End Else for Same Schedule Cross
  }            // End crossTimes For
}              // End crossover

// mutation - perform the operations associated with mutation of population
//   schedule data, specifically the generation of new randomized room and
//   time IDs.
void mutation( Schedule* population, int populationSize, float probability ) {
  // Calculate how many will be mutated.
  float calcMutate     = populationSize * probability;
  int   numMutate      = ceil( calcMutate );
  // System required for tracking already mutated entries.
  int alreadyMutated[numMutate];  // Array of previously mutated indices.
  int lastMutatedIndex = 0;       // Last index of that array filled.
  int i                = 0;
  for( i = 0; i < numMutate; i++ ) {
    alreadyMutated[i]  = -1;
  }
  // Mutation loop runs that many times.
  int m                = 0;
  for( m = 0; m < numMutate; m++ ) {
    // Randomly select which one to mutate.
    int toMutate       = 0;
    toMutate           = rand( ) % populationSize;
    // Track which have already been mutated.  Don't mutate the same one twice.
    // Search alreadyMutated for this index.
    int searchDupIter  = 0;
    bool foundDup      = false;
    for( searchDupIter = 0; searchDupIter < lastMutatedIndex; searchDupIter++ ) {
      if( alreadyMutated[searchDupIter] == m ) {
        foundDup       = true;
      }
    }
    // If this index is not found, add it and perform mutation of one of the room and time IDs.
    if( !foundDup ) {
      alreadyMutated[lastMutatedIndex]                         = m;
      lastMutatedIndex++;
      // Randomly select one of the courses.
      int randomCourse                                         = rand( ) % NUM_COURSES;
      // And perform mutation on that one course.
      population[toMutate].schedule[randomCourse].roomID       = rand( ) % NUM_ROOMS;
      population[toMutate].schedule[randomCourse].timePeriodID = rand( ) % NUM_TIME_PERIODS;
    // Otherwise add nothing to mutated array, perform no mutation, and adjust m so we have room to make another selection.
    } else {
      m--;
    }
    // Don't worry about mutating what's already crossed over.  That can happen.
  }
};

// averageFitness - perform a basic integer calculation of average fitness for
//   the generational average score display.
int averageFitness( Schedule* population, int populationSize ) {
  int runningFitness   = 0;
  int fitnessLoopIter  = 0;
  for( fitnessLoopIter = 0; fitnessLoopIter < populationSize; fitnessLoopIter++ ) {
    runningFitness    += population[fitnessLoopIter].score;
  }
  return runningFitness / populationSize;
}

// maxFitness - return the maximum fitness value for this generation's
//   population.
int maxFitness( Schedule* population, int populationSize ) {
  int maxIter  = 0;
  int maxScore = -999999;
  int maxIndex = -1;
  for( maxIter = 0; maxIter < populationSize; maxIter++ ) {
    if( population[maxIter].score > maxScore ) {
      maxScore = population[maxIter].score;
      maxIndex = maxIter;
    }
  }
  return population[maxIndex].score;
}

// minFitness - return the minimum fitness value for this generation's
//   population.
int minFitness( Schedule* population, int populationSize ) {
  int minIter  = 0;
  int minScore = 1000;
  int minIndex = -1;
  for( minIter = 0; minIter < populationSize; minIter++ ) {
    if( population[minIter].score < minScore ) {
      minScore = population[minIter].score;
      minIndex = minIter;
    }
  }
  return population[minIndex].score;
}



// END gaS.c  - EWG SDG
