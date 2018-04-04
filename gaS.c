
/*=================================================
    CS 1783 - Artificial Intelligence Programming
         Assignment III - Genetic Algorithms
           Evan William Gretok - 20161007
                        ga.c
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
#define true 1
#define false 0

// Global Variable Declaration and Initialization

// Sample Code for Additional Definitions
// Provided by Dr. James Bilitski of the University of Pittsburgh at Johnstown
// Used and Modified with Permission

typedef enum days {
  MWF,
  TH,
  LAST_DAY
} days;

char* dayNames [LAST_DAY] = { "MWF", "TH" };

TimePeriod timePeriods[NUM_TIME_PERIODS] = {
  {0,MWF,8},{1,MWF,9},{2,MWF,10},{3,MWF,11},{4,MWF,12},{5,MWF,1},{6,MWF,2},{7,MWF,3},{8,TH,8},{9,TH,930},{10,TH,11},{11,TH,1230},{12,TH,2},{13,TH,330}
};

Room rooms[NUM_ROOMS] = {
  {0,"BL134",30,true},
  {1,"BL138",50,true},
  {2,"KR224",40,false},
  {3,"KR124",20,false},
  {4,"KR206",30,true},
  {5,"Biddle123",35,false},
  {6,"Biddle125",40,false},
  {7,"Biddle205",45,false},
  {8,"ES100",100,true} };

  Professors professors[NUM_PROFS] = {
  {0,"Hagerich"},
  {1,"Bilitski"},
  {2,"Smigla"},
  {3,"Im"},
  {4,"Frederick"},
  {5,"Thomson"},
  {6,"Darling"},
  {7,"Hinderliter"},
  {8,"Ferencek"}
};

// The definition of a course.  Excludes room and time.  Just a helper to fill in default values.
Course coursesStub[NUM_COURSES] = {
  {0,"cs015",0,20,false},
  {1,"cs456",1,20,true},
  {2,"cs456",1,20,true},
  {3,"cs456",1,20,true},
  {4,"cs1783",1,15,true},
  {5,"cs455",0,20,true},
  {6,"cs015",2,35,true},
  {7,"cs015",3,35,false},
  {8,"cs015",3,35,false},
  {9,"cs015",4,35,false},
  {10,"math001",8,40,false},
  {11,"math001",8,50,false},
  {12,"math001",8,60,false},
  {13,"math002",5,40,false},
  {14,"math002",5,50,false},
  {15,"math002",5,60,false},
  {16,"soc100",6,45,true},
  {17,"soc100",6,40,true},
  {18,"soc100",6,35,true},
  {19,"cs047",1,15,true},
  {20,"cs047",1,15,true},
  {21,"psy200",7,30,false},
  {22,"psy200",7,35,false},
  {23,"psy200",7,30,false},
  {24,"cs045",0,20,true},
  {25,"cs045",0,20,true},
  {26,"cs015",2,20,true}
};

int   n               = 0;    // Population Size Input
int   POPULATION_SIZE = 0;    // Population Size
int   maxgen          = 0;    // Maximum Generations to Run
float pc              = 0.0;  // Probability of Crossover - Pc
float pm              = 0.0;  // Probability of Mutation  - Pm


// FUNCTION PROTOTYPES //////////////////////////////////////////////////////////////////

char* getProfessorName( int pid );
char* getRoomName( int rid );
int   getRoomSize( int rid );
void  printCourse( Course* cptr );
void  printPopulation( Schedule* population );
void  initializePopulation( Schedule* population );
void  scorePopulation( Schedule* population );
void  selectAndClone( Schedule* population );
void  crossover( Schedule* population );
void  mutation( Schedule* population );
int   averageFitness( Schedule* population );
int   maxFitness( Schedule* population );
int   minFitness( Schedule* population );


// MAIN /////////////////////////////////////////////////////////////////////////////////

int main( int argc, char *argv[] ) {

  // Create file (pointer) to save raw data to.
  FILE *log;
  FILE *timing;

  // Parse Input Parameters
  // Size n, generation iterations maxgen, probability of crossover pc, probability of mutation pm.
  // Ensures parameters were indeed passed.
  if( argc == 5 ) {
    // Takes n as that is specifically mentioned in the assignment.
    n               = atoi( argv[1] );
    // ...but puts it into POPULATION_SIZE so all of the predefined functions work.
    POPULATION_SIZE = n;
    maxgen          = atoi( argv[2] );
    pc              = atof( argv[3] );
    pm              = atof( argv[4] );
  } else {
    // Warn if nothing was passed.
    printf( "WARNING - You passed no parameters for generation limit, crossover probability, or mutation probability.\n" );
  }

  // Open File to Log Results
  log = fopen( "log.txt", "w+" );

  // Initialize Population of Required Size
  Schedule population[POPULATION_SIZE];  // An array of schedules.
  // Don't need a pointer!  We can pass array name as starting address!

  // Initialize Random Population
  initializePopulation( population );

  // Initial Score (Test, may remove later.)
  scorePopulation( population );

  // Log Score Data to File
  fprintf( log, "%i,%i,%i,%i\n", 0, minFitness( population ), averageFitness( population ), maxFitness( population ) );

  // Display Initial Population
  printPopulation( population );

  // Primary Loop - Score and Develop Population
  // Can set to terminate when the best fitness is achieved, or just set it to run
  // ~10,000 times and use the schedule with the best fitness in the final gen.
  int mainLoopIterator;

  for( mainLoopIterator = 0; mainLoopIterator < maxgen; mainLoopIterator++ ) {

    // Select Parents and Clone to Next Generation
    selectAndClone( population );

    // Perform Crossover
    crossover( population );

    // Perform Mutation
    mutation( population );

    // Score New Population
    scorePopulation( population );

    // Log Score Data to File
    fprintf( log, "%i,%i,%i,%i\n", mainLoopIterator + 1, minFitness( population ), averageFitness( population ), maxFitness( population ) );

    // Print Entire Population (Every 100 Generations)
    if( !( mainLoopIterator % 100) ) {
      printf( "Generation %i - Printing Current Population - ", mainLoopIterator );
      printPopulation( population );
      printf( "Average Fitness of this Generation is %i\n", averageFitness( population ) );
    }

  } // END Primary Loop

  // Print Final Population
  printf("\n\n = = =  Final Population As Follows  = = =  \n\n");
  printPopulation( population );

  // Select and Display the Best Scoring Member
  int bestIter, bestScore = -999999, bestIndex = -1;
  for( bestIter = 0; bestIter < POPULATION_SIZE; bestIter++ ) {
    if( population[bestIter].score > bestScore ) {
      bestScore = population[bestIter].score;
      bestIndex = bestIter;
    }
  }
  printf( "\n\nThe best scoring schedule developed is Schedule %i with score %i!", bestIndex, population[bestIndex].score );
  printf( "\n\n === SCHEDULE %i ==================================================================================================\n", bestIndex );
  printf ("CRN\tCourseName\tProf\tCrsSize\tNeedsMultimedia \tRmName\tRmSize\tRmHasMM\t    Days\tTime\n");
  int j;
  for ( j = 0; j < NUM_COURSES; ++j) {
    printCourse( &population[bestIndex].schedule[j] );
  }
  printf( " === SCORE %5i ==================================================================================================\n\n\n", population[bestIndex].score );

  // Close File
  fclose( log );

  // Conclusion Message
  printf( "  This concludes the functionality of this genetic algorithm.\n\n  Have a remarkably pleasant day!  - EWG SDG  \n\n" );

  return 0;
}


// Function Implementation

// getProfessorName - return a pointer to the proper professor name string from the
//   predefined structure based on ID.
char* getProfessorName( int pid ) {
  return professors[pid].professorName;
}

// getRoomName - return a pointer to the proper room name string from the predefined
//   structure based on ID. 
char* getRoomName( int rid ) {
  return rooms[rid].roomName;
}

// getRoomName - return the room size from the predefined structure based on ID.
int getRoomSize( int rid ) {
  return rooms[rid].roomSize;
}

// printCourse - display the relevant data from a single course passed in.
void printCourse( Course* cptr ) {
  char * profName;
  char * rmName;
  int roomSize;
  profName = getProfessorName(cptr -> professorID);
  rmName = getRoomName(cptr->roomID);
  roomSize = getRoomSize(cptr ->roomID);
  printf("%i\t%.10s\t%10s\t%5i\t%i\t            %10s\t%i\t     %i\t\t  %5s\t   %5i\n",
    cptr->CRN,
    cptr->courseName,
    profName,
    cptr->courseSize,
    cptr->multimedia,
    rmName,
    roomSize,
    rooms[cptr->roomID].multimedia,
    dayNames[timePeriods[cptr->timePeriodID].days],
    timePeriods[cptr->timePeriodID].startTime
  );
}

// printPopulation - display the entire population of schedules and scores.
void printPopulation( Schedule* population ) {
  int i;
  for ( i = 0; i < POPULATION_SIZE; ++i ) {
    printf( "\n\n === SCHEDULE %i ==================================================================================================\n", i );
    printf ("CRN\tCourseName\tProf\tCrsSize\tNeedsMultimedia \tRmName\tRmSize\tRmHasMM\t    Days\tTime\n");
    int j;
    for ( j = 0; j < NUM_COURSES; ++j) {
      printCourse( &population[i].schedule[j] );
    }
    printf( " === SCORE %5i ==================================================================================================\n", population[i].score );
  }
  printf( "\n\n" );
}

// initializePopulation - generate empty population from predefined course listing as
//   well as the random room and time IDs.
void initializePopulation( Schedule* population ) {
  srand(time(NULL));
  // Start by just copying the definition data in.
  int i;
  for ( i = 0; i < POPULATION_SIZE; ++i ) {
    memcpy( &population[i].schedule, coursesStub, sizeof(coursesStub) );
	  // Put in a random room and time.
    int j;
    for (j=0; j<NUM_COURSES; ++j) {
      population[i].schedule[j].roomID =  rand() % NUM_ROOMS;
      population[i].schedule[j].timePeriodID = rand() % NUM_TIME_PERIODS;
      population[i].score = 0;
    }
  }
}

// scorePopulation - loop through the population scoring each schedule per the
//   instructions provided.
void  scorePopulation( Schedule* population ) {
  // Loop through all of the schedules, scoring each.
  int i;
  for( i = 0; i < POPULATION_SIZE; i++ ) {
    // Loop through each class.
    // All start at zero in initialization, but zero again for certainty.
    population[i].score = 0;
    int j;
    for( j = 0; j < NUM_COURSES; j++ ) {
      // Check simple things, like multimedia and size.
      // For multimedia, 20 if successful, -50 if failed.
      // If the need for multimedia of this course in this population meets its original spec...
      if( ( population[i].schedule[j].multimedia == 1 ) && ( rooms[ population[i].schedule[j].roomID ].multimedia == 1 ) ) {
        // Add 20 to score if it needs and gets multimedia.
        population[i].score += 20;
      } else if( ( population[i].schedule[j].multimedia == 1 ) && ( rooms[ population[i].schedule[j].roomID ].multimedia == 0 ) ) {
        // Subtract 50 from score if it needs one but does not get one.
        population[i].score -= 50;
      }
      // Otherwise, no change to score if it doesn't need it but has it anyway or doesn't get it.
      // For size, 20 if successful, -70 if failed.
      // If the size of the room for this course in this population meets or exceeds its original spec...
      if( population[i].schedule[j].courseSize <= rooms[ population[i].schedule[j].roomID ].roomSize ) {
        // ...pass it by adding 20 to the score...
        population[i].score += 20;
      } else {
        // ...otherwise fail it and subtract 50.
        population[i].score -= 70;
      }
      // For the more complex shared teachers and times, need to look through classes BELOW this one (credit to Kevin Eshelman for algorithm).
      int k;
      for( k = j+1; k < NUM_COURSES; k++ ) {
        // Look for same teacher at same time.
        if( ( population[i].schedule[k].professorID == population[i].schedule[j].professorID ) && ( population[i].schedule[k].timePeriodID == population[i].schedule[j].timePeriodID ) ) {
          // If detected, subtract 300 points.
          population[i].score -= 300;
	  // And break this innermost loop to prevent over-punishment.
	  k = NUM_COURSES;
        }
      }
      // Implemented in two separate loops to prevent over-punishment for each.
      for( k = j+1; k < NUM_COURSES; k++ ) {
        // Look for same room at same time.
        if( ( population[i].schedule[k].roomID == population[i].schedule[j].roomID ) && ( population[i].schedule[k].timePeriodID == population[i].schedule[j].timePeriodID ) ) {
          // If detected, subtract 300 points.
          population[i].score -= 300;
	  // And break this innermost loop to prevent over-punishment.
	  k = NUM_COURSES;
        }
      }
    }
  }
}

// selectAndClone - perform the selection and assembly process for a new generation of
//   schedules.
void  selectAndClone( Schedule* population ) {
  // Make a copy of the current generation so that we can make a new one.
  Schedule lastGeneration[POPULATION_SIZE];
  int i;
  for( i = 0; i < POPULATION_SIZE; ++i ) {
    memcpy( &lastGeneration, population, POPULATION_SIZE*sizeof(*population) );
  }
  // Run selection.
  // Clone parents: one by one, twos, whatever.
  // Here chose one by one, tournament method.
  for( i = 0; i < (POPULATION_SIZE); ++i ) {
    // Randomly select two.
    int competitorOne;
    int competitorTwo;
    competitorOne = rand( ) % POPULATION_SIZE;
    competitorTwo = rand( ) % POPULATION_SIZE;
    // Not sure if needed, but make sure they're not the same.
    if( competitorOne == competitorTwo ) {
      i--;
    } else {
      // If competitorOne has a bigger score (or they're the same), clone it to the new population.
      if( lastGeneration[competitorOne].score >= lastGeneration[competitorTwo].score ){
        memcpy( &population[i].schedule, &lastGeneration[competitorOne].schedule, sizeof(population[i].schedule) );
        // Otherwise, clone competitorTwo.
      } else {
        memcpy( &population[i].schedule, &lastGeneration[competitorTwo].schedule, sizeof(population[i].schedule) );
      }
    }
  }
  // We finish with a new population of tournament selected clones from the old one.
};

// crossover - perform the operations associated with crossover of population schedule
//   data, specifically the swapping of room and time IDs between crossed schedules.
void  crossover( Schedule* population ) {
  // Calculate how many will be crossed over.
  float calcCross = POPULATION_SIZE * pc;
  int   numCross  = ceil( calcCross );
  // Crossover requires two schedules.  Needs even number.  Address this.
  if( numCross % 2 ) {
    numCross--;
  }
  // System required for tracking already crossed entries.
  int alreadyCrossed[numCross];  // Array of previously mutated indices.
  int lastCrossedIndex = 0;      // Last index of that array filled.
  int i;
  for( i = 0; i < numCross; i++ ) {
    alreadyCrossed[i] = -1;
  }
  // Crossover loop runs half that many times.
  int crossTimes = numCross/2;
  int c          = 0;
  for( c = 0; c < crossTimes; c++ ) {
    // Randomly select which two to cross.
    int crossOne = rand( ) % POPULATION_SIZE;
    int crossTwo = rand( ) % POPULATION_SIZE;
    // If these are the same, stop, and subtract from loop index to get another shot.
    if( crossOne == crossTwo ) {
      c--;
    } else {
      // Search alreadyCrossed for crossOne.
      int  searchDupIter = 0;
      bool foundDup      = false;
      for( searchDupIter = 0; searchDupIter < lastCrossedIndex; searchDupIter++ ) {
        if( alreadyCrossed[searchDupIter] == crossOne ) {
          foundDup = true;
        }
      }
      // If this index is not found, check for crossTwo.
      if( foundDup ) {
        c--;
      } else {
        // Search alreadyCrossed for crossTwo.
        int searchDupIter = 0;
        bool foundDup     = false;
        for( searchDupIter = 0; searchDupIter < lastCrossedIndex; searchDupIter++ ) {
          if( alreadyCrossed[searchDupIter] == crossTwo ) {
            foundDup = true;
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
          int divisionSpot = rand( ) % NUM_COURSES;
          // Pick whether we're shifting above or below.
          int shiftAbove   = rand( ) % 2;
          // Transfer the times and rooms above or below that point.
          if( shiftAbove ) {
            int crossLoopIter = 0;
            for( crossLoopIter = 0; crossLoopIter < divisionSpot; crossLoopIter++ ) {
              int swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].timePeriodID;
              population[crossOne].schedule[crossLoopIter].timePeriodID = population[crossTwo].schedule[crossLoopIter].timePeriodID;
              population[crossTwo].schedule[crossLoopIter].timePeriodID = swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].roomID;
              population[crossOne].schedule[crossLoopIter].roomID       = population[crossTwo].schedule[crossLoopIter].roomID;
              population[crossTwo].schedule[crossLoopIter].roomID       = swapper;
            }
          } else {
            int crossLoopIter = divisionSpot;
            for( crossLoopIter = divisionSpot; crossLoopIter < NUM_COURSES; crossLoopIter++ ) {
              int swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].timePeriodID;
              population[crossOne].schedule[crossLoopIter].timePeriodID = population[crossTwo].schedule[crossLoopIter].timePeriodID;
              population[crossTwo].schedule[crossLoopIter].timePeriodID = swapper;
              swapper                                                   = population[crossOne].schedule[crossLoopIter].roomID;
              population[crossOne].schedule[crossLoopIter].roomID       = population[crossTwo].schedule[crossLoopIter].roomID;
              population[crossTwo].schedule[crossLoopIter].roomID       = swapper;
            }
          }
        }  // END else for crossTwo duplicate check.
      }  // END else for crossOne duplicate check.
    }  // END else for same schedule cross.
  }
};

// mutation - perform the operations associated with mutation of population schedule
//   data, specifically the generation of new randomized room and time IDs.
void  mutation( Schedule* population ) {
  // Calculate how many will be mutated.
  float calcMutate     = POPULATION_SIZE * pm;
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
    int toMutate;
    toMutate           = rand( ) % POPULATION_SIZE;
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
int averageFitness( Schedule* population ) {
  int runningFitness   = 0;
  int fitnessLoopIter  = 0;
  for( fitnessLoopIter = 0; fitnessLoopIter < POPULATION_SIZE; fitnessLoopIter++ ) {
    runningFitness    += population[fitnessLoopIter].score;
  }
  return runningFitness / POPULATION_SIZE;
}

// maxFitness - return the maximum fitness value for this generation's population.
int maxFitness( Schedule* population ) {
  int maxIter  = 0;
  int maxScore = -999999;
  int maxIndex = -1;
  for( maxIter = 0; maxIter < POPULATION_SIZE; maxIter++ ) {
    if( population[maxIter].score > maxScore ) {
      maxScore = population[maxIter].score;
      maxIndex = maxIter;
    }
  }
  return population[maxIndex].score;
}

// minFitness - return the minimum fitness value for this generation's population.
int minFitness( Schedule* population ) {
  int minIter  = 0;
  int minScore = 1000;
  int minIndex = -1;
  for( minIter = 0; minIter < POPULATION_SIZE; minIter++ ) {
    if( population[minIter].score < minScore ) {
      minScore = population[minIter].score;
      minIndex = minIter;
    }
  }
  return population[minIndex].score;
}



// END gaS.c  - EWG SDG
