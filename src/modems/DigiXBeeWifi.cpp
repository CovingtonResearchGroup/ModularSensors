/**
 * @file DigiXBeeWifi.cpp
 * @copyright 2017-2022 Stroud Water Research Center
 * Part of the EnviroDIY ModularSensors library for Arduino
 * @author Sara Geleskie Damiano <sdamiano@stroudcenter.org>
 *
 * @brief Implements the DigiXBeeWifi class.
 */

// Included Dependencies
#include "DigiXBeeWifi.h"
#include "LoggerModemMacros.h"

// Constructor/Destructor
DigiXBeeWifi::DigiXBeeWifi(Stream* modemStream, int8_t powerPin,
                           int8_t statusPin, bool useCTSStatus,
                           int8_t modemResetPin, int8_t modemSleepRqPin,
                           const char* ssid, const char* pwd)
    : DigiXBee(powerPin, statusPin, useCTSStatus, modemResetPin,
               modemSleepRqPin),
#ifdef MS_DIGIXBEEWIFI_DEBUG_DEEP
      _modemATDebugger(*modemStream, DEEP_DEBUGGING_SERIAL_OUTPUT),
      gsmModem(_modemATDebugger, modemResetPin),
#else
      gsmModem(*modemStream, modemResetPin),
#endif
      gsmClient(gsmModem),
      _ssid(ssid),
      _pwd(pwd) {
}

// Destructor
DigiXBeeWifi::~DigiXBeeWifi() {}

MS_IS_MODEM_AWAKE(DigiXBeeWifi);
MS_MODEM_WAKE(DigiXBeeWifi);

// MS_MODEM_CONNECT_INTERNET(DigiXBeeWifi); has instability
// See https://github.com/neilh10/ModularSensors/issues/125             
bool DigiXBeeWifi::connectInternet(uint32_t maxConnectionTime) { 
    MS_START_DEBUG_TIMER                                          
    MS_DBG(F("\nDigiXbee Attempting to connect to WiFi network..."));      
    if (!(gsmModem.isNetworkConnected())) {                       
        if (!gsmModem.waitForNetwork(maxConnectionTime)) {        
            PRINTOUT(F("... WiFi connection failed"));            
            return false;                                         
        }                                                         
     }                                                            
    MS_DBG(F("... WiFi connected after"), MS_PRINT_DEBUG_TIMER, 
               F("milliseconds!"));                             
        return true;                                            
}
MS_MODEM_IS_INTERNET_AVAILABLE(DigiXBeeWifi);

MS_MODEM_GET_MODEM_BATTERY_DATA(DigiXBeeWifi);
MS_MODEM_GET_MODEM_TEMPERATURE_DATA(DigiXBeeWifi);

