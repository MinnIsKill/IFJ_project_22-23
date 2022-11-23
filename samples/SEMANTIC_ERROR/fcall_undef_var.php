<?php
declare(strict_types=1);
function foo(int $i, string $s, int $r) : int
{
    //ERROR[5]
    return $f;
}
//ERROR[3] but the above error will be first
boo();
foo(1,"hello",5);
