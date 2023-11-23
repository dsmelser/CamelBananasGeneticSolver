#include <stdio.h>
#include <stdlib.h> // Include for random number generation
#include <time.h>

// typedef struct {
//   void** data;
//   int count;
//   int size;
// } ManagedArray;

// ManagedArray* createManagedArray(int size) {
//   ManagedArray* array = malloc(sizeof(ManagedArray));
//   array->data = malloc(size);
//   array->count = 0;
//   array->size = size;
//   return array;
// }

// void addToManagedArray(ManagedArray* array, void* pointer) {
//   if (array->count >= array->size) {
//     // Double the size of the array
//     int newSize = array->size * 2;
//     void* newData = malloc(newSize);
//     memcpy(newData, array->data, array->size);

//     // Free the smaller array
//     free(array->data);

//     // Update the array properties
//     array->data = newData;
//     array->size = newSize;
//   }

//   // Add the pointer to the array
//   array->data[array->count] = pointer;
//   array->count++;
// }

// instruction set for controlling camel
//  0 - do nothing
//  1 - move right (opcode #2 units)
//  2 - move left (opcode #2 units)
//  3 - pick up bananas (opcode #2 units)
//  4 - drop bananas (opcode #2 units)

#define DO_NOTHING 0
// #define MOVE_RIGHT 1
// #define MOVE_LEFT 2
#define MOVE_TO 1
#define PICK_UP_BANANAS 3
#define DROP_BANANAS 4

#define MAX_CAMELS 7656
#define MAX_INSTRUCTIONS 30
#define VALS_PER_INSTRUCTION 2

#define START_GROUND_BANANAS 3000
#define MAX_CAMEL_BANANAS 1000
#define MIN_LOCATION 0
#define MAX_LOCATION 1000

#define min(a, b) (((a) < (b)) ? (a) : (b))

int tabLevel = 0;

void PrintTabs() {
  for (int i = 0; i < tabLevel; i++) {
    printf("\t");
  }
}

void RandomizeInstructionsForCamel(
    int program[MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION]) {
  //   tabLevel++;
  //   PrintTabs();
  //   printf("___RandomizeInstructionsForCamel___\n");
  //   fflush(stdout);

  // we're going to be a little smarter here

  // rules
  // if the camel has no bananas, it must pick up bananas (assuming there are
  // any) 								or do
  // nothing

  // if the camel has bananas, it can try one of the following
  // drop some number of bananas
  // move left to an existing pile of bananas
  // move right to an existing pile of bananas
  // move right some amount less than or equal to the number of bananas it has

  int camelLoc = 0;
  int bananas[MAX_LOCATION + 1];
  bananas[0] = START_GROUND_BANANAS;
  for (int loc = 1; loc <= MAX_LOCATION; loc++) {
    bananas[loc] = 0;
  }

  //   printf("Starting with picking up max bananas\n");
  //   fflush(stdout);
  //   sleep(2);

  program[0][0] = PICK_UP_BANANAS;
  program[0][1] =
      MAX_CAMEL_BANANAS; // we'll pick up as many as we can at the start
  int currentBananas = MAX_CAMEL_BANANAS;

  int pickUpAllowed = 0; // just picked up, redundant
  int dropAllowed = 0;   // just picked up, pointless to drop immediately
  int moveAllowed = 1;

  for (int instruction = 1; instruction < MAX_INSTRUCTIONS; instruction++) {
    // printf("\t%d\n", instruction);
    // printf("camelLoc: %d\n", camelLoc);
    // fflush(stdout);

    int availableBananas = bananas[camelLoc];
    // printf("availableBananas: %d\n", availableBananas);

    int maxPickup = min(MAX_CAMEL_BANANAS - currentBananas, availableBananas);
    // printf("maxPickup: %d\n", maxPickup);

    // printf("currentBananas: %d\n", currentBananas);
    // fflush(stdout);

    if (currentBananas <= 0) {
      dropAllowed = 0;
    }

    if (maxPickup <= 0) {
      pickUpAllowed = 0;
    }

    if (currentBananas <= 0) {
      moveAllowed = 0;
    }

    // easy case
    if (pickUpAllowed) {

      //   printf("picking %d bananas\n", maxPickup);
      //   fflush(stdout);
      //   sleep(2);
      // there are bananas here, pick them up
      program[instruction][0] = PICK_UP_BANANAS;
      program[instruction][1] = maxPickup;

      currentBananas += maxPickup;
      bananas[camelLoc] -= maxPickup;

      pickUpAllowed = 0;
      dropAllowed = 0;
      moveAllowed = 1;
    } else if (dropAllowed) {
      // we have bananas we can drop (after all we just moved to our target)
      // drop some bananas
      int drop = rand() % currentBananas + 1;
      program[instruction][0] = DROP_BANANAS;
      program[instruction][1] = drop;

      //   printf("dropping %d bananas\n", drop);
      //   fflush(stdout);
      //   sleep(2);

      currentBananas -= drop;
      bananas[camelLoc] += drop;

      pickUpAllowed = 0;
      dropAllowed = 0;
      moveAllowed = 1;

    } else if (moveAllowed) {
      int min = camelLoc - currentBananas;
      int max = camelLoc + currentBananas;
      if (min < MIN_LOCATION)
        min = MIN_LOCATION;
      if (max > MAX_LOCATION)
        max = MAX_LOCATION;

      int target = rand() % (max - min) + min;
      while (target == camelLoc) {
        target = rand() % (max - min) + min;
      }

      int offset = target - camelLoc;
      program[instruction][0] = MOVE_TO;
      program[instruction][1] = target;

      camelLoc = target;
      currentBananas -= abs(offset);

      moveAllowed = 0;
      // we'll allow drop if we just moved right
      // we'll allow pick up if we just moved left
      if (offset > 0) {
        dropAllowed = 1;
        pickUpAllowed = 0;
      } else {
        dropAllowed = 0;
        pickUpAllowed = 1;
      }

    } else {

      pickUpAllowed = 0;
      dropAllowed = 0;
      moveAllowed = 0;
      program[instruction][0] = DO_NOTHING;
      program[instruction][1] = 0;
    }
  }
}

