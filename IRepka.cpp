#include "IRepka.h"
// SET
std::string IRepka::set(const std::string& key, const std::string& val)
{
	std::lock_guard<std::mutex> lock(repka_mutex);
	if (!is_set(key)) {
		auto it = this->rmap.emplace(key, val);
		if (it.second)
			return "Success!";
		else
			return "Error!";
	}
	else {
		std::string result = get(key);
		this->rmap.erase(key);
		auto it = this->rmap.emplace(key, val);
		if (it.second)
			return "Success! " + result;
		else
			return "Error!";
	}
}
// GET
std::string IRepka::get(const std::string& key)
{
	auto it = this->rmap.find(key);
	if (it != this->rmap.end())
		return it->second;
	else
		return "Doesn't exist";

}
// DEL
std::string IRepka::del(const std::string& key)
{
	std::lock_guard<std::mutex> lock(repka_mutex);
	auto it = this->rmap.find(key);
	if (it != this->rmap.end()) {
		this->rmap.erase(key);
		return "Succsess";
	}
	else
		return "Doesn't exist";

}
// COUNT
std::string IRepka::count()
{
	return std::to_string(this->rmap.size());
}

// IS SET
bool IRepka::is_set(const std::string& key)
{
	auto it = this->rmap.find(key);
	return it != this->rmap.end();
}



// CREATE_DUMP
std::string IRepka::save_dump(const std::string& filename)
{
	std::string ERROR_MSG("Failed write attempt <" + filename + ">");
	std::lock_guard<std::mutex> lock(repka_mutex);
	std::ofstream outputFile(filename, std::ios::binary | std::ios::out); // open .bin file to save
	// err
	if (!outputFile) {
		return ERROR_MSG;
	}

	// write the size() to the file
	const std::size_t mapSize = this->rmap.size();
	if (!outputFile.write(reinterpret_cast<const char*>(&mapSize), sizeof(mapSize))) {
		return ERROR_MSG;
	}

	// get separated key and value
	for (auto& pair : this->rmap)
	{
		const std::string& key = pair.first;
		const std::string& value = pair.second;

		// write Size of key and value to the file first
		const std::size_t keySize = key.size();
		const std::size_t valueSize = value.size();
		if (!outputFile.write(reinterpret_cast<const char*>(&keySize), sizeof(keySize))) {
			return ERROR_MSG;
		}
		if (!outputFile.write(reinterpret_cast<const char*>(&valueSize), sizeof(valueSize))) {
			return ERROR_MSG;
		}

		// write the key and value to the file
		if (!outputFile.write(key.c_str(), key.size())) {
			return ERROR_MSG;
		}
		if (!outputFile.write(value.c_str(), value.size())) {
			return ERROR_MSG;
		}
	}
	outputFile.close();
	return "Dump has been succsessfuly created";
}

// LOAD DUMP
std::string IRepka::load_dump(const std::string& filename)
{
	std::lock_guard<std::mutex> lock(repka_mutex);
	this->rmap.clear();
	std::string ERROR_MSG("Failed. The file <"+filename+"> corrupted or doesn't exist");
	std::ifstream inputFile(filename, std::ios::binary); // open the file

	if (!inputFile){
		return ERROR_MSG;
	}

	// read the Size of map from the file
	std::size_t mapSize = 0;
	if (!inputFile.read(reinterpret_cast<char*>(&mapSize), sizeof(mapSize))) {
		return ERROR_MSG;
	}

	// read each pair from file to the cleared map;
	for (size_t i = 0; i < mapSize; i++)
	{
		// read Size of key and value from the file;
		std::size_t keySize = 0, valueSize = 0;
		if (!inputFile.read(reinterpret_cast<char*>(&keySize), sizeof(keySize))) {
			return ERROR_MSG;
		}
		if (!inputFile.read(reinterpret_cast<char*>(&valueSize), sizeof(valueSize))) {
			return ERROR_MSG;
		}

		// read the key and the value from file
		std::string key(keySize, '\0');
		std::string value(valueSize, '\0');
		if (!inputFile.read(&key[0], keySize)) {
			return ERROR_MSG;
		}
		if (!inputFile.read(&value[0], valueSize)) {
			return ERROR_MSG;
		}

		this->rmap.emplace(key, value);
	}
	inputFile.close();
	return "Dump has been succsessfully load";
}
