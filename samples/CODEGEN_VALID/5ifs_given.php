<?php
declare(strict_types=1);
/*
 *  N N
 *  I N
 *  F N
 *  B N
 *  S N
 */
if(null !== null)
{
    write("null !== null == true :: ERROR \n");
}
else
{
    write("null !== null == false:: OK \n");
}
if(1 !== null)
{
    write("1 !== null == true:: OK \n");
}
else
{
    write("1 !== null == false:: ERROR \n");
}
if(0 !== null)
{
    write("0 !== null == true :: OK \n");
}
else
{
    write("0 !== null == false:: ERROR \n");
}
if(1.1 !== null)
{
    write("1.1 !== null == true:: OK \n");
}
else
{
    write("1.1 !== null == false:: ERROR \n");
}
if(0.0 !== null)
{
    write("0.0 !== null == true:: OK \n");
}
else
{
    write("0.0 !== null == false:: ERROR \n");
}
if((1!==1) !== null)
{
    write("true !== null == true:: OK \n");
}
else
{
    write("true !== null == false:: ERROR \n");
}
if((0!==3) !== null)
{
    write("false !== null == true:: OK \n");
}
else
{
    write("false !== null == false:: ERROR \n");
}


if("" !== null)
{
    write("\"\" !== null == true:: OK \n");
}
else
{
    write("\"\" !== null == false:: ERROR \n");
}
if("empty" !== null)
{
    write("\"empty\" !== null == true:: OK \n");
}
else
{
    write("\"empty\" !== null == false:: ERROR \n");
}

/*
 *  N I
 *  I I
 *  F I
 *  B I
 *  S I -- X
 */
if(null !== 0)
{
    write("null !== 0 == true:: OK \n");
}
else
{
    write("null !== 0 == false:: ERROR \n");
}
if(null !== 1)
{
    write("null !== 1 == true:: OK \n");
}
else
{
    write("null !== 1 == false:: ERROR \n");
}
if(1 !== 0)
{
    write("1 !== 0 == true:: OK \n");
}
else
{
    write("1 !== 0 == false:: ERROR \n");
}
if(1 !== 1)
{
    write("1 !== 1 == true:: ERROR \n");
}
else
{
    write("1 !== 1 == false:: OK \n");
}

if(0 !== 0)
{
    write("0 !== 0 == true:: ERROR \n");
}
else
{
    write("0 !== 0 == false:: OK \n");
}
if(0 !== 1)
{
    write("0 !== 1 == true:: OK \n");
}
else
{
    write("0 !== 1 == false:: ERROR \n");
}

if(1.1 !== 0)
{
    write("1.1 !== 0 == true:: OK \n");
}
else
{
    write("1.1 !== 0 == false:: ERROR \n");
}
if(1.1 !== 3)
{
    write("1.1 !== 3 == true:: OK \n");
}
else
{
    write("1.1 !== 3 == false:: ERROR \n");
}
if(0.0 !== 1)
{
    write("0.0 !== 1 == true:: OK \n");
}
else
{
    write("0.0 !== 1 == false:: ERROR \n");
}
if(0.0 !== 0)
{
    write("0.0 !== 0 == true:: OK \n");
}
else
{
    write("0.0 !== 0 == false:: ERROR \n");
}
if((1!==1) !== 1)
{
    write("true !== 1 == true:: OK \n");
}
else
{
    write("true !== 1 == false:: ERROR \n");
}
if((1!==1) !== 0)
{
    write("true !== 0 == true:: OK \n");
}
else
{
    write("true !== 0 == false:: ERROR \n");
}
if((0!==3) !== 1)
{
    write("false !== 1 == true:: OK \n");
}
else
{
    write("false !== 1 == false:: ERROR \n");
}
if((0!==3) !== 0)
{
    write("false !== 0 == true:: OK \n");
}
else
{
    write("false !== 0 == false:: ERROR \n");
}
/*
if("" !== 1)
{
    write("\"\" !== 1 :: OK \n");
}
else
{
    write("\"\" !== 1 :: ERROR \n");
}
if("" !== 0)
{
    write("\"\" !== 0 :: OK \n");
}
else
{
    write("\"\" !== 0 :: ERROR \n");
}
if("empty" !== 1)
{
    write("\"empty\" !== 1 :: OK \n");
}
else
{
    write("\"empty\" !== 1 :: ERROR \n");
}
if("empty" !== 0)
{
    write("\"empty\" !== 0 :: OK \n");
}
else
{
    write("\"empty\" !== 0 :: ERROR \n");
}
 */

