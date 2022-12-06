<?php
declare(strict_types=1);
/*
 * N N
 * N I
 * N F
 * N B
 * N S
 */
write("\n========= NULL ****> X\n");
write("null * null = " , null*null    ," :: 0","\n");
write("null * 0 = "    , null*0       ," :: 0","\n");
write("null * 123 = "  , null*123     ," :: 0","\n");
write("null * 0.0 = "  , null*0.0     ," :: 0","\n");
write("null * 0.25 = " , null*0.25    ," :: 0","\n");
write("null * true = " , null*(1===1) ," :: 0","\n");
write("null * false = ", null*(1===2) ," :: 0","\n");

/*
 * I N
 * I I
 * I F
 * I B
 * I S
 */
write("\n========== INT ****> X\n");
write("0 * null = " , 0*null  ," :: 0","\n");
write("1 * null = " , 1*null  ," :: 0","\n");
write("0 * 0 = "    , 0*0     ," :: 0","\n");
write("1 * 123 = "  , 1*123   ," :: 123","\n");
write("0 * 0.0 = "  , 0*0.0   ," :: 0","\n");
write("1 * 0.25 = "  , 1*0.25 ," :: 0.25","\n");
write("0 * true = "  , 0*(1===1) ," :: 0","\n");
write("1 * true = "  , 1*(1===1) ," :: 1","\n");
write("0 * false = " , 0*(1===2) ," :: 0","\n");
write("1 * false = " , 1*(1===2) ," :: 0","\n");

/*
 * F N
 * F I
 * F F
 * F B
 * F S
 */
write("\n========== FLOAT ****> X\n");
write("0.0 * null = " , 0.0*null  ," :: 0","\n");
write("1.0 * null = " , 1.0*null  ," :: 0","\n");
write("0.0 * 0 = "    , 0.0*0     ," :: 0","\n");
write("1.0 * 123 = "  , 1.0*123   ," :: 123","\n");
write("0.0 * 0.0 = "  , 0.0*0.0   ," :: 0","\n");
write("1.0 * 0.25 = " , 1.0*0.25 ," :: 0.25","\n");
write("0.0 * true = " , 0.0*(1===1) ," :: 0","\n");
write("1.0 * true = " , 1.0*(1===1) ," :: 1","\n");
write("0.0 * false = ", 0.0*(1===2) ," :: 0","\n");
write("1.0 * false = ", 1.0*(1===2) ," :: 0","\n");

/*
 * B N
 * B I
 * B F
 * B B
 * B S
 */
write("\n========== BOOL ****> X\n");
write("true * null = " , (1===1)*null  ," :: 0","\n");
write("false * null = " , (1===2)*null  ," :: 0","\n");
write("true * 0 = "    , (1===1)*0     ," :: 0","\n");
write("false * 0 = "    , (1===2)*0     ," :: 0","\n");
write("true * 123 = "  , (1===1)*123   ," :: 123","\n");
write("false * 123 = "  , (1===2)*123   ," :: 0","\n");
write("true * 0.0 = "  , (1===1)*0.0   ," :: 0","\n");
write("false * 0.0 = "  , (1===2)*0.0   ," :: 0","\n");
write("true * 0.25 = " , (1===1)*0.25 ," :: 0.25","\n");
write("false * 0.25 = " , (1===2)*0.25 ," :: 0","\n");
write("true * true = " , (1===1)*(1===1) ," :: 1","\n");
write("false * true = " , (1===2)*(1===1) ," :: 0","\n");
write("ture * false = ", (1===1)*(1===2) ," :: 0","\n");
write("false* false = ", (1===2)*(1===2) ," :: 0","\n");
