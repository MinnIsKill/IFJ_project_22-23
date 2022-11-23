<?php
declare(strict_types=1);
function foo() : void
{
}
if(foo())
{
    //ERROR[5]
    return($a);
}
?>
