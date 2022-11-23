<?php
declare(strict_types=1);
$a = 1;
return;
return;
$a = 11;
//should be ERROR[4], but since there's a return before this, the program won't register any errors after return inside main
foo();
function foo(int $a, float $b, string $c) : void
{
    $a = 123;
}
