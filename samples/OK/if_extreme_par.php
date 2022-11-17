<?php
declare(strict_types=1);
if( ( foo() ) )
{
    $a = 1;
}

if( ( ( $a ) ) )
{
    $a = 1;
}

if( ( ( $b+(1) ) ) )
{
    $a = 1;
}

if( ( ( ($b) + (1) ) ) )
{
    $a = 1;
}

if( ( ( ($b) + 1 + (2) ) ) )
{
    $a = 1;
}
