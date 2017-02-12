﻿
/// @file DtpgBase.cc
/// @brief DtpgBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#define DEBUG_DTPG 0

#include "sa/DtpgBase.h"

#include "TpgNetwork.h"
#include "TpgFault.h"

#include "sa/DtpgStats.h"
#include "sa/BackTracer.h"
#include "sa/NodeValList.h"
#include "sa/ModelValMap.h"

#include "ym/SatSolver.h"
#include "ym/SatStats.h"
#include "ym/StopWatch.h"

#include "VidLitMap.h"


BEGIN_NAMESPACE_YM_SATPG_SA

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] root 故障伝搬の起点となるノード
DtpgBase::DtpgBase(const string& sat_type,
		   const string& sat_option,
		   ostream* sat_outp,
		   BackTracer& bt,
		   const TpgNetwork& network,
		   const TpgNode* root) :
  mSolver(sat_type, sat_option, sat_outp),
  mMaxNodeId(network.node_num()),
  mRoot(root),
  mMarkArray(mMaxNodeId, 0U),
  mGvarMap(mMaxNodeId),
  mFvarMap(mMaxNodeId),
  mDvarMap(mMaxNodeId),
  mBackTracer(bt),
  mTimerEnable(true)
{
  mNodeList.reserve(mMaxNodeId);
  mOutputList.reserve(network.ppo_num());
}

// @brief デストラクタ
DtpgBase::~DtpgBase()
{
}

// @breif 時間計測を制御する．
void
DtpgBase::timer_enable(bool enable)
{
  mTimerEnable = enable;
}

// @brief タイマーをスタートする．
void
DtpgBase::cnf_begin()
{
  timer_start();
}

// @brief タイマーを止めて CNF 作成時間に加える．
void
DtpgBase::cnf_end(DtpgStats& stats)
{
  USTime time = timer_stop();
  stats.mCnfGenTime += time;
  ++ stats.mCnfGenCount;
}

// @brief 時間計測を開始する．
void
DtpgBase::timer_start()
{
  if ( mTimerEnable ) {
    mTimer.reset();
    mTimer.start();
  }
}

/// @brief 時間計測を終了する．
USTime
DtpgBase::timer_stop()
{
  USTime time(0, 0, 0);
  if ( mTimerEnable ) {
    mTimer.stop();
    time = mTimer.time();
  }
  return time;
}

// @brief root の影響が外部出力まで伝搬する条件のCNF式を作る．
void
DtpgBase::gen_cnf_base()
{
  // root の TFO を mNodeList に入れる．
  set_tfo_mark(mRoot);
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      set_tfo_mark(onode);
    }
  }
  ymuint tfo_num = mNodeList.size();

  // TFO の TFI を mNodeList に入れる．
  for (ymuint rpos = 0; rpos < mNodeList.size(); ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    ymuint ni = node->fanin_num();
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      set_tfi_mark(inode);
    }
  }
  ymuint tfi_num = mNodeList.size();

  // TFO の部分に変数を割り当てる．
  for (ymuint rpos = 0; rpos < tfo_num; ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    SatVarId gvar = mSolver.new_var();
    SatVarId fvar = mSolver.new_var();
    SatVarId dvar = mSolver.new_var();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, fvar);
    mDvarMap.set_vid(node, dvar);

#if DEBUG_DTPG
    cout << "gvar(Node#" << node->id() << ") = " << gvar << endl
	 << "fvar(Node#" << node->id() << ") = " << fvar << endl
	 << "dvar(Node#" << node->id() << ") = " << dvar << endl;
#endif
  }

  // TFI の部分に変数を割り当てる．
  for (ymuint rpos = tfo_num; rpos < tfi_num; ++ rpos) {
    const TpgNode* node = mNodeList[rpos];
    SatVarId gvar = mSolver.new_var();

    mGvarMap.set_vid(node, gvar);
    mFvarMap.set_vid(node, gvar);

#if DEBUG_DTPG
    cout << "gvar(Node#" << node->id() << ") = " << gvar << endl
	 << "fvar(Node#" << node->id() << ") = " << gvar << endl;
#endif
  }


  //////////////////////////////////////////////////////////////////////
  // 正常回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfi_num; ++ i) {
    const TpgNode* node = mNodeList[i];
    node->make_cnf(mSolver, VidLitMap(node, mGvarMap));

