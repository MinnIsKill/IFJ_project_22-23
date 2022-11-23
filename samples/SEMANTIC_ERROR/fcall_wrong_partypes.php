<?php
declare(strict_types=1);
function foo(int $i, string $s, float $f, int $r) : float
{
    return $f;
}
//ERROR[4] and ERROR[3], but 4 will be first
foo(1,"hello",9,boo());