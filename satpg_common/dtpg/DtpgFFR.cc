
/// @file DtpgFFR.cc
/// @brief DtpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "DtpgFFR.h"

#include "TpgFault.h"
#include "TpgFFR.h"
#include "NodeValList.h"
#include "TestVector.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] fault_type 故障の種類
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgFFR::DtpgFFR(const string& sat_type,
		 const string& sat_option,
		 ostream* sat_outp,
		 FaultType fault_type,
		 const string& just_type,
		 const TpgNetwork& network,
		 const TpgFFR& ffr) :
  DtpgEngine(sat_type, sat_option, sat_outp, fault_type, just_type, network, ffr.root())
{
  cnf_begin();

  gen_cnf_base();

  cnf_end();
}

// @brief デストラクタ
DtpgFFR::~DtpgFFR()
{
}

// @brief テスト生成を行なう．
// @param[in] fault 対象の故障
// @param[out] testvect テストパタンを格納する変数
// @return 結果を返す．
DtpgResult
DtpgFFR::gen_pattern(const TpgFault* fault)
{
  const TpgNode* ffr_root = fault->tpg_onode()->ffr_root();
  ASSERT_COND( ffr_root == root_node() );

  // FFR 内の故障伝搬条件を ffr_cond に入れる．
  NodeValList ffr_cond = make_ffr_condition(fault);

  // ffr_cond の内容を assumptions に追加する．
  vector<SatLiteral> assumptions;
  conv_to_assumptions(ffr_cond, assumptions);

  vector<SatBool3> model;
  SatBool3 sat_res = solve(assumptions, model);
  if ( sat_res == SatBool3::True ) {
    TestVector testvect = backtrace(ffr_root, ffr_cond, model);
    return DtpgResult(testvect);
  }
  else if ( sat_res == SatBool3::False ) {
    return DtpgResult::make_untestable();
  }
  else { // sat_res == SatBool3::X
    return DtpgResult::make_undetected();
  }
}

END_NAMESPACE_YM_SATPG
