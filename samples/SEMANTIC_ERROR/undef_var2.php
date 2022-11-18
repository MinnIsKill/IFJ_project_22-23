<?php
declare(strict_types=1);
function foo(int $defined) : int
{
    $defined = $undefined;
    return $defined;
}