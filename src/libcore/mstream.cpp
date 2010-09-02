/*
    This file is part of Mitsuba, a physically based rendering system.

    Copyright (c) 2007-2010 by Wenzel Jakob and others.

    Mitsuba is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License Version 3
    as published by the Free Software Foundation.

    Mitsuba is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <mitsuba/core/mstream.h>

MTS_NAMESPACE_BEGIN

MemoryStream::MemoryStream(size_t initialSize)
 : Stream(), m_capacity(0), m_size(0), m_pos(0), m_data(NULL) {
	resize(initialSize);
}

MemoryStream::~MemoryStream() {
	if (m_data != NULL)
		free(m_data);
}

void MemoryStream::reset() {
	m_size = m_pos = 0;
}

void MemoryStream::resize(size_t size) {
	if (m_data == NULL)
		m_data = (uint8_t *) malloc(size);
	else
		m_data = (uint8_t *) realloc(m_data, size);

	if (size > m_capacity)
		memset(m_data + m_capacity, 0, size - m_capacity);

	m_capacity = size;
}

void MemoryStream::flush() {
	/* Ignore */
}

size_t MemoryStream::getPos() const {
	return m_pos;
}

size_t MemoryStream::getSize() const {
	return m_size;
}

void MemoryStream::setPos(size_t pPos) {
	m_pos = pPos;
	if (m_pos >= m_size) {
		m_size = pPos;
		if (m_size > m_capacity)
			resize(m_size);
	}
}

void MemoryStream::truncate(size_t size) {
	m_size = size;
	resize(size);
	if (m_pos > size)
		m_pos = size;
}

void MemoryStream::read(void *ptr, size_t size) {
	if (m_pos + size > m_size) 
		Log(EError, "Reading over the end of a memory stream!");
	memcpy(ptr, m_data + m_pos, size);
	m_pos += size;
}


void MemoryStream::write(const void *ptr, size_t size) {
	size_t endPos = m_pos + size;
//	for (size_t i=0; i<size; ++i)
//		printf("0x%x ", ((const char *) ptr)[i]);
//	printf("\n");
	if (endPos > m_size) {
		if (endPos > m_capacity) 
			resize(endPos);
		m_size = endPos;
	}
	memcpy(m_data + m_pos, ptr, size);
	m_pos += size;
}

bool MemoryStream::canRead() const {
	return true;
}

bool MemoryStream::canWrite() const {
	return true;
}

std::string MemoryStream::toString() const {
	std::ostringstream oss;
	oss << "MemoryStream[" << Stream::toString()
		<< ", size=" << m_size << ", pos=" << m_pos
		<< ", capacity=" << m_capacity << "]";
	return oss.str();
}

MTS_IMPLEMENT_CLASS(MemoryStream, false, Stream)
MTS_NAMESPACE_END
