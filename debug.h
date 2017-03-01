/* Debug */

#define VERBOSE

#ifdef VERBOSE
#define debug_print(args...){printf(args);}
#else
#define debug_print(args...)
#endif