bool DigiXBeeWifi::extraModemSetup(void) {
    bool success = true;
    /** First run the TinyGSM init() function for the XBee. */
    MS_DBG(F("Initializing the XBee..."));
    success &= gsmModem.init();
    if (!success) { MS_DBG(F("Failed init")); }
    gsmClient.init(&gsmModem);
    _modemName = gsmModem.getModemName();
    /** Then enter command mode to set pin outputs. */
    if (gsmModem.commandMode()) {
        String  xbeeSnLow,xbeeSnHigh;//XbeeDevHwVer,XbeeFwVer;
        gsmModem.getSeries();
        _modemName = gsmModem.getModemName();
        gsmModem.sendAT(F("SL"));  // Request Module MAC/Serial Number Low
        gsmModem.waitResponse(1000, xbeeSnLow);
        gsmModem.sendAT(F("SH"));  // Request Module MAC/Serial Number High
        gsmModem.waitResponse(1000, xbeeSnHigh);
        _modemSerialNumber = xbeeSnHigh+xbeeSnLow;
        gsmModem.sendAT(F("HV"));  // Request Module Hw Version
        gsmModem.waitResponse(1000, _modemHwVersion);
        gsmModem.sendAT(F("VR"));  // Firmware Version
        gsmModem.waitResponse(1000, _modemFwVersion);
        PRINTOUT(F("XbeeWiFi internet comms with"),_modemName, 
                F("Mac/Sn "), _modemSerialNumber,F("HwVer"),_modemHwVersion, F("FwVer"), _modemFwVersion);

        // Leave all unused pins disconnected. Use the PR command to pull all of
        // the inputs on the device high using 40 k internal pull-up resistors.
        // You do not need a specific treatment for unused outputs.
        //   Mask Bit Description
        // 1 0001  0 TH11 DIO4
        // 1 0002  1 TH17 DIO3
        // 1 0004  2 TH18 DIO2
        // 1 0008  3 TH19 DIO1
        // 1 0010  4 TH20 DIO0
        // 1 0020  5 TH16 DIO6/RTS
        // 0 0040  6 TH09 DIO8/DTR/Sleep Request
        // 0 0080  7 TH03 DIN
        // 1 0100  8 TH15 DIO5/Associate
        // 0 0200  9 TH13 DIO9/- OnSLEEP
        // 1 0400 10 TH04 DIO12
        // 1 0800 11 TH06 DIO10/PWM RSSI
        // 1 1000 12 TH07 DIO11/PWM1
        // 1 2000 13 TH12 DIO7/-CTR
        // 0 4000 14 TH02 DIO13/DOUT
        //   3D3F
        gsmModem.sendAT(GF("PR"), "3D3F");
        success &= gsmModem.waitResponse() == 1;
        if (!success) { MS_DBG(F("Fail PR "), success); }
#if !defined MODEMPHY_NEVER_SLEEPS
#define XBEE_SLEEP_SETTING 1
#define XBEE_SLEEP_ASSOCIATE 100
#else
#define XBEE_SLEEP_SETTING 0
#define XBEE_SLEEP_ASSOCIATE 40
#endif  // MODEMPHY_NEVER_SLEEPS
        // To use sleep pins they physically need to be enabled.
        // Set DIO8 to be used for sleep requests
        // NOTE:  Only pin 9/DIO8/DTR can be used for this function
        gsmModem.sendAT(GF("D8"), XBEE_SLEEP_SETTING);
        success &= gsmModem.waitResponse() == 1;
        // Turn on status indication pin - it will be HIGH when the XBee is
        // awake NOTE:  Only pin 13/ON/SLEEPnot/DIO9 can be used for this
        // function
        gsmModem.sendAT(GF("D9"), XBEE_SLEEP_SETTING);
        success &= gsmModem.waitResponse() == 1;
        if (!success) { MS_DBG(F("Fail D9 "), success); } /**/
        // /#endif //MODEMPHY_USE_SLEEP_PINS_SETTING
        // Turn on CTS pin - it will be LOW when the XBee is ready to receive
        // commands This can be used as proxy for status indication if the true
        // status pin is not accessible NOTE:  Only pin 12/DIO7/CTS can be used
        // for this function
        /*gsmModem.sendAT(GF("D7"),1);
        success &= gsmModem.waitResponse() == 1;
        if (!success) {MS_DBG(F("Fail D7 "),success);}*/
        // Turn on the associate LED (if you're using a board with one)
        // NOTE:  Only pin 15/DIO5 can be used for this function
        // gsmModem.sendAT(GF("D5"),1);
        // success &= gsmModem.waitResponse() == 1;
        // Turn on the RSSI indicator LED (if you're using a board with one)
        // NOTE:  Only pin 6/DIO10/PWM0 can be used for this function
        // gsmModem.sendAT(GF("P0"),1);
        // success &= gsmModem.waitResponse() == 1;
        // Set to TCP mode
        gsmModem.sendAT(GF("IP"), 1);
        success &= gsmModem.waitResponse() == 1;
        if (!success) { MS_DBG(F("Fail IP "), success); }

        // Put the XBee in pin sleep mode in conjuction with D8=1
        MS_DBG(F("Setting Sleep Options..."));
        gsmModem.sendAT(GF("SM"), XBEE_SLEEP_SETTING);
        success &= gsmModem.waitResponse() == 1;
        // Disassociate from network for lowest power deep sleep
        // 40 - Aay associated with AP during sleep - draws more current
        // (+10mA?) 100 -Cyclic sleep ST specifies time before reutnring to
        // sleep 200 - SRGD magic number
        gsmModem.sendAT(GF("SO"), XBEE_SLEEP_ASSOCIATE);
        success &= gsmModem.waitResponse() == 1;

        MS_DBG(F("Setting Wifi Network Options..."));
        // Put the network connection parameters into flash
        success &= gsmModem.networkConnect(_ssid, _pwd);
        // Set the socket timeout to 10s (this is default)
        if (!success) {
            MS_DBG(F("Fail Connect "), success);
            success = true;
        }
        gsmModem.sendAT(GF("TM"), 64);
        success &= gsmModem.waitResponse() == 1;
        //IPAddress newHostIp = IPAddress(0, 0, 0, 0); //default in NV
        gsmModem.sendAT(GF("DL"), GF("0.0.0.0"));
        success &= gsmModem.waitResponse() == 1;


        if (success) {
            MS_DBG(F("Setup Wifi Network "), _ssid);
        } else {
            MS_DBG(F("Failed Setting WiFi"), _ssid);
        }
        // Write changes to flash and apply them
        gsmModem.writeChanges();

        // Scan for AI  last node join request
        uint16_t loops = 0;
        int16_t  ui_db;
        uint8_t  status;
        String   ui_op;
        bool     apRegistered = false;
        PRINTOUT(F("Loop=Sec] rx db : Status #Polled Status every 1sec/30sec"));
        uint8_t reg_count = 0;
        #define TIMER_POLL_AP_STATUS_MSEC 300000
        for (unsigned long start = millis(); millis() - start < 300000;
             loops++) {
            ui_db = 0;  // gsmModem.getSignalQuality();
            gsmModem.sendAT(GF("AI"));
            status = gsmModem.readResponseInt(10000L);
            ui_op  = String(loops) + "=" + String((float)millis() / 1000) +
                "] " + String(ui_db) + ":0x" + String(status, HEX);
            if (0 == status) {
                ui_op += " Cnt=" + String(reg_count);
#define XBEE_SUCCESS_CNTS 3
                if (++reg_count > XBEE_SUCCESS_CNTS) {
                    PRINTOUT(ui_op);
                    apRegistered = true;
                    break;
                }
            } else {
                reg_count =0; //reset 
            }
            PRINTOUT(ui_op);
            //Need to pet the watchDog as 8sec timeout ~ but how, LoggerBase::petDog()
            delay(1000);
        }
        if (apRegistered) 
        { 
            MS_DBG(F("Get IP number"));
            String  xbeeRsp;
            uint8_t index              = 0;
            bool    AllocatedIpSuccess = false;
// Checkfor IP allocation
#define MDM_IP_STR_MIN_LEN 7
#define MDM_LP_IPMAX 16
            for (int mdm_lp = 1; mdm_lp <= MDM_LP_IPMAX; mdm_lp++) {
                delay(mdm_lp * 500);
                gsmModem.sendAT(F("MY"));  // Request IP #
                index = gsmModem.waitResponse(1000, xbeeRsp);
                MS_DBG(F("mdmIP["), mdm_lp, "/", MDM_LP_IPMAX, F("] '"),
                       xbeeRsp, "'=", xbeeRsp.length());
                if (0 != xbeeRsp.compareTo("0.0.0.0") &&
                    (xbeeRsp.length() > MDM_IP_STR_MIN_LEN)) {
                    AllocatedIpSuccess = true;
                    break;
                }
                xbeeRsp = "";
            }
            if (!AllocatedIpSuccess) {
                PRINTOUT(
                    F("XbeeWiFi not received IP# -hope it works next time"));
                // delay(1000);
                // NVIC_SystemReset();
                success = false;
            } else {
                // success &= AllocatedIpSuccess;
                PRINTOUT(F("XbeeWiFi IP# ["), xbeeRsp, F("]"));
                xbeeRsp = "";
                // Display DNS allocation
                bool DnsIpSuccess = false;
#define MDM_LP_DNSMAX 11
                for (int mdm_lp = 1; mdm_lp <= MDM_LP_DNSMAX; mdm_lp++) {
                    delay(mdm_lp * 500);
                    gsmModem.sendAT(F("NS"));  // Request DNS #
                    index &= gsmModem.waitResponse(1000, xbeeRsp);
                    MS_DBG(F("mdmDNS["), mdm_lp, "/", MDM_LP_DNSMAX, F("] '"),
                           xbeeRsp, "'");
                    if (0 != xbeeRsp.compareTo("0.0.0.0") &&
                        (xbeeRsp.length() > MDM_IP_STR_MIN_LEN)) {
                        DnsIpSuccess = true;
                        break;
                    }
                    xbeeRsp = "";
                }

                if (false == DnsIpSuccess) {
                    success = false;
                    PRINTOUT(F(
                        "XbeeWifi init test FAILED - hope it works next time"));
                } else {
                    PRINTOUT(F("XbeeWifi init test PASSED"));
                }
            }
#if 0   // defined MS_DIGIXBEEWIFI_DEBUG
        // as of 0.23.15 the modem as sensor has problems
                int16_t rssi, percent;
                getModemSignalQuality(rssi, percent);
                MS_DBG(F("mdmSQ["),toAscii(rssi),F(","),percent,F("%]"));
#endif  // MS_DIGIXBEEWIFI_DEBUG
            gsmModem.exitCommand();
        } 
        else 
        { // !apRegistered  could be invalid SSID, no SSID, or stuck module
            PRINTOUT(F(
                "XbeeWiFi AP not Registered - reseting module, hope it works "
                "next time"));
            loggerModem::modemHardReset();
            delay(50);
            // NVIC_SystemReset();
            success = false;
        }
    } else {
        success = false;
    }

    if (false == success) { PRINTOUT(F("Xbee '"), _modemName, F("' failed.")); }

    return success;
}


