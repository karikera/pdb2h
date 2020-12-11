#include "stdafx.h"
#include "declaration.h"
#include "tokenreader.h"

const Map<Text, TypeOfToken> tokenTypes = {
	{"unsigned", TypeOfToken::Primitive},
	{"int", TypeOfToken::Primitive},
	{"__int64", TypeOfToken::Primitive},
	{"char", TypeOfToken::Primitive},
	{"short", TypeOfToken::Primitive},
	{"long", TypeOfToken::Primitive},
	{"float", TypeOfToken::Primitive},
	{"double", TypeOfToken::Primitive},

	{"const", TypeOfToken::LeftSideModifier},
	{"__cdecl", TypeOfToken::LeftSideModifier},
	{"__stdcall", TypeOfToken::LeftSideModifier},
	{"__fastcall", TypeOfToken::LeftSideModifier},
	{"__vectorcall", TypeOfToken::LeftSideModifier},
	{"*", TypeOfToken::LeftSideModifier},
	{"&", TypeOfToken::LeftSideModifier},
	{"&&", TypeOfToken::LeftSideModifier},
};

UBool operator &(UBool a, UBool b) noexcept
{
	if (a == b) return a;
	if (a == UBool::Unknown) return b;
	if (b == UBool::Unknown) return a;
	unreachable();
}
ClassType operator &(ClassType a, ClassType b) noexcept
{
	if (a == b) return a;
	if (a == ClassType::Unknown) return b;
	if (b == ClassType::Unknown) return a;
	unreachable();
}
CtorType operator &(CtorType a, CtorType b) noexcept
{
	if (a == b) return a;
	if (a == CtorType::Unknown) return b;
	if (b == CtorType::Unknown) return a;
	unreachable();
}
const DeclarationType operator &(const DeclarationType& a, const DeclarationType& b) noexcept
{
	return {
		a.isValue & b.isValue,
		a.isType & b.isType,
		a.classType & b.classType,
		a.nestiable & b.nestiable,
		a.isTemplate & b.isTemplate,
		a.isTemplateSpecialized & b.isTemplateSpecialized,
		a.ctor & b.ctor,
	};
}

uint32_t Declaration::s_idcounter = 0;
Map<Text, Declaration*, true> Declaration::s_map;
Declaration Declaration::root(DType::Namespace);

bool verifyIdentifier(Text name) noexcept
{
	if (name.empty()) return false;
	{
		char chr = name.read();
		if (!(('a' <= chr && chr <= 'z') ||
			('A' <= chr && chr <= 'Z') ||
			chr == '_' || chr == '$')) return false;
	}
	for (char chr : name)
	{
		if (!(('a' <= chr && chr <= 'z') ||
			('A' <= chr && chr <= 'Z') ||
			chr == '_' || chr == '$' || chr == '#' ||
			('0' <= chr && chr <= '9'))) return false;
	}
	return true;
}
bool isSentence(Text text) noexcept
{
	bool space = false;
	for (char chr : text)
	{
		if (chr == ' ')
		{
			space = true;
		}
		else if ('a' <= chr && chr <= 'z')
		{
		}
		else if ('A' <= chr && chr <= 'Z')
		{
		}
		else
		{
			return false;
		}
	}
	return space;
}


