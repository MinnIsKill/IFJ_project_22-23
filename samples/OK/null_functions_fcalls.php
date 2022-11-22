<?php
declare(strict_types=1);
function foo(?int $a, ?float $b) : ?float
{
    $a = 1 === 1; //!!! 1 === 1 shouldn't be in assignment
    return $b + null;
}

foo(null,1.1);