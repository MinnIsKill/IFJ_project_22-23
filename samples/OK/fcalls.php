<?php
declare(strict_types=1);
function foo() : int
{
    return 1;
}
function moo(int $i) : int
{
    return $i*$i;
}
function bar(int $j) : int
{
    return $j-1;
}
function boo(int $m, int $n) : int
{
    return $m+$n;
}
$a = foo();
$a = moo(1);
$a = moo((1));
$b = bar(1+2+$a);
$d = boo(4,2);
$d = boo(moo(2),3);
$c = boo(1,moo(2));
$c = boo(1,moo(3));
$b = bar(moo(3));
