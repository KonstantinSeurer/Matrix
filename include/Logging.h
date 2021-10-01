/*
 * Logging.h
 *
 *  Created on: Jun 5, 2021
 *      Author: konstantin
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include "Base.h"

namespace matrix {

struct SourceLocation {
	String file;
	u32 line;

	SourceLocation(const String& file, u32 line): file(file), line(line) {
	}

	String toString() const {
		return file + ": " + matrix::toString(line);
	}
};

#define SOURCE_LOCATION() matrix::SourceLocation(__FILE__, __LINE__)

void out(const String& text);
void err(const String& text);

}

#endif /* LOGGING_H_ */
