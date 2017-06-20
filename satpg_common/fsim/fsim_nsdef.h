#ifndef FSIM_NSDEF_H
#define FSIM_NSDEF_H

/// @file fsim_nsdef.h
/// @brief Fsim 用の名前空間の定義
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2017 Yusuke Matsunaga
/// All rights reserved.

// 名前空間の定義ファイル
// 必ず先頭でインクルードしなければならない．

// namespace でネストするのがいやなので define マクロでごまかす．

#include "satpg.h"

#if FSIM_VAL2
#define FSIM_NAMESPACE nsFsim2
#define FSIM_CLASSNAME Fsim2
#define FSIM_VALTYPE PackedVal
#elif FSIM_VAL3
#define FSIM_NAMESPACE nsFsim3
#define FSIM_CLASSNAME Fsim3
#define FSIM_VALTYPE PackedVal3
#else
#error "Neither FSIM_VAL2 nor FSIM_VAL3 are not set"
#endif

#define BEGIN_NAMESPACE_YM_SATPG_FSIM \
BEGIN_NAMESPACE_YM_SATPG \
BEGIN_NAMESPACE(FSIM_NAMESPACE)

#define END_NAMESPACE_YM_SATPG_FSIM \
END_NAMESPACE(FSIM_NAMESPACE) \
END_NAMESPACE_YM_SATPG

#endif // FSIM_NSDEF_H
