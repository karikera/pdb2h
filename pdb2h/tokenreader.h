#pragma once

enum class TypeOfToken
{
	Identifier,
	Primitive,
	LeftSideModifier,
};

class TokenReader
{
private:
	Text m_text;
	const char* m_nextToken;

	void _read() noexcept
	{
		m_text.setBegin(m_nextToken);
		for (;;)
		{
			m_nextToken = m_text.find_ye(TOKEN_NEEDLE);
			if (m_nextToken != m_text.begin()) return;
			if (m_nextToken == m_text.end()) return;
			m_nextToken++;
			if (m_nextToken == m_text.end()) return;

			char chr = *m_text.begin();
			switch (chr)
			{
			case ' ':
				m_text.addBegin(1);
				continue;
			case '>':
			case '&':
			case '|':
			case '-':
			case '+':
			case ':':
			case '=':
				if (chr == *m_nextToken)
				{
					m_nextToken++;
				}
				break;
			}
			return;
		}
	}

public:
	static const Text TOKEN_NEEDLE;
	static const Map<Text, TypeOfToken> PRIMITIVES;
	static const Set<Text> MODIFIERS;

	TokenReader(Text text) noexcept;

	Text peek() noexcept;
	Text read() noexcept;
	bool eof(const char* eof) noexcept;

	Text remaining() noexcept;
	void moveTo(const char* ptr) noexcept;
	bool readIf(Text token) noexcept;
	void must(Text token) noexcept;
	bool readIfSentence(Text sentence) noexcept;
};
