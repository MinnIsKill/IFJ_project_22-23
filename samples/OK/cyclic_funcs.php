<?php
declare(strict_types=1);
function a(int $i): int 
{
    return b(1);
}
function b(int $i) : int
{
    return c($i);
}
function c(int $i) : int
{
    return a($i+1);
}
?>