/*
 *  N F
 *  I F
 *  F F
 *  B F
 *  S F -- X
 */
if(null !== 0.1)
{
    write("null !== 0.1 == true:: OK \n");
}
else
{
    write("null !== 0.0 == false:: ERROR \n");
}
if(null !== 1.1)
{
    write("null !== 1.1 == true:: OK \n");
}
else
{
    write("null !== 1.1 == false:: ERROR \n");
}
if(1 !== 0.1)
{
    write("1 !== 0.1 == true:: OK \n");
}
else
{
    write("1 !== 0.1 == false:: ERROR \n");
}
if(1 !== 1.1)
{
    write("1 !== 1.1 == true:: OK \n");
}
else
{
    write("1 !== 1.1 == false:: ERROR \n");
}

if(0 !== 0.1)
{
    write("0 !== 0.1 == true:: OK \n");
}
else
{
    write("0 !== 0.1 == false:: ERROR \n");
}
if(0 !== 1.1)
{
    write("0 !== 1.1 == true:: OK \n");
}
else
{
    write("0 !== 1.1 == false:: ERROR \n");
}

if(1.1 !== 1.0)
{
    write("1.1 !== 1.0 == true:: OK\n");
}
else
{
    write("1.1 !== 1.0 == false:: ERROR \n");
}
if(1.1 !== 3.1)
{
    write("1.1 !== 3.1 == true:: OK \n");
}
else
{
    write("1.1 !== 3.1 == false:: ERROR \n");
}
if(0.0 !== 1.1)
{
    write("0.0 !== 1.1 == true:: OK \n");
}
else
{
    write("0.0 !== 1.1 == false:: ERROR \n");
}
if(0.0 !== 0.1)
{
    write("0.0 !== 0.1 == true:: OK \n");
}
else
{
    write("0.0 !== 0.1 == false:: ERROR \n");
}
if((1!==1) !== 1.1)
{
    write("true !== 1.1 == true:: OK \n");
}
else
{
    write("true !== 1.1 == false:: ERROR \n");
}
if((1!==1) !== 0.1)
{
    write("true !== 0.1 == true:: OK \n");
}
else
{
    write("true !== 0.1 == false:: ERROR \n");
}
if((0!==3) !== 1.1)
{
    write("false !== 1.1 == true:: OK \n");
}
else
{
    write("false !== 1.1 == false:: ERROR \n");
}
if((0!==3) !== 0.1)
{
    write("false !== 0.1 == true:: OK \n");
}
else
{
    write("false !== 0.1 == false:: ERROR \n");
}

/*
 *  N B
 *  I B
 *  F B
 *  B B
 *  S B
 */
if(null !== (1!==1))
{
    write("null !== true == true:: OK \n");
}
else
{
    write("null !== true == false:: ERROR \n");
}
if(null !== (0!==3))
{
    write("null !== false == true:: OK \n");
}
else
{
    write("null !== false == false:: ERROR \n");
}
if(1 !== (0!==3))
{
    write("1 !== false == true:: OK \n");
}
else
{
    write("1 !== false == false:: ERROR \n");
}
if(1 !== (1!==1))
{
    write("1 !== true == true:: OK \n");
}
else
{
    write("1 !== true == false:: ERROR \n");
}

if(0 !== (0!==3))
{
    write("0 !== false == true:: OK \n");
}
else
{
    write("0 !== false == false:: ERROR \n");
}
if(0 !== (1!==1))
{
    write("0 !== true == true:: OK \n");
}
else
{
    write("0 !== true == false:: ERROR \n");
}