void DigiXBeeWifi::disconnectInternet(void) {
    // Ensure Wifi XBee IP socket torn down by forcing connection to localhost IP
    // For A XBee S6B bug, then force restart
    // Note: TinyGsmClientXbee.h:modemStop() had a hack for closing socket with Timeout=0 "TM0" for S6B disabled

    String oldRemoteIp = gsmClient.remoteIP();
    IPAddress newHostIp = IPAddress(127, 0, 0, 1); //localhost
    gsmClient.connect(newHostIp,80);
    //??gsmClient.modemConnect(newHostpP,80);// doesn't work
    MS_DBG(gsmModem.getBeeName(), oldRemoteIp, F(" disconnectInternet set to "),gsmClient.remoteIP());
    
    gsmModem.restart();
}


// Get the time from NIST via TIME protocol (rfc868)
uint32_t DigiXBeeWifi::getNISTTime(void) {
    // bail if not connected to the internet
    if (!isInternetAvailable()) {
        MS_DBG(F("No internet connection, cannot connect to NIST."));
        return 0;
    }

    gsmClient.stop();

    // Try up to 12 times to get a timestamp from NIST
#if !defined NIST_SERVER_RETRYS
#define NIST_SERVER_RETRYS 4
#endif  // NIST_SERVER_RETRYS
    String  nistIpStr;
    __attribute__((unused)) uint8_t index = 0;
    for (uint8_t i = 0; i < NIST_SERVER_RETRYS; i++) {
        // Must ensure that we do not ping the daylight more than once every 4
        // seconds.  NIST clearly specifies here that this is a requirement for
        // all software that accesses its servers:
        // https://tf.nist.gov/tf-cgi/servers.cgi
        while (millis() < _lastNISTrequest + 4000) {
            // wait
        }

        // Make TCP connection
        MS_DBG(F("\nConnecting to NIST daytime Server"));
        bool connectionMade = false;

        // This is the IP address of time-e-wwv.nist.gov
        // XBee's address lookup falters on time.nist.gov
        // NOTE:  This "connect" only sets up the connection parameters, the TCP
        // socket isn't actually opened until we first send data (the '!' below)
 
       // Uses "TIME" protocol on port 37 NIST: This protocol is expensive, since it
        // uses the complete tcp machinery to transmit only 32 bits of data.
        // FUTURE Users are *strongly* encouraged to upgrade to the network time protocol
        // (NTP), which is both more accurate and more robust.*/
#define TIME_PROTOCOL_PORT 37
#define IP_STR_LEN 18
        const char ipAddr[NIST_SERVER_RETRYS][IP_STR_LEN] = {
            {"132,163, 97, 1"},
            {"132, 163, 97, 2"},
            {"132, 163, 97, 3"},
            {"132, 163, 97, 4"}};
        IPAddress ip1(132, 163, 97, 1);  // Initialize
#if 0
        gsmModem.sendAT(F("time-e-wwv.nist.gov"));
        index = gsmModem.waitResponse(4000, nistIpStr);
        nistIpStr.trim();
        uint16_t nistIp_len = nistIpStr.length();
        if ((nistIp_len < 7) || (nistIp_len > 20)) 
        {
            ip1.fromString(ipAddr[i]);
            MS_DBG(F("Bad lookup"), nistIpStr, "'=", nistIp_len, F(" Using "),
                   ipAddr[i]);
        } else {
            ip1.fromString(nistIpStr);
            PRINTOUT(F("Good lookup mdmIP["), i, "/", NIST_SERVER_RETRYS,
                   F("] '"), nistIpStr, "'=", nistIp_len);
        }
#else
        ip1.fromString(ipAddr[i]);
        PRINTOUT(F("NIST lookup mdmIP["), i, "/", NIST_SERVER_RETRYS,
                   F("] with "), ip1);
//                   F("] with "), ipAddr[i]);                   
#endif 
        connectionMade = gsmClient.connect(ip1, TIME_PROTOCOL_PORT);
        // Need to send something before connection is made
        gsmClient.println('!');

        // Wait up to 5 seconds for a response
        if (connectionMade) {
            uint32_t start = millis();
            while (gsmClient && gsmClient.available() < 4 &&
                   millis() - start < 5000L) {
                // wait
            }

            if (gsmClient.available() >= 4) {
                MS_DBG(F("NIST responded after"), millis() - start, F("ms"));
                byte response[4] = {0};
                gsmClient.read(response, 4);
                gsmClient.stop();
                return parseNISTBytes(response);
            } else {
                MS_DBG(F("NIST Time server did not respond!"));
                gsmClient.stop();
            }
        } else {
            MS_DBG(F("Unable to open TCP to NIST!"));
        }
    }
    return 0;
}


