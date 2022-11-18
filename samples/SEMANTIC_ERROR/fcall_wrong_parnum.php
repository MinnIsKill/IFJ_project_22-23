<?php
declare(strict_types=1);
function foo(int $i, string $s, float $f, int $r) : float
{
    return $f;
}
foo(1,"hello");