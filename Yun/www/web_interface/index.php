<?php 
require("funcions.php");
require("header.html");

if($_POST)
{
	if(isset($_POST['id']))
	{
		update_state($_POST['State'],$_POST['id']);
	}
}

$all=get_states();
echo "<br /><center>";
foreach($all['inputs'] as $k => $v)
{
	if($v=="1")echo "<img src=lighton.jpg title={$k}>";
	else echo "<img src=lightoff.jpg  title={$k}>";
}

foreach($all['outputs'] as $i => $v)
{
	$green_on=" ";
	$green_off=" style='background-color:green' ";
	if($v)
	{
	$green_off=" ";
	$green_on=" style='background-color:green' ";
		
	}
?>
			<br /><form id="relay" method="POST">
			<input type=hidden name ="id" value="<?=$i?>" />
			<input type="submit" id="<?=$i?>" class="commandButton" value="<?=$i?>"/>
    	<input type="submit" id="<?=$i?>_on" name="State" class="commandButton"  value="On" <?=$green_on?>"/>
    	<input type="submit" id="<?=$i?>_off" name="State" class="commandButton" value="Off" <?=$green_off?>"/><br />
			</form>
<?php }?>
	

</body>

</html>

