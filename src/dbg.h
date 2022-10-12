/**
 * @brief Helper dbugging functions
 * @file dbg.h
 * @author Jan Lutonský, xluton02
 **/

#ifdef DEBUG
#   include<stdlib.h>
#   define dbgprint(...) fprintf(stderr,"%s : %s @ %ld : ",__FILE__,__func__,__LINE__);fprintf(stderr,__VA_ARGS__);fputc('\n',stderr);
#else
#   define dbgprint(...)
#endif
