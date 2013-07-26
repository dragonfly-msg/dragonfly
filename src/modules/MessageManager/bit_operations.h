#ifndef _BIT_OPERATIONS_H_
#define _BIT_OPERATIONS_H_

#define check_flag_bits( A, bitmask)  (((A) & (bitmask)) == (bitmask))
#define clear_flag_bits( A, bitmask)  ((A) &= ~(bitmask))
#define set_flag_bits( A, bitmask)    ((A) |= (bitmask))

#endif //#ifndef _BIT_OPERATIONS_H_
