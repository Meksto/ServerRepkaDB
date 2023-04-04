#pragma once
#ifndef COMM_RESPONSE_H
#define COMM_RESPONSE_H
#include <sstream>
#include <algorithm>
#include "IRepka.h"
std::string command_response(IRepka& repka, char buf[]);

std::string set_handler(IRepka& repka, const std::string& key, const std::string& value);
std::string get_handler(IRepka& repka, const std::string& key);
std::string del_handler(IRepka& repka, const std::string& key);
std::string count_handler(IRepka& repka);
std::string save_dump_handler(IRepka& repka, const std::string& file_name);
std::string load_dump_handler(IRepka& repka, const std::string& file_name);

enum class CommandType {
    SET,
    GET,
    DEL,
    COUNT,
    SAVE_DUMP,
    LOAD_DUMP,
    INCORRECT_INPUT
};

CommandType parse_command(const std::string& cmd, const std::string& key, const std::string& value);


std::string command_response(IRepka& repka, char buf[]) {
    std::string command(buf);
    std::stringstream ss(command);
    std::string cmd, key, value;
    ss >> cmd >> key >> value;

    CommandType type = parse_command(cmd,key,value);

    switch (type) {
        case CommandType::SET:
            return set_handler(repka, key, value);
        case CommandType::GET:
            return get_handler(repka, key);
        case CommandType::DEL:
            return del_handler(repka, key);
        case CommandType::COUNT:
            return count_handler(repka);
        case CommandType::SAVE_DUMP:
            return save_dump_handler(repka, key);
        case CommandType::LOAD_DUMP:
            return load_dump_handler(repka, key);
        case CommandType::INCORRECT_INPUT:
        default:
            return "Incorrect input.\nAvailiable command: set <key> <value> | get <key> | del <key> | count \nDump command: save_dump <filename> | load_dump <filename> \nAvailiable symbols: \"0-9\" \"a-z\" \"A-Z\"";
    }
}

std::string set_handler(IRepka& repka, const std::string& key, const std::string& value) {
    return repka.set(key, value);
}

std::string get_handler(IRepka& repka, const std::string& key) {
    return repka.get(key);
}

std::string del_handler(IRepka& repka, const std::string& key) {
    return repka.del(key);
}

std::string count_handler(IRepka& repka) {
    return repka.count();
}

std::string save_dump_handler(IRepka& repka, const std::string& file_name) {
    return repka.save_dump(file_name+".bin");
}

std::string load_dump_handler(IRepka& repka, const std::string& file_name) {
    return repka.load_dump(file_name+".bin");
}

bool is_alphanumeric(const char& c) {
    return std::isalnum(static_cast<unsigned char>(c)) != 0;
}

CommandType parse_command(const std::string& cmd, const std::string& key, const std::string& value) {
    if (!std::all_of(key.begin(),key.end(),is_alphanumeric))
    {
        return CommandType::INCORRECT_INPUT; //to do: new INCORR_INPUT for only key
    }
    if (cmd == "set" && !key.empty() && !value.empty()) {
        return CommandType::SET;
    }
    else if (cmd == "get" && !key.empty() && value.empty()) {
        return CommandType::GET;
    }
    else if (cmd == "del" && !key.empty() && value.empty()) {
        return CommandType::DEL;
    }
    else if (cmd == "count") {
        return CommandType::COUNT;
    }
    else if (cmd == "save_dump" && !key.empty() && value.empty()) {
        return CommandType::SAVE_DUMP;
    }
    else if (cmd == "load_dump" && !key.empty() && value.empty()) {
        return CommandType::LOAD_DUMP;
    }
    else {
        return CommandType::INCORRECT_INPUT;
    }
}
#endif