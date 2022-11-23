<?php
declare(strict_types=1);
function foo(int $i) : int
{
    //ERROR[6]
    return;
}