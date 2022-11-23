<?php
declare(strict_types=1);
function pizza(int $a, int $b, ?string $s, int $sum, float $d, string $s2) : ?string
{
    if ($s === "blegh"){
        return null;
    } else {
        if ($sum > 1000){
            return "yummy";
        }
    }
    return null;
}

function add(int $a, int $b, int $c) : int
{
    return $a + $b + $c;
}
$mozzarella = 1;
$pineapple = "blegh";
pizza(1+$mozzarella,(420),$pineapple,add(9000,$mozzarella,621),(9.7),"dddd");
