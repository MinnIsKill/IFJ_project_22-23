<?php
declare(strict_types=1);
function foo(int $r) : int
{
    return 1;
}
//ERROR[5]
foo($a);
