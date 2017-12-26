#pragma once
#include <vector>
#include <fstream>

class FileReader {
public:
	FileReader(const char* fileName, bool skipBlankline);
	~FileReader();

public:
	bool ReadLine(std::string& text, int* lineNumber);

private:
	bool skipBlankline_;
	int lineNumber_;
	std::ifstream ifs_;
};

typedef std::pair<std::string, std::string> ProductionText;

struct GrammarText {
	std::string lhs;
	typedef std::vector<ProductionText> ProductionTextContainer;
	ProductionTextContainer productions;

	void Clear();
	bool Empty() const;
};

typedef std::vector<GrammarText> GrammarTextContainer;

class GrammarReader {
public:
	GrammarReader(const char* source);

public:
	const GrammarTextContainer& GetGrammars() const;

private:
	void ReadGrammars();
	const char* SplitGrammar(const char*& text);
	bool GetLine(const char*& ptr, std::string& line);

private:
	const char* source_;
	GrammarTextContainer grammars_;
};
