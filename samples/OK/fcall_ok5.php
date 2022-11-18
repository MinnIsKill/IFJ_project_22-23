<?php
declare(strict_types=1);
$b = 3;
function foo(string $s1) : string
{
    $string = $s1 . " world";
    return $string;
}
$a = foo("hello");