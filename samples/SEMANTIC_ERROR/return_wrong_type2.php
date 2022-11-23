<?php
declare(strict_types=1);
function foo(float $f) : int
{
    //ERROR[4]
    return $f;
}