// Microcontroller Server Computer 0.1
// Copyright (C) 2019 Ercan Ersoy (http://ercanersoy.net)
// This file is licensed by MIT License.

// Including header files
#include <Ethernet.h>
#include <SD.h>
#include <SPI.h>
#include <WebServer.h>

// Defining MAC address
uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
// Defining HTTP Server
WebServer http_server("", 80);

// Request file function
void request_file(WebServer &server, WebServer::ConnectionType type, char **url_path, char *url_tail, bool url_complete)
{
  // If request is GET
  if (type == WebServer::GET)
  {
    // Requesting file
    show_file(server, url_path);
  }
}

// Show file function
void show_file(WebServer &server, char **path)
{
  // Defining counter
  int counter = 0;
  // Defining path buffer
  String path_buffer = "";
  // Defining File
  File f;
  // Defining character buffer
  int character_buffer = 0;

  // Initializing path buffer
  path_buffer += String(path[counter]);

  // Sending HTTP success
  server.httpSuccess();

  // File or directory opening
  while((f = SD.open(path_buffer, FILE_READ)).isDirectory())
  {
    // Increasing counter
    counter++;

    // Add file or directory to path buffer
    path_buffer += '/';
    path_buffer += String(path[counter]);
  }

  // If file not found
  if(!f)
  {
    // Send error message
    server.write("File not found.");
    // Exit function
    return;
  }

  // File reading
  while((character_buffer = f.read()) != -1)
  {
    // Sending characters
    server.write(character_buffer);
  }

  // Fiel closing
  f.close();
}

// Index function
void index(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool url_complete)
{
  // Defining index file
  char *index_file[] = { "INDEX.HTM" };

  // Request index file
  request_file(server, WebServer::GET, index_file, NULL, NULL);
}

// Setup function
void setup()
{
  // Initalizing UART
  Serial.begin(9600);

  // Initalizing Ethernet
  Ethernet.begin(mac);
  // Write local IP to UART
  Serial.println(Ethernet.localIP());

  // If SD card not open
  if (!SD.begin(4))
  {
    // Send error message
    Serial.println(F("SD card not open."));
  }

  // Initalizing HTTP server
  http_server.begin();
  // Set default command of HTTP server
  http_server.setDefaultCommand(&index);
  // Set url path command of HTTP server
  http_server.setUrlPathCommand(&request_file);
}

// Loop function
void loop()
{
  // Processing connection of HTTP server
  http_server.processConnection();
}
