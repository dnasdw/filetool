#include "filetool.h"

CFileTool::SOption CFileTool::s_Option[] =
{
	{ USTR("split"), USTR('s'), USTR("split the target file") },
	{ USTR("join"), USTR('j'), USTR("join the target file") },
	{ USTR("file"), USTR('f'), USTR("the target file") },
	{ USTR("dir"), USTR('d'), USTR("the dir for the target file") },
	{ USTR("verbose"), USTR('v'), USTR("show the info") },
	{ USTR("help"), USTR('h'), USTR("show this help") },
	{ nullptr, 0, nullptr }
};

CFileTool::CFileTool()
	: m_eAction(kActionNone)
	, m_bVerbose(false)
{
}

CFileTool::~CFileTool()
{
}

int CFileTool::ParseOptions(int a_nArgc, UChar* a_pArgv[])
{
	if (a_nArgc <= 1)
	{
		return 1;
	}
	for (int i = 1; i < a_nArgc; i++)
	{
		int nArgpc = static_cast<int>(UCslen(a_pArgv[i]));
		if (nArgpc == 0)
		{
			continue;
		}
		int nIndex = i;
		if (a_pArgv[i][0] != USTR('-'))
		{
			UPrintf(USTR("ERROR: illegal option\n\n"));
			return 1;
		}
		else if (nArgpc > 1 && a_pArgv[i][1] != USTR('-'))
		{
			for (int j = 1; j < nArgpc; j++)
			{
				switch (parseOptions(a_pArgv[i][j], nIndex, a_nArgc, a_pArgv))
				{
				case kParseOptionReturnSuccess:
					break;
				case kParseOptionReturnIllegalOption:
					UPrintf(USTR("ERROR: illegal option\n\n"));
					return 1;
				case kParseOptionReturnNoArgument:
					UPrintf(USTR("ERROR: no argument\n\n"));
					return 1;
				case kParseOptionReturnOptionConflict:
					UPrintf(USTR("ERROR: option conflict\n\n"));
					return 1;
				}
			}
		}
		else if (nArgpc > 2 && a_pArgv[i][1] == USTR('-'))
		{
			switch (parseOptions(a_pArgv[i] + 2, nIndex, a_nArgc, a_pArgv))
			{
			case kParseOptionReturnSuccess:
				break;
			case kParseOptionReturnIllegalOption:
				UPrintf(USTR("ERROR: illegal option\n\n"));
				return 1;
			case kParseOptionReturnNoArgument:
				UPrintf(USTR("ERROR: no argument\n\n"));
				return 1;
			case kParseOptionReturnOptionConflict:
				UPrintf(USTR("ERROR: option conflict\n\n"));
				return 1;
			}
		}
		i = nIndex;
	}
	return 0;
}

int CFileTool::CheckOptions()
{
	if (m_eAction == kActionNone)
	{
		UPrintf(USTR("ERROR: nothing to do\n\n"));
		return 1;
	}
	if (m_eAction != kActionHelp)
	{
		if (m_sFileName.empty())
		{
			UPrintf(USTR("ERROR: no --file option\n\n"));
			return 1;
		}
		if (m_sDirName.empty())
		{
			UPrintf(USTR("ERROR: no --dir option\n\n"));
			return 1;
		}
	}
	return 0;
}

int CFileTool::Help()
{
	UPrintf(USTR("filetool %") PRIUS USTR(" by dnasdw\n\n"), AToU(FILETOOL_VERSION).c_str());
	UPrintf(USTR("usage: filetool [option...] [option]...\n"));
	UPrintf(USTR("sample:\n"));
	UPrintf(USTR("  filetool -svfd input.zip outputdir\n"));
	UPrintf(USTR("  filetool -jvfd output.zip inputdir\n"));
	UPrintf(USTR("\n"));
	UPrintf(USTR("option:\n"));
	SOption* pOption = s_Option;
	while (pOption->Name != nullptr || pOption->Doc != nullptr)
	{
		if (pOption->Name != nullptr)
		{
			UPrintf(USTR("  "));
			if (pOption->Key != 0)
			{
				UPrintf(USTR("-%c,"), pOption->Key);
			}
			else
			{
				UPrintf(USTR("   "));
			}
			UPrintf(USTR(" --%-8") PRIUS, pOption->Name);
			if (UCslen(pOption->Name) >= 8 && pOption->Doc != nullptr)
			{
				UPrintf(USTR("\n%16") PRIUS, USTR(""));
			}
		}
		if (pOption->Doc != nullptr)
		{
			UPrintf(USTR("%") PRIUS, pOption->Doc);
		}
		UPrintf(USTR("\n"));
		pOption++;
	}
	return 0;
}

int CFileTool::Action()
{
	if (m_eAction == kActionSplit)
	{
		if (!splitFile())
		{
			UPrintf(USTR("ERROR: split file failed\n\n"));
			return 1;
		}
	}
	if (m_eAction == kActionJoin)
	{
		if (!joinFile())
		{
			UPrintf(USTR("ERROR: join file failed\n\n"));
			return 1;
		}
	}
	if (m_eAction == kActionHelp)
	{
		return Help();
	}
	return 0;
}

