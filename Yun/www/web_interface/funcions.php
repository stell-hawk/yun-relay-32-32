<?php 
function update_state($state,$pin)
{
	//echo $state;
	switch ($state)
	{
	case "On"	:
			$mode="digital";
			$command=1;
		break;
	case "Off"	:
			$mode="digital";
			$command=0;
		break;
	case "Input"	:
			$mode="mode";
			$command="input";
		break;
	case "Output"	:
			$mode="mode";
			$command="output";
		break;
	default: return false;

	}
	list($trash,$pin)=explode("(",$pin);
	
	$service_url = 'http://127.0.0.1/arduino/'.$mode.'/' .(int)$pin.'/' . $command;
  //echo $service_url;
  $curl = curl_init($service_url);
   
  // Send cURL to Yun board
  curl_setopt($curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4 ); 
  curl_setopt($curl,CURLOPT_RETURNTRANSFER,1);
  curl_setopt($curl, CURLOPT_USERPWD, "root:dragino");
  $curl_response = curl_exec($curl);
  curl_close($curl);
  //usleep(1*1000000);
}

function get_states()
{
require("bridgeclient.class.php");
$client = new bridgeclient(); 
$all = $client->getall(); 
//echo "<pre>";
//var_dump($all["InPorts"]);
$inPorts=explode(',',$all["inPorts"]);
$outPorts=explode(',',$all["outPorts"]);
//var_dump($outPorts);
foreach ($inPorts as $i=>$v)
{
	$out["inputs"][($i+101)."(".$v.")"]=$all['D'.$v];
}
foreach ($outPorts as $i=>$v)
{
	$out["outputs"][($i+1)."(".$v.")"]=$all['D'.$v];
}
//var_dump($out);
return $out;
}