void RandomizeInstructions(
    int instructions[MAX_CAMELS][MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION]) {
  //   tabLevel++;
  //   PrintTabs();
  //   printf("___RandomizeInstructions___\n");
  //   fflush(stdout);

  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    RandomizeInstructionsForCamel(instructions[camel]);
  }
  //   tabLevel--;
}

int* GenerateStartingLocations() {
  int* camelLoc = malloc(sizeof(int) * MAX_CAMELS);
  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    camelLoc[camel] = 0;
  }
  return camelLoc;
}

int* GenerateStartingBananas() {
  int* camelBananas = malloc(sizeof(int) * MAX_CAMELS);
  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    camelBananas[camel] = 0;
  }
  return camelBananas;
}

int* GenerateStartingGroundBananas() {
  int* groundBananas = malloc(sizeof(int) * MAX_CAMELS * (MAX_LOCATION + 1));
  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    groundBananas[camel * (MAX_LOCATION + 1)] = START_GROUND_BANANAS;
    for (int loc = MIN_LOCATION + 1; loc <= MAX_LOCATION; loc++) {
      groundBananas[camel * (MAX_LOCATION + 1) + loc] = 0;
    }
  }
  return groundBananas;
}

long long* GenerateStartingErrors() {
  long long* camelErrors = malloc(sizeof(long long) * MAX_CAMELS);
  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    camelErrors[camel] = 0;
  }
  return camelErrors;
}

int* GenerateStartingFinalBananas() {
  int* finalBananas = malloc(sizeof(int) * MAX_CAMELS);
  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    finalBananas[camel] = 0;
  }
  return finalBananas;
}