bool DigiXBeeWifi::getModemSignalQuality(int16_t& rssi, int16_t& percent) {
    bool success = true;

    // Initialize float variable
    int16_t signalQual = -9999;
    percent            = -9999;
    rssi               = -9999;

    // NOTE:  using Google doesn't work because there's no reply
    MS_DBG(F("Opening connection to NIST to check connection strength..."));
    // This is the IP address of time-c-g.nist.gov
    // XBee's address lookup falters on time.nist.gov
    // NOTE:  This "connect" only sets up the connection parameters, the TCP
    // socket isn't actually opened until we first send data (the '!' below)
    // IPAddress ip(132, 163, 97, 6);
    // gsmClient.connect(ip, 37);
    // Wait so NIST doesn't refuse us!
    //while (millis() < _lastNISTrequest + 4000) {}
    // Need to send something before connection is made
    //gsmClient.println('!');
    //uint32_t start = millis();
    //delay(100);  // Need this delay!  Can get away with 50, but 100 is safer.
    //while (gsmClient && gsmClient.available() < 4 && millis() - start < 5000L) {
    //}

    // Assume measurement from previous connection
    // Get signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no service/signal.
    // The TinyGSM getSignalQuality function returns the same "no signal"
    // value (99 CSQ or 0 RSSI) in all 3 cases.
    MS_DBG(F("Getting signal quality:"));
    signalQual = gsmModem.getSignalQuality();
    MS_DBG(F("Raw signal quality:"), signalQual);

    if (gsmClient.connected()) { gsmClient.stop(); }

    // Convert signal quality to RSSI
    rssi    = signalQual;
    percent = getPctFromRSSI(signalQual);

    MS_DBG(F("RSSI:"), rssi);
    MS_DBG(F("Percent signal strength:"), percent);

    return success;
}