#if DEBUG_DTPG
    cout << "Node#" << node->id() << ": gvar("
	 << gvar(node) << ") := " << node->gate_type()
	 << "(";
    for (ymuint j = 0; j < node->fanin_num(); ++ j) {
      const TpgNode* inode = node->fanin(j);
      cout << " " << gvar(inode);
    }
    cout << ")" << endl;
#endif
  }


  //////////////////////////////////////////////////////////////////////
  // 故障回路の CNF を生成
  //////////////////////////////////////////////////////////////////////
  for (ymuint i = 0; i < tfo_num; ++ i) {
    const TpgNode* node = mNodeList[i];
    if ( node != mRoot ) {
      node->make_cnf(mSolver, VidLitMap(node, mFvarMap));

#if DEBUG_DTPG
    cout << "Node#" << node->id() << ": fvar("
	 << fvar(node) << ") := " << node->gate_type()
	 << "(";
    for (ymuint j = 0; j < node->fanin_num(); ++ j) {
      const TpgNode* inode = node->fanin(j);
      cout << " " << fvar(inode);
    }
    cout << ")" << endl;
#endif
    }
    make_dchain_cnf(node);
  }


  //////////////////////////////////////////////////////////////////////
  // 故障の検出条件
  //////////////////////////////////////////////////////////////////////
  ymuint no = mOutputList.size();
  vector<SatLiteral> odiff(no);
  for (ymuint i = 0; i < no; ++ i) {
    const TpgNode* node = mOutputList[i];
    SatLiteral dlit(dvar(node));
    odiff[i] = dlit;
  }
  mSolver.add_clause(odiff);

  if ( !mRoot->is_ppo() ) {
    // mRoot の dlit が1でなければならない．
    mSolver.add_clause(SatLiteral(dvar(mRoot)));
  }
}

// @brief 故障伝搬条件を表すCNF式を生成する．
// @param[in] node 対象のノード
void
DtpgBase::make_dchain_cnf(const TpgNode* node)
{
  SatLiteral glit(mGvarMap(node));
  SatLiteral flit(mFvarMap(node));
  SatLiteral dlit(mDvarMap(node));

  // dlit -> XOR(glit, flit) を追加する．
  // 要するに正常回路と故障回路で異なっているとき dlit が 1 となる．
  mSolver.add_clause(~glit, ~flit, ~dlit);
  mSolver.add_clause( glit,  flit, ~dlit);

#if DEBUG_DTPG
  cout << "dvar(Node#" << node->id() << ") -> "
       << glit << " XOR " << flit << endl;
#endif

  if ( node->is_ppo() ) {
    mSolver.add_clause(~glit,  flit,  dlit);
    mSolver.add_clause( glit, ~flit,  dlit);

#if DEBUG_DTPG
    cout << "!dvar(Node#" << node->id() << ") -> "
	 << glit << " = " << flit << endl;
#endif
  }
  else {
    // dlit -> ファンアウト先のノードの dlit の一つが 1

#if DEBUG_DTPG
    cout << "dvar(Node#" << node->id() << ") -> ";
#endif
    ymuint nfo = node->fanout_num();
    if ( nfo == 1 ) {
      SatLiteral odlit(mDvarMap(node->fanout(0)));
      mSolver.add_clause(~dlit, odlit);

#if DEBUG_DTPG
      cout << odlit << endl;
#endif
    }
    else {
      vector<SatLiteral> tmp_lits(nfo + 1);
      for (ymuint i = 0; i < nfo; ++ i) {
	const TpgNode* onode = node->fanout(i);
	tmp_lits[i] = SatLiteral(mDvarMap(onode));

#if DEBUG_DTPG
	cout << " " << mDvarMap(onode);
#endif
      }

#if DEBUG_DTPG
      cout << endl;
#endif
      tmp_lits[nfo] = ~dlit;
      mSolver.add_clause(tmp_lits);

      const TpgNode* imm_dom = node->imm_dom();
      if ( imm_dom != nullptr ) {
	SatLiteral odlit(mDvarMap(imm_dom));
	mSolver.add_clause(~dlit, odlit);

#if DEBUG_DTPG
	cout << "dvar(Node#" << node->id() << ") -> "
	     << odlit << endl;
#endif
      }
    }
  }
}