int* SortCamels(int* finalBananas, long long* camelErrors) {
  int* sortedCamels = malloc(sizeof(int) * MAX_CAMELS);

  int camelScores[MAX_CAMELS];
  for (int camel = 0; camel < MAX_CAMELS; camel++) {
    camelScores[camel] = 0;
  }

  for (int i = 0; i < MAX_CAMELS; i++) {
    for (int j = i + 1; j < MAX_CAMELS; j++) {
      if (finalBananas[i] < finalBananas[j]) {
        camelScores[j]++;
      } else if (finalBananas[i] > finalBananas[j]) {
        camelScores[i]++;
      } else {
        if (camelErrors[i] < camelErrors[j]) {
          camelScores[i]++;
        } else if (camelErrors[i] > camelErrors[j]) {
          camelScores[j]++;
        } else {
          camelScores[i]++;
        }
      }
    }
  }

  for (int i = 0; i < MAX_CAMELS; i++) {
    int position = (MAX_CAMELS - 1) - camelScores[i];
    sortedCamels[position] = i;
  }

  return sortedCamels;
}

void RunProgramForCamel(
    int camel,
    int instructions[MAX_CAMELS][MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION],
    int* camelLoc, int* bananasOnGround, int* camelBananas) {

  camelLoc[camel] = 0;
  camelBananas[camel] = 0;
  bananasOnGround[camel * (MAX_LOCATION + 1)] = 3000;
  for (int loc = 1; loc <= MAX_LOCATION; loc++) {
    bananasOnGround[camel * (MAX_LOCATION + 1) + loc] = 0;
  }

  for (int ip = 0; ip < MAX_INSTRUCTIONS; ip++) {
    int instr = instructions[camel][ip][0];
    int opcode = instructions[camel][ip][1];

    switch (instr) {
    case DO_NOTHING:
      break;
    case MOVE_TO:
      int dist = abs(opcode - camelLoc[camel]);
      // make sure we're not trying to move to the current location
      // make sure the camel eats as it goes
      if (camelLoc[camel] != opcode && camelBananas[camel] >= dist) {
        camelLoc[camel] = opcode;
        camelBananas[camel] -= dist;
      } else {
        // printf("!!! Camel %d tried to move to %d from %d but only has %d "
        //        "bananas\n",
        //        camel, opcode, camelLoc[camel], camelBananas[camel]);
        instructions[camel][ip][0] = 0;
        instructions[camel][ip][1] = 0;
      }
      break;
    case PICK_UP_BANANAS:
      // make sure there are at least as many bananas on the
      // ground as we're trying to pick up if not we'll just
      // zero out this instruction
      if (bananasOnGround[camel * (MAX_LOCATION + 1) + camelLoc[camel]] >=
              opcode &&
          camelBananas[camel] + opcode <= MAX_CAMEL_BANANAS) {
        bananasOnGround[camel * (MAX_LOCATION + 1) + camelLoc[camel]] -= opcode;
        camelBananas[camel] += opcode;
      } else {
        instructions[camel][ip][0] = 0;
        instructions[camel][ip][1] = 0;
      }
      break;
    case DROP_BANANAS:
      // make sure there are at least as many bananas in the
      // camel's pack as we're trying to drop

      // if not we'll just zero out this instruction
      if (camelBananas[camel] >= opcode) {
        bananasOnGround[camel * (MAX_LOCATION + 1) + camelLoc[camel]] += opcode;
        camelBananas[camel] -= opcode;
      } else {
        instructions[camel][ip][0] = 0;
        instructions[camel][ip][1] = 0;
      }
      break;
    }
  }

  // drop any remaining bananas
  //   bananasOnGround[camel * (MAX_LOCATION + 1) + camelLoc[camel]] +=
  //       camelBananas[camel];
  //   camelBananas[camel] = 0;
}

void ScoreCamel(int camel, int* finalBananas, int* bananasOnGround,
                long long* camelErrors) {
  // primary sort value (higher is better)
  finalBananas[camel] =
      bananasOnGround[camel * (MAX_LOCATION + 1) + MAX_LOCATION];

  // tie-breaker (lower is better)
  camelErrors[camel] = 3000000;

  for (int loc = MIN_LOCATION; loc <= MAX_LOCATION; loc++) {
    long long bananas = bananasOnGround[camel * (MAX_LOCATION + 1) + loc];
    long long marginalScore = bananas * loc;
    camelErrors[camel] -= marginalScore;
  }
}

