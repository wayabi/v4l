#ifndef __U_PROFILE_TIMER__
#define __U_PROFILE_TIMER__

#include <string>
#include <map>
#include <deque>
#include <utility>
#include <ostream>

class profile_timer {
private:
	class timer_info {
	public:
		long long t_last_;
		int size_buf_average_;
		std::deque<long long> duration_;
	};

public:
	static void count_fps();
	static float get_fps();
	static long long get_microsec();
	static void start(const char* tag, int size_buf_average);
	static void start(const std::string& tag, int size_buf_average);
	static void end(const std::string& tag);
	static void end();
	static double get_average(const std::string& tag);
	static void dump(std::ostream& out);

private:
	static std::map<std::string, timer_info> map_;
	static int count_fps_;
	static long long time_fps_;
	static float fps_;
	static std::string tag_last_;
};

#endif
