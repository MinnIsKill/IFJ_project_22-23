<?php
declare(strict_types=1);
function foo(?int $a, float $b) : int
{
    return $b + $a;
}

function boo(?string $a ) : void
{
    return null;
}

function whaat(?string $a ) : void
{
    return;
}

function zoo(?string $a ) : ?string
{
    if($a === null)
    {
        return "NULL" ;
    }
    return null;
}

