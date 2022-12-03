<?php
declare(strict_types=1);

$a = "123";
if($a)
{
    write("STRING - OK\n");
}
else
{
    write("STRING - ERROR\n");

}

$a = "";
if($a)
{
    write("EMPTY STRING - ERROR\n");
}
else
{
    write("EMPTY STRING - OK\n");

}

$a = null;
if($a)
{
    write("NULL - ERROR\n");
}
else
{
    write("NULL - OK\n");

}

$a = 1;
if($a)
{
    write("INT(1) - OK\n");
}
else
{
    write("INT(1)  - ERROR\n");

}

$a = 0;
if($a)
{
    write("INT(0) - ERROR\n");
}
else
{
    write("INT(0)  - OK\n");

}

$a = 0.1;
if($a)
{
    write("FLOAT(0.1) - OK\n");
}
else
{
    write("FLOAT(0.1)  - ERROR\n");

}

$a = 0.0;
if($a)
{
    write("FLOAT(0.0) - ERROR\n");
}
else
{
    write("FLOAT(0.0)  - OK\n");

}
