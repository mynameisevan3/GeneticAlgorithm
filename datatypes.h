
/*=================================================
              Genetic Algorithm - gaS.c
              Originally Developed For
    CS 1783 - Artificial Intelligence Programming
         Assignment III - Genetic Algorithms
                    Refined For
      ECE 2195 - Parallel Computer Architecture
           Evan William Gretok - 20180404
  =================================================*/


// Size Definitions
typedef int bool;
#define NUM_UNITS        8
#define NUM_DATA_SETS    8
#define NUM_APPS         27
#define NUM_TIME_PERIODS 14

// TimePeriod Definition
typedef struct TimePeriod
{
  int timePeriodID;  // Unique ID for identifying a period.
  int dayNight;      // An identifier that specifies the day/night state of the TimePeriod.
  int startTime;     // Start time of TimePeriod.
} TimePeriod;

// Compute Unit Definition
typedef struct ComputeUnit
{
  int  unitID;           // Unique ID for identifying a compute unit.
  char unitName[30];     // Compute unit name and number.
  int  computeUnitSize;  // May represent resources available to a compute unit, such as memory, throughput, etc.
  bool network;          // Does this compute unit have a sufficient high-bandwidth interconnect?
} ComputeUnit;

// DataSet Definition
typedef struct DataSet
{
  int  datasetID;        // Unique ID for identifying data set or source.
  char datasetName[50];  // Name of data set or source.
} DataSet;

// Course Definition
typedef struct Application
{
  const int  APN;           // Unique ID for identifying an application.
  const char appName[20];   // I.e., SobelFilter.
  const int  datasetID;     // From DataSet structure.
  const int  appSize;       // Max compute resources needed, relates to computeUnitSize in ComputeUnit structure.
  const bool network;       // Is a high-bandwidth interconnect needed?
        int  timePeriodID;  // When is the app to execute?
        int  unitID;        // To which compute unit is this task assigned?
          // These are the parts that will be determined by the GA!
} Application;

typedef struct Schedule
{
  Application schedule[NUM_APPS];  // The desired applications that make up the schedule.
  int    score;                    // The score of this schedule.
} Schedule;



// END datatypes.h  - EWG SDG
