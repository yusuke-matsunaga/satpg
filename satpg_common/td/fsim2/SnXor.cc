
/// @file SnXor.cc
/// @brief SnXor の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SnXor.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM2

//////////////////////////////////////////////////////////////////////
// SnXor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXor::SnXor(ymuint32 id,
	     const vector<SimNode*>& inputs) :
  SnGate(id, inputs)
{
}

// @brief デストラクタ
SnXor::~SnXor()
{
}

// @brief ゲートタイプを返す．
GateType
SnXor::gate_type() const
{
  return kGateXOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnXor::_calc_hval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->hval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val ^= mFanins[i]->hval();
  }
  return new_val;
}

// @brief 正常値の計算を行う．
PackedVal
SnXor::_calc_gval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->gval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val ^= mFanins[i]->gval();
  }
  return new_val;
}

// @brief 故障値の計算を行う．
PackedVal
SnXor::_calc_fval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->fval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val ^= mFanins[i]->fval();
  }
  return new_val;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnXor::calc_gobs(ymuint ipos)
{
  return kPvAll1;
}

// @brief 内容をダンプする．
void
SnXor::dump(ostream& s) const
{
  ymuint n = mNfi;
  s << "XOR(" << mFanins[0]->id();
  for (ymuint i = 1; i < n; ++ i) {
    s << ", " << mFanins[i]->id();
  }
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnXor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXor2::SnXor2(ymuint32 id,
	       const vector<SimNode*>& inputs) :
  SnGate2(id, inputs)
{
}

// @brief デストラクタ
SnXor2::~SnXor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnXor2::gate_type() const
{
  return kGateXOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnXor2::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  return pat0 ^ pat1;
}

// @brief 正常値の計算を行う．
PackedVal
SnXor2::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  return pat0 ^ pat1;
}

// @brief 故障値の計算を行う．
PackedVal
SnXor2::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  return pat0 ^ pat1;
}

// @brief ゲートの入力から出力までの可観測性を計算する．
PackedVal
SnXor2::calc_gobs(ymuint ipos)
{
  return kPvAll1;
}

// @brief 内容をダンプする．
void
SnXor2::dump(ostream& s) const
{
  s << "XOR2(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnXnor
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXnor::SnXnor(ymuint32 id,
	       const vector<SimNode*>& inputs) :
  SnXor(id, inputs)
{
}

// @brief デストラクタ
SnXnor::~SnXnor()
{
}

// @brief ゲートタイプを返す．
GateType
SnXnor::gate_type() const
{
  return kGateXNOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnXnor::_calc_hval()
{
  ymuint n = mNfi;
  PackedVal val = mFanins[0]->hval();
  for (ymuint i = 1; i < n; ++ i) {
    val ^= mFanins[i]->hval();
  }
  return ~val;
}

// @brief 正常値の計算を行う．
PackedVal
SnXnor::_calc_gval()
{
  ymuint n = mNfi;
  PackedVal val = mFanins[0]->gval();
  for (ymuint i = 1; i < n; ++ i) {
    val ^= mFanins[i]->gval();
  }
  return ~val;
}

// @brief 故障値の計算を行う．
PackedVal
SnXnor::_calc_fval()
{
  ymuint n = mNfi;
  PackedVal new_val = mFanins[0]->fval();
  for (ymuint i = 1; i < n; ++ i) {
    new_val ^= mFanins[i]->fval();
  }
  return ~new_val;
}

// @brief 内容をダンプする．
void
SnXnor::dump(ostream& s) const
{
  ymuint n = mNfi;
  s << "XNOR(" << mFanins[0]->id();
  for (ymuint i = 1; i < n; ++ i) {
    s << ", " << mFanins[i]->id();
  }
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// SnXnor2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnXnor2::SnXnor2(ymuint32 id,
		 const vector<SimNode*>& inputs) :
  SnXor2(id, inputs)
{
}

// @brief デストラクタ
SnXnor2::~SnXnor2()
{
}

// @brief ゲートタイプを返す．
GateType
SnXnor2::gate_type() const
{
  return kGateXNOR;
}

// @brief 1時刻前の正常値の計算を行う．
PackedVal
SnXnor2::_calc_hval()
{
  PackedVal pat0 = mFanins[0]->hval();
  PackedVal pat1 = mFanins[1]->hval();
  return ~(pat0 ^ pat1);
}

// @brief 正常値の計算を行う．
PackedVal
SnXnor2::_calc_gval()
{
  PackedVal pat0 = mFanins[0]->gval();
  PackedVal pat1 = mFanins[1]->gval();
  return ~(pat0 ^ pat1);
}

// @brief 故障値の計算を行う．
PackedVal
SnXnor2::_calc_fval()
{
  PackedVal pat0 = mFanins[0]->fval();
  PackedVal pat1 = mFanins[1]->fval();
  return ~(pat0 ^ pat1);
}

// @brief 内容をダンプする．
void
SnXnor2::dump(ostream& s) const
{
  s << "XNOR2(" << mFanins[0]->id();
  s << ", " << mFanins[1]->id();
  s << ")" << endl;
}

END_NAMESPACE_YM_SATPG_FSIM2
