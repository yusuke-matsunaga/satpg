#ifndef DTPG_NSDEF_H
#define DTPG_NSDEF_H

/// @file dtpg_nsdef.h
/// @brief dtpg_nsdef のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#include "satpg.h"

//////////////////////////////////////////////////////////////////////
// 名前空間の定義用マクロ
// namespace でネストするのがいやなので define マクロでごまかす．
//////////////////////////////////////////////////////////////////////

#define BEGIN_NAMESPACE_YM_SATPG_DTPG \
BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE(nsDtpg)

#define END_NAMESPACE_YM_SATPG_DTPG \
END_NAMESPACE(nsDtpg) \
END_NAMESPACE_YM_SATPG

#endif // DTPG_NSDEF_H
