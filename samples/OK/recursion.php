<?php
declare(strict_types=1);

function goo( float $a, int $b ) : int
{
    if($b === 1 )
    {
        return $b;
    }
    $a = 1;
    goo(10.1,2);
}

