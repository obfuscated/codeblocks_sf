#ifndef AVCONFIG_H_INCLUDED
#define AVCONFIG_H_INCLUDED

#include <string>

struct avVersionValues
{
	long Major;
	long Minor;
	long Build;
	long Revision;
	long BuildCount;
	avVersionValues() : Major(1), Minor(0), Build(0), Revision(0), BuildCount(1) {}
};

struct avStatus
{
	std::string SoftwareStatus;
	std::string Abbreviation;
	avStatus() : SoftwareStatus("Alpha"), Abbreviation("a") {}
};

struct avVersionState
{
	avVersionValues Values;
	avStatus Status;
	long BuildHistory;
	avVersionState() : BuildHistory(0) {}
};

struct avScheme
{
	long MinorMax;
	long BuildMax;
	long RevisionMax;
	long RevisionRandMax;
	long BuildTimesToIncrementMinor;
	avScheme() : MinorMax(10), BuildMax(0), RevisionMax(0), RevisionRandMax(10), BuildTimesToIncrementMinor(100) {}
	bool operator!=(const avScheme& Other) const
	{
	    return (MinorMax != Other.MinorMax || BuildMax != Other.BuildMax ||
                RevisionMax != Other.RevisionMax || RevisionRandMax != Other.RevisionRandMax ||
                BuildTimesToIncrementMinor != Other.BuildTimesToIncrementMinor);
	}
};

struct avSettings
{
	bool Autoincrement;
	bool DateDeclarations;
	bool DoAutoIncrement;
	bool AskToIncrement;
	std::string Language;
	bool Svn;
	std::string SvnDirectory;
	std::string HeaderPath;
	avSettings() : Autoincrement(true), DateDeclarations(true), DoAutoIncrement(false), AskToIncrement(false),
        Language("C++"), Svn(false), SvnDirectory(), HeaderPath("version.h") {}
	bool operator!=(const avSettings& Other) const
	{
	    return (Autoincrement != Other.Autoincrement || DateDeclarations != Other.DateDeclarations ||
                DoAutoIncrement != Other.DoAutoIncrement || AskToIncrement != Other.AskToIncrement ||
                Language != Other.Language || Svn != Other.Svn ||
                SvnDirectory != Other.SvnDirectory || HeaderPath != Other.HeaderPath);
	}
};

struct avChangesLog
{
	bool GenerateChanges;
	std::string AppTitle;
	std::string ChangesLogPath;
	avChangesLog() : GenerateChanges(false), AppTitle("released version %M.%m.%b of %p"),
        ChangesLogPath("ChangesLog.txt") {}
	bool operator!=(const avChangesLog& Other) const
	{
	    return (GenerateChanges != Other.GenerateChanges || AppTitle != Other.AppTitle ||
                ChangesLogPath != Other.ChangesLogPath);
	}
};

struct avConfig
{
	avScheme Scheme;
	avSettings Settings;
	avChangesLog ChangesLog;
	avConfig() {}
	bool operator!=(const avConfig& Other) const
	{
	    return (Scheme != Other.Scheme || Settings != Other.Settings ||
                ChangesLog != Other.ChangesLog);
	}
};

#endif // AVCONFIG_H_INCLUDED
