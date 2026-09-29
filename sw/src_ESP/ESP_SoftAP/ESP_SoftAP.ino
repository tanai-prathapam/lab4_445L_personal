// =============================================================================
//
//  ESP8266  —  SoftAP WebSocket Server  —  EE445L Lab 4E Clock Controller
//  
//   Rev 4: 5/20/26             M. McDermott
//
//  Peer-to-peer variant: ESP8266 creates its own WiFi access point.
//  No router or infrastructure network is required.
//
//  How to use
//  ──────────
//  1. Flash this sketch to the ESP8266.
//
//  2. On your laptop / phone, join the WiFi network:
//       SSID  : your-name_partner-name
//       Pass  : 12345678
//     (change AP_SSID / AP_PASS to be something unique)
//
//  3. Open main.htm in a browser.  The page connects to ws://192.168.4.1:81/
//     That IP is the ESP's fixed SoftAP address — no discovery needed.
//
//  Architecture
//  ────────────
//                    USB/Serial                WebSocket (WiFi)
//    MSPM0  ─────────────────────────  ESP8266  ─────────────────────  Browser
//              mode,hour,min,sec  →             →  "mode,hour,min,sec"
//              ←  cmd char                      ←  "1".."7"
//
//  Serial protocol with MSPM0 
//  ─────────────────────────────────────────────────────────────────────────
//  Runtime  MSPM0 → ESP : "mode,hour,minute,second\n"   (~1 s cadence)
//  Runtime  ESP  → MSPM0: single-char command + "\n"     ("1".."7")
//
//  Required Arduino library
//  ─────────────────────────
//  "WebSockets" by Markus Sattler  (install via Arduino Library Manager)
//
// =============================================================================


// ----------------------------------------------------------------
// ----------------    COMPILE-TIME SETTINGS   --------------------
//
//#define DEBUG1                      // Basic status messages on Serial port
#define DEBUG2                      // Basic status messages on Serial port

// ************************* CHANGE THE SSID TO BE UNIQUE ***********************
#define AP_SSID     "aleena_tanai"        // SoftAP network name. CHANGE THIS TO BE UNIQUE  (≤ 32 chars)
#define AP_PASS     "12345678"      // SoftAP password       (≥ 8 chars, or "" for open)
#define AP_CHANNEL  6               // WiFi channel  1-13
#define AP_HIDDEN   0               // 0 = broadcast SSID,  1 = hidden

// Fixed IP that the ESP hands itself as the SoftAP gateway.
// Browsers always connect to this address — no DHCP lookup needed.
// 192.168.4.1 is the ESP8266 SoftAP default
//
// ----------------------------------------------------------------------------

#define AP_IP_STR   "192.168.4.1"   // Make sure this IP address matches button.js

#define WS_PORT     81              // WebSocket server port
#define RDY         2               // GPIO_2 → RDY output to MSPM0
#define BAUD        115200

// ----------------------------------------------------------------
// ---------------   INCLUDES    ----------------------------------
//
#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>       // arduinoWebSockets by Markus Sattler

#include <stdio.h>
#include <string.h>

// ----------------------------------------------------------------
// ----------------  RUNTIME VARIABLES   -------------------------
//
char  mode[2]         = "";
char  hour_buf[3]     = "";
char  minute_buf[3]   = "";
char  second_buf[3]   = "";
char  ADC_buf[4]      = ""; //3 characters (up to 100) + null terminator
char  cmd[20];
char  ser_buf[128];

static const IPAddress AP_IP(192, 168, 4, 1);		// Change to new IP
static const IPAddress AP_GW(192, 168, 4, 1);		// Change to new IP
static const IPAddress AP_MASK(255, 255, 255, 0);

// ----------------------------------------------------------------
// ----------------  WebSocket Server  ----------------------------
//
WebSocketsServer webSocket = WebSocketsServer(WS_PORT);


// ================================================================
//  Setup_SoftAP
//  ─────────────
//  Starts the ESP8266 as a WiFi access point, then optionally reads
//  (and discards) the startup CSV the MSPM0 sends so MSPM0 firmware
//  does not need to change.
// ================================================================

void Setup_SoftAP(void) {

    // ── 1.  Signal the MSPM0 that ESP is initializing ────────────
    digitalWrite(RDY, LOW);   delay(200);

    // ── 2.  Start the access point ──────────────────────────────
 
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);  delay(100);
    WiFi.mode(WIFI_AP);   delay(100);
    WiFi.softAPConfig(AP_IP, AP_GW, AP_MASK);
    
    bool ok = WiFi.softAP(AP_SSID, AP_PASS, 6, 0, 4);        // Debug

    delay(500);

    #ifdef DEBUG2
    // Display this info you need the MAC address 
    Serial.printf("\n\nsoftAP: %s\n", ok ? "OK" : "FAIL");
    Serial.print  ("IP : "); Serial.println(WiFi.softAPIP());
    Serial.print  ("MAC: "); Serial.println(WiFi.softAPmacAddress()); 
    Serial.println("\n[AP] SoftAP started\n\n");

    #endif 

    // ── 3.  Drain the startup data from MSPM0 (fields ignored) ────
    //        This keeps MSPM0 firmware compatible; it still sends
    //        "eid,ssid,password[,broker,port]\n" and we just absorb it.
    Serial.flush();
    delay(100);

    // Wait up to 3 s for MSPM0 to start sending; if nothing arrives
    // within that window we continue anyway (stand-alone bench use).
    //
    unsigned long t0 = millis();
    while (Serial.available() == 0 && (millis() - t0) < 3000) {}

    if (Serial.available() > 0) {
        // Read and discard the config line
        while (Serial.available() > 0) {
            char c = Serial.read();
            (void)c;                // intentionally unused
            delay(2);
        }
        #ifdef DEBUG3
        Serial.println("[AP] Received and discarded MSPM0 data string (not needed in SoftAP mode)");
        #endif
    } else {
        #ifdef DEBUG3
        Serial.println("[AP] No MSPM0 config string received (bench/standalone mode)");
        #endif
    }

    // ── 4.  Signal MSPM0 that setup is complete ───────────────────
    digitalWrite(RDY, HIGH);

}   // END Setup_SoftAP


