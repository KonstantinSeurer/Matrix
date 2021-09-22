/*
 * RL.cpp
 *
 *  Created on: Jun 17, 2021
 *      Author: konstantin
 */

#include "RL.h"
#include <stdio.h>
#include "Logging.h"

namespace matrix {

RL::RL(const String &protocol, const String &location, const String &path) :
		protocol(protocol), location(location), path(path) {
}

RL::RL(const String &rl) {
	u32 protocolSplit = rl.find("://");
	protocol = rl.substr(0, protocolSplit);
	u32 locationSplit = rl.find("/", protocolSplit + 3);
	location = rl.substr(protocolSplit + 3, locationSplit - protocolSplit - 3);
	path = rl.substr(locationSplit + 1, rl.length() - locationSplit - 1);
}

void RL::read(Array<u8> &target) const {
	if (protocol == "file") {
		FILE* file = fopen(path.c_str(), "r");
		if (!file) {
			err("Could not open file '" + path + "'!");
			return;
		}

		fseek(file, 0L, SEEK_END);
		s64 size = ftell(file);
		rewind(file);

		target.resize(size);
		fread(target.data(), size, 1, file);

		fclose(file);
	}
}

String RL::readText() const {
	Array<u8> data;
	read(data);
	return String((const char*) data.data(), data.size());
}

RL RL::uri(const String& uri) const {
	RL result;
	result.protocol = protocol;
	result.location = location;
	result.path = path.substr(0, path.find_last_of('/') + 1) + uri;
	return result;
}

}
