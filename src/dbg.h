/**
 * @brief Helper dbugging functions
 * @file dbg.h
 * @author Jan Lutonský, xluton02
 **/

#ifdef DEBUG
#   include<stdlib.h>
#   define dbgprint(...) fprintf(stderr,"%s : %s @ %d : ",__FILE__,__func__,__LINE__);fprintf(stderr,__VA_ARGS__);fputc('\n',stderr);
#else
#   define dbgprint(...)
#endif

#ifdef INFO
#   include<stdlib.h>
#   define infoprint(...) fprintf(stderr,"%s : %s @ %d : ",__FILE__,__func__,__LINE__);fprintf(stderr,__VA_ARGS__);fputc('\n',stderr);
#   define INFORUN(...) do{__VA_ARGS__}while(0)
#else
#   define infoprint(...)
#   define INFORUN(...) 
#endif
