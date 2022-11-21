<?php
declare(strict_types=1);
$a = foo(1);
function foo(int $i) : int
{
    return $i;
    $a = $b;
}
$b = 2;