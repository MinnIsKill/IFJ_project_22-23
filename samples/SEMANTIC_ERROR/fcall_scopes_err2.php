<?php
declare(strict_types=1);
$b = 3;
function foo(int $i) : int
{
    //ERROR[5]
    return $b;
}