#include "profile_timer.h"
#include <chrono>
#include <iomanip>

using namespace std;

std::map<std::string, profile_timer::timer_info> profile_timer::map_;
int profile_timer::count_fps_ = 0;
long long profile_timer::time_fps_ = 0;
float profile_timer::fps_ = 0;
std::string profile_timer::tag_last_;

void profile_timer::count_fps()
{
	long long t = get_microsec();
	if(time_fps_ == 0){
		time_fps_ = t;
		return;
	}
	++count_fps_;
	long long diff = t - time_fps_;
	if(diff >= 1000 * 1000){
		time_fps_ = t;
		fps_ = count_fps_ / (diff / 1000000.0);
		count_fps_ = 0;
	}
}

float profile_timer::get_fps()
{
	return fps_;
}

long long profile_timer::get_microsec()
{
	auto t0 = std::chrono::high_resolution_clock::now();        
	return t0.time_since_epoch() / std::chrono::microseconds(1);
}

void profile_timer::start(const char* tag, int size_buf_average)
{
	start(string(tag), size_buf_average);
}


void profile_timer::start(const std::string& tag, int size_buf_average)
{
	tag_last_ = tag;
	if(map_.count(tag) > 0){
		map_[tag].t_last_ = get_microsec();
	}else{
		map_[tag] = timer_info();
		map_[tag].t_last_ = get_microsec();
		map_[tag].size_buf_average_ = size_buf_average;
	}
}

void profile_timer::end(const std::string& tag)
{
	if(map_.count(tag) > 0){
		map_[tag].duration_.push_back(get_microsec() - map_[tag].t_last_);
		if(map_[tag].duration_.size() > map_[tag].size_buf_average_){
			map_[tag].duration_.pop_front();
		}
	}
}

void profile_timer::end()
{
	if(tag_last_.size() == 0) return;
	end(tag_last_);
}

double profile_timer::get_average(const std::string& tag)
{
	if(map_.count(tag) > 0){
		int s = map_[tag].duration_.size();
		if(s == 0) return 0;
		long long sum = 0;
		auto ite = map_[tag].duration_.begin();
		for(int i=0;i<s;++i){
			sum += *ite;
			++ite;
		}
		return (double)sum / s;
	}else{
		return 0;
	}
}

void profile_timer::dump(ostream& out)
{
	//clear is too slow.
	//out << std::system("clear") << endl;
	out << "fps: " << get_fps() << endl;
	for(auto ite = map_.begin();ite != map_.end();++ite){
		out << std::setw(16) << std::left << ite->first << ": " << 
			std::setw(12) << std::right << (int)get_average(ite->first) << endl;
	}
}
