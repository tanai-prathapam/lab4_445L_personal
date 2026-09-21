/*!
 * @defgroup wifi
 * @brief wireless communication with ESP8266
  <table>
<caption id="ESP8266pins">Interface of ESP8266 to the MSPM0G3507 LaunchPad</caption>
<tr><th>Pin <th>ESP8266<th>MSPM0<th>Motor board version 7.1 or 7.2
<tr><td>1 <td>URxD    <td>PB17      <td>UART2 out of MSPM0, into ESP8266 115200 baud
<tr><td>2 <td>GPIO0   <td>          <td>+3.3V for normal operation (ground to flash)
<tr><td>3 <td>GPIO2   <td>PB19      <td>GPIO, high/float on startup, has internal pullup, can be used for I/O
<tr><td>4 <td>GND     <td>Gnd       <td>GND (70mA)
<tr><td>5 <td>UTxD    <td>PB18      <td>UART out of ESP8266, UART2 into MSPM0 115200 baud
<tr><td>6 <td>Ch_PD   <td>          <td>chip select, 10k resistor to 3.3V
<tr><td>7 <td>Reset   <td>PA25      <td>MSPM0 GPIO output, can issue output low to cause hardware reset
<tr><td>8 <td>Vcc     <td>          <td>regulated 3.3V supply with at least 70mA
</table>
 * @{*/

/**
 * @file      ESP8266.h
 * @brief     ESP8266 interface
 * @details   Driver for ESP8266 module to act as a WiFi client or server. 
Vcc is a separate regulated 3.3V supply with at least 215mA.
Currently restricted to one incoming or outgoing connection at a time.
- Original code by Steven Prickett (steven.prickett@gmail.com)
- Modified version by Dung Nguyen, Wally Guzman
- Modified by Jonathan Valvano, March 28, 2017
- Consolidated by Andreas Gerstlauer, April 6, 2020 
- Converted to MSPM0G3507 UART2 by Jonathan Valvano, Jan 26, 2026
 * @version   ECE445M RTOS V1.2
 * @author    Steven Prickett, Dung Nguyen, Wally Guzman, Andreas Gerstlauer, Jonathan Valvano
 * @copyright Copyright 2026 by Jonathan W. Valvano, valvano@mail.utexas.edu,
 * @warning   AS-IS
 * @note      For more information see  http://users.ece.utexas.edu/~valvano/
 * @date      May 12, 2026
 * @note Only client tested on the MSPM0G3507
<table>
<caption id="ESP8266pins2">Interface of ESP8266 to the MSPM0G3507 LaunchPad</caption>
<tr><th>Pin <th>ESP8266<th>MSPM0<th>Motor board version 7.1 or 7.2
<tr><td>1 <td>URxD    <td>PB17      <td>UART2 out of MSPM0, into ESP8266 115200 baud
<tr><td>2 <td>GPIO0   <td>          <td>+3.3V for normal operation (ground to flash)
<tr><td>3 <td>GPIO2   <td>PB19      <td>GPIO, high/float on startup, has internal pullup, can be used for I/O
<tr><td>4 <td>GND     <td>Gnd       <td>GND (215mA)
<tr><td>5 <td>UTxD    <td>PB18      <td>UART out of ESP8266, UART2 into MSPM0 115200 baud
<tr><td>6 <td>Ch_PD   <td>          <td>chip select, 10k resistor to 3.3V
<tr><td>7 <td>Reset   <td>PA25      <td>MSPM0 GPIO output, can issue output low to cause hardware reset
<tr><td>8 <td>Vcc     <td>          <td>regulated 3.3V supply with at least 215mA
</table>
 * \image html ESP8266circuit.png  width=600px
 * \image html esp8266Layout.png  width=600px
 * @note  Ok to not access PB19 because of the internal pullup in ESP8266
  ******************************************************************************/

#ifndef ESP8266_H
#define ESP8266_H

