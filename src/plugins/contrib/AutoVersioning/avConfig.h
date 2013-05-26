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

struct avCode
{
	std::string HeaderGuard;
	std::string NameSpace;
	std::string Prefix;

	avCode() : HeaderGuard("VERSION_H"), NameSpace("AutoVersion"), Prefix(""){}

	bool operator!=(const avCode& Other) const
	{
	    return (HeaderGuard != Other.HeaderGuard || NameSpace != Other.NameSpace ||
                Prefix != Other.Prefix);
	}
};

struct avSettings
{
	bool Autoincrement;
	bool DateDeclarations;
	bool DoAutoIncrement;
	// GJH 03/03/10 Added manifest updating.
	bool UpdateManifest;
	bool AskToIncrement;
	bool UseDefine;
	std::string Language;
	bool Svn;
	std::string SvnDirectory;
	std::string HeaderPath;
	avSettings() : Autoincrement(true), DateDeclarations(true), DoAutoIncrement(false), AskToIncrement(false),
        UseDefine(false), Language("C++"), Svn(false), SvnDirectory(), HeaderPath("version.h") {}
	bool operator!=(const avSettings& Other) const
	{
	    return (Autoincrement != Other.Autoincrement || UpdateManifest != Other.UpdateManifest ||
		DateDeclarations != Other.DateDeclarations || DoAutoIncrement != Other.DoAutoIncrement ||
		AskToIncrement != Other.AskToIncrement || Language != Other.Language || UseDefine != Other.UseDefine ||
		Svn != Other.Svn || SvnDirectory != Other.SvnDirectory ||
		HeaderPath != Other.HeaderPath);
	}
};

struct avChangesLog
{
	bool ShowChangesEditor;
	std::string AppTitle;
	std::string ChangesLogPath;
	avChangesLog() : ShowChangesEditor(false), AppTitle("released version %M.%m.%b of %p"),
        ChangesLogPath("ChangesLog.txt") {}
	bool operator!=(const avChangesLog& Other) const
	{
	    return (ShowChangesEditor != Other.ShowChangesEditor || AppTitle != Other.AppTitle ||
                ChangesLogPath != Other.ChangesLogPath);
	}
};

struct avConfig
{
    avCode Code;
	avScheme Scheme;
	avSettings Settings;
	avChangesLog ChangesLog;
	avConfig() {}
	bool operator!=(const avConfig& Other) const
	{
	    return (Scheme != Other.Scheme || Settings != Other.Settings ||
                ChangesLog != Other.ChangesLog || Code != Other.Code);
	}
};

#endif // AVCONFIG_H_INCLUDED
