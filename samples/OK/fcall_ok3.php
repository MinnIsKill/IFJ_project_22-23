<?php
declare(strict_types=1);
function foo(int $i) : int
{
    $i = $i + 1;
    return $i;
}
$a = foo(1+2);