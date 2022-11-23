<?php
declare(strict_types=1);
function foo(int $defined) : int
{
    //ERROR[5]
    $defined = $undefined;
    return $defined;
}