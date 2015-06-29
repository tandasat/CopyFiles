﻿// Copyright (c) 2015, tandasat. All rights reserved.
// Use of this source code is governed by a MIT-style license that can be
// found in the LICENSE file.

//
// This module declares interfaces to the CopyFileIterator class.
//
#pragma once

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

namespace CopyFilesNative {

public ref class CopyFileIterator sealed
{
public:
  CopyFileIterator(Platform::String ^ QueryPath, bool OnlyListing);
  Platform::String ^ NextFile();
  uint64 GetLastFileSize();

private:
  class Impl;
  std::shared_ptr<Impl> m_Impl;
};

}

////////////////////////////////////////////////////////////////////////////////
//
// prototypes
//

////////////////////////////////////////////////////////////////////////////////
//
// variables
//

////////////////////////////////////////////////////////////////////////////////
//
// implementations
//