
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
#define NUM_ROOMS 9
#define NUM_PROFS 9
#define NUM_COURSES 27
#define NUM_TIME_PERIODS 14

// TimePeriod Definition
typedef struct TimePeriod
{
  int timePeriodID;  // Unique ID for identifying a period.
  int days;          // An identifier that specifies the days.
  int startTime;     // Start time of course.
} TimePeriod;

// Room Definition
typedef struct Room
{
  int  roomID;        // Unique ID for identifying a room.
  char roomName[30];  // Room name and number.
  int  roomSize;      // Number of students that can fit.
  bool multimedia;    // Multimedia available in room.
} Room;

// Professors Definition
typedef struct Professors
{
  int  professorID;        // Unique ID for identifying professors.
  char professorName[50];  // Name of professor.
} Professors;

// Course Definition
typedef struct Course
{
  const int  CRN;             // Unique ID for identifying a course offering.
  const char courseName[20];  // I.e., CS1783.
  const int  professorID;     // From Professors structure.
  const int  courseSize;      // Max number of people enrolled.
  const bool multimedia;      // Multimedia needed?
        int  timePeriodID;    // When is the class?
        int  roomID;          // Where is the class?
          // These are the parts that will be determined by the GA!
} Course;

typedef struct Schedule
{
  Course schedule[NUM_COURSES];  // The classes that make up the schedule.
  int    score;                  // The score of this schedule.
} Schedule;



// END datatypes.h  - EWG SDG
