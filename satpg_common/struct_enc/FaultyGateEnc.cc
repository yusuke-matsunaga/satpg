
/// @file FaultyGateEnc.cc
/// @brief FaultyGateEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "FaultyGateEnc.h"

#include "TpgNode.h"
#include "GateType.h"
#include "VidMap.h"

#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] solver SATソルバ
// @param[in] varmap 変数番号のマップ
// @param[in] fault 対象の故障
FaultyGateEnc::FaultyGateEnc(SatSolver& solver,
			     const VidMap& varmap,
			     const TpgFault* fault) :
  mSolver(solver),
  mVarMap(varmap),
  mFault(fault)
{
}

// @brief デストラクタ
FaultyGateEnc::~FaultyGateEnc()
{
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] var_map 変数マップ
void
FaultyGateEnc::make_cnf(const TpgNode* node)
{
  make_cnf(node, mVarMap(node));
}

// @brief ノードの入出力の関係を表すCNF式を作る．
// @param[in] node 対象のノード
// @param[in] ovar 出力の変数
void
FaultyGateEnc::make_cnf(const TpgNode* node,
			SatVarId ovar)
{
  SatLiteral olit(ovar);

  if ( mFault->tpg_onode() == node ) {
    // 対象故障のあるノードの場合
    int fval = mFault->val();
    if ( mFault->is_stem_fault() ) {
      // 出力の故障
      if ( fval ) {
	// 1縮退故障
	mSolver.add_clause(olit);
      }
      else {
	// 0縮退故障
	mSolver.add_clause(~olit);
      }
    }
    else {
      // 入力の故障
      int ni = node->fanin_num();
      Array<const TpgNode*> fanin_array = node->fanin_list();
      int fpos = mFault->tpg_pos();
      switch ( node->gate_type() ) {
      case GateType::Const0:
      case GateType::Const1:
      case GateType::Input:
	ASSERT_NOT_REACHED;
	break;

      case GateType::Buff:
	if ( fval ) {
	  // 入力の1縮退故障
	  mSolver.add_clause(olit);
	}
	else {
	  // 入力の0縮退故障
	  mSolver.add_clause(~olit);
	}
	break;

      case GateType::Not:
	if ( fval ) {
	  // 入力の1縮退故障
	  mSolver.add_clause(~olit);
	}
	else {
	  // 入力の0縮退故障
	  mSolver.add_clause(olit);
	}
	break;

      case GateType::And:
	if ( fval == 0 ) {
	  // 入力の0縮退故障
	  mSolver.add_clause(~olit);
	}
	else {
	  // 入力の1縮退故障
	  switch ( ni ) {
	  case 2:
	    {
	      SatLiteral ilit0;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		break;
	      }
	      mSolver.add_eq_rel( olit, ilit0);
	    }
	    break;

	  case 3:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		break;
	      }
	      mSolver.add_andgate_rel( olit, ilit0, ilit2);
	    }
	    break;

	  case 4:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      SatLiteral ilit2;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 3:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[2]);
		break;
	      }
	      mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
	    }
	    break;

	  default:
	    ASSERT_COND( ni > 4 );
	    {
	      vector<SatLiteral> ilits;
	      ilits.reserve(ni - 1);
	      for (int i = 0; i < ni; ++ i) {
		if ( i != fpos ) {
		  ilits.push_back(lit(fanin_array[i]));
		}
	      }
	      mSolver.add_andgate_rel( olit, ilits);
	    }
	    break;
	  }
	}
	break;

      case GateType::Nand:
	if ( fval == 0 ) {
	  // 入力の 0 縮退故障
	  mSolver.add_clause(olit);
	}
	else {
	  switch ( ni ) {
	  case 2:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[0]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[1]);
		break;
	      }
	      mSolver.add_eq_rel( ~olit, ilit0 );
	    }
	    break;

	  case 3:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      SatLiteral ilit2;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		break;
	      }
	      mSolver.add_nandgate_rel( olit, ilit0, ilit1 );
	    }
	    break;

	  case 4:
	    {
	      SatLiteral ilit0 = lit(fanin_array[0]);
	      SatLiteral ilit1 = lit(fanin_array[1]);
	      SatLiteral ilit2 = lit(fanin_array[2]);
	      SatLiteral ilit3 = lit(fanin_array[3]);
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 3:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[2]);
		break;
	      }
	      mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2 );
	    }
	    break;

	  default:
	    ASSERT_COND( ni > 4 );
	    {
	      vector<SatLiteral> ilits;
	      ilits.reserve(ni - 1);
	      for (int i = 0; i < ni; ++ i) {
		if ( i != fpos ) {
		  ilits.push_back(lit(fanin_array[i]));
		}
	      }
	      mSolver.add_nandgate_rel( olit, ilits);
	    }
	    break;
	  }
	}
	break;

      case GateType::Or:
	if ( fval == 1 ) {
	  // 入力の 1 縮退故障
	  mSolver.add_clause(olit);
	}
	else {
	  // 入力の 0 縮退故障
	  switch ( ni ) {
	  case 2:
	    {
	      SatLiteral ilit0;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		break;
	      case 1:
		ilit1 = lit(fanin_array[0]);
		break;
	      }
	      mSolver.add_eq_rel( olit, ilit0 );
	    }
	    break;

	  case 3:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		break;
	      }
	      mSolver.add_orgate_rel( olit, ilit0, ilit1 );
	    }
	    break;

	  case 4:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      SatLiteral ilit2;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 3:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[2]);
		break;
	      }
	      mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2 );
	    }
	    break;

	  default:
	    ASSERT_COND( ni > 4 );
	    {
	      vector<SatLiteral> ilits;
	      ilit.reserve(ni - 1);
	      for ( int i = 0; i < ni; ++ i ) {
		if ( i != fpos ) {
		  ilits.push_back(lit(fanin_array[i]));
		}
	      }
	      mSolver.add_orgate_rel( olit, ilits);
	    }
	    break;
	  }
	}
	break;

      case GateType::Nor:
	if ( fval == 0 ) {
	  // 入力の1縮退故障
	  mSolver.add_clause(~olit);
	}
	else {
	  switch ( ni ) {
	  case 2:
	    {
	      SatLiteral ilit0;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		break;
	      }
	      mSolver.add_eq_rel( ~olit, ilit0 );
	    }
	    break;

	  case 3:
	    {
	      SatLiteral ilit0;
	      SatLiteral ilit1;
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		break;
	      }
	      mSolver.add_norgate_rel( olit, ilit0, ilit1 );
	    }
	    break;

	  case 4:
	    {
	      SatLiteral ilit0 = lit(fanin_array[0]);
	      SatLiteral ilit1 = lit(fanin_array[1]);
	      SatLiteral ilit2 = lit(fanin_array[2]);
	      switch ( fpos ) {
	      case 0:
		ilit0 = lit(fanin_array[1]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 1:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[2]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 2:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[3]);
		break;
	      case 3:
		ilit0 = lit(fanin_array[0]);
		ilit1 = lit(fanin_array[1]);
		ilit2 = lit(fanin_array[2]);
		break;
	      }
	      mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2 );
	    }
	    break;

	  default:
	    ASSERT_COND( ni > 4 );
	    {
	      vector<SatLiteral> ilits;
	      ilits.reserve(ni - 1);
	      for (int i = 0; i < ni; ++ i) {
		if ( i != fpos ) {
		  ilits.push_back(lit(fanin_array[i]));
		}
	      }
	      mSolver.add_norgate_rel( olit, ilits);
	    }
	    break;
	  }
	}
	break;

      case GateType::Xor:
	ASSERT_COND( ni == 2 );
	{
	  SatLiteral ilit0;
	  switch ( fpos ) {
	  case 0:
	    ilit0 = lit(fanin_array[1]);
	    break;
	  case 1:
	    ilit0 = lit(fanin_array[0]);
	    break;
	  }
	  if ( fval ) {
	    mSolver.add_eq_rel( ~olit, ilit0 );
	  }
	  else {
	    mSolver.add_eq_rel( olit, ilit0 );
	  }
	}
	break;

      case GateType::Xnor:
	ASSERT_COND( ni == 2 );
	{
	  SatLiteral ilit0 = lit(fanin_array[0]);
	  switch ( fpos ) {
	  case 0:
	    ilit0 = lit(fanin_array[1]);
	    break;
	  case 1:
	    ilit1 = lit(fanin_array[0]);
	    break;
	  }
	  if ( fval ) {
	    mSolver.add_eq_rel( olit, ilit0 );
	  }
	  else {
	    mSolver.add_eq_rel( ~olit, ilit0 );
	  }
	}
	break;

      default:
	ASSERT_NOT_REACHED;
	break;
      }
    }
    return;
  }

  int ni = node->fanin_num();
  Array<const TpgNode*> fanin_array = node->fanin_list();
  switch ( node->gate_type() ) {
  case GateType::Const0:
    mSolver.add_clause(~olit);
    break;

  case GateType::Const1:
    mSolver.add_clause( olit);
    break;

  case GateType::Input:
    // なにもしない．
    break;

  case GateType::Buff:
    {
      SatLiteral ilit = lit(fanin_array[0]);
      mSolver.add_eq_rel( ilit,  olit);
    }
    break;

  case GateType::Not:
    {
      SatLiteral ilit = lit(fanin_array[0]);
      mSolver.add_eq_rel( ilit, ~olit);
    }
    break;

  case GateType::And:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	mSolver.add_andgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	mSolver.add_andgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_andgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nand:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	mSolver.add_nandgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_nandgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Or:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	mSolver.add_orgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	mSolver.add_orgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_orgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Nor:
    switch ( ni ) {
    case 2:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	mSolver.add_norgate_rel( olit, ilit0, ilit1);
      }
      break;

    case 3:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2);
      }
      break;

    case 4:
      {
	SatLiteral ilit0 = lit(fanin_array[0]);
	SatLiteral ilit1 = lit(fanin_array[1]);
	SatLiteral ilit2 = lit(fanin_array[2]);
	SatLiteral ilit3 = lit(fanin_array[3]);
	mSolver.add_norgate_rel( olit, ilit0, ilit1, ilit2, ilit3);
      }
      break;

    default:
      ASSERT_COND( ni > 4 );
      {
	vector<SatLiteral> ilits(ni);
	for (int i = 0; i < ni; ++ i) {
	  ilits[i] = lit(fanin_array[i]);
	}
	mSolver.add_norgate_rel( olit, ilits);
      }
      break;
    }
    break;

  case GateType::Xor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = lit(fanin_array[0]);
      SatLiteral ilit1 = lit(fanin_array[1]);
      mSolver.add_xorgate_rel( olit, ilit0, ilit1);
    }
    break;

  case GateType::Xnor:
    ASSERT_COND( ni == 2 );
    {
      SatLiteral ilit0 = lit(fanin_array[0]);
      SatLiteral ilit1 = lit(fanin_array[1]);
      mSolver.add_xnorgate_rel( olit, ilit0, ilit1);
    }
    break;

  default:
    ASSERT_NOT_REACHED;
    break;
  }
}

// @brief ノードに対応するリテラルを返す．
SatLiteral
FaultyGateEnc::lit(const TpgNode* node)
{
  return SatLiteral(mVarMap(node));
}

END_NAMESPACE_YM_SATPG
