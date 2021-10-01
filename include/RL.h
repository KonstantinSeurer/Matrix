/*
 * RL.h
 *
 *  Created on: Jun 17, 2021
 *      Author: konstantin
 */

#ifndef IO_RL_H_
#define IO_RL_H_

#include "Base.h"

namespace matrix {

class RL {
private:
	String protocol;
	String location;
	String path;
public:
	RL(const String &protocol, const String &location, const String &path);

	RL(const String &rl);

	RL() :
			protocol(""), location(""), path("") {
	}

	const String& getProtocol() const {
		return protocol;
	}

	const String& getLocation() const {
		return location;
	}

	const String& getPath() const {
		return path;
	}

	void read(Array<u8> &target) const;

	String readText() const;

	RL uri(const String &uri) const;
};

}

#endif /* IO_RL_H_ */
