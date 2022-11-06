<?php
declare(strict_types=1);


$var_string = null;
$var_string1 =   "Hello \n\" World \\\042";
$var_string2="Hi-- :?._!/()<>@{}+*|";
$var_string3  =  "Xxx" .$var_string1. "x";

if(?string $var_string === null)
{
    echo "True"
    ;
}

function f (?string $string, ?string $string2)
{
    return $string + $string2;
}

f
(null, "Ax\n-.45\n")
;

f("\\n Hello \n, 1", null);

f(null, null);

f("xx",
"x")
;
                    ?>