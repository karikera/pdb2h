#include "stdafx.h"
#include "tokenreader.h"
#include "declaration.h"
#include "demangler.h"

void parseSymbol(Text name, SYMBOL_INFO* info) noexcept
{
	{
		static uint symbolCount = 0;
		static timepoint prev = timepoint::now();
		timepoint now = timepoint::now();

		duration passed = now - prev;
		symbolCount++;

		if (passed >= 100_ms)
		{
			cout << "Read Symbols: " << symbolCount << '\r';
			prev += 100_ms;
		}
	}


	if (name.size() >= 2001)
	{
		cerr << "Symbol name is too long: " << name.subarr(0, 50) << "..." << endl;
		return;
	}

	info->TypeIndex;        // Type Index of symbol
	info->Index;
	info->Size;
	uint64_t address = info->Address - info->ModBase;
	info->Flags;
	info->Value;            // Value of symbol, ValuePresent should be 1
	info->Register;         // register holding value or pointer to value
	info->Scope;            // scope of the symbol


	if (info->Name[0] == '?')
	{
		MangleParser mangleParser(name);
		mangleParser.parse();
	}
	return;

	TokenReader parse = name;

	switch (info->Tag)
	{
	case SymTagFunction:
		Declaration::root.parse(&parse, DType::Function, "");
		break;
	case SymTagPublicSymbol:
		Declaration::root.parse(&parse, DType::Unknown, "");
		break;
	default:
		debug();
		break;
	}
	_assert(parse.remaining().empty());
}

int wmain()
{
	Declaration* b = MangleParser("?_Copy@?$_Func_impl_no_alloc@P6A?AV?$unique_ptr@VSelectorDefinition@@U?$default_delete@VSelectorDefinition@@@std@@@std@@XZV?$unique_ptr@VBehaviorDefinition@@U?$default_delete@VBehaviorDefinition@@@std@@@2@$$V@std@@EEBAPEAV?$_Func_base@V?$unique_ptr@VBehaviorDefinition@@U?$default_delete@VBehaviorDefinition@@@std@@@std@@$$V@2@PEAX@Z").parse();
	try
	{
		uint64_t base = 0x100000;
		PdbReader reader(base, u"bedrock_server.pdb");
		{
			PdbReader::SymbolInfo info = reader.getInfo();


			// Display information about symbols 
			ucout << u"PdbReader: Loaded symbols:" << info.type << u'\n';

			// Image name 
			if (!info.imageName.empty()) ucout << u"PdbReader: Image name: " << info.imageName << u'\n';

			// Loaded image name 
			if (!info.loadedImageName.empty()) ucout << u"PdbReader: Loaded image name: " << info.loadedImageName << u'\n';

			// Loaded PDB name 
			if (!info.loadedPdbName.empty()) ucout << u"PdbReader: Loaded pdb name: " << info.loadedPdbName << u'\n';

			if (info.unmatched) ucout << u"PdbReader: Warning: Unmatched symbols.\n";

			// Contents 
			ucout << u"PdbReader: Line numbers: " << (info.lineNumbers ? u"Available" : u"Not available") << endl;
			ucout << u"PdbReader: Global symbols: " << (info.globalSymbols ? u"Available" : u"Not available") << endl;
			ucout << u"PdbReader: Type information: " << (info.typeInfo ? u"Available" : u"Not available") << endl;
			ucout << u"PdbReader: Source indexing: " << (info.sourceIndexed ? u"Yes" : u"No") << endl;
			ucout << u"PdbReader: Public symbols: " << (info.publics ? u"Available" : u"Not available") << endl;

		}
		reader.getAllEx([](Text name, SYMBOL_INFO* info)->bool {
			__try
			{
				parseSymbol(name, info);
			}
			__except(EXCEPTION_CONTINUE_EXECUTION)
			{
			}
			return true;
			});
	}
	catch (FunctionError& err)
	{
		cout << "func: " << err.getFunctionName() << endl;
		cout << "err: " << err.getErrorCode() << endl;
		cout << "err: " << err.getMessage<char>() << endl;
	}

	_getch();

	return 0;
}