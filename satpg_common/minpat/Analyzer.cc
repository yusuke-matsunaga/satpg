
/// @file Analyzer.cc
/// @brief Analyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Analyzer.h"
#include "FaultInfo.h"
#include "DtpgFFR.h"
#include "TpgFFR.h"
#include "TpgFault.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
Analyzer::Analyzer(const TpgNetwork& network,
		   FaultType fault_type) :
  mNetwork(network),
  mFaultType(fault_type)
{
}

int nex_num;

// @brief デストラクタ
Analyzer::~Analyzer()
{
}

// @brief 初期化する
// @param[in] loop_limit 反復回数の上限
void
Analyzer::init(int loop_limit)
{
  string sat_type;
  string sat_option;
  ostream* sat_outp = nullptr;
  string just_type;
  vector<FaultInfo*> tmp_fault_list;
  nex_num = 0;
  tmp_fault_list.reserve(mNetwork.rep_fault_num());
  for ( auto& ffr: mNetwork.ffr_list() ) {
    // FFR ごとに検出可能な故障をもとめる．
    DtpgFFR dtpg(sat_type, sat_option, sat_outp, mFaultType, just_type, mNetwork, ffr);
    vector<const TpgFault*> fault_list;
    vector<NodeValList> ffr_cond_list;
    for ( auto fault: ffr.fault_list() ) {
      NodeValList ffr_cond = dtpg.make_ffr_condition(fault);
      vector<SatLiteral> assumptions;
      dtpg.conv_to_assumptions(ffr_cond, assumptions);
      vector<SatBool3> model;
      SatBool3 sat_res = dtpg.solve(assumptions, model);
      if ( sat_res == SatBool3::True ) {
	fault_list.push_back(fault);
	ffr_cond_list.push_back(ffr_cond);
      }
    }
    // 支配関係を調べ，代表故障のみを残す．
    int nf = fault_list.size();
    vector<bool> mark(nf, false);
    for ( auto i1: Range(nf) ) {
      if ( mark[i1] ) {
	continue;
      }
      const NodeValList& ffr_cond1 = ffr_cond_list[i1];
      // ffr_cond1 を否定した節を加える．
      // 制御変数は clit1
      SatVarId cvar1 = dtpg.new_variable();
      SatLiteral clit1(cvar1);
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(ffr_cond1.size() + 1);
      tmp_lits.push_back(~clit1);
      for ( auto nv: ffr_cond1 ) {
	SatLiteral lit1 = dtpg.conv_to_literal(nv);
	tmp_lits.push_back(~lit1);
      }
      dtpg.add_clause(tmp_lits);
      for ( auto i2: Range(nf) ) {
	if ( i2 == i1 ) {
	  continue;
	}
	if ( mark[i2] ) {
	  continue;
	}
	const NodeValList& ffr_cond2 = ffr_cond_list[i2];
	vector<SatLiteral> assumptions;
	assumptions.reserve(ffr_cond2.size() + 1);
	dtpg.conv_to_assumptions(ffr_cond2, assumptions);
	assumptions.push_back(clit1);
	vector<SatBool3> dummy;
	SatBool3 sat_res = dtpg.solve(assumptions, dummy);
	if ( sat_res == SatBool3::False ) {
	  // fault2 を検出する条件のもとで fault1 を検出しない
	  // 割り当てが存在しない．→ fault1 は支配されている．
	  mark[i1] = true;
	  break;
	}
      }
    }
    for ( auto i: Range(nf) ) {
      if ( mark[i] ) {
	continue;
      }
      auto fault = fault_list[i];
      auto fi = analyze_fault(dtpg, fault, loop_limit);
      ASSERT_COND ( fi != nullptr );
      tmp_fault_list.push_back(fi);
    }
  }
  cout << "Total faults: " << tmp_fault_list.size() << endl;
  cout << "incomplete faults: " << nex_num << endl;

#if 0
  int nf = tmp_fault_list.size();
  vector<pair<FaultInfo*, FaultInfo*>> tmp_conflict_list;
  vector<pair<FaultInfo*, FaultInfo*>> tmp_compatible_list;
  for ( auto i1: Range(nf - 1) ) {
    FaultInfo* fi1 = tmp_fault_list[i1];
    if ( fi1->is_dominated() ) {
      continue;
    }
    const TpgFault* fault1 = fi1->fault();
    auto suff_cond1 = fi1->sufficient_cond();
    for ( auto i2: Range(i1 + 1, nf) ) {
      FaultInfo* fi2 = tmp_fault_list[i2];
      if ( fi2->is_dominated() ) {
	continue;
      }
      const TpgFault* fault2 = fi2->fault();
      auto suff_cond2 = fi2->sufficient_cond();
      int res = compare(suff_cond1, suff_cond2);
      if ( res == -1 ) {
	// ２つの故障は衝突している．
	tmp_conflict_list.push_back(make_pair(fi1, fi2));
      }
      else if ( res & 1 ) { // res == 1 or res == 3
	// fault1 が fault2 を支配している．
	fi2->set_dominated();
      }
      else if ( res & 2 ) {
	// fault2 が fault1 を支配している．
	fi1->set_dominated();
      }
      else {
	// fault1 と fault2 は両立する可能性がある．
	tmp_compatible_list.push_back(make_pair(fi1, fi2));
      }
    }
  }
  // mark のついた故障をドロップする．
  fault_list.clear();
  fault_list.reserve(nf);
  for ( auto fi: tmp_fault_list ) {
    if ( !fi->is_dominated() ) {
      fault_list.push_back(fi);
    }
  }

  // 衝突リストを作る．
  int n_confl = 0;
  int n_compat = 0;
  for ( auto fi_pair: tmp_conflict_list ) {
    FaultInfo* fi1 = fi_pair.first;
    FaultInfo* fi2 = fi_pair.second;
    if ( !fi1->is_dominated() && !fi2->is_dominated() ) {
      fi1->add_conflict(fi2);
      fi2->add_conflict(fi1);
      ++ n_confl;
    }
  }

  // 両立関係を調べて両立リストを作る．
  for ( auto fi_pair: tmp_compatible_list ) {
    FaultInfo* fi1 = fi_pair.first;
    FaultInfo* fi2 = fi_pair.second;
    if ( fi1->is_dominated() || fi2->is_dominated() ) {
      continue;
    }
    auto suff_cond1 = fi1->sufficient_cond();
    auto suff_cond2 = fi2->sufficient_cond();
    vector<SatLiteral> assumptions;
    dtpg.conv_to_assumptions(suff_cond1, assumptions);
    dtpg.conv_to_assumptions(suff_cond2, assumptions);
    vector<SatBool3> model;
    SatBool3 sat_res = dtpg.solve(assumptions, model);
    if ( sat_res == SatBool3::True ) {
      // 両立している．
      fi1->add_compatible(fi2);
      fi2->add_compatible(fi1);
      ++ n_compat;
    }
    else {
      // 衝突している．
      fi1->add_conflict(fi2);
      fi2->add_conflict(fi1);
      ++ n_confl;
    }
  }
#endif
}

