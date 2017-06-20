
/// @file TpgGateInfo.cc
/// @brief TpgGateInfo の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "TpgGateInfo.h"
#include "SimpleGateInfo.h"
#include "CplxGateInfo.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// 論理式中の演算子の数を数える．
ymuint
count_expr(const Expr& expr)
{
  if ( !expr.is_op() ) {
    return 0;
  }

  ymuint n = 1;
  ymuint nc = expr.child_num();
  for (ymuint i = 0; i < nc; ++ i) {
    n += count_expr(expr.child(i));
  }
  return n;
}

// 追加で必要なノード数を数える．
ymuint
extra_node_count(const Expr& expr,
		 ymuint ni)
{
  // まず入力部分に挿入されるノード数を数える．
  ymuint n = 0;
  for (ymuint i = 0; i < ni; ++ i) {
    ymuint p_num = expr.litnum(VarId(i), false);
    ymuint n_num = expr.litnum(VarId(i), true);
    ASSERT_COND( p_num > 0 || n_num > 0 );
    if ( n_num == 0 ) {
      if ( p_num > 1 ) {
	n += 1;
      }
    }
    else { // n_num > 0
      if ( p_num > 0 ) {
	n += 2;
      }
      else {
	n += 1;
      }
    }
  }

  // 次に論理式自体で必要となるノード数を数える．
  // ただし，根のノードはカウント済みなので1つ減らす．
  n += count_expr(expr) - 1;

  return n;
}

// calc_c_val の下請け関数
Val3
ccv_sub(const Expr& expr,
	const vector<Val3>& ivals)
{
  if ( expr.is_zero() ) {
    return kVal0;
  }
  if ( expr.is_one() ) {
    return kVal1;
  }
  if ( expr.is_posiliteral() ) {
    ymuint iid = expr.varid().val();
    return ivals[iid];
  }
  if ( expr.is_negaliteral() ) {
    ymuint iid = expr.varid().val();
    return ~ivals[iid];
  }

  ymuint nc = expr.child_num();
  if ( expr.is_and() ) {
    bool has_x = false;
    for (ymuint i = 0; i < nc; ++ i) {
      Val3 ival = ccv_sub(expr.child(i), ivals);
      if ( ival == kVal0 ) {
	return kVal0;
      }
      if ( ival == kValX ) {
	has_x = true;
      }
    }
    if ( has_x ) {
      return kValX;
    }
    return kVal1;
  }

  if ( expr.is_or() ) {
    bool has_x = false;
    for (ymuint i = 0; i < nc; ++ i) {
      Val3 ival = ccv_sub(expr.child(i), ivals);
      if ( ival == kVal1 ) {
	return kVal1;
      }
      if ( ival == kValX ) {
	has_x = true;
      }
    }
    if ( has_x ) {
      return kValX;
    }
    return kVal0;
  }

  if ( expr.is_or() ) {
    Val3 val = kVal0;
    for (ymuint i = 0; i < nc; ++ i) {
      Val3 ival = ccv_sub(expr.child(i), ivals);
      if ( ival == kValX ) {
	return kValX;
      }
      val = val ^ ival;
    }
    return val;
  }

  ASSERT_NOT_REACHED;
  return kValX;
}

// 制御値の計算を行う．
Val3
calc_c_val(const Expr& expr,
	   ymuint ni,
	   ymuint ipos,
	   Val3 val)
{
  vector<Val3> ivals(ni, kValX);
  ivals[ipos] = val;
  return ccv_sub(expr, ivals);
}

// 制御値の計算を行う．
Val3
c_val(GateType gate_type,
      Val3 ival)
{
  switch ( gate_type ) {
  case kGateCONST0:
  case kGateCONST1:
    // 常に X
    return kValX;

  case kGateBUFF:
    // そのまま返す．
    return ival;

  case kGateNOT:
    // 反転して返す．
    return ~ival;

  case kGateAND:
    // 0 の時のみ 0
    return ival == kVal0 ? kVal0 : kValX;

  case kGateNAND:
    // 0 の時のみ 1
    return ival == kVal0 ? kVal1 : kValX;

  case kGateOR:
    // 1 の時のみ 1
    return ival == kVal1 ? kVal1 : kValX;

  case kGateNOR:
    // 1 の時のみ 0
    return ival == kVal1 ? kVal0 : kValX;

  case kGateXOR:
  case kGateXNOR:
    // 常に X
    return kValX;

  default:
    break;
  }
  ASSERT_NOT_REACHED;
  return kValX;
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス SimpleGateInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] gate_type ゲートタイプ
SimpleGateInfo::SimpleGateInfo(GateType gate_type) :
  mGateType(gate_type)
{
  mCVal[0] = c_val(gate_type, kVal0);
  mCVal[1] = c_val(gate_type, kVal1);
}

