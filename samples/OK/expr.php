<?php
declare(strict_types=1);
$a = 1+2*3/4+foo()+(5+(6));
function foo() : int
{
    return 7*(2-8);
}