// @brief 故障の検出条件の解析を行う．
// @param[in] dtpg DTPGエンジン
// @param[in] fault 対象の故障
// @param[in] loop_limit 反復回数の上限
// @return FaultInfo を返す．
FaultInfo*
Analyzer::analyze_fault(DtpgFFR& dtpg,
			const TpgFault* fault,
			int loop_limit)
{
  // FFR 内の伝搬条件をリテラルに変換して加えたSAT問題を解く．
  NodeValList ffr_cond = dtpg.make_ffr_condition(fault);
  SatBool3 sat_res;
  vector<SatBool3> model;
  {
    vector<SatLiteral> assumptions;
    dtpg.conv_to_assumptions(ffr_cond, assumptions);
    sat_res = dtpg.solve(assumptions, model);
  }
  if ( sat_res == SatBool3::True ) {
    // 検出可能
    // 検出条件の解析を行う．
    Expr expr = dtpg.get_sufficient_conditions(fault, model);
    NodeValList tmp_cond = common_cube(expr);
    // 必要条件を求める．
    // ffr_cond は無条件で必要条件となる．
    NodeValList mand_cond(ffr_cond);
    for ( auto nv: tmp_cond ) {
      // nv を否定した条件を加えて解があるか調べる．
      vector<SatLiteral> assumptions;
      dtpg.conv_to_assumptions(mand_cond, assumptions);
      SatLiteral lit1 = dtpg.conv_to_literal(nv);
      assumptions.push_back(~lit1);
      vector<SatBool3> dummy;
      if ( dtpg.solve(assumptions, dummy) == SatBool3::False ) {
	// 充足不能なので nv は必要割当
	mand_cond.add(nv);
      }
    }
    bool exhausted = true;
    expr = restrict(expr, mand_cond);
    if ( !expr.is_constant() ) {
      SatVarId cvar1 = dtpg.new_variable();
      SatLiteral clit1(cvar1);
      // 別解を求める．
      exhausted = false;
      Expr expr1(expr);
      for ( int i = 0; i < loop_limit; ++ i ) {
	// expr1 を否定した節を追加する．
	dtpg.add_negation(expr1, clit1);
	vector<SatLiteral> assumptions;
	dtpg.conv_to_assumptions(mand_cond, assumptions);
	assumptions.push_back(clit1);
	vector<SatBool3> model;
	if ( dtpg.solve(assumptions, model) == SatBool3::True ) {
	  expr1 = dtpg.get_sufficient_conditions(fault, model);
	  expr |= expr1;
	}
	else {
	  exhausted = true;
	  break;
	}
      }
    }
    if ( !exhausted ) {
      ++ nex_num;
    }
    auto fi = new FaultInfo(fault, mand_cond, expr);
    return fi;
  }
  else {
    return nullptr;
  }
}

