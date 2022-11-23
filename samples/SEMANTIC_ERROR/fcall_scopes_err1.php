<?php
declare(strict_types=1);
$b = 3;
function foo(int $i) : int
{
    $i = $i + 1;
    return $i + 2;
}
//ERROR[5]
$b = $i;