int MergeRepeats(int program[MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION]) {
  int repeats = 0;
  for (int ip = 0; ip < MAX_INSTRUCTIONS - 1; ip++) {
    if (program[ip][0] != DO_NOTHING && program[ip][0] == program[ip + 1][0]) {
      program[ip][1] += program[ip + 1][1];
      repeats++;

      // zero out the repeated command so it can be removed later
      program[ip + 1][0] = 0;
      program[ip + 1][1] = 0;
    } else if (program[ip][0] == DROP_BANANAS &&
               program[ip + 1][0] == PICK_UP_BANANAS) {
      int drop = program[ip][1];
      int pick = program[ip + 1][1];

      if (drop > pick) {
        // this is a net drop, lower the drop amount by how much we pick up
        // right aftewards then kill the pick command
        program[ip][1] -= pick;
        program[ip + 1][0] = 0;
        program[ip + 1][1] = 0;
        repeats++;
      } else if (drop < pick) {
        // this is a net pick up, lower the pick amount by how much we drop
        // right aftewards
        program[ip + 1][1] -= drop;
        program[ip][0] = 0;
        program[ip][1] = 0;
        repeats++;
      } else {
        // this is effectively doing nothing. kill both commands
        program[ip][0] = 0;
        program[ip][1] = 0;
        program[ip + 1][0] = 0;
        program[ip + 1][1] = 0;
        repeats++;
      }
    } else if (program[ip][0] == PICK_UP_BANANAS &&
               program[ip + 1][0] == DROP_BANANAS) {
      int pick = program[ip][1];
      int drop = program[ip + 1][1];

      if (drop > pick) {
        // this is a net drop, lower the drop amount by how much we pick up
        // right before then kill the pick command
        program[ip + 1][1] -= pick;
        program[ip][0] = 0;
        program[ip][1] = 0;
        repeats++;
      } else if (drop < pick) {
        // this is a net pick up, lower the pick amount by how much we drop
        // right aftewards
        program[ip][1] -= drop;
        program[ip + 1][0] = 0;
        program[ip + 1][1] = 0;
        repeats++;
      } else {
        // this is effectively doing nothing. kill both commands
        program[ip][0] = 0;
        program[ip][1] = 0;
        program[ip + 1][0] = 0;
        program[ip + 1][1] = 0;
        repeats++;
      }
    }
  }
  return repeats;
}

void RemoveZeros(int program[MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION]) {
  int skip = 0;
  for (int ip = 0; ip + skip < MAX_INSTRUCTIONS; ip++) {

    if (program[ip + skip][0] == 0) {
      skip++;
      ip--;
    } else {
      program[ip][0] = program[ip + skip][0];
      program[ip][1] = program[ip + skip][1];
    }
  }

  for (int i = MAX_INSTRUCTIONS - skip; i < MAX_INSTRUCTIONS; i++) {
    program[i][0] = 0;
    program[i][1] = 0;
  }
}

void CompressProgram(int program[MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION]) {
  int repeats = 0;
  do {
    repeats = MergeRepeats(program);
    RemoveZeros(program);
  } while (repeats > 0);
}

void PrintProgram(int program[MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION]) {
  for (int ip = 0; ip < MAX_INSTRUCTIONS; ip++) {
    int instr = program[ip][0];
    int opcode = program[ip][1];

    switch (instr) {
    case DO_NOTHING:
      break;
    case MOVE_TO:
      printf("\tmove to %d\n", opcode);
      break;
    case PICK_UP_BANANAS:
      printf("\ttake %d\n", opcode);
      break;
    case DROP_BANANAS:
      printf("\tdrop %d\n", opcode);
      break;
    }
  }
}