if(1.1 !== (0!==3))
{
    write("1.1 !== false == true:: OK \n");
}
else
{
    write("1.1 !== false == false:: ERROR \n");
}
if(1.1 !== (1!==1))
{
    write("1.1 !== true == true:: OK \n");
}
else
{
    write("1.1 !== true == false:: ERROR \n");
}
if(0.0 !== (1!==1))
{
    write("0.0 !== true == true:: OK \n");
}
else
{
    write("0.0 !== true == false:: ERROR \n");
}
if(0.0 !== (0!==3))
{
    write("0.0 !== false == true:: OK \n");
}
else
{
    write("0.0 !== false == false:: ERROR \n");
}
if((1!==1) !== (1!==1))
{
    write("true !== true == true:: ERROR \n");
}
else
{
    write("true !== true == false:: OK \n");
}
if((1!==1) !== (0!==3))
{
    write("true !== false == true:: OK \n");
}
else
{
    write("true !== false == false:: ERROR \n");
}
if((0!==3) !== (1!==1))
{
    write("false !== true == true:: OK \n");
}
else
{
    write("false !== true == false:: ERROR \n");
}
if((0!==3) !== (0!==3))
{
    write("false !== false == true:: ERROR \n");
}
else
{
    write("false !== false == false:: OK \n");
}
if("" !== (1!==1))
{
    write("\"\" !== true == true:: OK \n");
}
else
{
    write("\"\" !== true == false:: ERROR \n");
}
if("" !== (0!==3))
{
    write("\"\" !== false == true:: OK \n");
}
else
{
    write("\"\" !== false == false:: ERROR \n");
}
if("empty" !== (1!==1))
{
    write("\"empty\" !== true == true:: OK \n");
}
else
{
    write("\"empty\" !== true == false:: ERROR \n");
}
if("empty" !== (0!==3))
{
    write("\"empty\" !== false == true:: OK \n");
}
else
{
    write("\"empty\" !== false == false:: ERROR \n");
}

/*
 *  N S
 *  I S -- X
 *  F S -- X
 *  B S
 *  S S
 */
if(null !== "")
{
    write("null !== \"\" == true:: OK \n");
}
else
{
    write("null !== \"\" == false:: ERROR \n");
}
if(null !== "empty")
{
    write("null !== \"empty\" == true:: OK \n");
}
else
{
    write("null !== \"empty\" == false:: ERROR \n");
}
/*
if(1 !== "")
{
    write("1 !== \"\" == true:: ERROR \n");
}
else
{
    write("1 !== \"\" == false:: OK \n");
}
if(1 !== "empty")
{
    write("1 !== \"empty\" == true:: OK \n");
}
else
{
    write("1 !== \"empty\" == false:: ERROR \n");
}

if(0 !== "")
{
    write("0 !== \"\" == true:: OK \n");
}
else
{
    write("0 !== \"\" == false:: ERROR \n");
}
if(0 !== "empty")
{
    write("0 !== \"empty\" == true:: OK \n");
}
else
{
    write("0 !== \"empty\" == false:: ERROR \n");
}

if(1.1 !== "")
{
    write("1.1 !== \"\" == true:: ERROR \n");
}
else
{
    write("1.1 !== \"\" == false:: OK \n");
}
if(1.1 !== "empty")
{
    write("1.1 !== \"empty\" == true:: ERROR \n");
}
else
{
    write("1.1 !== \"empty\" == false:: OK \n");
}
if(0.0 !== "empty")
{
    write("0.0 !== \"empty\" == true:: OK \n");
}
else
{
    write("0.0 !== \"empty\" == false:: ERROR \n");
}
if(0.0 !== "")
{
    write("0.0 !== \"\" == true:: OK \n");
}
else
{
    write("0.0 !== \"\" == false:: ERROR \n");
}
 */
if((1!==1) !== "empty")
{
    write("true !== \"empty\" == true:: OK \n");
}
else
{
    write("true !== \"empty\" == false:: ERROR \n");
}
if((1!==1) !== "")
{
    write("true !== \"\" == true:: OK \n");
}
else
{
    write("true !== \"\" == false:: ERROR \n");
}
if((0!==3) !== "empty")
{
    write("false !== \"empty\" == true:: OK \n");
}
else
{
    write("false !== \"empty\" == false:: ERROR \n");
}
if((0!==3) !== "")
{
    write("false !== \"\" == true:: OK \n");
}
else
{
    write("false !== \"\" == false:: ERROR \n");
}
if("" !== "empty")
{
    write("\"\" !== \"empty\" == true:: OK \n");
}
else
{
    write("\"\" !== \"empty\" == false:: ERROR \n");
}
if("" !== "")
{
    write("\"\" !== \"\" == true:: ERROR \n");
}
else
{
    write("\"\" !== \"\" == false:: OK \n");
}
if("empty" !== "empty")
{
    write("\"empty\" !== \"empty\" == true:: ERROR \n");
}
else
{
    write("\"empty\" !== \"empty\" == false:: OK \n");
}
if("empty" !== "")
{
    write("\"empty\" !== \"\" == true:: OK \n");
}
else
{
    write("\"empty\" !== \"\" == false:: ERROR \n");
}
