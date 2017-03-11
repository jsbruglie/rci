/** @file debug.h
 *  @brief Macros for debug and verbose options
 *
 *  Contains the Macro definitions for debug and
 *  verbose options.
 *  `debug_print` call functions are not processed by the 
 *  compiler if `VERBOSE` is not defined.
 *
 *  @author João Borrego
 *  @author Pedro Abreu
 *  @bug No known bugs.
 */

#define VERBOSE

#ifdef VERBOSE
#define debug_print(args...){printf(args);}
#else
#define debug_print(args...)
#endif