int main() {
  time_t t;
  srand((unsigned)time(&t));

  int instructions[MAX_CAMELS][MAX_INSTRUCTIONS][VALS_PER_INSTRUCTION];

  RandomizeInstructions(instructions);

  int* camelLoc = GenerateStartingLocations();
  int* camelBananas = GenerateStartingBananas();
  int* bananasOnGround = GenerateStartingGroundBananas(); // needs to be 1001

  long long* camelErrors = GenerateStartingErrors();
  int* finalBananas = GenerateStartingFinalBananas();

  // run the simulation
  for (int camel = 0; camel < MAX_CAMELS; camel++) {

    RunProgramForCamel(camel, instructions, camelLoc, bananasOnGround,
                       camelBananas);

    ScoreCamel(camel, finalBananas, bananasOnGround, camelErrors);
  }

  int mutationRate = 1;
  int genSinceLastBest = 0;
  int lastBest = -1;
  while (1) {
    int* sortedCamels = SortCamels(finalBananas, camelErrors);

    if (sortedCamels[0] != lastBest) {

      lastBest = sortedCamels[0];
      printf("\nNew best camel: %d\tMarket Bananas: %d\tError: %lld\n",
             sortedCamels[0], finalBananas[sortedCamels[0]],
             camelErrors[sortedCamels[0]]);
      PrintProgram(instructions[sortedCamels[0]]);

      genSinceLastBest = 0;
      mutationRate = 1;

    } else {
      genSinceLastBest++;
      if (genSinceLastBest % (MAX_CAMELS / 2) == 0) {
        mutationRate++;
      }
      printf("\rMutation Rate: %d\tGen Since New Best:%d", mutationRate,
             genSinceLastBest);
      fflush(stdout);
    }

    // printf("_________________________________________________\n");
    // for (int cp = 0; cp < 5; cp++) {
    //   int camel = sortedCamels[cp];
    //   printf("Camel: ");
    //   fflush(stdout);
    //   printf("%d\t", camel);
    //   fflush(stdout);
    //   printf("Market Bananas: ");
    //   fflush(stdout);
    //   printf("%d\t", finalBananas[camel]);
    //   fflush(stdout);
    //   printf("Error: %lld\n", camelErrors[camel]);
    //   fflush(stdout);
    // }

    // sleep(2);

    for (int camel = 0; camel < MAX_CAMELS; camel++) {
      CompressProgram(instructions[camel]);
    }

    for (int sortedParentA = 0; sortedParentA < MAX_CAMELS / 2;
         sortedParentA++) {
      int sortedParentB = rand() % (MAX_CAMELS / 2);

      int parentA = sortedCamels[sortedParentA];
      int parentB = sortedCamels[sortedParentB];

      int sortedChildC = sortedParentA + (MAX_CAMELS / 2);
      int childC = sortedCamels[sortedChildC];

      //   printf("!!! Replacing camel[%d] with child of camel[%d] and
      //   camel[%d]\n",
      //          childC, parentA, parentB);

      for (int ip = 0; ip < MAX_INSTRUCTIONS; ip++) {

        int parentAInstruction = instructions[parentA][ip][0];
        int parentAOpcode = instructions[parentA][ip][1];

        int parentBInstruction = instructions[parentB][ip][0];
        int parentBOpcode = instructions[parentB][ip][1];

        if (parentAInstruction == parentBInstruction) {
          // keep the shared instruction
          instructions[childC][ip][0] = parentAInstruction;
          if (parentAOpcode != parentBOpcode) {
            // pick a random opcode between the two
            // with a SLIGHT chance to choose outside the range

            int range = abs(parentAOpcode - parentBOpcode);

            // we'll expand the range by 10% to give a slight chance
            // of choosing outside the range

            int offset = range / 10;

            if (offset < 1) {
              offset = 1;
            }

            int min =
                parentAOpcode < parentBOpcode ? parentAOpcode : parentBOpcode;
            int max =
                parentAOpcode > parentBOpcode ? parentAOpcode : parentBOpcode;

            min -= offset;
            max += offset;

            if (min < 1)
              min = 1;
            if (max > 1000)
              max = 1000;

            int opcode = rand() % (max - min) + min;

            instructions[childC][ip][1] = opcode;
          } else {
            // keep the shared opcode
            instructions[childC][ip][1] = parentAOpcode;
          }
        } else {
          int parent = rand() % 2;
          if (parent == 0) {
            instructions[childC][ip][0] = instructions[parentA][ip][0];
            instructions[childC][ip][1] = instructions[parentA][ip][1];
          } else {
            instructions[childC][ip][0] = instructions[parentB][ip][0];
            instructions[childC][ip][1] = instructions[parentB][ip][1];
          }
        }
      }

      // mutate the child
      for (int m = 0; m < mutationRate; m++) {
        int ip = rand() % MAX_INSTRUCTIONS;
        while (instructions[childC][ip][0] == 0) {
          ip = rand() % MAX_INSTRUCTIONS;
        }

        switch (ip) {
        case MOVE_TO:
          instructions[childC][ip][1] += rand() % 2 == 0 ? 1 : -1;
          if (instructions[childC][ip][1] < 0)
            instructions[childC][ip][1] = 1;
          if (instructions[childC][ip][1] > 1000)
            instructions[childC][ip][1] = 999;
          break;
        case PICK_UP_BANANAS:
          instructions[childC][ip][1] += rand() % 2 == 0 ? 1 : -1;
          if (instructions[childC][ip][1] < 1)
            instructions[childC][ip][1] = 2;
          if (instructions[childC][ip][1] > 1000)
            instructions[childC][ip][1] = 999;
          break;
        case DROP_BANANAS:
          instructions[childC][ip][1] += rand() % 2 == 0 ? 1 : -1;
          if (instructions[childC][ip][1] < 1)
            instructions[childC][ip][1] = 2;
          if (instructions[childC][ip][1] > 1000)
            instructions[childC][ip][1] = 999;
          break;
        }
      }

      RunProgramForCamel(childC, instructions, camelLoc, bananasOnGround,
                         camelBananas);
      CompressProgram(instructions[childC]);

      int parentADiffs = 0;
      int parentBDiffs = 0;

      for (int ip = 0; ip < MAX_INSTRUCTIONS; ip++) {
        if (instructions[childC][ip][0] != instructions[parentA][ip][0] ||
            instructions[childC][ip][1] != instructions[parentA][ip][1]) {
          parentADiffs++;
          if (parentADiffs > 0 && parentBDiffs > 0) {
            break;
          }
        }

        if (instructions[childC][ip][0] != instructions[parentB][ip][0] ||
            instructions[childC][ip][1] != instructions[parentB][ip][1]) {
          parentBDiffs++;
          if (parentADiffs > 0 && parentBDiffs > 0) {
            break;
          }
        }
      }

      int neededClone = 0;
      if (parentADiffs == 0 || parentBDiffs == 0) {
        neededClone = 1;
        // child is a clone of one of the parents
        // not useful
        // replace with a random camel

        // printf("\nChild is a clone of a parent, replacing with totally random
        // "
        //        "camel\n");
        // fflush(stdout);

        // int printFrom = parentA;
        // if (parentBDiffs > 0) {
        //   printFrom = parentB;
        // }
        // printf("Matching Parent:\n");
        // fflush(stdout);
        // PrintProgram(instructions[printFrom]);
        // sleep(5);

        // printf("Child (pre-random):\n");
        // fflush(stdout);
        // PrintProgram(instructions[childC]);
        // sleep(5);

        RandomizeInstructionsForCamel(instructions[childC]);

        // printf("New camel (pre-run):\n");
        // fflush(stdout);
        // PrintProgram(instructions[childC]);
        // sleep(5);

        RunProgramForCamel(childC, instructions, camelLoc, bananasOnGround,
                           camelBananas);

        // printf("New camel (post-run, pre-compression):\n");
        // fflush(stdout);
        // PrintProgram(instructions[childC]);
        // sleep(5);

        CompressProgram(instructions[childC]);

        // printf("New camel (post-compression):\n");
        // fflush(stdout);
        // PrintProgram(instructions[childC]);
        // sleep(5);
      }

      ScoreCamel(childC, finalBananas, bananasOnGround, camelErrors);

      //   if (neededClone) {
      //     printf("\n\t-New camel score-\tMarket Bananas: %d\tError: %lld",
      //            finalBananas[childC], camelErrors[childC]);
      //     fflush(stdout);
      //   }
    }

    free(sortedCamels);
  }
  return 0;
}
