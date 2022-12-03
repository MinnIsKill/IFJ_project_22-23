<?php
declare(strict_types=1);
$a = 1;
$b = "1";
//ERROR[7]
while($a !== $b)
{
    return(1);
}
?>
