<?php
declare(strict_types=1);
$a = 6;

6+3;

foo() + 123;

("why we have to support this feature");

(1+1);

(foo());

$a = foo();

//ERROR[7]
$a = foo() + "yo mama";

function foo() : int
{
    return 1;
}
