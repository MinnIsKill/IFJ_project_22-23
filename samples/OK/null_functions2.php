<?php
declare(strict_types=1);
function foo(?int $a, ?float $b) : ?float
{
    return $b + null;
}

function boo(?string $a) : ?string
{
    return "-";
}

function moo(?int $a) : void
{
    $a = null + null;
    return;
}

function whaat(?string $a ) : void
{
    $a = null . null;
    return;
}