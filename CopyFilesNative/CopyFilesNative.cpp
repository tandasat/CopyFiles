// Copyright (c) 2015, tandasat. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.

//
// This module declares implements the CopyFileIterator class
//
#include "pch.h"
#include "CopyFilesNative.h"

namespace CopyFilesNative {
////////////////////////////////////////////////////////////////////////////////
//
// macro utilities
//

////////////////////////////////////////////////////////////////////////////////
//
// constants and macros
//

////////////////////////////////////////////////////////////////////////////////
//
// types
//

class CopyFileIterator::Impl {
 public:
  explicit Impl(const std::wstring &QueryPath, bool OnlyListing);
  std::wstring NextFile();
  uint64 GetLastFileSize() const;

 private:
  bool m_OnlyListing;
  std::wstring m_QueryFolder;
  std::shared_ptr<std::remove_pointer_t<HANDLE>> m_Handle;
  bool m_IsCurrentFileConsumed;
  WIN32_FIND_DATA m_FindFileData;
};

////////////////////////////////////////////////////////////////////////////////
//
// prototypes
//

bool CopyToLocalFolder(const wchar_t *SourceFilePath, const wchar_t *FileName);

////////////////////////////////////////////////////////////////////////////////
//
// variables
//

////////////////////////////////////////////////////////////////////////////////
//
// implementations
//

//
CopyFileIterator::CopyFileIterator(Platform::String ^ QueryPath,
                                   bool OnlyListing)
    : m_Impl(std::make_shared<Impl>(QueryPath->Data(), OnlyListing)) {}

// Returns next file name found, or "" if not found
Platform::String ^ CopyFileIterator::NextFile() {
  const auto foundFile = m_Impl->NextFile();
  return ref new Platform::String(foundFile.c_str());
}

// Returns a size of the file the last found
uint64 CopyFileIterator::GetLastFileSize() { return m_Impl->GetLastFileSize(); }

// Copies a file to the local folder
bool CopyToLocalFolder(const wchar_t *SourceFilePath, const wchar_t *FileName) {
  std::wstring localPath =
      ::Windows::Storage::ApplicationData::Current->LocalFolder->Path->Data();
  localPath += L"\\";
  if (S_OK !=
      ::CopyFile2(SourceFilePath, (localPath + FileName).c_str(), nullptr)) {
    LOG_DEBUG("Failed %S", SourceFilePath);
    return false;
  }
  LOG_DEBUG("Copied %S", SourceFilePath);
  return true;
}

// Creates a search HANDLE
CopyFileIterator::Impl::Impl(const std::wstring &QueryPath, bool OnlyListing)
    : m_OnlyListing(OnlyListing),
      m_QueryFolder(QueryPath.substr(0, QueryPath.find_last_of(L"\\")) + L"\\"),
      m_Handle(std::shared_ptr<std::remove_pointer_t<HANDLE>>(
          ::FindFirstFileEx(QueryPath.c_str(), FindExInfoBasic, &m_FindFileData,
                            FindExSearchNameMatch, nullptr, 0),
          &::FindClose)),
      m_IsCurrentFileConsumed(false) {
  if (!m_Handle.get()) {
    throw std::runtime_error("FindFirstFileEx() failed.");
  }
}

// Finds a next file. If it was found, copies it to a local folder and returns
// its path. If no more file was found, returns "".
std::wstring CopyFileIterator::Impl::NextFile() {
  if (m_IsCurrentFileConsumed) {
    if (!::FindNextFile(m_Handle.get(), &m_FindFileData)) {
      return {};
    }
  }

  // Sleep(2000); // Emulate delay with IO

  std::wstring foundFile;
  for (BOOL shouldContinue = TRUE; shouldContinue;
       shouldContinue = ::FindNextFile(m_Handle.get(), &m_FindFileData)) {
    if (m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ||
        m_FindFileData.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT ||
        ::wcscmp(m_FindFileData.cFileName, L".") == 0 ||
        ::wcscmp(m_FindFileData.cFileName, L"..") == 0) {
      continue;
    }

    foundFile = m_FindFileData.cFileName;
    if (!m_OnlyListing &&
        !CopyToLocalFolder((m_QueryFolder + foundFile).c_str(),
                           m_FindFileData.cFileName)) {
      continue;
    }
    break;
  }
  m_IsCurrentFileConsumed = true;
  return foundFile;
}

uint64 CopyFileIterator::Impl::GetLastFileSize() const {
  ULARGE_INTEGER size = {};
  size.LowPart = m_FindFileData.nFileSizeLow;
  size.HighPart = m_FindFileData.nFileSizeHigh;
  return size.QuadPart;
}

}  // namespace CopyFilesNative
