/* 
 * CS:APP Data Lab 
 * 
 * <Please put your name and userid here>
 * Sam Chen
 * 
 * bits.c - Source file with your solutions to the Lab.
 *          This is the file you will hand in to your instructor.
 *
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */

#if 0
/*
 * Instructions to Students:
 *
 * STEP 1: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

INTEGER CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions.
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
  7. Use any data type other than int.  This implies that you
     cannot use arrays, structs, or unions.

 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. Performs right shifts arithmetically.
  3. Has unpredictable behavior when shifting if the shift amount
     is less than 0 or greater than 31.


EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }
  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }

FLOATING POINT CODING RULES

For the problems that require you to implement floating-point operations,
the coding rules are less strict.  You are allowed to use looping and
conditional control.  You are allowed to use both ints and unsigneds.
You can use arbitrary integer and unsigned constants. You can use any arithmetic,
logical, or comparison operations on int or unsigned data.

You are expressly forbidden to:
  1. Define or use any macros.
  2. Define any additional functions in this file.
  3. Call any functions.
  4. Use any form of casting.
  5. Use any data type other than int or unsigned.  This means that you
     cannot use arrays, structs, or unions.
  6. Use any floating point data types, operations, or constants.


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operations (integer, logical,
     or comparison) that you are allowed to use for your implementation
     of the function.  The max operator count is checked by dlc.
     Note that assignment ('=') is not counted; you may use as many of
     these as you want without penalty.
  3. Use the btest test harness to check your functions for correctness.
  4. Use the BDD checker to formally verify your functions
  5. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.

/*
 * STEP 2: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the BDD checker to formally verify that your solutions produce 
 *      the correct answers.
 */


#endif
//1
/* 
 * bitXor - x^y using only ~ and & 
 *   Example: bitXor(4, 5) = 1
 *   Legal ops: ~ &
 *   Max ops: 14
 *   Rating: 1
 */
int bitXor(int x, int y) {
  /*
   * By De Morgan's law
   * ~(A & B) = (~A | ~B)
   * ~(A | B) = (~A & ~B)
   */
  return ~(~x & ~y) & ~(x & y);
}
/* 
 * tmin - return minimum two's complement integer 
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 4
 *   Rating: 1
 */
int tmin(void) {
  return (1 << 31);
}
//2
/*
 * isTmax - returns 1 if x is the maximum, two's complement number,
 *     and 0 otherwise 
 *   Legal ops: ! ~ & ^ | +
 *   Max ops: 10
 *   Rating: 1
 */
int isTmax(int x) {
  return !(~(x + 1 + x) | (!(x + 1)));
}
/* 
 * allOddBits - return 1 if all odd-numbered bits in word set to 1
 *   where bits are numbered from 0 (least significant) to 31 (most significant)
 *   Examples allOddBits(0xFFFFFFFD) = 0, allOddBits(0xAAAAAAAA) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 2
 */
int allOddBits(int x) {
  int b3 = (x | 0x55);
  int b2 = x | (0x55 << 8);
  int b1 = x | (0x55 << 16);
  int b0 = x | (0x55 << 24);
  return !(~(b0 | b1 | b2 | b3));
}
/* 
 * negate - return -x 
 *   Example: negate(1) = -1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 */
int negate(int x) {
  return (~x + 1);
}
//3
/* 
 * isAsciiDigit - return 1 if 0x30 <= x <= 0x39 (ASCII codes for characters '0' to '9')
 *   Example: isAsciiDigit(0x35) = 1.
 *            isAsciiDigit(0x3a) = 0.
 *            isAsciiDigit(0x05) = 0.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 3
 */
int isAsciiDigit(int x) {
  // High byte shold be 00h 00h 00h 3xh
  int checkHighByte = !(~(((x ^ (~0x30)) >> 4)));
  // Lower four bits
  // 1. 0b0xxx
  // 2. 0bx00x 
  int cond1 = !(x & 0x08);
  int cond2 = !(x & 0x06);
  return checkHighByte & (cond1 | cond2);
}
/* 
 * conditional - same as x ? y : z 
 *   Example: conditional(2,4,5) = 4
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3
 */
