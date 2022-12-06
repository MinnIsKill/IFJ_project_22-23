<?php
declare(strict_types=1);
/*
 *  N N
 *  I N
 *  F N
 *  B N
 *  S N
 */
if(null > null)
{
    write("null > null == true :: ERROR \n");
}
else
{
    write("null > null == false:: OK \n");
}
if(1 > null)
{
    write("1 > null == true:: ERROR \n");
}
else
{
    write("1 > null == false:: OK \n");
}
if(0 > null)
{
    write("0 > null == true :: ERROR \n");
}
else
{
    write("0 > null == false:: OK \n");
}
if(1.1 > null)
{
    write("1.1 > null == true:: ERROR \n");
}
else
{
    write("1.1 > null == false:: OK \n");
}
if(0.0 > null)
{
    write("0.0 > null == true:: ERROR \n");
}
else
{
    write("0.0 > null == false:: OK \n");
}
if((1===1) > null)
{
    write("true > null == true:: ERROR \n");
}
else
{
    write("true > null == false:: OK \n");
}
if((0===3) > null)
{
    write("false > null == true:: ERROR \n");
}
else
{
    write("false > null == false:: OK \n");
}


if("" > null)
{
    write("\"\" > null == true:: ERROR \n");
}
else
{
    write("\"\" > null == false:: OK \n");
}
if("empty" > null)
{
    write("\"empty\" > null == true:: ERROR \n");
}
else
{
    write("\"empty\" > null == false:: OK \n");
}

/*
 *  N I
 *  I I
 *  F I
 *  B I
 *  S I -- X
 */
if(null > 0)
{
    write("null > 0 == true:: ERROR \n");
}
else
{
    write("null > 0 == false:: OK \n");
}
if(null > 1)
{
    write("null > 1 == true:: ERROR \n");
}
else
{
    write("null > 1 == false:: OK \n");
}
if(1 > 0)
{
    write("1 > 0 == true:: OK \n");
}
else
{
    write("1 > 0 == false:: ERROR \n");
}
if(1 > 1)
{
    write("1 > 1 == true:: ERROR \n");
}
else
{
    write("1 > 1 == false:: OK \n");
}

if(0 > 0)
{
    write("0 > 0 == true:: ERROR \n");
}
else
{
    write("0 > 0 == false:: OK \n");
}
if(0 > 1)
{
    write("0 > 1 == true:: ERROR \n");
}
else
{
    write("0 > 1 == false:: OK \n");
}

if(1.1 > 0)
{
    write("1.1 > 0 == true:: OK \n");
}
else
{
    write("1.1 > 0 == false:: ERROR \n");
}
if(1.1 > 3)
{
    write("1.1 > 3 == true:: ERROR \n");
}
else
{
    write("1.1 > 3 == false:: OK \n");
}
if(0.0 > 1)
{
    write("0.0 > 1 == true:: ERROR \n");
}
else
{
    write("0.0 > 1 == false:: OK \n");
}
if(0.0 > 0)
{
    write("0.0 > 0 == true:: ERROR \n");
}
else
{
    write("0.0 > 0 == false:: OK \n");
}
if((1===1) > 1)
{
    write("true > 1 == true:: ERROR \n");
}
else
{
    write("true > 1 == false:: OK \n");
}
if((1===1) > 0)
{
    write("true > 0 == true:: OK \n");
}
else
{
    write("true > 0 == false:: ERROR \n");
}
if((0===3) > 1)
{
    write("false > 1 == true:: ERROR \n");
}
else
{
    write("false > 1 == false:: OK \n");
}
if((0===3) > 0)
{
    write("false > 0 == true:: ERROR \n");
}
else
{
    write("false > 0 == false:: OK \n");
}
/*
if("" > 1)
{
    write("\"\" > 1 :: ERROR \n");
}
else
{
    write("\"\" > 1 :: OK \n");
}
if("" > 0)
{
    write("\"\" > 0 :: ERROR \n");
}
else
{
    write("\"\" > 0 :: OK \n");
}
if("empty" > 1)
{
    write("\"empty\" > 1 :: ERROR \n");
}
else
{
    write("\"empty\" > 1 :: OK \n");
}
if("empty" > 0)
{
    write("\"empty\" > 0 :: ERROR \n");
}
else
{
    write("\"empty\" > 0 :: OK \n");
}
 */

/*
 *  N F
 *  I F
 *  F F
 *  B F
 *  S F -- X
 */
if(null > 0.1)
{
    write("null > 0.1 == true:: ERROR \n");
}
else
{
    write("null > 0.0 == false:: OK \n");
}
if(null > 1.1)
{
    write("null > 1.1 == true:: ERROR \n");
}
else
{
    write("null > 1.1 == false:: OK \n");
}
if(1 > 0.1)
{
    write("1 > 0.1 == true:: OK \n");
}
else
{
    write("1 > 0.1 == false:: ERROR \n");
}
if(1 > 1.1)
{
    write("1 > 1.1 == true:: ERROR \n");
}
else
{
    write("1 > 1.1 == false:: OK \n");
}

