/**
    Purpose: Implements the encryption algorithms
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/

#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include "datatypes.h"


/*****************************************************************************
******************************************************************************
*********** Definitions of functions used in encryption and other  ***********
******************************************************************************
*****************************************************************************/


/*
* @ generates a pseudo-random sequence
*   gives a pointer to the new sequence */
ntstatus generateRandomSequence(dword seed, qword ubound, dword **random_sequence);

/*
* @ generates a random permutation */
ntstatus generatePermutation(qword ubound, const dword *random_sequence, dword **permutation_sequence);

/*
* @ replaces the array with the inverse of the permutation */
ntstatus inversePermutation(qword ubound, dword **permutation_sequence);

/*
* @ apply the permutation 'permutation_sequence' on array 'src_sequence'
*   of size/upper_bound ubound and element size 'szElement', and store the new array in 'dest_sequence' */
ntstatus generatePermuttedSequence(dword *permutation_sequence,qword ubound, void *src_sequence,void **dest_sequence, dword szElement);

/*
* @ swaps values between them */
void swap(dword *v1, dword *v2);

/*
* @ Georg Marsaglia xorshift32 */
dword xorshift32(dword seed);

/*
* @ determines whether system compiled on is little or big endiannes */
dword getEndian();

#endif
