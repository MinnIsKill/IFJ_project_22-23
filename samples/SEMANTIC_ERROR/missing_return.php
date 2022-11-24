<?php
declare(strict_types=1);
$a = 1;
function loo() : int
{
    return 7;
}
$a = 2;
function foo(int $a) : int
{
    $a = loo();
    //ERROR[6]
}