//
//  test_config.cpp
//  Defines every extern variable the engine expects from the host app.
//  Defaults mirror the macOS app (AppDelegate.m) so behaviour matches production.
//
#include "Engine.h"
#include "Vietnamese.h"
#include "Macro.h"
#include "SmartSwitchKey.h"
#include "ConvertTool.h"

// Engine.h externs
int vLanguage = 1;              // 1: Vietnamese
int vInputType = 0;             // 0: Telex
int vFreeMark = 0;              // 0: No (restrict free mark)
int vCodeTable = 0;             // 0: Unicode
int vSwitchKeyStatus = 0x7A000206;
int vCheckSpelling = 1;         // 1: Yes
int vUseModernOrthography = 1;  // 1: oà / uý
int vQuickTelex = 0;
int vRestoreIfWrongSpelling = 0;
int vFixRecommendBrowser = 1;
int vUseMacro = 0;
int vUseMacroInEnglishMode = 0;
int vAutoCapsMacro = 0;
int vUseSmartSwitchKey = 0;
int vUpperCaseFirstChar = 0;
int vTempOffSpelling = 0;
int vAllowConsonantZFWJ = 0;
int vQuickStartConsonant = 0;
int vQuickEndConsonant = 0;
int vRememberCode = 0;
int vOtherLanguage = 0;
int vTempOffOpenKey = 0;