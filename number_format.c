

#include "number_format.h"


int correct_number_format(int messed_up) {
      
      if(messed_up & TWOS_COMPLEMENT_SIGN_MASK) 
       messed_up -= 65536;
       
       return messed_up;

 }  