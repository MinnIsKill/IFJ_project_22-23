<?php
declare(strict_types=1);
function foo(int $i, string $s, int $r) : int
{
    return 1;
}
//ERROR[5]
foo(1,$a,"hello");
