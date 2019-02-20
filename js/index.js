// Based on an example:
//https://github.com/don/cordova-plugin-ble-central


// ASCII only
function bytesToString(buffer) {
    return String.fromCharCode.apply(null, new Uint8Array(buffer));
}

// ASCII only
function stringToBytes(string) {
    var array = new Uint8Array(string.length);
    for (var i = 0, l = string.length; i < l; i++) {
        array[i] = string.charCodeAt(i);
    }
    return array.buffer;
}

// this is ble hm-10 UART service
/*var blue= {
    serviceUUID: "0000FFE0-0000-1000-8000-00805F9B34FB",
    characteristicUUID: "0000FFE1-0000-1000-8000-00805F9B34FB"
};*/

//the bluefruit UART Service
var blue ={
	serviceUUID: '6e400001-b5a3-f393-e0a9-e50e24dcca9e',
    txCharacteristic: '6e400002-b5a3-f393-e0a9-e50e24dcca9e', // transmit is from the phone's perspective
    rxCharacteristic: '6e400003-b5a3-f393-e0a9-e50e24dcca9e'  // receive is from the phone's perspective
}

var ConnDeviceId;
var deviceList =[];
 
function onLoad(){
	document.addEventListener('deviceready', onDeviceReady, false);
    bleDeviceList.addEventListener('touchstart', conn, false); // assume not scrolling
}

function onDeviceReady(){
	refreshDeviceList();
}

	 
function refreshDeviceList(){
	//deviceList =[];
	document.getElementById("bleDeviceList").innerHTML = ''; // empties the list
	if (cordova.platformId === 'android') { // Android filtering is broken
		ble.scan([], 5, onDiscoverDevice, onError);
	} else {
		//alert("Disconnected");
		ble.scan([blue.serviceUUID], 5, onDiscoverDevice, onError);
	}
}


function onDiscoverDevice(device){
	//Make a list in html and show devises
	
		var listItem = document.createElement('li'),
		html = device.name+ "," + device.id;
		listItem.innerHTML = html;
		document.getElementById("bleDeviceList").appendChild(listItem);
	
}


function conn(){
	var  deviceTouch= event.srcElement.innerHTML;
	document.getElementById("debugDiv").innerHTML =""; // empty debugDiv
	var deviceTouchArr = deviceTouch.split(",");
	ConnDeviceId = deviceTouchArr[1];
	document.getElementById("debugDiv").innerHTML += "<br>"+deviceTouchArr[0]+"<br>"+deviceTouchArr[1]; //for debug:
	ble.connect(ConnDeviceId, onConnect, onConnError);
 }
 
 //success
function onConnect(){
	document.getElementById("statusDiv").innerHTML = " Status: Connected";
	document.getElementById("bleId").innerHTML = ConnDeviceId;
	ble.startNotification(ConnDeviceId, blue.serviceUUID, blue.rxCharacteristic, onData, onError);
}

//failure
function onConnError(){
	alert("Problem connecting");
	document.getElementById("statusDiv").innerHTML = " Status: Disonnected";
}

// str = "12, 60"
// var partsOfStr = str.split(",")
// var partsOfStr[0]  // "12"
// partsOfStr[1] // " 60"
// parseInt("60")  // 60
// parseInt(" 12") // 12
 function onData(data){ // data received from Arduino
	//document.getElementById("receiveDiv").innerHTML =  bytesToString(data) + "<br/>";
	//var data = str.split(",");
  
	
	var datastr = bytesToString(data);
	
var KgRep = datastr.split(".")
	
	
	//var data[0] = parseInt();
	
	document.getElementById("receiveDivRep").innerHTML = String(KgRep[0]); 
	document.getElementById("receiveDivKg").innerHTML = String(KgRep[1]);

/*
	var KgRep = data.split(",");

	
	
	//var data[0] = parseInt();
	
	document.getElementById("receiveDivRep").innerHTML = String(KgRep[0]); 
	document.getElementById("receiveDivKg").innerHTML = String(KgRep[1]); 
	*/
	}

function data(txt){
	messageInput.value = txt;
}	


function sendData() { // send data to Arduino
	 var data = stringToBytes(messageInput.value);
	ble.writeWithoutResponse(ConnDeviceId, blue.serviceUUID, blue.txCharacteristic, data, onSend, onError);
}
	
function onSend(){
	document.getElementById("sendDiv").innerHTML = "Sent: " + messageInput.value + "<br/>";
}

function disconnect() {
	ble.disconnect(deviceId, onDisconnect, onError);
}

function onDisconnect(){
	document.getElementById("statusDiv").innerHTML = "Status: Disconnected";
}
function onError(reason)  {
	alert("ERROR: " + reason); // real apps should use notification.alert
}
function insertText() {
	document.getElementById("aboutContent").innerHTML = "Fitness Digital Monitor helps you keep track of your fitness routines: How much weight you use and how many reps and sets you take! This will help you keep track of your development as well as how much weight you used on previous workouts on the different machines. It is also great at motivating you, seeing your constant improvements";		
	}
function nextSet() {

    
	/* 
 document.write(res[0]+"<br/>");
 document.write(res[1]);
*/
 
document.getElementById("antalSet").innerHTML = bytesToString("Hej" + "<br/");
}

var i = 0;
function nextSet() {
	i = i + 1;
	
	document.getElementById("aboutContent3").innerHTML = i;


	}
	
	//document.getElementById("aboutContent3").innerHTML = }
	
