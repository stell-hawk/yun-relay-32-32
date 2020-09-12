function relayOn(pin){

	$.get( "update_state.php", { 
		"command": "1", "pin":pin} );
	var elem = document.getElementById(pin+'_on');
	elem.style.backgroundColor="green";
	elem = document.getElementById(pin + '_off');
	elem.style.backgroundColor="";
}

function relayOff(pin){

	$.get( "update_state.php", { 
		command: "0", "pin":pin} );
	var elem = document.getElementById(pin+'_off');
	elem.style.backgroundColor="green";
	elem = document.getElementById(pin + '_on');
	elem.style.backgroundColor="";

}
function relayModeInput(pin){

	$.get( "update_mode.php", { 
		command: "input", "pin":pin} );
}
function relayModeOutput(pin){

	$.get( "update_mode.php", { 
		command: "output", "pin":pin} );
}