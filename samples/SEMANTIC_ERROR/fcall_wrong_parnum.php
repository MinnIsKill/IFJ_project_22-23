<?php
declare(strict_types=1);
function foo(int $i, string $s, float $f, int $r) : float
{
    return $f;
}
//ERROR[4]
foo(1,"hello");