#include <stdint.h>
/*!
 * @brief Encryption modes
 */
#define ESP8266_ENCRYPT_MODE_OPEN            0
#define ESP8266_ENCRYPT_MODE_WEP             1
#define ESP8266_ENCRYPT_MODE_WPA_PSK         2
#define ESP8266_ENCRYPT_MODE_WPA2_PSK        3
#define ESP8266_ENCRYPT_MODE_WPA_WPA2_PSK    4

/*!
 * @brief ESP8266 modes Client, AP, AP/Client
 */
#define ESP8266_WIFI_MODE_CLIENT            1
#define ESP8266_WIFI_MODE_AP                2
#define ESP8266_WIFI_MODE_AP_AND_CLIENT     3


/**
 * @details Initialize the ESP8266-01.
 * Configure UART2 for 115200bps operation.
 * @param  rx_echo 1 for receive echo to UART0
 * @param  tx_echo 1 for transmit echo to UART0
 * @return 1 for success, 0 for failure (no ESP detected)
 * @note A common reason for this to fail is if the ESP8266 has been programmed with alternate code (different from original)
 * @brief  Initializes ESP8266
 * @note see ESP8266 files in datasheets folder
*/
int ESP8266_Init(int rx_echo, int tx_echo);

/**
 * @details Connect the ESP8266-01 to the Wifi hotspot.
 * Bring interface up and connect to Wifi.
 * @param  verbose true to enable debug output
 * @return 1 on success, 0 on failure
 * @brief  Connects ESP8266
 * @note specify SSID_NAME and PASSKEY at top of esp8266.c
 */
int ESP8266_Connect(int verbose);


/**
 * @details Start server on specific port.
 * @param  port is the server port number
 * @param  timeout is the server timeout, 0-28800 seconds
 * @return 1 on success, 0 on failure
 * @brief  Start server 
 * @note This has not been tested on the MSPM0
 */
int ESP8266_StartServer(uint16_t port, uint16_t timeout);


/**
 * @details Stop server and set to single-client mode.
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Stop server 
 * @note This has not been tested on the MSPM0
 */
int ESP8266_StopServer(void);

/**
 * @details Soft resets the esp8266 module.
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Reset the esp8266 
 * @note A common reason for this to fail is if the ESP8266 has been programmed with alternate code (different from original)
 */
int ESP8266_Reset(void);

/**
 * @details Restore the ESP8266 module to default values.
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Restore default settings 
 */
 int ESP8266_Restore(void);

/**
 * @details Get Version Number. See the results in the debug stream
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Version number 
 */
int ESP8266_GetVersionNumber(void);


/**
 * @details Get MAC address. See the results in the debug stream
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  MAC address 
 */
int ESP8266_GetMACAddress(void);

/**
 * @details Configures the esp8266 to operate as a wifi client, access point, or both
- ESP8266_WIFI_MODE_CLIENT            1
- ESP8266_WIFI_MODE_AP                2
- ESP8266_WIFI_MODE_AP_AND_CLIENT     3
 * @param  mode 1 2 or 3
 * @return 1 on success, 0 on failure
 * @brief  Set mode 
 * @note Only client mode been tested on the MSPM0
 */
int ESP8266_SetWifiMode(uint8_t mode);
 

/**
 * @details Enables the esp8266 connection mux, required for starting tcp server
 * @param  enabled  0 (single) or 1 (multiple)
 * @return 1 on success, 0 on failure
 * @brief  Set mode 
 * @note Has not been tested on the MSPM0
 */
int ESP8266_SetConnectionMux(uint8_t enabled);

/**
 * @details Lists available wifi access points
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Access Points 
 */
int ESP8266_ListAccessPoints(void);

/**
 * @details Joins a wifi access point using specified ssid and password
 * @param  ssid is the name of the AP
 * @param  password is the password of the AP
 * @return 1 on success, 0 on failure
 * @brief  Join AP
 * @note This is called within ESP8266_Connect
 */
