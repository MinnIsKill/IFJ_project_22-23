<?php
declare(strict_types=1);

function goo( float $a, int $b ) : int
{
    if($a === 1 )
    {
        //ERROR[6]
        return;
    }
    $a = 1;
    goo(1.1,2);
}

