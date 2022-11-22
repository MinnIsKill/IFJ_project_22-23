<?php
declare(strict_types=1);
function foo(?int $a, float $b) : float
{
    return $b + $a;
}

function boo(?string $a ) : void
{
    return null;
}

function whaat(?string $a ) : void
{
    $a = null . null;
    return;
}

function zoo(?string $a ) : ?string
{
    $a = null . "1";
    if($a === null)
    {
        return "NULL" ;
    }
    return null;
}