int ESP8266_JoinAccessPoint(const char* ssid, const char* password);

/**
 * @details Disconnects from currently connected wifi access point
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Quit AP
 */
int ESP8266_QuitAccessPoint(void);


/**
 * @details Configures esp8266 wifi soft access point settings.
 * Use this function only when in AP mode (and not in client mode)
- ESP8266_ENCRYPT_MODE_OPEN            0
- ESP8266_ENCRYPT_MODE_WEP             1
- ESP8266_ENCRYPT_MODE_WPA_PSK         2
- ESP8266_ENCRYPT_MODE_WPA2_PSK        3
- ESP8266_ENCRYPT_MODE_WPA_WPA2_PSK    4
 * @param  ssid name of the AP
 * @param  password password of the AP
 * @param  channel channel of the AP
 * @param  encryptMode mode of the AP, 0 to 4
 * @return 1 on success, 0 on failure
 * @brief  Configure AP
 * @note Has not been tested on the MSPM0
 */
int ESP8266_ConfigureAccessPoint(const char* ssid, const char* password, uint8_t channel, uint8_t encryptMode);


/**
 * @details Get local IP address. See the results in the debug stream.
 * @param  none is the password of the AP
 * @return 1 on success, 0 on failure
 * @brief  Get IP address
 * @note This is called within ESP8266_Connect
 */
int ESP8266_GetIPAddress(void);


/**
 * @details Set SSL client configuration.
 * Requires certificates to be flashed into the ESP firmware
 * @param  verifyClient enable/disable client certificate checks
 * @param  verifyServer enable/disable server certificate checks
 * @return 1 on success, 0 on failure
 * @brief  Set SSL
 * @note This function has not been tested on the MSPM0
 */
int ESP8266_SetSSLClientConfiguration(int verifyClient, int verifyServer);

/**
 * @details Set SSL buffer size
 * @param  bufferSize buffer size between 2048 and 4096
 * @return 1 on success, 0 on failure
 * @brief  SSL buffer size
 * @note This function has not been tested on the MSPM0
 */
int ESP8266_SetSSLBufferSize(uint16_t bufferSize);


/**
 * @details Establish TCP or SSL connection.
 * Connect a socket in this client with a socket in the server.
 * For the ECE445L RTOS server, one must immediately send the TCP, and
 * this socket will automatically close.
 * The ESP only seems to have limited SSL support, does not work with all servers
 * @param  IPaddress IP address or web page as a string
 * @param  port as a number
 * @param  keepalive keepalive time (0 if none)
 * @param  ssl 0 for TCP, 1 for SSL
 * @return 1 on success, 0 on failure
 * @brief  Make connection
 * @note Only TCP has been tested (not SSL)
 */
int ESP8266_MakeTCPConnection(char *IPaddress, uint16_t port, uint16_t keepalive, int ssl);


/**
 * @details Send a TCP packet to server.
 * The incoming data stream from the server is searched for "status=".
 * For the ECE445M RTOS server, 
 -# ESP8266_MakeTCPConnection();
 -# ESP8266_StartReceiveSearch();
 -# ESP8266_Send();
 -# This socket will automatically close.
 -# s = ESP8266_GetReceiveBuffer(); // pointer to search string
 * @param  fetch payload as an ASCII string
 * @return 1 on success, 0 on failure
 * @brief  Send TCP
 * @note Only TCP has been tested (not SSL)
 */
int ESP8266_Send(const char* fetch);

/**
 * @details Send a string to server using ESP TCP-send buffer.
 * Used when implementing a server
 * @param  fetch payload as an ASCII string to send
 * @return 1 on success, 0 on failure
 * @brief  Send buffered TCP
 * @note This has not been tested on the MSPM0
 */
int ESP8266_SendBuffered(const char* fetch);


/**
 * @details Check status of last buffered segment.
 * This is part of the server code
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Check status 
 * @note This has not been tested on the MSPM0
 */
