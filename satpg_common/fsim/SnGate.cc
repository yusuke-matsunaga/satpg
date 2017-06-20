
/// @file SnGate.cc
/// @brief SnGate の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SnGate.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

BEGIN_NONAMESPACE

// ゲート名を出力する．
void
dump_gate(ostream& s,
	  GateType gate_type)
{
  switch ( gate_type ) {
  case kGateCONST0: s << "CONST0"; break;
  case kGateCONST1: s << "CONST1"; break;
  case kGateBUFF:   s << "BUFF";   break;
  case kGateNOT:    s << "NOT";    break;
  case kGateAND:    s << "AND";    break;
  case kGateNAND:   s << "NAND";   break;
  case kGateOR:     s << "OR";     break;
  case kGateNOR:    s << "NOR";    break;
  case kGateXOR:    s << "XOR";    break;
  case kGateXNOR:   s << "XNOR";   break;
  default: ASSERT_NOT_REACHED;
  }
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// @class SnGate SimNode.h
// @brief 多入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate::SnGate(ymuint id,
	       const vector<SimNode*>& inputs) :
  SimNode(id),
  mFaninNum(inputs.size()),
  mFanins(new SimNode*[mFaninNum])
{
  ymuint max_level = 0;
  for (ymuint i = 0; i < mFaninNum; ++ i) {
    SimNode* input = inputs[i];
    ASSERT_COND(input );
    mFanins[i] = input;
    ymuint level = input->level() + 1;
    if ( max_level < level ) {
      max_level = level;
    }
  }
  set_level(max_level);
}

// @brief デストラクタ
SnGate::~SnGate()
{
  delete [] mFanins;
}

// @brief ファンイン数を得る．
ymuint
SnGate::fanin_num() const
{
  return _fanin_num();
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate::fanin(ymuint pos) const
{
  ASSERT_COND( pos < _fanin_num() );
  return _fanin(pos);
}

// @brief 内容をダンプする．
void
SnGate::dump(ostream& s) const
{
  dump_gate(s, gate_type());
  s << "(" << _fanin(0)->id();
  ymuint n = _fanin_num();
  for (ymuint i = 1; i < n; ++ i) {
    s << ", " << _fanin(i)->id();
  }
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate1 SimNode.h
// @brief 1入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate1::SnGate1(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 1 );
  ASSERT_COND(inputs[0] );
  mFanin = inputs[0];
  set_level(mFanin->level() + 1);
}

// @brief デストラクタ
SnGate1::~SnGate1()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate1::fanin_num() const
{
  return 1;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate1::fanin(ymuint pos) const
{
  return mFanin;
}

// @brief 内容をダンプする．
void
SnGate1::dump(ostream& s) const
{
  dump_gate(s, gate_type());
  s << "(" << _fanin()->id() << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate2 SimNode.h
// @brief 2入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate2::SnGate2(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 2 );
  ASSERT_COND(inputs[0] );
  ASSERT_COND(inputs[1] );
  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  ymuint level = mFanins[0]->level();
  if ( level < mFanins[1]->level() ) {
    level = mFanins[1]->level();
  }
  set_level(level + 1);
}

// @brief デストラクタ
SnGate2::~SnGate2()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate2::fanin_num() const
{
  return 2;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate2::fanin(ymuint pos) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate2::dump(ostream& s) const
{
  dump_gate(s, gate_type());
  s << "2(" << _fanin(0)->id();
  s << ", " << _fanin(1)->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate3 SimNode.h
// @brief 3入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate3::SnGate3(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 3 );
  ASSERT_COND(inputs[0] );
  ASSERT_COND(inputs[1] );
  ASSERT_COND(inputs[2] );
  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  ymuint level = mFanins[0]->level();
  if ( level < mFanins[1]->level() ) {
    level = mFanins[1]->level();
  }
  if ( level < mFanins[2]->level() ) {
    level = mFanins[2]->level();
  }
  set_level(level + 1);
}

// @brief デストラクタ
SnGate3::~SnGate3()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate3::fanin_num() const
{
  return 3;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate3::fanin(ymuint pos) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate3::dump(ostream& s) const
{
  dump_gate(s, gate_type());
  s << "3(" << _fanin(0)->id();
  s << ", " << _fanin(1)->id();
  s << ", " << _fanin(2)->id();
  s << ")" << endl;
}


//////////////////////////////////////////////////////////////////////
// @class SnGate4 SimNode.h
// @brief 4入力ゲートの基底クラス
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SnGate4::SnGate4(ymuint id,
		 const vector<SimNode*>& inputs) :
  SimNode(id)
{
  ASSERT_COND(inputs.size() == 4 );
  ASSERT_COND(inputs[0] );
  ASSERT_COND(inputs[1] );
  ASSERT_COND(inputs[2] );
  ASSERT_COND(inputs[3] );
  mFanins[0] = inputs[0];
  mFanins[1] = inputs[1];
  mFanins[2] = inputs[2];
  mFanins[3] = inputs[3];
  ymuint level = mFanins[0]->level();
  if ( level < mFanins[1]->level() ) {
    level = mFanins[1]->level();
  }
  if ( level < mFanins[2]->level() ) {
    level = mFanins[2]->level();
  }
  if ( level < mFanins[3]->level() ) {
    level = mFanins[3]->level();
  }
  set_level(level + 1);
}

// @brief デストラクタ
SnGate4::~SnGate4()
{
}

// @brief ファンイン数を得る．
ymuint
SnGate4::fanin_num() const
{
  return 4;
}

// @brief pos 番めのファンインを得る．
SimNode*
SnGate4::fanin(ymuint pos) const
{
  return mFanins[pos];
}

// @brief 内容をダンプする．
void
SnGate4::dump(ostream& s) const
{
  dump_gate(s, gate_type());
  s << "4(" << _fanin(0)->id();
  s << ", " << _fanin(1)->id();
  s << ", " << _fanin(2)->id();
  s << ", " << _fanin(3)->id();
  s << ")" << endl;
}

END_NAMESPACE_YM_SATPG_FSIM
