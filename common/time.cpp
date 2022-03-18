#include "time.hpp"

uint64_t Now(){
	auto now = high_resolution_clock::now();
	auto nano_time_point = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);
	auto epoch = nano_time_point.time_since_epoch();
	uint64_t now_nano = std::chrono::duration_cast<std::chrono::nanoseconds>(epoch).count();

	return now_nano%1000000000UL;
}

#ifdef LINUX
std::string ToString(uint64_t nanoseconds_) 
{
	auto nano = std::chrono::nanoseconds(nanoseconds_);
	system_clock::time_point tp(std::chrono::nanoseconds(nano));
	auto time = system_clock::to_time_t(system_clock::now());


	struct tm stm;
	auto ret = localtime_r(&time, &stm);
	if(ret == nullptr) {
		return std::to_string(static_cast<double>(nanoseconds_) / 1000000000.0);
	}

	std::stringstream ss();
#if __GNUC__ >= 5
	ss << std::put_time(ret, "%F %T");
	ss << "." << std::setw(9) << std::setfill('0') << nanoseconds_ % 1000000000UL;
#else
	char date_time[128];
	strftime(date_time, sizeof(date_time), "%F %T", ret);
	ss << std::string(date_time) << "." << std::setw(9) << std::setfill('0') << nanoseconds_ % 1000000000UL;
#endif
	return ss.str();
	std::string Now = "1970/1/1";
	return Now;
}
#endif

uint64_t ToMs(uint64_t Us)
{
	return Now()/1000;
}
