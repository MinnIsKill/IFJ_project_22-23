<?php
declare(strict_types=1);
function boo() : void
{
    return;
}
function moo(string $s) : string
{
    $s = "Hello";
    $s = $s . " World";
    return $s;
}
function foo(int $a, float $b, string $c) : float
{
    $a = 123;
    $b = $a + 0.123;
    return $b;
}