// @brief デストラクタ
SimpleGateInfo::~SimpleGateInfo()
{
}

// @brief ゲートタイプを返す．
GateType
SimpleGateInfo::gate_type() const
{
  return mGateType;
}

// @brief 論理式を返す．
Expr
SimpleGateInfo::expr() const
{
  // ダミー
  return Expr::make_zero();
}

// @brief 追加ノード数を返す．
ymuint
SimpleGateInfo::extra_node_num() const
{
  return 0;
}

// @brief 制御値を返す．
// @param[in] pos 入力位置
// @param[in] val 値
Val3
SimpleGateInfo::cval(ymuint pos,
		     Val3 val) const
{
  int bval = (val == kVal0) ? 0 : 1;
  return mCVal[bval];
}


//////////////////////////////////////////////////////////////////////
// クラス CplxGateInfo
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] expr 論理式
CplxGateInfo::CplxGateInfo(ymuint ni,
			   const Expr& expr) :
  mExpr(expr),
  mCVal(ni * 2)
{
  mExtraNodeNum = extra_node_count(expr, ni);
  for (ymuint i = 0; i < ni; ++ i) {
    mCVal[i * 2 + 0] = calc_c_val(expr, ni, i, kVal0);
    mCVal[i * 2 + 1] = calc_c_val(expr, ni, i, kVal1);
  }
}

// @brief デストラクタ
CplxGateInfo::~CplxGateInfo()
{
}

// @brief ゲートタイプを返す．
GateType
CplxGateInfo::gate_type() const
{
  return kGateCPLX;
}

// @brief 論理式を返す．
Expr
CplxGateInfo::expr() const
{
  return mExpr;
}

// @brief 追加ノード数を返す．
ymuint
CplxGateInfo::extra_node_num() const
{
  return mExtraNodeNum;
}

// @brief 制御値を返す．
// @param[in] pos 入力位置
// @param[in] val 値
Val3
CplxGateInfo::cval(ymuint pos,
		   Val3 val) const
{
  int bval = (val == kVal0) ? 0 : 1;
  return mCVal[pos * 2 + bval];
}


//////////////////////////////////////////////////////////////////////
// クラス TpgGateInfoMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
TpgGateInfoMgr::TpgGateInfoMgr()
{
  mSimpleType[0] = new SimpleGateInfo(kGateCONST0);
  mSimpleType[1] = new SimpleGateInfo(kGateCONST1);
  mSimpleType[2] = new SimpleGateInfo(kGateBUFF);
  mSimpleType[3] = new SimpleGateInfo(kGateNOT);
  mSimpleType[4] = new SimpleGateInfo(kGateAND);
  mSimpleType[5] = new SimpleGateInfo(kGateNAND);
  mSimpleType[6] = new SimpleGateInfo(kGateOR);
  mSimpleType[7] = new SimpleGateInfo(kGateNOR);
  mSimpleType[8] = new SimpleGateInfo(kGateXOR);
  mSimpleType[9] = new SimpleGateInfo(kGateXNOR);
}

// @brief デストラクタ
TpgGateInfoMgr::~TpgGateInfoMgr()
{
  for (ymuint i = 0; i < 10; ++ i) {
    delete mSimpleType[i];
  }
  for (ymuint i = 0; i < mList.size(); ++ i) {
    delete mList[i];
  }
}

// @brief 組み込み型のオブジェクトを返す．
// @param[in] gate_type ゲートタイプ
const TpgGateInfo*
TpgGateInfoMgr::simple_type(GateType gate_type)
{
  switch ( gate_type ) {
  case kGateCONST0: return mSimpleType[0];
  case kGateCONST1: return mSimpleType[1];
  case kGateBUFF:   return mSimpleType[2];
  case kGateNOT:    return mSimpleType[3];
  case kGateAND:    return mSimpleType[4];
  case kGateNAND:   return mSimpleType[5];
  case kGateOR:     return mSimpleType[6];
  case kGateNOR:    return mSimpleType[7];
  case kGateXOR:    return mSimpleType[8];
  case kGateXNOR:   return mSimpleType[9];
  default: break;
  }
  ASSERT_NOT_REACHED;
  return nullptr;
}

// @brief 複合型のオブジェクトを返す．
// @param[in] ni 入力数
// @param[in] expr 論理式
const TpgGateInfo*
TpgGateInfoMgr::complex_type(ymuint ni,
			     const Expr& expr)
{
  TpgGateInfo* node_info = new CplxGateInfo(ni, expr);
  mList.push_back(node_info);
  return node_info;
}

END_NAMESPACE_YM_SATPG
