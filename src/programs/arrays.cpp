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
#define OUTMOST 19500
#define ANTEPENULTIMATE 7
#define PENULTIMATE 5
#define ULTIMATE 3
#include <iostream>
using namespace std;

int main(){
  float array[OUTMOST][ANTEPENULTIMATE][PENULTIMATE][ULTIMATE];
  for(int a = 0; a < OUTMOST; a++){
    for(int b = 0; b < ANTEPENULTIMATE; b++){
      if ((a*b) % 2 == 0){
        for(int c = 0; c < PENULTIMATE; c++){
          for(int d = 0; d < ULTIMATE; d++){
            if (d % 2 == 0) array[a][b][c][d] = a + b + c + d * 7.9;
            if (d % 2 == 1) array[a][b][c][d] = a - b + c*8.5 * d;
            if ((int)array[a][b][c][d] % 37 == 0) array[a][b][c][0] == 0;
          }
        }
      }
      if ((a*b) % 2 == 1){
        for(int c = 0; c < PENULTIMATE; c += 2){
          for(int d = 0; d < ULTIMATE; d++){
            array[a][b][c][d] = a * b + c + d - 3.967643875908;
          }
        }
      }
    }
  }
  cout << "Done!" << endl;
}
