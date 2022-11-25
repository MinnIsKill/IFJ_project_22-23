<?php
declare(strict_types=1);

if("123")
{
    write("STRING - OK\n");
}
else
{
    write("STRING - ERROR\n");

}

if("")
{
    write("EMPTY STRING - ERROR\n");
}
else
{
    write("EMPTY STRING - OK\n");

}

if(null)
{
    write("NULL - ERROR\n");
}
else
{
    write("NULL - OK\n");

}

if(1)
{
    write("INT(1) - OK\n");
}
else
{
    write("INT(1)  - ERROR\n");

}

if(0)
{
    write("INT(0) - ERROR\n");
}
else
{
    write("INT(0)  - OK\n");

}


if(0.1)
{
    write("FLOAT(0.1) - OK\n");
}
else
{
    write("FLOAT(0.1)  - ERROR\n");

}

if(0.0)
{
    write("FLOAT(0.0) - ERROR\n");
}
else
{
    write("FLOAT(0.0)  - OK\n");

}
