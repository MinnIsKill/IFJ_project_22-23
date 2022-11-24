<?php
declare(strict_types=1);
function spool(int $a) : int
{
    write($a);
    if($a > 0)
    {
        $b = spool($a - 1);
        if($b > 4 / 2 )
        {
            write("ClOsE");
        }
    }
    else
    {
        write("MIDPOINT");
    }
    write($a);
    return($a);
}

write("spool","START");
spool(5);
write("spool","END");
