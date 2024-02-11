#pragma once

#include "ip_dumper.hxx"
#include "hardware_dumper.hxx"

namespace general_dumper
{
	struct dump
	{
		struct ip_dumper::ip_header ip_dump;
		struct hardware_dumper::hardware_dump hardware_dump;

		dump() : ip_dump(ip_dumper::get_ip_struct()), hardware_dump(hardware_dumper::get_hardware_struct()) {}
	};
}
