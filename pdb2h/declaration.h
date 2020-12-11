#pragma once

#include "namerule.h"

class TokenReader;

enum class ClassType
{
	Unknown,
	No,
	Structure,
	Class,
	Namespace,
	Enum,
	Union,
	Function,
};

enum class UBool
{
	Unknown,
	No,
	Yes,
};

enum class CtorType
{
	Unknown,
	No,
	Ctor,
	Dtor,
};

struct DeclarationType
{
	UBool isValue;
	UBool isType;
	ClassType classType;
	UBool nestiable;
	UBool isTemplate;
	UBool isTemplateSpecialized;
	CtorType ctor;
};

class Declaration
{
public:
	Declaration(AText name, AText baseName, DeclarationType type) noexcept;
	Text name() noexcept;
	Text baseName() noexcept;
	void checkName() noexcept;
	void testTemplateMatch(TokenReader* reader) noexcept;
	Declaration* templateSpecialize(View<Declaration*> params) noexcept;
	Declaration* parseTemplate(TokenReader* reader) noexcept;
	void intersectType(DeclarationType type) noexcept;
	Declaration* parse(TokenReader* reader, DeclarationType type, const char* eof) noexcept;
	Declaration* get(Text name, Text baseName, DeclarationType type, bool specialName = false) noexcept;
	Declaration* get(Text name, DeclarationType type, bool specialName = false) noexcept;

	static Declaration root;

private:
	Declaration(DeclarationType type) noexcept;

	uint32_t m_id;

	Declaration* m_parent;
	DeclarationType m_type;
	const AText m_name;
	const AText m_baseName;
	AText m_absPath;

	Array<Declaration*> m_specialized;
	Array<Declaration*> m_templateParameters;

	static uint32_t s_idcounter;
	static Map<Text, Declaration*, true> s_map;
};


UBool operator &(UBool a, UBool b) noexcept;
ClassType operator &(ClassType a, ClassType b) noexcept;
CtorType operator &(CtorType a, CtorType b) noexcept;
const DeclarationType operator &(const DeclarationType& a, const DeclarationType& b) noexcept;


namespace DType
{
	static const DeclarationType Unknown = {
	};
	static const DeclarationType Namespace = {
		UBool::No, UBool::No, ClassType::Namespace, UBool::Yes, UBool::No, UBool::No, CtorType::No,
	};
	static const DeclarationType NamespaceLike = {
		UBool::Unknown, UBool::Unknown, ClassType::Unknown, UBool::Yes, UBool::Unknown, UBool::Unknown, CtorType::Unknown,
	};

	static const DeclarationType Value = {
		UBool::Yes, UBool::No, ClassType::No, UBool::No, UBool::Unknown, UBool::Unknown, CtorType::Unknown,
	};
	static const DeclarationType Function = {
		UBool::Yes, UBool::No, ClassType::Function, UBool::Yes, UBool::Unknown, UBool::Unknown, CtorType::Unknown,
	};
	static const DeclarationType Variable = {
		UBool::Yes, UBool::No, ClassType::No, UBool::No, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType Constant = {
		UBool::Yes, UBool::No, ClassType::No, UBool::No, UBool::No, UBool::Unknown, CtorType::No,
	};

	static const DeclarationType Type = {
		UBool::No, UBool::Yes, ClassType::Unknown, UBool::Unknown, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType FunctionType = {
		UBool::No, UBool::Yes, ClassType::No, UBool::No, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType ComplexType = {
		UBool::No, UBool::Yes, ClassType::No, UBool::No, UBool::No, UBool::No, CtorType::No,
	};
	static const DeclarationType ClassLike = {
		UBool::No, UBool::Yes, ClassType::Unknown, UBool::Yes, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType Class = {
		UBool::No, UBool::Yes, ClassType::Class, UBool::Yes, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType Union = {
		UBool::No, UBool::Yes, ClassType::Union, UBool::Yes, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType Structure = {
		UBool::No, UBool::Yes, ClassType::Structure, UBool::Yes, UBool::No, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType PrimitiveType = {
		UBool::No, UBool::Yes, ClassType::No, UBool::No, UBool::No, UBool::No, CtorType::No,
	};
	static const DeclarationType Enum = {
		UBool::No, UBool::Yes, ClassType::Enum, UBool::No, UBool::No, UBool::No, CtorType::No,
	};

	static const DeclarationType Template = {
		UBool::No, UBool::No, ClassType::Unknown, UBool::Unknown, UBool::Yes, UBool::No, CtorType::Unknown,
	};
	static const DeclarationType TemplateClass = {
		UBool::No, UBool::No, ClassType::Class, UBool::No, UBool::Yes, UBool::No, CtorType::No,
	};
	static const DeclarationType TemplateStructure = {
		UBool::No, UBool::No, ClassType::Structure, UBool::No, UBool::Yes, UBool::No, CtorType::No,
	};
	static const DeclarationType TemplateFunction = {
		UBool::No, UBool::No, ClassType::Function, UBool::Yes, UBool::Yes, UBool::No, CtorType::Unknown,
	};
	static const DeclarationType TemplateParameter = {
		UBool::Unknown, UBool::Unknown, ClassType::Unknown, UBool::Unknown, UBool::Unknown, UBool::Unknown, CtorType::No,
	};
	static const DeclarationType TemplateSpecialized = {
		UBool::Unknown, UBool::Unknown, ClassType::Unknown, UBool::Unknown, UBool::Unknown, UBool::Yes, CtorType::Unknown,
	};

	static const DeclarationType FunctionLike = {
		UBool::Unknown, UBool::No, ClassType::Function, UBool::Yes, UBool::Unknown, UBool::Unknown, CtorType::Unknown,
	};
	static const DeclarationType Constructor = {
		UBool::Unknown, UBool::No, ClassType::Function, UBool::Yes, UBool::Unknown, UBool::Unknown, CtorType::Ctor,
	};
	static const DeclarationType Destructor = {
		UBool::Unknown, UBool::No, ClassType::Function, UBool::Yes, UBool::Unknown, UBool::Unknown, CtorType::Dtor,
	};
}
