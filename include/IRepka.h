#pragma once
#include <string>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <mutex>
#ifndef I_REPKA_H
#define I_REPKA_H

class IRepka
{
public:
	// main interface
	std::string virtual set(const std::string& key, const std::string& val);
	std::string virtual get(const std::string& key);
	std::string virtual del(const std::string& key);
	std::string virtual count();
	bool virtual is_set(const std::string& key);
	// dump
	std::string virtual save_dump(const std::string& filename);
	std::string virtual load_dump(const std::string& filename);
private:
	std::unordered_map<std::string, std::string> rmap;
	std::mutex repka_mutex;
};


#endif