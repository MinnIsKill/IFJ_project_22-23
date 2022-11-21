<?php
declare(strict_types=1);
function moo(int $i) : int
{
    return $i*$i;
}
function boo(int $m, int $n) : int
{
    return $m+$n;
}
$d = boo(moo(2),3);