Declaration::Declaration(AText name, AText baseName, DeclarationType type) noexcept
	:m_name(move(name)), m_baseName(baseName), m_type(type)
{
	m_id = s_idcounter++;
	m_parent = nullptr;
}
Declaration::Declaration(DeclarationType type) noexcept
	:m_type(type)
{
	m_id = s_idcounter++;
	m_parent = nullptr;
	s_map.insert(m_absPath, this);
}
Text Declaration::name() noexcept
{
	return m_name;
}
Text Declaration::baseName() noexcept
{
	return m_baseName;
}
void Declaration::checkName() noexcept
{
	_assert(!m_name.empty());
	if (m_name.startsWith('-'))
	{
		_assert(this == &root);
		_assert(m_name.subarr(1).numberonly());
		intersectType(DType::Constant);
	}
	else if (m_name.numberonly())
	{
		if (this == &root)
		{
			intersectType(DType::Constant);
		}
		else
		{
			intersectType(DType::Function);
		}
	}
	else if (m_name.startsWith("operator") && "!~%^&*(-=+|<>?/[ "_tx.contains(m_name[8]))
	{
		intersectType(DType::Function);
	}
	else
	{
		if (m_name.startsWith('<') && m_name.endsWith('>'))
		{
			_assert(verifyIdentifier((m_name + 1).cut(m_name.end() - 1)));
		}
		else if (m_name.startsWith('~'))
		{
			_assert(verifyIdentifier(m_name + 1));
		}
		else
		{
			_assert(verifyIdentifier(m_name));
		}
	}
}
void Declaration::testTemplateMatch(TokenReader* reader) noexcept
{
	TText tconcat = TText::concat(m_baseName, "#S");
	intersectType(DType::TemplateSpecialized);
	if (!reader->readIf(">"))
	{
		for (;;)
		{
			Declaration* decl = root.parse(reader, DType::TemplateParameter, ",>");
			tconcat << '#';
			tconcat << hexf(decl->m_id);
			if (!reader->readIf(","))
			{
				reader->must(">");
				break;
			}
		}
	}
	_assert(tconcat == m_name);
}
Declaration* Declaration::templateSpecialize(View<Declaration*> params) noexcept
{
	TText templname;
	templname << m_baseName;
	templname << "#S";
	for (Declaration* decl : params)
	{
		templname << '#';
		templname << hexf(decl->m_id);
	}
	Declaration* specialized = m_parent->get(templname, m_baseName, DType::TemplateSpecialized, true);
	specialized->m_templateParameters = move(m_templateParameters);
	m_specialized.push(specialized);
	return specialized;
}
Declaration* Declaration::parseTemplate(TokenReader* reader) noexcept
{
	Array<Declaration*> params;
	intersectType(DType::Template);
	if (!reader->readIf(">"))
	{
		for (;;)
		{
			params.push(root.parse(reader, DType::TemplateParameter, ",>"));
			if (!reader->readIf(","))
			{
				reader->must(">");
				break;
			}
		}
	}
	return templateSpecialize(params);
}
void Declaration::intersectType(DeclarationType type) noexcept
{
	m_type = m_type & type;
}
Declaration* Declaration::get(Text name, Text baseName, DeclarationType type, bool specialName) noexcept
{
	AText abspath = AText::concat(m_absPath, '/', name);
	auto res = s_map.insert(abspath, nullptr);
	if (!res.second)
	{
		Declaration* decl = res.first->second;
		decl->intersectType(type);
		_assert(decl->m_parent == this);
		return decl;
	}
	Declaration* decl = _new Declaration(name, baseName, type);
	if (!specialName) decl->checkName();
	decl->m_absPath = move(abspath);
	decl->m_parent = this;
	res.first->second = decl;
	return decl;
}
Declaration* Declaration::get(Text name, DeclarationType type, bool specialName) noexcept
{
	return get(name, name, type, specialName);
}

