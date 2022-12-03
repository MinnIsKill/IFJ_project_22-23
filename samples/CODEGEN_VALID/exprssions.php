<?php
declare(strict_types=1);

write( "  |\\\n",
       "  | \\\n",
       " a|  \\\n",
       "  |   \\\n",
       "  -----\n",
       "     b\n"
      );

write("zadejte a : ");
$a = readf();
while($a===null)
{
    write("vstup neni desetine cislo, zkus znovu.\n");
    write("zadejte a : ");
    $a = readf();
}

write("zadejte b : ");
$b = readf();
while($b===null)
{
    write("vstup neni desetine cislo, zkus znovu.\n");
    write("zadejte b : ");
    $b = readf();
}

$obsah = $a * $b / 2;
$prepona2 = $a * $a + $b * $b; 

write("obsah : ",$obsah,"\n");
write("prepona^2 : ",$prepona2,"\n");