CFileTool::EParseOptionReturn CFileTool::parseOptions(const UChar* a_pName, int& a_nIndex, int a_nArgc, UChar* a_pArgv[])
{
	if (UCscmp(a_pName, USTR("split")) == 0)
	{
		if (m_eAction == kActionNone)
		{
			m_eAction = kActionSplit;
		}
		else if (m_eAction != kActionSplit && m_eAction != kActionHelp)
		{
			return kParseOptionReturnOptionConflict;
		}
	}
	else if (UCscmp(a_pName, USTR("join")) == 0)
	{
		if (m_eAction == kActionNone)
		{
			m_eAction = kActionJoin;
		}
		else if (m_eAction != kActionJoin && m_eAction != kActionHelp)
		{
			return kParseOptionReturnOptionConflict;
		}
	}
	else if (UCscmp(a_pName, USTR("file")) == 0)
	{
		if (a_nIndex + 1 >= a_nArgc)
		{
			return kParseOptionReturnNoArgument;
		}
		m_sFileName = a_pArgv[++a_nIndex];
	}
	else if (UCscmp(a_pName, USTR("dir")) == 0)
	{
		if (a_nIndex + 1 >= a_nArgc)
		{
			return kParseOptionReturnNoArgument;
		}
		m_sDirName = a_pArgv[++a_nIndex];
	}
	else if (UCscmp(a_pName, USTR("verbose")) == 0)
	{
		m_bVerbose = true;
	}
	else if (UCscmp(a_pName, USTR("help")) == 0)
	{
		m_eAction = kActionHelp;
	}
	return kParseOptionReturnSuccess;
}

CFileTool::EParseOptionReturn CFileTool::parseOptions(int a_nKey, int& a_nIndex, int m_nArgc, UChar* a_pArgv[])
{
	for (SOption* pOption = s_Option; pOption->Name != nullptr || pOption->Key != 0 || pOption->Doc != nullptr; pOption++)
	{
		if (pOption->Key == a_nKey)
		{
			return parseOptions(pOption->Name, a_nIndex, m_nArgc, a_pArgv);
		}
	}
	return kParseOptionReturnIllegalOption;
}

bool CFileTool::splitFile()
{
	FILE* fp = UFopen(m_sFileName.c_str(), USTR("rb"));
	if (fp == nullptr)
	{
		return false;
	}
	Fseek(fp, 0, SEEK_END);
	n64 nFileSize = Ftell(fp);
	Fseek(fp, 0, SEEK_SET);
	const size_t uBufferSize = 0x100000;
	n32 nBufferCount = static_cast<n32>(nFileSize / uBufferSize);
	static u8 uBuffer[uBufferSize] = {};
	if (!UMakeDir(m_sDirName.c_str()))
	{
		fclose(fp);
		return false;
	}
	for (n32 i = 0; i < nBufferCount; i++)
	{
		UString sFileName = Format(USTR("%") PRIUS USTR("/%08d.bin"), m_sDirName.c_str(), i);
		FILE* fpSub = UFopen(sFileName.c_str(), USTR("wb"));
		if (fpSub == nullptr)
		{
			fclose(fp);
			return false;
		}
		if (m_bVerbose)
		{
			UPrintf(USTR("save: %") PRIUS USTR("\n"), sFileName.c_str());
		}
		fread(uBuffer, 1, uBufferSize, fp);
		fwrite(uBuffer, 1, uBufferSize, fpSub);
		fclose(fpSub);
	}
	if (nFileSize % uBufferSize != 0)
	{
		nFileSize %= uBufferSize;
		UString sFileName = Format(USTR("%") PRIUS USTR("/%08d.bin"), m_sDirName.c_str(), nBufferCount);
		FILE* fpSub = UFopen(sFileName.c_str(), USTR("wb"));
		if (fpSub == nullptr)
		{
			fclose(fp);
			return false;
		}
		fread(uBuffer, 1, static_cast<u32>(nFileSize), fp);
		fwrite(uBuffer, 1, static_cast<u32>(nFileSize), fpSub);
		fclose(fpSub);
	}
	fclose(fp);
	return true;
}

bool CFileTool::joinFile()
{
	FILE* fp = UFopen(m_sFileName.c_str(), USTR("wb"));
	if (fp == nullptr)
	{
		return false;
	}
	for (n32 i = 0; ; i++)
	{
		UString sFileName = Format(USTR("%") PRIUS USTR("/%08d.bin"), m_sDirName.c_str(), i);
		FILE* fpSub = UFopen(sFileName.c_str(), USTR("rb"), false);
		if (fpSub == nullptr)
		{
			break;
		}
		if (m_bVerbose)
		{
			UPrintf(USTR("load: %") PRIUS USTR("\n"), sFileName.c_str());
		}
		fseek(fpSub, 0, SEEK_END);
		u32 uBufferSize = ftell(fpSub);
		fseek(fpSub, 0, SEEK_SET);
		u8* pBuffer = new u8[uBufferSize];
		fread(pBuffer, 1, uBufferSize, fpSub);
		fclose(fpSub);
		fwrite(pBuffer, 1, uBufferSize, fp);
		delete[] pBuffer;
	}
	fclose(fp);
	return true;
}

int UMain(int argc, UChar* argv[])
{
	CFileTool tool;
	if (tool.ParseOptions(argc, argv) != 0)
	{
		return tool.Help();
	}
	if (tool.CheckOptions() != 0)
	{
		return 1;
	}
	return tool.Action();
}
