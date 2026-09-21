/*
   WebSocket + SoftAP Control Interface — EE445L Lab 4F Clock

   Author: Mark McDermott       June 21, 2026

   The ESP8266 runs as a WiFi Access Point (no router needed).
   Join the network "EE445L_Clock" / "ee445l!" on your device,
   then open main.htm — it connects directly to the ESP at the
   fixed SoftAP address 192.168.4.1:81. This needs to be changed
   to unique address for each team.

   Protocol (unchanged from network WebSocket version):
     ESP → Browser : "mode,hour,minute,second"  (CSV, ~1 s cadence)
     Browser → ESP : single-char command string  ("1"–"7")

*/

// -----------------------------------------------------------------------
// Configuration — fixed SoftAP address; no discovery needed
//
var esp_host  = "192.168.4.1";   // ESP8266 SoftAP gateway 
var ws_port   = "81";
var ws_url    = "ws://" + esp_host + ":" + ws_port + "/";

// -----------------------------------------------------------------------
// Global state
//
var ws        = null;
var hour      = "";
var minute    = "";
var second    = "";
var mil_time  = "0";

// -----------------------------------------------------------------------
// connect() — main entry point, called after page load
//
function connect() {
    setStatus("Connecting to " + ws_url + " …", "orange");
    console.info("Connecting to ESP8266 SoftAP WebSocket:", ws_url);

    ws = new WebSocket(ws_url);

    ws.onopen    = onOpen;
    ws.onclose   = onClose;
    ws.onerror   = onError;
    ws.onmessage = onMessage;
}

function onOpen(event) {
    console.log("WebSocket connected");
    setStatus("Connected to ESP8266 (" + esp_host + ")", "green");
}

function onClose(event) {
    console.log("WebSocket closed:", event.code, event.reason);
    setStatus("Disconnected — refresh to reconnect", "red");
    window.alert("WebSocket connection closed!\nRefresh to reconnect.");
}

function onError(event) {
    console.error("WebSocket error:", event);
    setStatus("Connection error — is your device on EE445L_Clock WiFi?", "red");
}

// -----------------------------------------------------------------------
// onMessage — parse "mode,hour,minute,second" from ESP
//
function onMessage(event) {
    var data = event.data.trim();
    console.log("RX:", data);

    var parts = data.split(",");
    if (parts.length >= 4) {
        mil_time = parts[0];
        hour     = update(parseInt(parts[1]));
        minute   = update(parseInt(parts[2]));
        second   = update(parseInt(parts[3]));
    }
}

// -----------------------------------------------------------------------
// sendCommand — send a single-char command to the ESP
//
function sendCommand(cmd) {
    if (ws && ws.readyState === WebSocket.OPEN) {
        ws.send(cmd);
        console.info("TX:", cmd);
    } else {
        console.warn("WebSocket not connected — command dropped:", cmd);
    }
}

// -----------------------------------------------------------------------
// Button handlers
//
function toggle_mil_time() { sendCommand("1"); } 

// ***solution***
   
function inc_hour()        { sendCommand("2"); }
function dec_hour()        { sendCommand("3"); }
function inc_min()         { sendCommand("4"); }
function dec_min()         { sendCommand("5"); }
function inc_sec()         { sendCommand("6"); }
function dec_sec()         { sendCommand("7"); }

//*******end of solution*****

// -----------------------------------------------------------------------
// Board_Time — updates the #board-clock div every second
//
function Board_Time() {

    var period = "";
    var h = parseInt(hour, 10);

    if (mil_time === "0" || mil_time === 0) {
        if (h === 0) {
            h = 12; period = "AM";
        } else if (h < 12) {
            period = "AM";
        } else if (h === 12) {
            period = "PM";
        } else {
            h = h - 12; period = "PM";
        }
    }

    document.getElementById("board-clock").innerText =
        update(h) + " : " + minute + " : " + second + " " + period;

    setTimeout(Board_Time, 1000);
}

// -----------------------------------------------------------------------
// setStatus — updates the small connection-status line on the page
//
function setStatus(msg, color) {
    var el = document.getElementById("ws-status");
    if (el) {
        el.innerText = msg;
        el.style.color = color || "black";
    }
}

function update(t) {
    if (t < 10) return "0" + t;
    return "" + t;
}

Board_Time();