if(0 > 0.1)
{
    write("0 > 0.1 == true:: ERROR \n");
}
else
{
    write("0 > 0.1 == false:: OK \n");
}
if(0 > 1.1)
{
    write("0 > 1.1 == true:: ERROR \n");
}
else
{
    write("0 > 1.1 == false:: OK \n");
}

if(1.1 > 1.0)
{
    write("1.1 > 1.0 == true:: OK\n");
}
else
{
    write("1.1 > 1.0 == false:: ERROR \n");
}
if(1.1 > 3.1)
{
    write("1.1 > 3.1 == true:: ERROR \n");
}
else
{
    write("1.1 > 3.1 == false:: OK \n");
}
if(0.0 > 1.1)
{
    write("0.0 > 1.1 == true:: ERROR \n");
}
else
{
    write("0.0 > 1.1 == false:: OK \n");
}
if(0.0 > 0.1)
{
    write("0.0 > 0.1 == true:: ERROR \n");
}
else
{
    write("0.0 > 0.1 == false:: OK \n");
}
if((1===1) > 1.1)
{
    write("true > 1.1 == true:: ERROR \n");
}
else
{
    write("true > 1.1 == false:: OK \n");
}
if((1===1) > 0.1)
{
    write("true > 0.1 == true:: OK \n");
}
else
{
    write("true > 0.1 == false:: ERROR \n");
}
if((0===3) > 1.1)
{
    write("false > 1.1 == true:: ERROR \n");
}
else
{
    write("false > 1.1 == false:: OK \n");
}
if((0===3) > 0.1)
{
    write("false > 0.1 == true:: ERROR \n");
}
else
{
    write("false > 0.1 == false:: OK \n");
}

/*
 *  N B
 *  I B
 *  F B
 *  B B
 *  S B
 */
if(null > (1===1))
{
    write("null > true == true:: ERROR \n");
}
else
{
    write("null > true == false:: OK \n");
}
if(null > (0===3)
{
    write("null > false == true:: ERROR \n");
}
else
{
    write("null > false == false:: OK \n");
}
if(1 > (0===3))
{
    write("1 > false == true:: OK \n");
}
else
{
    write("1 > false == false:: ERROR \n");
}
if(1 > (1===1))
{
    write("1 > true == true:: ERROR \n");
}
else
{
    write("1 > true == false:: OK \n");
}

if(0 > (0===3))
{
    write("0 > false == true:: ERROR \n");
}
else
{
    write("0 > false == false:: OK \n");
}
if(0 > (1===1))
{
    write("0 > true == true:: ERROR \n");
}
else
{
    write("0 > true == false:: OK \n");
}

if(1.1 > (0===3))
{
    write("1.1 > false == true:: OK \n");
}
else
{
    write("1.1 > false == false:: ERROR \n");
}
if(1.1 > (1===1))
{
    write("1.1 > true == true:: ERROR \n");
}
else
{
    write("1.1 > true == false:: OK \n");
}
if(0.0 > (1===1))
{
    write("0.0 > true == true:: ERROR \n");
}
else
{
    write("0.0 > true == false:: OK \n");
}
if(0.0 > (0===3))
{
    write("0.0 > false == true:: ERROR \n");
}
else
{
    write("0.0 > false == false:: OK \n");
}
if((1===1) > (1===1))
{
    write("true > true == true:: ERROR \n");
}
else
{
    write("true > true == false:: OK \n");
}
if((1===1) > (0===3))
{
    write("true > false == true:: OK \n");
}
else
{
    write("true > false == false:: ERROR \n");
}
if((0===3) > (1===1))
{
    write("false > true == true:: ERROR \n");
}
else
{
    write("false > true == false:: OK \n");
}
if((0===3) > (0===3)
{
    write("false > false == true:: ERROR \n");
}
else
{
    write("false > false == false:: OK \n");
}
if("" > (1===1))
{
    write("\"\" > true :: ERROR \n");
}
else
{
    write("\"\" > true :: OK \n");
}
if("" > (0===3))
{
    write("\"\" > false :: ERROR \n");
}
else
{
    write("\"\" > false :: OK \n");
}
if("empty" > (1===1))
{
    write("\"empty\" > true :: ERROR \n");
}
else
{
    write("\"empty\" > true :: OK \n");
}
if("empty" > (0===3))
{
    write("\"empty\" > false :: ERROR \n");
}
else
{
    write("\"empty\" > false :: OK \n");
}
