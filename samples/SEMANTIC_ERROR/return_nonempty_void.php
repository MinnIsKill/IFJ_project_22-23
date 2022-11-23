<?php
declare(strict_types=1);
function foo(int $i) : void
{
    //ERROR[6]
    return $i;
}