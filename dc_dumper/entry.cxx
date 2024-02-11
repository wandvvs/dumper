#include "general_dumper.hxx"

int main()
{
	struct general_dumper::dump info;

	std::cout << info.hardware_dump.gpu << std::endl;
	std::cout << info.ip_dump.city << std::endl;

	info.hardware_dump.print_fields();
	info.ip_dump.print_fields();
}