// @brief 故障の影響がFFRの根のノードまで伝搬する条件を作る．
// @param[in] fault 対象の故障
// @param[out] assign_list 結果の値割り当てリスト
void
DtpgBase::make_ffr_condition(const TpgFault* fault,
			     NodeValList& assign_list)
{
#if DEBUG_DTPG
  cout << "make_ffr_condition" << endl;
#endif

  // 故障の活性化条件を作る．
  const TpgNode* inode = fault->tpg_inode();
  // 0 縮退故障の時に 1 にする．
  bool val = (fault->val() == 0);
  assign_list.add(inode, val);

  // ブランチの故障の場合，ゲートの出力までの伝搬条件を作る．
  if ( fault->is_branch_fault() ) {
    const TpgNode* onode = fault->tpg_onode();
    Val3 nval = onode->nval();
    if ( nval != kValX ) {
      ymuint ni = onode->fanin_num();
      bool val = (nval == kVal1);
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* inode1 = onode->fanin(i);
	if ( inode1 != inode ) {
	  assign_list.add(inode1, val);

#if DEBUG_DTPG
	  cout << "  Node#" << inode1->id() << ": ";
	  if ( val ) {
	    cout << "1" << endl;
	  }
	  else {
	    cout << "0" << endl;
	  }
#endif
	}
      }
    }
  }

  // FFR の根までの伝搬条件を作る．
  for (const TpgNode* node = fault->tpg_onode(); node->fanout_num() == 1;
       node = node->fanout(0)) {
    const TpgNode* fonode = node->fanout(0);
    ymuint ni = fonode->fanin_num();
    if ( ni == 1 ) {
      continue;
    }
    Val3 nval = fonode->nval();
    if ( nval == kValX ) {
      continue;
    }
    bool val = (nval == kVal1);
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode1 = fonode->fanin(i);
      if ( inode1 != node ) {
	assign_list.add(inode1, val);

#if DEBUG_DTPG
	cout << "  Node#" << inode1->id() << ": ";
	if ( val ) {
	  cout << "1" << endl;
	}
	else {
	  cout << "0" << endl;
	}
#endif
      }
    }
  }

#if DEBUG_DTPG
  cout << endl;
#endif
}

// @brief 一つの SAT問題を解く．
// @param[in] assumptions 値の決まっている変数のリスト
// @param[in] assign_list 仮定(値割り当て)のリスト
// @param[in] fault 対象の故障
// @param[in] root 故障位置のノード
// @param[in] output_list 故障に関係のある外部出力のリスト
// @param[out] nodeval_list 結果の値割り当てリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
DtpgBase::solve(const vector<SatLiteral>& assumptions,
		const NodeValList& assign_list,
		const TpgFault* fault,
		NodeValList& nodeval_list,
		DtpgStats& stats)
{
  StopWatch timer;

  SatStats prev_stats;
  mSolver.get_stats(prev_stats);

  timer.reset();
  timer.start();

  ymuint n0 = assumptions.size();
  ymuint n = assign_list.size();
  vector<SatLiteral> assumptions1(n + n0);
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    const TpgNode* node = nv.node();
    bool inv = !nv.val();
    assumptions1[i] = SatLiteral(gvar(node), inv);
  }
  for (ymuint i = 0; i < n0; ++ i) {
    assumptions1[i + n] = assumptions[i];
  }

  vector<SatBool3> model;
  SatBool3 ans = mSolver.solve(assumptions1, model);

  timer.stop();
  USTime time = timer.time();

  SatStats sat_stats;
  mSolver.get_stats(sat_stats);
  //sat_stats -= prev_stats;

  if ( ans == kB3True ) {
    timer.reset();
    timer.start();

    // パタンが求まった．
    ModelValMap val_map(mGvarMap, mFvarMap, model);

    // バックトレースを行う．
    mBackTracer(fault->tpg_onode()->ffr_root(), mOutputList, val_map, nodeval_list);

    timer.stop();
    stats.mBackTraceTime += timer.time();

    stats.update_det(sat_stats, time);
  }
  else if ( ans == kB3False ) {
    // 検出不能と判定された．
    stats.update_red(sat_stats, time);
  }
  else {
    // ans == kB3X つまりアボート
    stats.update_abort(sat_stats, time);
  }

  return ans;
}

END_NAMESPACE_YM_SATPG_SA