Declaration* Declaration::parse(TokenReader* reader, DeclarationType type, const char* eof) noexcept
{
	Text from = reader->remaining();
	Declaration* decl = this;

	bool isEnum = reader->readIf("enum");
	bool isFunctionPointer = reader->readIf("&");

	for (;;)
	{
		if (reader->eof(eof))
		{
			decl->intersectType(type);
			return decl;
		}
		if (reader->readIf("`"))
		{
			Text remaining = reader->remaining();
			const char* next = remaining.find('\'');
			Text sentence = remaining.cut(next);
			if (isSentence(sentence))
			{
				sentence.addBegin(-1);
				sentence.addEnd(1);
				reader->moveTo(sentence.end());

				if (sentence == "`anonymous namespace`")
				{
					decl = decl->get(sentence, sentence, DType::Namespace, true);
				}
				else
				{
					if (sentence.endsWith(" for '"))
					{
						decl = decl->parse(reader, DType::Variable, "'");
						reader->must("'");
						reader->must("'");
					}
					decl = decl->get(sentence, sentence, DType::Function, true);
					decl->intersectType(DType::Function);
				}
			}
			else
			{
				decl = decl->parse(reader, DType::FunctionLike, "\'");
				reader->must("\'");
			}
		}
		else
		{
			DeclarationType identifierType = DType::Unknown;

			Text token = reader->read();
			// check token tails
			if (token == "operator")
			{
				Text nextToken = reader->read();
				if (nextToken == "(")
				{
					reader->must(")");
					token.addEnd(2);
				}
				else if (nextToken == "[")
				{
					reader->must("]");
					token.addEnd(2);
				}
				else if (nextToken == "<")
				{
					if (reader->readIf("<")) token.addEnd(2);
					else token.addEnd(1);
				}
				else
				{
					token.setEnd(nextToken.end());
				}
			}
			else if (token == "-")
			{
				Text nextToken = reader->read();
				token.setEnd(nextToken.end());
			}
			else if (token == "~")
			{
				Text nextToken = reader->read();
				token.setEnd(nextToken.end());
			}
			if (token.startsWith('<') || token.startsWith("~<"))
			{
				Text lambdaToken = reader->read();
				lambdaToken.must("lambda_");
				if (lambdaToken == "invoker_cdecl")
				{
					identifierType = DType::Function;
				}
				else
				{
					identifierType = DType::Class;
				}

				Text lambdaTokenEnd = reader->read();
				_assert(lambdaTokenEnd == ">");
				token.setEnd(lambdaTokenEnd.end());
			}

			// check ctor
			CtorType ctorType;
			if (token.startsWith('~'))
			{
				_assert(token.subarr(1) == decl->m_baseName);
				ctorType = CtorType::Dtor;
			}
			else if (token == decl->m_baseName)
			{
				ctorType = CtorType::Ctor;
			}
			else
			{
				ctorType = CtorType::No;
			}
			if (ctorType != CtorType::No)
			{
				if (decl->m_type.isTemplateSpecialized == UBool::Yes)
				{
					reader->must("<");
					decl->testTemplateMatch(reader);
				}
			}

			// parse template
			if (reader->readIf("<"))
			{
				switch (ctorType)
				{
				case CtorType::Ctor:
					decl = decl->get(Special::CTOR_T, token, DType::Constructor, true);
					break;
				case CtorType::Dtor:
					debug();
					decl = decl->get(Special::DTOR_T, token, DType::Destructor, true);
					break;
				case CtorType::No:
					decl = decl->get(TText::concat(token, Special::TEMPLATE_SUFFIX), token, identifierType);
					break;
				}
				decl = decl->parseTemplate(reader);
			}
			else
			{
				switch (ctorType)
				{
				case CtorType::Ctor:
					decl = decl->get(Special::CTOR, token, DType::Constructor, true);
					break;
				case CtorType::Dtor:
					decl = decl->get(Special::DTOR, token, DType::Destructor, true);
					break;
				case CtorType::No:
					decl = decl->get(token, token, identifierType);
					break;
				}
			}
		}

		if (!reader->readIf("::")) break;
		decl->intersectType(DType::NamespaceLike);
	}

	// parse modifier
	if (isEnum) decl->intersectType(DType::Enum);
	if (isFunctionPointer)
	{
		decl = decl->get("*", "*", DType::ComplexType, true);
	}

	int modifierLevel = 0;
	while (modifierLevel != 0 || !reader->eof(eof))
	{
		TypeOfToken tokenType = tokenTypes.get(reader->peek(), TypeOfToken::Identifier);
		DeclarationType dtype;
		Text token;
		if (tokenType == TypeOfToken::Primitive)
		{
			token = reader->read();
			dtype = DType::PrimitiveType;
		}
		else if (tokenType == TypeOfToken::LeftSideModifier)
		{
			token = reader->read();
			dtype = DType::ComplexType;
		}
		else
		{
			if (reader->readIf("("))
			{
				if (modifierLevel == 0 && tokenTypes.get(reader->peek(), TypeOfToken::Identifier) != TypeOfToken::LeftSideModifier)
				{
					decl->intersectType(DType::Type);
					goto _function;
				}
				modifierLevel++;
				continue;
			}
			else if (reader->readIf("["))
			{
				token = reader->read();
				Text size = reader->read();
				_assert(size.numberonly());
				token.setEnd(size.end() + 1);
				dtype = DType::ComplexType;
			}
			else
			{
				if (reader->readIf(")"))
				{
					_assert(modifierLevel != 0);
					modifierLevel--;
					while (modifierLevel != 0)
					{
						reader->must(")");
						modifierLevel--;
					}
					break;
				}
				else
				{
					Declaration* _member = root.parse(reader, DType::Unknown, "*)");
					decl = decl->get(TText::concat("#MEMOF#", _member->m_absPath), _member->m_baseName, DType::ComplexType, true);
					continue;
				}
			}
		}
		decl->intersectType(DType::Type);
		decl = decl->get(token, token, dtype, true);
	}

	if (reader->readIf("("))
	{
		// parse function
	_function:
		Text functypeToken = reader->peek();
		functypeToken.addBegin(-1);

		TmpArray<Declaration*> decls;
		if (!reader->readIf(")"))
		{
			for (;;)
			{
				Declaration* param = root.parse(reader, DType::Type, ",)");
				decls.push(param);
				if (!reader->readIf(",")) break;
			}
			reader->must(")");
		}

		{
			TSZ functype;
			functype << "#FUNC#" << decl->m_absPath;
			for (Declaration* param : decls)
			{
				functype << '#' << param->m_absPath;
			}
			if (reader->readIf("const")) functype << "#const";

			functypeToken.setEnd(reader->peek().begin());
			decl = root.get(functype, functypeToken, DType::FunctionType, true);
		}
	}
	else if (reader->readIf("["))
	{
		// parse array
		Text token = reader->read();
		_assert(token.numberonly());
		reader->must("]");
		token.addBegin(-1);
		token.addEnd(1);

		decl = decl->get(token, token, DType::ComplexType, true);
	}
	_assert(modifierLevel == 0);
	_assert(reader->eof(eof));
	decl->intersectType(type);
	return decl;
}
