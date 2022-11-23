<?php
declare(strict_types=1);
function foo(string $s1) : string
{
    //ERROR[4]
    return 1;
}