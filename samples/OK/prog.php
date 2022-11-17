<?php
declare(strict_types=1);
$a = foo(1,2);
if(1)
{
   $c = 12;
    while($c === 5)
    {
        $a = bar();
        if($a === 0)
        {
            return(4);
        }
    }
}

return;

function foo(int $g) : string
{
    print($g);
}
