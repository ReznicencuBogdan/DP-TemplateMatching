/**
    Purpose: Implements the encryption algorithms
    @version 1.0.0 5/12/2018
    @code may not be used without the consent of the author
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "algorithms.h"
#include "message.h"

/*****************************************************************************
******************************************************************************
*********** Implementation of functions used in encryption and other  ********
******************************************************************************
*****************************************************************************/

dword xorshift32(dword seed)
{
  seed = seed ^ seed << 13;
  seed = seed ^ seed >> 17;
  seed = seed ^ seed << 5;

  return seed;
}

void swap(dword *v1, dword *v2)
{
  dword aux = *v1;
  *v1 = *v2;
  *v2 = aux;
}

ntstatus generateRandomSequence(dword seed, qword ubound, dword **random_sequence)
{
  if(random_sequence == NULL) {
    debugMessage("generateRandomSequence: error parent ptr. random_sequence is null.");
    return false;
  }


  (*random_sequence) = (dword*)malloc(ubound * sizeof(dword));

  if(*random_sequence == NULL) {
    debugMessage("generateRandomSequence: error *random_sequence is null.");
    return false;
  }

  dword r = seed;

  for(qword k = 0; k < ubound ; k++) {
    r = xorshift32(r);
    (*random_sequence)[k] = r;
  }

  return true;
}

ntstatus inversePermutation(qword ubound, dword **permutation_sequence)
{
  /* replaces the original sequence with the inverse */
  if(permutation_sequence==NULL) {
    debugMessage("inversePermutation: permutation_sequence is null.");
    return false;
  }

  if(*permutation_sequence == NULL){
    debugMessage("inversePermutation: *permutation_sequence is null.");
    return false;
  }

  dword *inverse_permutation = (dword*)malloc(ubound * sizeof(dword));

  if(inverse_permutation == NULL){
    debugMessage("inversePermutation: inverse_permutation is null.");
    return false;
  }

  for(qword k=0;k<ubound;k++)
    inverse_permutation[(*permutation_sequence)[k]] = k;

  free(*permutation_sequence);

  (*permutation_sequence) = inverse_permutation;

  return true;
}

ntstatus generatePermutation(qword ubound,const dword *random_sequence, dword **permutation_sequence)
{
  if(random_sequence == NULL || permutation_sequence == NULL){
    debugMessage("generatePermutation: random_sequence or permutation_sequence is null.");
    return false;
  }

  // allocate the space needed to hold the permutaion
  (*permutation_sequence) = (dword*)malloc(ubound * sizeof(dword));

  if(*permutation_sequence == NULL){
    debugMessage("generatePermutation: *permutation_sequence is null.");
    return false;
  }

  qword permutIndex;
  dword randomIndex;

  for(permutIndex=0; permutIndex<ubound; permutIndex++)
    (*permutation_sequence)[permutIndex] = permutIndex;

  for(permutIndex=ubound-1;permutIndex>=1;permutIndex--) {
    randomIndex = random_sequence[ubound - permutIndex - 1] % (permutIndex+1);
    swap( &((*permutation_sequence)[randomIndex]) , &((*permutation_sequence)[permutIndex]));
  }

  return true;
}

ntstatus generatePermuttedSequence(dword *permutation_sequence,qword ubound, void *src_sequence,void **dest_sequence, dword szElement)
{
  if(dest_sequence == NULL){
    debugMessage("generatePermuttedSequence: dest_sequence is null.");
    return false;
  }

  (*dest_sequence) = (void*)malloc(ubound * szElement);

  if(*dest_sequence == NULL){
    debugMessage("generatePermuttedSequence: *dest_sequence is null.");
    return false;
  }

  qword offset_src, offset_dst;

  for(qword k=0;k<ubound;k++)  {
    offset_src = k * szElement * sizeof(unsigned char);
    offset_dst = permutation_sequence[k] * szElement * sizeof(unsigned char);

    memcpy( (unsigned char*)(*dest_sequence) + offset_dst, (unsigned char*)src_sequence + offset_src, szElement);
  }

  return true;
}

dword getEndian()
{
  dword i = 1;
  char *c = (char*)&i;

  if (*c) return little_endian;

  return big_endian;    // big endian
}