bool DigiXBeeWifi::updateModemMetadata(void) {
    bool success = true;

    // Unset whatever we had previously
    loggerModem::_priorRSSI           = -9999;
    loggerModem::_priorSignalPercent  = -9999;
    loggerModem::_priorBatteryState   = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorBatteryPercent = -9999;
    loggerModem::_priorModemTemp      = -9999;

    // Initialize variable
    int16_t rssi = -9999;
    // int16_t  percent = -9999;
    uint16_t volt_mV = 9999;

    // Enter command mode only once for temp and battery
    MS_DBG(F("Entering Command Mode to update modem metadata:"));
    success &= gsmModem.commandMode();

    // Assume a signal has already been established.
    // Try to get a valid signal quality
    // NOTE:  We can't actually distinguish between a bad modem response, no
    // modem response, and a real response from the modem of no
    // service/signal. The TinyGSM getSignalQuality function returns the
    // same "no signal" value (99 CSQ or 0 RSSI) in all 3 cases.

    // Try up to 5 times to get a signal quality
    int8_t num_trys_remaining = 5;
    do {
        rssi = gsmModem.getSignalQuality();
        MS_DBG(F("Raw signal quality("), num_trys_remaining, F("):"), rssi);
        if (rssi != 0 && rssi != -9999) break;
        num_trys_remaining--;
    } while ((rssi == 0 || rssi == -9999) && num_trys_remaining);


    // Convert signal quality to a percent
    loggerModem::_priorSignalPercent = getPctFromRSSI(rssi);
    MS_DBG(F("CURRENT Percent signal strength:"),
           loggerModem::_priorSignalPercent);

    loggerModem::_priorRSSI = rssi;
    MS_DBG(F("CURRENT RSSI:"), rssi);


    MS_DBG(F("Getting input voltage:"));
    volt_mV = gsmModem.getBattVoltage();
    MS_DBG(F("CURRENT Modem battery (mV):"), volt_mV);
    if (volt_mV != 9999) {
        loggerModem::_priorBatteryVoltage = static_cast<float>(volt_mV / 1000);
    } else {
        loggerModem::_priorBatteryVoltage = static_cast<float>(-9999);
    }

    MS_DBG(F("Getting chip temperature:"));
    loggerModem::_priorModemTemp = getModemChipTemperature();
    MS_DBG(F("CURRENT Modem temperature(C):"), loggerModem::_priorModemTemp);

    // Exit command mode
    MS_DBG(F("Leaving Command Mode:"));
    gsmModem.exitCommand();

    ++updateModemMetadata_cnt;
    if (0 == rssi || (XBEE_RESET_THRESHOLD <= updateModemMetadata_cnt)) {
        updateModemMetadata_cnt = 0;
        /** Since not giving an rssi value, restart the modem for next time.
         * This is likely to take over 2 seconds         */
        PRINTOUT(F("updateModemMetadata forcing restart xbee..."));
        success &= gsmModem.restart();
    }

    return success;
}


