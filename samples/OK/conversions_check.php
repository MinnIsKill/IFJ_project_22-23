<?php
declare(strict_types=1);
function foo() : int
{
    return 1;
}
//strings
$a = "hello" . null; //NULL_TO_STR
$a = null . null; //NULL_TO_STR
//ints
$b = 1 + 3;
$b = null;    //NULL_TO_INT
//floats
$b = 4.3 + 1; //INT_TO_FLOAT
$b = null;    //NULL_TO_FLOAT
$b = null + null;
$b = null / null;
$b = 8;       //INT_TO_FLOAT
$b = $b + 2.2;//INT_TO_FLOAT
//floats gallore
$b = 1 + 1.7 + 8 + 20 + foo();