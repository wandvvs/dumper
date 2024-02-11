#pragma once
#define CURL_STATICLIB
#define IS_PARSE_LOCAL_IP

#include <curl\curl.h>
#include <nlohmann/json.hpp>

#include <Windows.h>

#include <iostream>
#include <string>

std::string IP_URL = "http://ip-api.com/json/";

using json = nlohmann::json;

size_t write_callback(void* contents, size_t size, size_t nmemb, std::string* output)
{
	size_t total_size = size * nmemb;
	output->append((char*)contents, total_size);
	return total_size;
}

namespace ip_dumper
{
	struct ip_header
	{
		std::string ip;
		std::string status;
		std::string country;
		std::string country_code;
		std::string region;
		std::string region_name;
		std::string city;
		std::string time_zone;
		std::string isp;

		void print_fields() const
		{
			std::cout << "========================== IP INFORMATION =================================\n" << std::endl;

			std::cout << "IP: " << ip << std::endl;
			std::cout << "Status: " << status << std::endl;
			std::cout << "Country: " << country << std::endl;
			std::cout << "Country Code: " << country_code << std::endl;
			std::cout << "Region: " << region << std::endl;
			std::cout << "Region Name: " << region_name << std::endl;
			std::cout << "City: " << city << std::endl;
			std::cout << "Time Zone: " << time_zone << std::endl;
			std::cout << "ISP: " << isp << std::endl;

			std::cout << "==========================================================================\n" << std::endl;
		}
	};
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

		CURL* curl;
		CURLcode res;
		std::string json_data;

		curl_global_init(CURL_GLOBAL_DEFAULT);
		curl = curl_easy_init();

		if (curl)
		{

			curl_easy_setopt(curl, CURLOPT_URL, IP_URL);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &json_data);

			res = curl_easy_perform(curl);

			if (res != CURLE_OK)
			{
				std::cerr << "Failed to send post request to " << IP_URL << " :" << curl_easy_strerror(res) << std::endl;
			}
			else
			{
				json parsed_json = json::parse(json_data);
				
				ip_info.ip =		   parsed_json["query"];
				ip_info.city =		   parsed_json["city"];
				ip_info.country =	   parsed_json["country"];
				ip_info.country_code = parsed_json["countryCode"];
				ip_info.region =	   parsed_json["region"];
				ip_info.region_name =  parsed_json["regionName"];
				ip_info.time_zone =	   parsed_json["timezone"];
				ip_info.status =	   parsed_json["status"];
				ip_info.region_name =  parsed_json["regionName"];
				ip_info.isp =		   parsed_json["isp"];
			}

			curl_easy_cleanup(curl);
		}
		else
		{
			std::cout << "Failed to init libcurl: " << GetLastError() << std::endl;
			return {};
		}

		return ip_info;
	}
}