int conditional(int x, int y, int z) {
  // if (x != 0) return y
  // if (x == 0) return z
  // Assume a form: A & y | B & z
  // if (x != 0) A = -1;
  // if (x == 0) A = 0; 
  // Oppositely,
  // if (x != 0) B = 0;
  // if (x == 0) B = -1; 
  int B = ((!x) << 31) >> 31;
  int A = ~B;
  return (A & y) | (B & z);
}
/* 
 * isLessOrEqual - if x <= y  then return 1, else return 0 
 *   Example: isLessOrEqual(4,5) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
int isLessOrEqual(int x, int y) {
  int signMask = 1 << 31;
  // if y - x >= 0, x <= y
  int is_YminusX_pos = !((y + (~x + 1)) & signMask);
  // Two exceptions
  // 1. Positive overflow
  // When y > 0 && x < 0, y - x might get a result y - x < 0 
  // which is not true.
  // So the is_YminusX_pos can't be trusted in this case
  // if y > 0 && x < 0, we should return true regardless of isYminusXpos
  int isYposXneg = (!(y & signMask)) & (!((~x) & signMask));
  // 2. Negative overflow
  // When y < 0 && x > 0, y - x might get a result y - x > 0 
  // which is not true.
  // So the is_YminusX_pos can't be trusted in this case
  // if y < 0 && x > 0, we should return false regardless of isYminusXpos.
  int isYnegXpos = (!(~y & signMask)) & (!(x & signMask));

  return (!isYnegXpos) & (is_YminusX_pos | isYposXneg);
}
//4
/* 
 * logicalNeg - implement the ! operator, using all of 
 *              the legal operators except !
 *   Examples: logicalNeg(3) = 0, logicalNeg(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */
int logicalNeg(int x) {
  // positive filter
  // x >= 0 => filter = 0b1111...111
  // x < 0  => filter = 0b0000...000
  int filterPos = ((1 << 31) & (~x)) >> 31;
  int Tmax = ~(1 << 31);
  // 1. Property of negative number
  // if x == 0, x >> 31 is 0
  // if x < 0, x >> 31 is 0xffffffff
  // negative number can easily get true while 0 get false
  // 2. Mapping all positive number (except for 0) to negative number
  // F(x) >= 0, if x == 0
  // F(x) < 0, if x > 0
  // => F(x) = x + Tmax
  // if x == 0, x + Tmax > 0
  // if x > 0, x + Tmax < 0
  int allToNeg = x + (filterPos & Tmax);
  // 3. Separate all non-zero number from zero
  int xToBool = (allToNeg >> 31) & 1;
  // 4. logical not
  int logicalNotX = xToBool ^ 1;
  
  return logicalNotX;
}
/* howManyBits - return the minimum number of bits required to represent x in
 *             two's complement
 *  Examples: howManyBits(12) = 5
 *            howManyBits(298) = 10
 *            howManyBits(-5) = 4
 *            howManyBits(0)  = 1
 *            howManyBits(-1) = 1
 *            howManyBits(0x80000000) = 32
 *  Legal ops: ! ~ & ^ | + << >>
 *  Max ops: 90
 *  Rating: 4
 */
