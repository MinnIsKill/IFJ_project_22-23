<?php
declare(strict_types=1);
$b = 3;
function foo(int $i, int $j) : int
{
    return $i + $j;
}
$a = foo(1+2,3*3);