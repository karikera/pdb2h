#pragma once

class Declaration;

class MangleParser
{
public:
	MangleParser(Text text) noexcept;
	Declaration* parse() noexcept;
	Declaration* parseComplex() noexcept;
	Declaration* parseCvClassModifier() noexcept;
	Declaration* parseTypeModifier() noexcept;
	Declaration* parseType() noexcept;
	Declaration* parseTemplate() noexcept;
	Declaration* specialName() noexcept;
	void cvClassModifier() noexcept;

private:
	Text m_read;
	Array<Declaration*> m_refs;
};
