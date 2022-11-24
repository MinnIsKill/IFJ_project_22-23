<?php
declare(strict_types=1);
$a = 1;
function loo() : int
{
    return 7;
}
$a = 2;
//ERROR[3]
function loo(int $a) : float
{
}