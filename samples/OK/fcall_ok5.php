<?php
declare(strict_types=1);
$b = 3;
function foo(string $s1) : string
{
    $str = $s1 . " world";
    return $str;
}
$a = foo("hello");