// @brief 論理式に含まれるキューブを求める．
// @param[in] expr 論理式
NodeValList
Analyzer::common_cube(const Expr& expr)
{
  ASSERT_COND( !expr.is_constant() );

  NodeValList cube;
  if ( expr.is_posiliteral() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    cube.add(node, 1, true);
  }
  else if ( expr.is_negaliteral() ) {
    int id = expr.varid().val();
    const TpgNode* node = mNetwork.node(id);
    cube.add(node, 1, false);
  }
  else if ( expr.is_and() ) {
    int n = expr.child_num();
    for ( int i: Range(n) ) {
      NodeValList cube1 = common_cube(expr.child(i));
      cube += cube1;
    }
  }
  else if ( expr.is_or() ) {
    // 最初のキューブだけを使う．
    ASSERT_COND( expr.child_num() > 0 );
    cube = common_cube(expr.child(0));
  }
  else if ( expr.is_xor() ) {
    ASSERT_NOT_REACHED;
  }

  return cube;
}

// @brief 必要割り当てに従って論理式を簡単化する．
// @param[in] expr 論理式
// @param[in] mand_cond 必要割り当て
// @return 簡単化した論理式を返す．
Expr
Analyzer::restrict(const Expr& expr,
		   const NodeValList& mand_cond)
{
  HashMap<VarId, bool> val_map;
  for ( auto nv: mand_cond ) {
    const TpgNode* node = nv.node();
    bool val = nv.val();
    val_map.add(VarId(node->id()), val);
  }
  return _restrict_sub(expr, val_map);
}

// @brief restrict の下請け関数
// @param[in] expr 論理式
// @param[in] val_map 割り当てマップ
Expr
Analyzer::_restrict_sub(const Expr& expr,
			const HashMap<VarId, bool>& val_map)
{
  ASSERT_COND( !expr.is_constant() );

  if ( expr.is_posiliteral() ) {
    VarId var = expr.varid();
    bool val;
    if ( val_map.find(var, val) ) {
      if ( val == false ) {
	return Expr::const_zero();
      }
      else if ( val == true ) {
	return Expr::const_one();
      }
    }
    return expr;
  }
  else if ( expr.is_negaliteral() ) {
    VarId var = expr.varid();
    bool val;
    if ( val_map.find(var, val) ) {
      if ( val == false ) {
	return Expr::const_one();
      }
      else if ( val == true ) {
	return Expr::const_zero();
      }
    }
    return expr;
  }
  else if ( expr.is_and() ) {
    int n = expr.child_num();
    ASSERT_COND( n > 0 );
    Expr ans_expr = _restrict_sub(expr.child(0), val_map);
    for ( int i: Range(1, n) ) {
      ans_expr &= _restrict_sub(expr.child(i), val_map);
    }
    return ans_expr;
  }
  else if ( expr.is_or() ) {
    int n = expr.child_num();
    ASSERT_COND( n > 0 );
    Expr ans_expr = _restrict_sub(expr.child(0), val_map);
    for ( int i: Range(1, n) ) {
      ans_expr |= _restrict_sub(expr.child(i), val_map);
    }
    return ans_expr;
  }
  else if ( expr.is_xor() ) {
    ASSERT_NOT_REACHED;
  }

  return Expr::const_zero();
}

END_NAMESPACE_YM_SATPG
