#include "stdafx.h"
#include "demangler.h"

#include "declaration.h"

MangleParser::MangleParser(Text text) noexcept
	:m_read(text)
{
}
Declaration* MangleParser::parse() noexcept
{
	_assert(m_read.read() == '?');
	return parseComplex();
}
Declaration* MangleParser::parseComplex() noexcept
{
	if (m_read.readIf('?')) return specialName();
	
	Text name = m_read.readwith('@');
	Declaration* decl = parseComplex();
	return decl->get(name, name, DType::Unknown);
}
Declaration* MangleParser::parseCvClassModifier() noexcept
{
	char chr = m_read.read();
	switch (chr)
	{
	case '7':
	case '6':
		return 
	default:
		debug();
		break;
	}
	return nullptr;
}
Declaration* MangleParser::parseTypeModifier() noexcept
{
	return nullptr;
}
Declaration* MangleParser::parseType() noexcept
{
	Declaration* decl = nullptr;
	char chr = m_read.read();
	switch (chr)
	{
	case '?': // template parameter
		unreachable();
		break;
	case 'A':
		return parseTypeModifier()->get("&", DType::ComplexType, true);
	case 'B':
		return parseTypeModifier()->get("*", DType::ComplexType)->get("volatile", DType::ComplexType, true);
	case 'C':
		return Declaration::root.get("char", DType::PrimitiveType)->get("signed", DType::PrimitiveType);
	case 'D':
		return Declaration::root.get("char", DType::PrimitiveType);
	case 'E':
		return Declaration::root.get("char", DType::PrimitiveType)->get("unsigned", DType::PrimitiveType);
	case 'F':
		return Declaration::root.get("short", DType::PrimitiveType);
	case 'G':
		return Declaration::root.get("short", DType::PrimitiveType)->get("unsigned", DType::PrimitiveType);
	case 'H':
		return Declaration::root.get("int", DType::PrimitiveType);
	case 'I':
		return Declaration::root.get("int", DType::PrimitiveType)->get("unsigned", DType::PrimitiveType);
	case 'J':
		return Declaration::root.get("long", DType::PrimitiveType);
	case 'K':
		return Declaration::root.get("long", DType::PrimitiveType)->get("unsigned", DType::PrimitiveType);
	case 'L':
		return Declaration::root.get("__int128", DType::PrimitiveType);
	case 'M':
		return Declaration::root.get("float", DType::PrimitiveType);
	case 'N':
		return Declaration::root.get("double", DType::PrimitiveType);
	case 'O':
		return Declaration::root.get("long", DType::PrimitiveType)->get("double", DType::PrimitiveType);
	case 'P':
		return parseTypeModifier()->get("*", DType::ComplexType, true);
	case 'Q':
		return parseTypeModifier()->get("*", DType::ComplexType, true)->get("const", DType::ComplexType, true);
	case 'R':
		return parseTypeModifier()->get("*", DType::ComplexType, true)->get("volatile", DType::ComplexType);
	case 'S':
		return parseTypeModifier()->get("*", DType::ComplexType, true)->get("volatile", DType::ComplexType)->get("const", DType::ComplexType);
	case 'T':
		// union
		decl = parse();
		decl->intersectType(DType::Union);
		return decl;
	case 'U':
		// struct
		decl = parse();
		decl->intersectType(DType::Structure);
		return decl;
	case 'V':
		// class
		decl = parse();
		decl->intersectType(DType::Class);
		return decl;
	case 'W':
		// enum
		decl = parse();
		decl->intersectType(DType::Enum);
		return decl;
	case 'X':
		// or coclass
		return Declaration::root.get("void", "void", DType::PrimitiveType);
	case 'Y':
		// cointerface
		unreachable();
		break;
	case 'Z':
		// ... (ellipsis)
		unreachable();
		break;
	case '_':
		chr = m_read.read();
		switch (chr)
		{
		case '$': // __w64
			unreachable();
			break;
		case 'D':
			return Declaration::root.get("__int8", "__int8", DType::PrimitiveType);
		case 'E':
			return Declaration::root.get("__int8", "__int8", DType::PrimitiveType)->get("unsigned", "unsigned", DType::PrimitiveType);
		case 'F':
			return Declaration::root.get("__int16", "__int16", DType::PrimitiveType);
		case 'G':
			return Declaration::root.get("__int16", "__int16", DType::PrimitiveType)->get("unsigned", "unsigned", DType::PrimitiveType);
		case 'H':
			return Declaration::root.get("__int32", "__int32", DType::PrimitiveType);
		case 'I':
			return Declaration::root.get("__int32", "__int32", DType::PrimitiveType)->get("unsigned", "unsigned", DType::PrimitiveType);
		case 'J':
			return Declaration::root.get("__int64", "__int64", DType::PrimitiveType);
		case 'K':
			return Declaration::root.get("__int64", "__int64", DType::PrimitiveType)->get("unsigned", "unsigned", DType::PrimitiveType);
		case 'L':
			return Declaration::root.get("__int128", "__int128", DType::PrimitiveType);
		case 'M':
			return Declaration::root.get("__int128", "__int128", DType::PrimitiveType)->get("unsigned", "unsigned", DType::PrimitiveType);
		case 'N':
			return Declaration::root.get("bool", "bool", DType::PrimitiveType);
		case 'O': // Array
			unreachable();
			break;
		case 'S':
			return Declaration::root.get("char16_t", "char16_t", DType::PrimitiveType);
		case 'U':
			return Declaration::root.get("char32_t", "char32_t", DType::PrimitiveType);
		case 'W':
			return Declaration::root.get("wchar_t", "wchar_t", DType::PrimitiveType);
		case 'X': // coclass
			unreachable();
			break;
		case 'Y': // cointerface
			unreachable();
			break;
		default:
			break;
		}
		break;
	case '$':
		if (m_read.readIf('$'))
		{
			chr = m_read.read();
			switch (chr)
			{
			case 'A': // Type modifier (function)
				unreachable();
				break;
			case 'C': // Array type in template
				unreachable();
				break;
			case 'F': // Function modifier (managed function [Managed C++ or C++/CLI])
				unreachable();
				break;
			case 'Q': // Type modifier (rvalue reference)
				unreachable();
				break;
			case 'R': // Type modifier (volatile rvalue reference)
				unreachable();
				break;
			case 'T': // std::nullptr_t
				return Declaration::root.get("nullptr_t", "nullptr_t", DType::PrimitiveType);
			case 'V': // Empty type parameter pack
				unreachable();
				break;
			case 'Z': // End template parameter pack
				unreachable();
				break;
			default:
				unreachable();
				break;
			}
		}
		else
		{
			// template parameter
			m_read.must('$');
			unreachable();
		}
		break;
	default:
		if ('0' <= chr && chr <= '9')
		{
			size_t idx = (size_t)((int)chr - (int)'0');
			_assert(idx < m_refs.size());
			decl = m_refs[idx];
		}
		else
		{
			unreachable();
		}
		break;
	}
	debug();
	return decl;
}
Declaration* MangleParser::parseTemplate() noexcept
{
	Text name = m_read.readwith('@');

	// template
	Array<Declaration*> params;
	params.reserve(16);
	for (;;)
	{
		Declaration* param = parseType();
		if (param == nullptr) break;
		m_refs.push(param);
		params.push(param);
	}

	Declaration* decl = parseComplex();
	decl = decl->get(name, name, DType::Template);
	return decl->templateSpecialize(params);
}
Declaration* MangleParser::specialName() noexcept
{
	Declaration* decl;
	char chr = m_read.read();

	auto declByName = [this](Text name){
		Declaration * decl = parseComplex();
		return decl->get(name, name, DType::Function, true);
	};
	switch (chr)
	{
	case '$':
		return parseTemplate();
	case '0':
		decl = parseComplex();
		decl->get(Special::CTOR, decl->baseName(), DType::Constructor, true);
		break;
	case '1':
		decl = parseComplex();
		decl->get(Special::DTOR, TText::concat('~', decl->baseName()), DType::Destructor, true);
		break;
	case '2': decl = declByName("operator new"); break;
	case '3': decl = declByName("operator delete"); break;
	case '4': decl = declByName("operator ="); break;
	case '5': decl = declByName("operator >>"); break;
	case '6': decl = declByName("operator <<"); break;
	case '7': decl = declByName("operator !"); break;
	case '8': decl = declByName("operator =="); break;
	case '9': decl = declByName("operator !="); break;
	case 'A': decl = declByName("operator []"); break;
	case 'B': decl = declByName("operator  cast"); break;
	case 'C': decl = declByName("operator ->"); break;
	case 'D': decl = declByName("operator *"); break;
	case 'E': decl = declByName("operator ++"); break;
	case 'F': decl = declByName("operator --"); break;
	case 'G': decl = declByName("operator -"); break;
	case 'H': decl = declByName("operator +"); break;
	case 'I': decl = declByName("operator &"); break;
	case 'J': decl = declByName("operator ->*"); break;
	case 'K': decl = declByName("operator /"); break;
	case 'L': decl = declByName("operator %"); break;
	case 'M': decl = declByName("operator <"); break;
	case 'N': decl = declByName("operator <="); break;
	case 'O': decl = declByName("operator >"); break;
	case 'P': decl = declByName("operator >="); break;
	case 'Q': decl = declByName("operator ,"); break;
	case 'R': decl = declByName("operator ()"); break;
	case 'S': decl = declByName("operator ~"); break;
	case 'T': decl = declByName("operator ^"); break;
	case 'U': decl = declByName("operator |"); break;
	case 'V': decl = declByName("operator &&"); break;
	case 'W': decl = declByName("operator ||"); break;
	case 'X': decl = declByName("operator *="); break;
	case 'Y': decl = declByName("operator +="); break;
	case 'Z': decl = declByName("operator -="); break;
	case '_':
		chr = m_read.read();
		switch (chr)
		{
		case '0': decl = declByName("operator /="); break;
		case '1': decl = declByName("operator %="); break;
		case '2': decl = declByName("operator >>="); break;
		case '3': decl = declByName("operator <<="); break;
		case '4': decl = declByName("operator &="); break;
		case '5': decl = declByName("operator |="); break;
		case '6': decl = declByName("operator ^="); break;
		case '7': decl = declByName("`vftable'"); break;
		case '8': decl = declByName("`vbtable'"); break;
		case '9': decl = declByName("`vcall'"); break;
		case 'A': decl = declByName("`typeof'"); break;
		case 'B': decl = declByName("`local static guard'"); break;
		case 'C': decl = declByName("String constant (see below)"); break;
		case 'D': decl = declByName("`vbase destructor'"); break;
		case 'E': decl = declByName("`vector deleting destructor'"); break;
		case 'F': decl = declByName("`default constructor closure'"); break;
		case 'G': decl = declByName("`scalar deleting destructor'"); break;
		case 'H': decl = declByName("`vector constructor iterator'"); break;
		case 'I': decl = declByName("`vector destructor iterator'"); break;
		case 'J': decl = declByName("`vector vbase constructor iterator'"); break;
		case 'K': decl = declByName("`virtual displacement map'"); break;
		case 'L': decl = declByName("`eh vector constructor iterator'"); break;
		case 'M': decl = declByName("`eh vector destructor iterator'"); break;
		case 'N': decl = declByName("`eh vector vbase constructor iterator'"); break;
		case 'O': decl = declByName("`copy constructor closure'"); break;
		case 'P': decl = declByName("`udt returning' (prefix)"); break;
		case 'Q': decl = declByName("Unknown"); break;
		case 'R': decl = declByName("RTTI-related code (see below)"); break;
		case 'S': decl = declByName("`local vftable'"); break;
		case 'T': decl = declByName("`local vftable constructor closure'"); break;
		case 'U': decl = declByName("operator new[]"); break;
		case 'V': decl = declByName("operator delete[]"); break;
		case 'W': decl = declByName("`omni callsig'"); break;
		case 'X': decl = declByName("`placement delete closure'"); break;
		case 'Y': decl = declByName("`placement delete[] closure'"); break;
		case '_':
			chr = m_read.read();
			switch (chr)
			{
			case 'A': decl = declByName("`managed vector constructor iterator'"); break;
			case 'B': decl = declByName("`managed vector destructor iterator'"); break;
			case 'C': decl = declByName("`eh vector copy constructor iterator'"); break;
			case 'D': decl = declByName("`eh vector vbase copy constructor iterator'"); break;
			case 'E': decl = declByName("`dynamic initializer' (Used by CRT entry point to construct non-trivial? global objects)"); break;
			case 'F': decl = declByName("`dynamic atexit destructor` (Used by CRT to destroy non-trivial? global objects on program exit)"); break;
			case 'G': decl = declByName("`vector copy constructor iterator'"); break;
			case 'H': decl = declByName("`vector vbase copy constructor iterator'"); break;
			case 'I': decl = declByName("`managed vector copy constructor iterator'"); break;
			case 'J': decl = declByName("`local static thread guard'"); break;
			case 'K': decl = declByName("user-defined literal operator"); break;
			default: unreachable(); break;
			}
			break;
		default: unreachable(); break;
		}
		break;
	default: unreachable(); break;
	}

	return decl;
}
void MangleParser::cvClassModifier() noexcept
{

	
}