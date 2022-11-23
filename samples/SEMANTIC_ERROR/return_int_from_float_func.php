<?php
declare(strict_types=1);
function foo(int $i) : float
{
    //ERROR[4]
    return $i;
}