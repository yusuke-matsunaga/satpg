﻿#ifndef FSIM3_NSDEF_H
#define FSIM3_NSDEF_H

/// @file fsim3_nsdef.h
/// @brief Fsim3 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

// namespace でネストするのがいやなので define マクロでごまかす．

#include "sa/sa_nsdef.h"

#define BEGIN_NAMESPACE_YM_SATPG_FSIM \
BEGIN_NAMESPACE_YM_SATPG_SA \
BEGIN_NAMESPACE(nsFsim3)

#define END_NAMESPACE_YM_SATPG_FSIM \
END_NAMESPACE(nsFsim3) \
END_NAMESPACE_YM_SATPG_SA

#endif // FSIM3_NSDEF_H
