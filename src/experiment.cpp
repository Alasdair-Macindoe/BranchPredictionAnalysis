/* 
* MIT License
* Copyright (c) 2018 Alasdair Macindoe

* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:

* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.

* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include <fstream>
#include <iostream>
#include <tuple>
#include <map>
#include <assert.h>
#include <math.h>
using namespace std;
/**
 * Created 05-10-17 by Alasdair Macindoe.

 * This is the main experimentor object. It requires a function to be passed to
 * it in its constructor.
 */
class Experiment{
  private:
    /** Stores the number of correctly guessed values */
    unsigned int correct = 0;
    /** Stores the number of incorrectly guessed values */
    unsigned int incorrect = 0;
    /** Stores the file that the input should be read from */
    fstream file;
    /** Stores the function that should be used to detrmine whether a branch should be taken */
    bool (*takeBranch)(unsigned long long branch) = nullptr;
    void (*adapt)(unsigned long long, bool, bool) = nullptr;


  public:
    /**
     * Only constructor for this object.
     * @param filename the input file that should be read
     * @param takeBranch a reference to the function that should be used
     */
    Experiment(string filename, bool (*takeBranch)(unsigned long long branch), void (*adapt)(unsigned long long, bool, bool)){
      file = fstream(filename);
      this->takeBranch = takeBranch;
      this->adapt = adapt;
      if(!file) throw runtime_error("File not loaded correctly");
      if(takeBranch == nullptr) throw logic_error("Invalid function passed");
    };

    /**
     * Returns the number of correctly guessed branches
     * @return the number of correctly guessed branches
     */
    unsigned int getCorrect(){
      return correct;
    }

    /**
     * Returns the number of incorrectly guessed branches
     * @return the number of inthesecorrectly guessed branches
     */
    unsigned int getIncorrect(){
      return incorrect;
    }

    /**
     * Returns the number of branches read so far
     * @return the number of branches read so far
     */
    unsigned int getTotal(){
      return correct + incorrect;
    }

    /**
     * Calculates the percentage of correctly guessed branches
     * @return the percentage of correctly guessed branches
     */
    double getPercentageCorrect(){
      unsigned int total = getTotal();
      if(!total) throw logic_error("Division by 0 attempted. No data read.");
      return (100.0/total)*getCorrect();
    }

    /**
     * Main entry into the file. Calculates, by reading the entire file, a guess for
     * each line in the file and then returns the percentage of guesses that were correct.
     * Note: Files of the wrong format will crash the program.
     * @see getPercentageCorrect
     * @return the percentage of correctly guessed branches
     */
    double readFile(){
      unsigned long long branch_number;
      bool taken;
      bool shouldTake;
      while(file >> branch_number >> taken){
        shouldTake = takeBranch(branch_number);
        if(shouldTake == taken){
          correct++;
          adapt(branch_number, shouldTake, shouldTake);
        }else{
          incorrect++;
          adapt(branch_number, !shouldTake, shouldTake);
        }
      }
      return getPercentageCorrect();
    }
};

/* Does not updation */
// branch number, was it taken, what was our guess
void default_adapt(unsigned long long branch, bool taken, bool guess){
  ;
}

/* Guesses that a branch is always taken */
bool alwaysTake(unsigned long long branch){
  return true;
}

/* Guesses that a branch is never taken */
bool alwaysNotTake(unsigned long long branch){
  return false;
}

/* Uses 2-bit predictor. eg prediction must be wrong twice before it changes */
unsigned short bpb[4096];
#define BIT_MASK 3
#define CORRECT_MASK 1
unsigned short MASK = 4095;

bool twoBitTake(unsigned long long branch){
  return (bpb[branch & MASK] & BIT_MASK > CORRECT_MASK);
}

/* Adapts for two bit after knowing the results */
void twoBitAdapt(unsigned long long branch, bool taken, bool guess){
  assert((branch & MASK) <= MASK); //defensive programming woo.
  bpb[branch & MASK] = bpb[branch & MASK] << 1;
  if(taken) bpb[branch & MASK] = bpb[branch & MASK] | CORRECT_MASK;
}

/* Correlating predictor. */
//M is the history window we use
unsigned short M = 4;
//N is the the n-bit predictor
unsigned short N = 4;
unsigned int threshold = 0;
unsigned int correlatingMask = 0;
unsigned int nBitMask = 0;
unsigned int history = 0;
#define LOOKUP_MASK 1023
#define GSHARE_MASK 255
#define GSHARE_THRESHOLD 128
unsigned short table[LOOKUP_MASK + 1][LOOKUP_MASK + 1];

bool correlatingTake(unsigned long long branch){
  //using the branch number find what part we should look at
  //then use the history to look up the next part
  return((table[branch & LOOKUP_MASK][history & correlatingMask] & nBitMask) >= threshold);
}

void correlatingAdapt(unsigned long long branch, bool taken, bool guess){
  unsigned int lowBits = branch & LOOKUP_MASK;
  unsigned int correctNBits = history & correlatingMask;
  table[lowBits][correctNBits] = table[lowBits][correctNBits] << 1;
  history = history << 1;
  if(taken) {
    table[lowBits][correctNBits] = table[lowBits][correctNBits] | CORRECT_MASK;
    history = history | CORRECT_MASK;
  }
}

