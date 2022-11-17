#include"parser.h"

void parser_print_header(const p_codes p)
{
    if(p != P_SUCCESS)
    {
        fprintf(stderr,"  ++~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~++\n"
                       " //  parser failed with : %-18s//\n"
                       "++~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~++\n",
                       p_codes_str(p)
                );
    }
    else
    {
        fprintf(stderr,"++=====================++\n"
                       "||  parser succesfull  ||\n"
                       "++=====================++\n"
                );
    }
}