int ESP8266_SendBufferedStatus(void);


/**
 * @details Receive a TCP packet from server.
 * Reads from data input until end of line or max length is reached
 * For the GetWeatherMap server, 
 -# ESP8266_MakeTCPConnection(); 
 -# ESP8266_Send();
 -# ESP8266_Receive();
 -# ESP8266_CloseTCPConnection();
 * @param  fetch received payload as an ASCII string
 * @param  max size of the ASCII array
 * @return 1 on success, 0 on failure
 * @brief  Receive TCP
 */
int ESP8266_Receive(char* fetch, uint32_t max);


/**
 * @details Close TCP connection
 * For the GetWeaterMap server, 
 -# ESP8266_MakeTCPConnection(); 
 -# ESP8266_Send();
 -# ESP8266_Receive();
 -# ESP8266_CloseTCPConnection();
 * @param  none 
 * @return 1 on success, 0 on failure
 * @brief  Close TCP socket
 */
int ESP8266_CloseTCPConnection(void);


/**
 * @details Set data transmission passthrough mode
 * @param  mode 0 not data mode, 1 data mode; return "Link is builded"
 * @return 1 on success, 0 on failure
 * @brief  Set transmission mode
 * @note This has not been tested on the MSPM0
 */
int ESP8266_SetDataTransmissionMode(uint8_t mode);


/**
 * @details Get network connection status. See the results in the debug stream.
 * @param  none is the password of the AP
 * @return 1 on success, 0 on failure
 * @brief  Get status
 */
int ESP8266_GetStatus(void);


/**
 * @details Enables tcp server on specified port.
 * @param  port number of the server port
 * @return 1 on success, 0 on failure
 * @brief  Enable server 
 * @note This has not been tested on the MSPM0
 */
int ESP8266_EnableServer(uint16_t port);


/**
 * @details Set connection timeout for tcp server, 0-28800 seconds
 * @param  timeout 0-28800 seconds
 * @return 1 on success, 0 on failure
 * @brief  Set server timeout
 * @note This has not been tested on the MSPM0
 */
int ESP8266_SetServerTimeout(uint16_t timeout);


/**
 * @details Wait for incoming connection on server
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Wait for connection
 * @note This has not been tested on the MSPM0
 */
int ESP8266_WaitForConnection(void);


/**
 * @details Disables tcp server
 * @param  none
 * @return 1 on success, 0 on failure
 * @brief  Disable server
 * @note This has not been tested on the MSPM0
 */
int ESP8266_DisableServer(void);


/**
 * @details Set a search string to find response from server.
 * The incoming data stream from the server is searched for "status=".
 * After match, it will collect ASCII characters while greater than or equal to 'A'.
 * It will stop collecting ASCII character is less than 'A'.
 * if rxecho is active, receive data is also streamed to ReceiveBuffer
 * For the ECE445M RTOS server, 
 -# ESP8266_MakeTCPConnection();
 -# ESP8266_StartReceiveSearch();
 -# ESP8266_Send();
 -# This socket will automatically close.
 -# s = ESP8266_GetReceiveBuffer(); // pointer to search string
 * @param  search is the ASCII string to search for
 * @return 1 on success, 0 on failure
 * @brief  Set search string
 */
void ESP8266_StartReceiveSearch(char *search);
/**
 * @details Get a search string withing response from server.
 * The search string was previously specified by ESP8266_StartReceiveSearch();
 * For the ECE445M RTOS server, 
 -# ESP8266_MakeTCPConnection();
 -# ESP8266_StartReceiveSearch();
 -# ESP8266_Send();
 -# This socket will automatically close.
 -# s = ESP8266_GetReceiveBuffer(); // pointer to search string
 * @param  none
 * @return pointer to string on success, 0 on failure
 * @brief  Get search string
 */
char * ESP8266_GetReceiveBuffer(void);
#endif