/* GShare */
unsigned int _calc_position(unsigned long long branch, unsigned int history){
  //do xoring.
  unsigned long long longHistory = (unsigned long long) history & 1023;
  unsigned int branchLookup = (branch ^ longHistory);
  return branchLookup & MASK;
}

bool gShareTake(unsigned long long branch){
  unsigned int branchLookup = _calc_position(branch, history);
  //we can reuse the history in correlating predictor
  assert(branchLookup <= MASK);
  return((bpb[branchLookup] & GSHARE_MASK) > GSHARE_THRESHOLD);
}

void gShareAdapt(unsigned long long branch, bool taken, bool guess){
  unsigned int branchLookup = _calc_position(branch, history);
  bpb[branchLookup] = bpb[branchLookup] << 1;
  history = history << 1;
  if (taken){
     bpb[branchLookup] |= 1;
     history |= 1;
   }
}

/* Profiler */
#define pMASK 1023
#define pHIST_MASK 15
#define pTHRESHOLD 1
unsigned short pHistory[pMASK + 1][4];
unsigned short pGlobalHistory = 0;

bool profileTake(unsigned long long branch){
  return (pHistory[branch & pMASK][pGlobalHistory & pHIST_MASK] & 3 >= pTHRESHOLD);
}

void profileAdapt(unsigned long long branch, bool taken, bool guess){
  pGlobalHistory << 1;
  pHistory[branch & pMASK][pGlobalHistory & pHIST_MASK]  << 1;
  if(taken){
    pGlobalHistory |= 1;
    pHistory[branch & pMASK][pGlobalHistory & pHIST_MASK] |= 1;
  }
}

void reset(){
  for (int i =0; i < 4096; i++) bpb[i] = 0;
  history = 0;
  for (int i =0; i <= LOOKUP_MASK; i++) for(int j = 0; j < LOOKUP_MASK; j++) table[i][j] = 0;
  for (int i =0; i <= pMASK; i++) for(int j = 0; j < 4; j++) pHistory[i][j] = pTHRESHOLD + 1;
}

int main(int argc, char** argv){
  string file;
  if (argc > 1) {
    file = argv[1];
  }else{
    return 0;
  }

  cout <<"File: " << file << endl;
  //Run all experiments
  cout << "Always Take" << endl;
  Experiment* ex = new Experiment(file, alwaysTake, default_adapt);
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Always Not Take" << endl;
  ex = new Experiment(file, alwaysNotTake, default_adapt);
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Two Bit Predictor: MASK = 512" << endl;
  reset();
  ex = new Experiment(file, twoBitTake, twoBitAdapt);
  MASK = 511;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Two Bit Predictor: MASK = 1024" << endl;
  reset();
  ex = new Experiment(file, twoBitTake, twoBitAdapt);
  MASK = 1023;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Two Bit Predictor: MASK = 2048" << endl;
  reset();
  ex = new Experiment(file, twoBitTake, twoBitAdapt);
  MASK = 2047;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Two Bit Predictor: MASK = 4096" << endl;
  reset();
  ex = new Experiment(file, twoBitTake, twoBitAdapt);
  MASK = 4095;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Correlating Predictor (1, 1)" << endl;
  reset();
  ex = new Experiment(file, correlatingTake, correlatingAdapt);
  M = 1;
  N = 1;
  correlatingMask = pow(2, 1) - 1;
  nBitMask = pow(2, 1) - 1;
  threshold = (pow(2, N) + 1) / 2;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Correlating Predictor (2, 2)" << endl;
  reset();
  ex = new Experiment(file, correlatingTake, correlatingAdapt);
  M = 2;
  N = 2;
  correlatingMask = pow(2, 2) - 1;
  nBitMask = pow(2, 2) - 1;
  threshold = (pow(2, N) + 1) / 2;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Correlating Predictor (4, 2)" << endl;
  reset();
  ex = new Experiment(file, correlatingTake, correlatingAdapt);
  M = 4;
  N = 2;
  correlatingMask = pow(2, 4) - 1;
  nBitMask = pow(2, 2) - 1;
  threshold = (pow(2, N) + 1) / 2;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Correlating Predictor (2, 4)" << endl;
  reset();
  ex = new Experiment(file, correlatingTake, correlatingAdapt);
  M = 2;
  N = 4;
  correlatingMask = pow(2, 2) - 1;
  nBitMask = pow(2, 4) - 1;
  threshold = (pow(2, N) + 1) / 2;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Correlating Predictor (4, 6)" << endl;
  reset();
  ex = new Experiment(file, correlatingTake, correlatingAdapt);
  M = 4;
  N = 6;
  correlatingMask = pow(2, 4) - 1;
  nBitMask = pow(2, 6) - 1;
  threshold = (pow(2, N) + 1) / 2;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "Correlating Predictor (6, 4)" << endl;
  reset();
  ex = new Experiment(file, correlatingTake, correlatingAdapt);
  M = 6;
  N = 4;
  correlatingMask = pow(2, 6) - 1;
  nBitMask = pow(2, 4) - 1;
  threshold = (pow(2, N) + 1) / 2;
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "gshare Predictor" << endl;
  reset();
  ex = new Experiment(file, gShareTake, gShareAdapt);
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;

  cout << "profiled approach" << endl;
  reset();
  ex = new Experiment(file, profileTake, profileAdapt);
  cout << "Correct: " << ex->readFile() << "%" << endl;
  delete ex;
}