int howManyBits(int x) {
  int b5, b4, b3, b2, b1, b0;
  // if x >= 0, find the highest bit position with 1
  // the minimum number of bits required to represent x in
  // two's complement is (the highest bit position with 1) + 1
  // EX. 101
  //  => (the highest bit position with 1) is 3
  //  => the minimum number of bits required to represent x in
  //     two's complement is 3+1=4
  // Oppositely, if x < 0, find the highest bit position with 0
  // To simplify, turn x into ~x if x < 0
  // if (x >= 0) x
  // if (x < 0) ~x
  // asssume the form x ^ A
  // => if (x >= 0) A = 0
  //    if (x < 0) A = 0b1111...11
  // => A = (x >> 31)
  int A = x >> 31;
  int absX = (x ^ A);
  b5 = (!!(absX >> 16)) << 4; // if more than 16 bits needed
  absX >>= b5; 
  b4 = (!!(absX >> 8)) << 3; // if more than 8 bits needed
  absX >>= b4;
  b3 = (!!(absX >> 4)) << 2; // if more than 4 bits needed
  absX >>= b3;
  b2 = (!!(absX >> 2)) << 1; // if more than 2 bits needed
  absX >>= b2;
  b1 = !!(absX >> 1);
  absX >>= b1;
  b0 = absX;

  return b5 + b4 + b3 + b2 + b1 + b0 + 1;
}
//float
/* 
 * floatScale2 - Return bit-level equivalent of expression 2*f for
 *   floating point argument f.
 *   Both the argument and result are passed as unsigned int's, but
 *   they are to be interpreted as the bit-level representation of
 *   single-precision floating point values.
 *   When argument is NaN, return argument
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
unsigned floatScale2(unsigned uf) {
  // Note. unsigned is a shorthand for unsigned int 
  // float bits layout:
  // | s | exp | mantissa |
  // | 1 |  8  |    23    |
  int sign = uf & (1 << 31);
  int exp = (uf >> 23) & 0xff;
  int mantissa = uf & 0x7fffff;
  // Denormalized
  if (exp == 0xff) { // inf or nan
    return uf;
  }
  if (exp == 0) { // closed to 0
    if (!((1 << 23) & mantissa)) { // MSB of mantissa is 0
      mantissa <<= 1;
    } else {
      exp += 1;
    }
  } else { // Normalized
    exp += 1;
  }
  
  return sign | (exp << 23) | mantissa;
}
/* 
 * floatFloat2Int - Return bit-level equivalent of expression (int) f
 *   for floating point argument f.
 *   Argument is passed as unsigned int, but
 *   it is to be interpreted as the bit-level representation of a
 *   single-precision floating point value.
 *   Anything out of range (including NaN and infinity) should return
 *   0x80000000u.
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. also if, while
 *   Max ops: 30
 *   Rating: 4
 */
int floatFloat2Int(unsigned uf) {
  // Note. unsigned is a shorthand for unsigned int 
  // float bits layout:
  // | s | exp | mantissa |
  // | 1 |  8  |    23    |
  int signMask = (1 << 31);
  int sign = uf & signMask;
  int exp = (uf >> 23) & 0xff;
  int E = exp - 127;
  int mantissa = uf & 0x7fffff;
  int mantissaExtend = (1 << 24);
  mantissa |= mantissaExtend; 

  if (E < 0) {  // close to zero
    return 0;
  } else if (E == 0) {
    mantissa = 1;
  } else if (E >= 31) { // inf ,nan or out of range
    return (1 << 31);
  } else if ((E > 0) && (E <= 23)) {
    mantissa >>= (23 - E);
  } else { // (E > 23) && (E < 31)
    mantissa <<= (E - 23);
  }
  if (!sign) { // positive
    return mantissa;
  } else { // negaive
    return (~mantissa) + 1;
  }
}
/* 
 * floatPower2 - Return bit-level equivalent of the expression 2.0^x
 *   (2.0 raised to the power x) for any 32-bit integer x.
 *
 *   The unsigned value that is returned should have the identical bit
 *   representation as the single-precision floating-point number 2.0^x.
 *   If the result is too small to be represented as a denorm, return
 *   0. If too large, return +INF.
 * 
 *   Legal ops: Any integer/unsigned operations incl. ||, &&. Also if, while 
 *   Max ops: 30 
 *   Rating: 4
 */
unsigned floatPower2(int x) {
  int sign, exp, mantissa;
  sign = (x & (1 << 31));
  exp = x + 127; 
  mantissa = 0;
  if (exp >= 255) {
    exp = 0xff;
  } else if (exp < 0) {
    sign = 0;
    exp = 0;
  }

  return sign | (exp << 23) | mantissa;
}
