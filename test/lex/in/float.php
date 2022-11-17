<?php
declare(strict_types=1);

/*float


*/
$var_float = 0.001;
$var_float2=0.02;
$var_float3  =  0.03e-2;
$var_float4 =0.04e3;

$sum=$var_float+$var_float2;
$sum=$var_float3+$var_float4;
$sub = $var_float  - $var_float2  ;

$mul= $var_float3*  $var_float4;

 $div =$var_float/ $var_float2  ;


$var_float=== $var_float;
$var_float4!== $var_float2 ;
$var_float4 >= $var_float3;
$var_float2 <=$var_float  ;


function f_x (?float $float, ?float $float)
{
    return $float + $float;
}

f_x(null, null);

f_x
(
    $var_float,
    $var_float4
);

f_x(null, 
$var_float2);

f_x
($var_float
,
null);
                            $var_float<$var_float3;
$var_float4>$var_float2 ;?>