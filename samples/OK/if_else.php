<?php
declare(strict_types=1);
function foo(int $i) : int
{
    return $i;
}
$a = 1;
if($a === foo(1))
{
    $a = 1;
}
else
{
    $a = 2;
}
