#ifndef P_ASSERT_H_
#define P_ASSERT_H_

#include <assert.h>
#include <iostream>


#define ASSERT(left,operator,right) { if(!((left) operator (right))){ std::cerr << "ASSERT FAILED: " << #left << #operator << #right << " @ " << __FILE__ << " (" << __LINE__ << "). " << #left << "=" << (left) << "; " << #right << "=" << (right) << std::endl; assert((left) operator (right));}}

#ifdef DEBUG 
#define debug_print(fmt, ...) \
							printf("[%s:%d] ", __FILE__,__LINE__);\
              do { fprintf(stdout, fmt, __VA_ARGS__); } while (0)
#else
#define debug_print(fmt, ...) ;
#endif









#endif