// Az extensions
void DigiXBeeWifi::setWiFiId(const char* newSsid, bool copyId) {
    uint8_t newSsid_sz = strlen(newSsid);
    _ssid              = newSsid;
    if (copyId) {
/* Do size checks, allocate memory for the LoggerID, copy it there
 *  then set assignment.
 */
#define WIFI_SSID_MAX_sz 32
        if (newSsid_sz > WIFI_SSID_MAX_sz) {
            char* WiFiId2 = (char*)newSsid;
            PRINTOUT(F("\n\r   LoggerModem:setWiFiId too long: Trimmed to "),
                     newSsid_sz);
            WiFiId2[newSsid_sz] = 0;  // Trim max size
            newSsid_sz          = WIFI_SSID_MAX_sz;
        }
        if (NULL == _ssid_buf) {
            _ssid_buf = new char[newSsid_sz + 2];  // Allow for trailing 0
        } else {
            PRINTOUT(F("\nLoggerModem::setWiFiId error - expected NULL ptr"));
        }
        if (NULL == _ssid_buf) {
            // Major problem
            PRINTOUT(F("\nLoggerModem::setWiFiId error -no buffer "),
                     _ssid_buf);
        } else {
            strcpy(_ssid_buf, newSsid);
            _ssid = _ssid_buf;
            //_ssid2 =  _ssid_buf;
        }
        MS_DBG(F("\nsetWiFiId cp "), _ssid, " sz: ", newSsid_sz);
    }
}

void DigiXBeeWifi::setWiFiPwd(const char* newPwd, bool copyId) {
    uint8_t newPwd_sz = strlen(newPwd);
    _pwd              = newPwd;

    if (copyId) {
/* Do size checks, allocate memory for the LoggerID, copy it there
 *  then set assignment.
 */
#define WIFI_PWD_MAX_sz 63  // Len 63 printable chars + 0
        if (newPwd_sz > WIFI_PWD_MAX_sz) {
            char* pwd2 = (char*)newPwd;
            PRINTOUT(F("\n\r   LoggerModem:setWiFiPwd too long: Trimmed to "),
                     newPwd_sz);
            pwd2[newPwd_sz] = 0;  // Trim max size
            newPwd_sz       = WIFI_PWD_MAX_sz;
        }
        if (NULL == _pwd_buf) {
            _pwd_buf = new char[newPwd_sz + 2];  // Allow for trailing 0
        } else {
            PRINTOUT(F("\nLoggerModem::setWiFiPwd error - expected NULL ptr"));
        }
        if (NULL == _pwd_buf) {
            // Major problem
            PRINTOUT(F("\nLoggerModem::setWiFiPwd error -no buffer "),
                     _pwd_buf);
        } else {
            strcpy(_pwd_buf, newPwd);
            _pwd = _pwd_buf;
        }
        MS_DEEP_DBG(F("\nsetWiFiPwd cp "), _ssid, " sz: ", newPwd_sz);
    }
}

String DigiXBeeWifi::getWiFiId(void) {
    return _ssid;
}
String DigiXBeeWifi::getWiFiPwd(void) {
    return _pwd;
}
//If needed can provide specific information
//String DigiXBeeWifi::getModemDevId(void) {return "DigiXbeeWiFiId";}


