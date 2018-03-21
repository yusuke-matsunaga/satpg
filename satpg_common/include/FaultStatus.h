#ifndef FAULTSTATUS_H
#define FAULTSTATUS_H

/// @file FaultStatus.h
/// @brief FaultStatus のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2007, 2012-2014, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
/// @brief 故障の状態
//////////////////////////////////////////////////////////////////////
enum class FaultStatus {
  /// @brief 未検出
  Undetected,
  /// @brief 検出
  Detected,
  /// @brief テスト不能 (冗長)
  Untestable,
  /// @brief アボート
  Aborted
};


/// @brief 内容を表す文字列を返す．
inline
const char*
str(FaultStatus fs)
{
  switch ( fs ) {
  case FaultStatus::Undetected: return "undetected";
  case FaultStatus::Detected:   return "detected";
  case FaultStatus::Untestable: return "untestable";
  case FaultStatus::Aborted:    return "aborted";
  default: break;
  }
  ASSERT_NOT_REACHED;
  return "";
}

END_NAMESPACE_YM_SATPG

#endif // FAULTSTATUS_H
