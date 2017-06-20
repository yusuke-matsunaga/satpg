
/// @file SnInput.cc
/// @brief SnInput の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SnInput.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM2

//////////////////////////////////////////////////////////////////////
// SnInput
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnInput::SnInput(ymuint32 id) :
  SimNode(id)
{
  set_level(0);
}

// @brief デストラクタ
SnInput::~SnInput()
{
}

// @brief ゲートタイプを返す．
//
// ここでは kGateBUFF を返す．
GateType
SnInput::gate_type() const
{
  return kGateBUFF;
}

// @brief ファンイン数を得る．
ymuint
SnInput::nfi() const
{
  return 0;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnInput::fanin(ymuint pos) const
{
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnInput::_calc_hval()
{
  return kPvAll0;
}

// @brief 正常値の計算を行う．
PackedVal
SnInput::_calc_gval()
{
  return kPvAll0;
}

// @brief 故障値の計算を行う．
PackedVal
SnInput::_calc_fval()
{
  return kPvAll0;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnInput::calc_gobs(ymuint ipos)
{
  return kPvAll0;
}

// @brief 内容をダンプする．
void
SnInput::dump(ostream& s) const
{
  s << "INPUT" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnBuff
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnBuff::SnBuff(ymuint32 id,
	       const vector<SimNode*>& inputs) :
  SnGate1(id, inputs)
{
}

// @brief デストラクタ
SnBuff::~SnBuff()
{
}

// @brief ゲートタイプを返す．
GateType
SnBuff::gate_type() const
{
  return kGateBUFF;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnBuff::_calc_hval()
{
  return mFanin->hval();
}

// @brief 正常値の計算を行う．
PackedVal
SnBuff::_calc_gval()
{
  return mFanin->gval();
}

// @brief 故障値の計算を行う．
PackedVal
SnBuff::_calc_fval()
{
  return mFanin->fval();
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnBuff::calc_gobs(ymuint ipos)
{
  return kPvAll1;
}

// @brief 内容をダンプする．
void
SnBuff::dump(ostream& s) const
{
  s << "BUFF(" << mFanin->id() << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnNot
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnNot::SnNot(ymuint32 id,
	     const vector<SimNode*>& inputs) :
  SnBuff(id, inputs)
{
}

// @brief デストラクタ
SnNot::~SnNot()
{
}

// @brief ゲートタイプを返す．
GateType
SnNot::gate_type() const
{
  return kGateNOT;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnNot::_calc_hval()
{
  return ~mFanin->hval();
}

// @brief 正常値の計算を行う．
PackedVal
SnNot::_calc_gval()
{
  return ~mFanin->gval();
}

// @brief 故障値の計算を行う．
PackedVal
SnNot::_calc_fval()
{
  return ~mFanin->fval();
}

// @brief 内容をダンプする．
void
SnNot::dump(ostream& s) const
{
  s << "NOT(" << mFanin->id() << ")" << endl;
}

END_NAMESPACE_YM_SATPG_FSIM2
