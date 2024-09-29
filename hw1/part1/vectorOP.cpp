#include "PPintrin.h"

// implementation of absSerial(), but it is vectorized using PP intrinsics
void absVector(float *values, float *output, int N)
{
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float zero = _pp_vset_float(0.f);
  __pp_mask maskAll, maskIsNegative, maskIsNotNegative;

  //  Note: Take a careful look at this loop indexing.  This example
  //  code is not guaranteed to work when (N % VECTOR_WIDTH) != 0.
  //  Why is that the case?
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {

    // All ones
    maskAll = _pp_init_ones();

    // All zeros
    maskIsNegative = _pp_init_ones(0);

    // Load vector of values from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];

    // Set mask according to predicate
    _pp_vlt_float(maskIsNegative, x, zero, maskAll); // if (x < 0) {

    // Execute instruction using mask ("if" clause)
    _pp_vsub_float(result, zero, x, maskIsNegative); //   output[i] = -x;

    // Inverse maskIsNegative to generate "else" mask
    maskIsNotNegative = _pp_mask_not(maskIsNegative); // } else {

    // Execute instruction ("else" clause)
    _pp_vload_float(result, values + i, maskIsNotNegative); //   output[i] = x; }

    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
  }
}

void clampedExpVector(float *values, int *exponents, float *output, int N)
{
  //
  // PP STUDENTS TODO: Implement your vectorized version of
  // clampedExpSerial() here.
  //
  // Your solution should work for any value of
  // N and VECTOR_WIDTH, not just when VECTOR_WIDTH divides N
  //
  __pp_vec_float x;
  __pp_vec_int exp;
  __pp_vec_int zero = _pp_vset_int(0);
  __pp_vec_int one = _pp_vset_int(1);
  __pp_vec_float exp_max = _pp_vset_float(float(EXP_MAX));
  __pp_vec_float result;
  __pp_mask maskAll, maskIsPositive, maskgtExpMax;

  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    if (i + VECTOR_WIDTH > N){
      // number of remaining elements less than VECTOR_WIDTH, reuse (i + VECTOR_WIDTH + 1) - N elements
      i -= (i + VECTOR_WIDTH + 1) - N;
      maskAll = _pp_init_ones(N - i - 1);
      maskAll = _pp_mask_not(maskAll);
    }
    else{
      // All ones
      maskAll = _pp_init_ones();
    }

    maskIsPositive = maskAll;
    
    // Set result
    _pp_vset_float(result, 1.f, maskAll);

    // Load vector of values and exponents from contiguous memory addresses
    _pp_vload_float(x, values + i, maskAll); // x = values[i];
    _pp_vload_int(exp, exponents + i, maskAll);

    // Set mask according to predicate
    _pp_vgt_int(maskIsPositive, exp, zero, maskAll);

    while(_pp_cntbits(maskIsPositive) > 0){
      _pp_vmult_float(result, result, x, maskIsPositive);
      _pp_vsub_int(exp, exp, one, maskIsPositive);
      _pp_vgt_int(maskIsPositive, exp, zero, maskAll);
    }
    
    // Set to 9.999999 if greater than EXP_MAX
    _pp_vgt_float(maskgtExpMax, result, exp_max, maskAll);
    _pp_vset_float(result, 9.999999f, maskAll);
    
    // Write results back to memory
    _pp_vstore_float(output + i, result, maskAll);
    
    //同步處理VECTOR_WIDTH個elements

    //如果剩下的elements不足VECTOR_WIDTH個，i-=(N-VECTOR_WIDTH)並且把倒退的mask設定成false
  }
}

// returns the sum of all elements in values
// You can assume N is a multiple of VECTOR_WIDTH
// You can assume VECTOR_WIDTH is a power of 2
float arraySumVector(float *values, int N)
{

  //
  // PP STUDENTS TODO: Implement your vectorized version of arraySumSerial here
  //
  __pp_vec_float x;
  __pp_vec_float result;
  __pp_vec_float interleave;
  __pp_mask even = _pp_init_ones(VECTOR_WIDTH / 2);
  __pp_mask maskAll;
  for (int i = 0; i < N; i += VECTOR_WIDTH)
  {
    maskAll = _pp_init_ones();

    _pp_vload_float(x, values + i, maskAll);

    _pp_hadd_float(result, x);
    _pp_interleave_float(interleave, result);
  }

  return 0.0;
}