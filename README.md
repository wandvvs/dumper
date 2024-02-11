# IP and Hardware info dumper

## Description
Contains two structures with characteristics and information on IP address, also contains one common one, which is shown in the example. The overall structure contains two x's.

## Tip:
If you want to parse a specific IP address and not a local one. (By default, the local IP is parsed)
Then comment on the string in general_dumper.hxx and add the IP address to the get_ip_struct function with the argument.

```cpp
#define IS_PARSE_LOCAL_IP

#ifdef IS_PARSE_LOCAL_IP	
	ip_header get_ip_struct()
#else
	ip_header get_ip_struct(std::string ip)
#endif
	{
		ip_header ip_info;
#ifndef IS_PARSE_LOCAL_IP
		std::string IP_URL = "http://ip-api.com/json/" + ip + "?fields=17039359";
#else
#endif
```

## Dependencies:
- libcurl
- nlohmann/json
- winapi

## Usage:
```cpp
#include "general_dumper.hxx"

int main()
{
	struct general_dumper::dump info;

	std::cout << info.hardware_dump.gpu << std::endl;
	std::cout << info.ip_dump.city << std::endl;

	info.hardware_dump.print_fields();
	info.ip_dump.print_fields();
}
```

## Output:
```
NVIDIA GeForce RTX 3050 Ti Laptop GPU
Norwell
========================== HARDWARE INFORMATION ===========================

HWID: {9401f61f-b2f1-11ee-9915-806e6f6e6963}
CPU: 11th Gen Intel(R) Core(TM) i5-11400H @ 2.70GHz
GPU: NVIDIA GeForce RTX 3050 Ti Laptop GPU
Date: 11-02-2024 18:25:04
Computer Name: KOMPUTER
User Name: wand
Clipboard Text: struct dump
{
===========================================================================

========================== IP INFORMATION =================================

IP: 93.184.216.34
Status: success
Country: United States
Country Code: US
Region: MA
Region Name: Massachusetts
City: Norwell
Time Zone: America/New_York
ISP: Edgecast Inc.
==========================================================================
```
