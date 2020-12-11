#include "stdafx.h"
#include "tokenreader.h"


const Text TokenReader::TOKEN_NEEDLE = "`\':<>,()!~%^&*-=+|?/[]{}: ";

TokenReader::TokenReader(Text text) noexcept
	:m_text(text)
{
	m_nextToken = text.begin();
	_read();
}

Text TokenReader::peek() noexcept
{
	return Text(m_text.data(), m_nextToken);
}

Text TokenReader::read() noexcept
{
	Text out = peek();
	_read();
	return out;
}

bool TokenReader::eof(const char* eof) noexcept
{
	char chr = *m_text.data();
	while (*eof != '\0')
	{
		if (chr == *eof) return true;
		eof++;
	}
	return chr == '\0';
}

Text TokenReader::remaining() noexcept
{
	return m_text;
}

void TokenReader::moveTo(const char* ptr) noexcept
{
	m_text.setBegin(ptr);
	m_nextToken = ptr;
	_read();
}

bool TokenReader::readIf(Text token) noexcept
{
	if (peek() == token)
	{
		_read();
		return true;
	}
	else
	{
		return false;
	}
}

void TokenReader::must(Text token) noexcept
{
	_assert(readIf(token));
}

bool TokenReader::readIfSentence(Text sentence) noexcept
{
	if (m_text.startsWith(sentence))
	{
		m_nextToken = m_text.data() + sentence.size();
		_read();
		return true;
	}
	return false;
}
