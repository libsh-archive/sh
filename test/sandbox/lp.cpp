#include <sh.hpp>
#include "ShLp.hpp"

using namespace SH;
using namespace std;


int main() 
{
  shInit();
  shSetBackend("cc");

  /*
  valarray<double> c(3);  
  c[0] = 3.0f;
  c[1] = 1.0f;
  c[2] = 2.0f;

  valarray<valarray<double> > A(3);
  for(int i = 0; i < 3; ++i) A[i].resize(3);
  A[0][0] = 1.0; A[0][1] = 1.0; A[0][2] = 3.0;
  A[1][0] = 2.0; A[1][1] = 2.0; A[1][2] = 5.0;
  A[2][0] = 4.0; A[2][1] = 1.0; A[2][2] = 2.0;

  valarray<double> b(3);
  b[0] = 30.0; b[1] = 24.0; b[2] = 36.0;

  valarray<double> x(3);
  */


  /*  Example from CLR
  int M = 4; int N = 2;
  valarray<double> A(8);
  A[0] = 1.0; A[1] = 1.0;
  A[2] = 1.0; A[3] = -1.0;
  A[4] = -1.0; A[5] = 1.0;
  A[6] = -1.0; A[7] = -1.0;

  valarray<double> b(1.0f, 4);

  valarray<double> c(2);
  c[0] = 0.0;
  c[1] = 1.0;

  valarray<bool> bounded(false, 2);
   */

  /* Example from Numerical Recipes */
  int M = 5; int N = 4;
  valarray<double> A(0.0f, M * N);
  valarray<double> b(1.0f, M);
  A[0] = 1; A[2] = 2; b[0] = 740;
  A[5] = 2; A[7] = -7; b[1] = 0;
  A[9] = -1; A[10] = 1; A[11] = -2; b[2] = -0.5;
  A[12] = A[13] = A[14] = A[15] = 1; b[3] = 9;
  A[16] = A[17] = A[18] = A[19] = -1; b[4] = -9;

  valarray<double> c(N);
  c[0] = 1; c[1] = 1; c[2] = 3; c[3] = -0.5;

  try {
    //valarray<double> x = shLpSimplexRelaxed(M, N, A, b, c, bounded);
    valarray<double> x = shLpSimplex(M, N, A, b, c);
  } catch (const ShUnboundedLpException &e) {
    std::cout << "Unbounded LP" << std::endl;
  } catch (const ShInfeasibleLpException &e) {
    std::cout << "Infeasible LP" << std::endl;
  }

  return 0;
}

