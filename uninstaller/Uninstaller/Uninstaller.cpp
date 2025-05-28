#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <string>
#include <vector>
#include <stdexcept>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Shell32.lib")
#pragma comment(lib, "Shlwapi.lib")

static void DeleteFileIfExists(const std::string& filePath) {
    if (PathFileExistsA(filePath.c_str())) {
        DeleteFileA(filePath.c_str());
    }
    else {
        MessageBoxA(NULL, ("File not found: " + filePath).c_str(), "nca Uninstaller", MB_OK | MB_ICONWARNING);
    }
}

static void DeleteFolderIfExists(const std::string& folderPath) {
    if (PathFileExistsA(folderPath.c_str())) {
        SHFILEOPSTRUCTA fileStructure = { 0 };
        std::string pathWithNull = folderPath + '\0';

        fileStructure.wFunc = FO_DELETE;
        fileStructure.pFrom = pathWithNull.c_str();
        fileStructure.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_NOERRORUI;

        SHFileOperationA(&fileStructure);
    }
    else {
        MessageBoxA(NULL, ("Folder not found: " + folderPath).c_str(), "nca Uninstaller", MB_OK | MB_ICONWARNING);
    }
}

static void DeleteGameData() {
    char gamePath[MAX_PATH];

    GetModuleFileNameA(NULL, gamePath, MAX_PATH);
    PathRemoveFileSpecA(gamePath);
    PathAppendA(gamePath, "..");

    std::vector<std::string> folderPaths;
    {
        char tempPath[MAX_PATH];
        PathCombineA(tempPath, gamePath, "Nameless Cat Architect_Data");
        folderPaths.push_back(tempPath);
        PathCombineA(tempPath, gamePath, "MonoBleedingEdge");
        folderPaths.push_back(tempPath);
    }

    std::vector<std::string> filePaths;
    {
        char tempPath[MAX_PATH];
        PathCombineA(tempPath, gamePath, "Nameless Cat Architect.exe");
        filePaths.push_back(tempPath);
        PathCombineA(tempPath, gamePath, "UnityCrashHandler64.exe");
        filePaths.push_back(tempPath);
        PathCombineA(tempPath, gamePath, "UnityPlayer.dll");
        filePaths.push_back(tempPath);
    }

    for (const auto& folder : folderPaths) {
        DeleteFolderIfExists(folder);
    }

    for (const auto& file : filePaths) {
        DeleteFileIfExists(file);
    }
}

static void DeleteLevelData() {
    char roamingAppData[MAX_PATH];

    SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, roamingAppData);

    char appData[MAX_PATH];
    strcpy_s(appData, roamingAppData);
    PathAppendA(appData, "..");

    char ncaDataPath[MAX_PATH];
    PathCombineA(ncaDataPath, appData, "LocalLow\\iwtoq\\Nameless Cat Architect");

    DeleteFolderIfExists(ncaDataPath);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    try {
        int confirm = MessageBoxA(NULL,
            "Are you sure you want to uninstall Nameless Cat Architect?",
            "nca Uninstaller",
            MB_YESNO | MB_ICONQUESTION);
        if (confirm == IDNO)
            return 0;

        DeleteGameData();

        int deleteLevel = MessageBoxA(NULL,
            "Do you want to delete level data?\nIncludes your levels in the editor, imported levels and settings. Does not include exported levels\nDO NOT DO THIS IF YOU'RE JUST UPDATING",
            "nca Uninstaller",
            MB_YESNO | MB_ICONQUESTION);

        if (deleteLevel == IDYES)
            DeleteLevelData();

        MessageBoxA(NULL, "Uninstallation complete.\nNow delete the uninstaller", "nca Uninstaller", MB_OK | MB_ICONINFORMATION);
    }
    catch (const std::exception& ex) {
        std::string errorMsg = "Error: ";
        errorMsg += ex.what();
        MessageBoxA(NULL, errorMsg.c_str(), "nca Uninstaller", MB_OK | MB_ICONERROR);
    }

    return 0;
}