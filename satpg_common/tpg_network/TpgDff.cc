
/// @file TpgDff.cc
/// @brief TpgDff の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "TpgDff.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgDff
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgDff::TpgDff()
{
  mInput = nullptr;
  mOutput = nullptr;
  mClock = nullptr;
  mClear = nullptr;
  mPreset = nullptr;
}

// @brief デストラクタ
TpgDff::~TpgDff()
{
}

// @brief ID番号を返す．
ymuint
TpgDff::id() const
{
  return mId;
}

// @brief 入力端子のノードを返す．
TpgNode*
TpgDff::input() const
{
  return mInput;
}

// @brief 出力端子のノードを返す．
TpgNode*
TpgDff::output() const
{
  return mOutput;
}

// @brief クロック端子のノードを返す．
TpgNode*
TpgDff::clock() const
{
  return mClock;
}

// @brief クリア端子のノードを返す．
TpgNode*
TpgDff::clear() const
{
  return mClear;
}

// @brief プリセット端子のノードを返す．
TpgNode*
TpgDff::preset() const
{
  return mPreset;
}

END_NAMESPACE_YM_SATPG
