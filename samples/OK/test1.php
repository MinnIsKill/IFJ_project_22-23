<?php
declare(strict_types=1);
$a = 1;
$b = "weird";

foo($a);

function foo(int $a) : int
{
    $b = 2;
    return 3;
}