// ================================================================
//  webSocketEvent
//  ───────────────
//  Handles all WebSocket events.  Text messages from the browser
//  are forwarded to the MSPM0 over the hardware serial port.
// ================================================================

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {

    switch (type) {

        // ── Client disconnected ──────────────────────────────────
        case WStype_DISCONNECTED:
            #ifdef DEBUG1
            Serial.printf("[WS] Client #%u disconnected  (clients now: %u)\n",
                          num, webSocket.connectedClients());
            #endif
            break;

        // ── Client connected ─────────────────────────────────────
        case WStype_CONNECTED: {
            IPAddress ip = webSocket.remoteIP(num);
            #ifdef DEBUG1
            Serial.printf("[WS] Client #%u connected from %s  (total: %u)\n",
                          num, ip.toString().c_str(),
                          webSocket.connectedClients());
            #endif
            break;
        }

        // ── Text message from browser → forward to MSPM0 ─────────
        case WStype_TEXT:
            payload[length] = '\0';
            #ifdef DEBUG1
            Serial.printf("[WS] Cmd from client #%u : %s\n", num, payload);
            #endif
            if (length > 0) {
                strncpy(cmd, (char *)payload, sizeof(cmd) - 1);
                cmd[sizeof(cmd) - 1] = '\0';
                Serial.println(cmd);        // MSPM0 reads this line
            }
            break;

        default:
            break;
    }
}


// ================================================================
//  msp2ws
//  ────────
//  Called every loop iteration.  Reads complete CSV lines from the
//  MSPM0 serial port and broadcasts them to all connected browsers.
//
//  Input  (from MSPM0) : "mode,hour,minute,second\n"
//  Output (to browser): "mode,hour,minute,second"   (no newline)
// ================================================================

void msp2ws(void) {

    static int bufpos = 0;

    while (Serial.available() > 0) {

        char inchar = Serial.read();

        if (inchar != '\n') {
            if (bufpos < (int)(sizeof(ser_buf) - 1)) {
                ser_buf[bufpos++] = inchar;
            }
            delay(2);
        } else {
            if (bufpos > 0) {
                ser_buf[bufpos] = '\0';

                // Parse CSV fields
                char tmp[128];
                strncpy(tmp, ser_buf, sizeof(tmp) - 1);
                tmp[sizeof(tmp) - 1] = '\0';

                char *tok;
                tok = strtok(tmp, ",");   if (tok) strncpy(mode,        tok, sizeof(mode)        - 1);
                tok = strtok(NULL, ",");  if (tok) strncpy(hour_buf,    tok, sizeof(hour_buf)    - 1);
                tok = strtok(NULL, ",");  if (tok) strncpy(minute_buf,  tok, sizeof(minute_buf)  - 1);
                tok = strtok(NULL, ",");  if (tok) strncpy(second_buf,  tok, sizeof(second_buf)  - 1);
                tok = strtok(NULL, ",");  if (tok) strncpy(ADC_buf, tok, sizeof(ADC_buf) - 1);

                // Broadcast to all connected browsers
                char msg[40];
                snprintf(msg, sizeof(msg), "%s,%s,%s,%s,%s", mode, hour_buf, minute_buf, second_buf, ADC_buf);

                #ifdef DEBUG1
                Serial.print("[WS] TX → browser: ");
                Serial.println(msg);
                #endif

                webSocket.broadcastTXT(msg);
            }

            // Reset buffer
            memset(ser_buf, 0, sizeof(ser_buf));
            bufpos = 0;
        }
    }
}


// ================================================================
//  setup
// ================================================================

void setup() {

    Serial.begin(BAUD);
    Serial.flush();

    pinMode(0, INPUT);
    pinMode(RDY, OUTPUT);
    digitalWrite(RDY, LOW);

    Setup_SoftAP();

    // Start WebSocket server
    webSocket.begin();
    webSocket.onEvent(webSocketEvent);

    #ifdef DEBUG1
    Serial.println("[WS] WebSocket server ready");
    Serial.printf ("[WS] ws://%s:%d/\n", AP_IP_STR, WS_PORT);
    #endif
}


// ================================================================
//  loop
// ================================================================

void loop() {
    webSocket.loop();   // Service WebSocket clients
    msp2ws();          // Poll MSPM0 serial → broadcast to client
}



