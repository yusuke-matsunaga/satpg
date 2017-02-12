﻿
/// @file DtpgM.cc
/// @brief DtpgM の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.

#define DEBUG_DTPGM 0

#include "sa/DtpgM.h"
#include "TpgFault.h"
#include "TpgNetwork.h"
#include "VectLitMap.h"


BEGIN_NAMESPACE_YM_SATPG_SA

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
// @param[in] network 対象のネットワーク
// @param[in] mffc_root MFFC の根のノード
DtpgM::DtpgM(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt,
	     const TpgNetwork& network,
	     const TpgNode* mffc_root) :
  DtpgBase(sat_type, sat_option, sat_outp, bt, network, mffc_root),
  mElemArray(mffc_root->mffc_elem_num()),
  mElemPosMap(network.max_fault_id()),
  mElemVarArray(mffc_root->mffc_elem_num())
{
  ymuint ne = mffc_root->mffc_elem_num();
  for (ymuint i = 0; i < ne; ++ i) {
    const TpgNode* node1 = mffc_root->mffc_elem(i);
    mElemArray[i] = node1;
    // node1 を根とする FFR に含まれる故障を求める．
    ymuint nf = network.ffr_fault_num(node1->id());
    for (ymuint j = 0; j < nf; ++ j) {
      const TpgFault* f = network.ffr_fault(node1->id(), j);
      mElemPosMap[f->id()] = i;
    }
  }
}

// @brief デストラクタ
DtpgM::~DtpgM()
{
}

// @brief CNF 式を作る．
void
DtpgM::cnf_gen(DtpgStats& stats)
{
  cnf_begin();

  // root 以降の伝搬条件を作る．
  gen_cnf_base();

  if ( mElemArray.size() > 1 ) {
    make_mffc_condition();
  }

  cnf_end(stats);
}

// @briefテスト生成を行う．
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
SatBool3
DtpgM::dtpg(const TpgFault* fault,
	    NodeValList& nodeval_list,
	    DtpgStats& stats)
{
  NodeValList assign_list;

  //timer.start();

  // FFR 内の故障活性化&伝搬条件を求める．
  make_ffr_condition(fault, assign_list);

  vector<SatLiteral> assumptions;

  if ( mElemArray.size() > 1 ) {
    // FFR の根の出力に故障を挿入する．
    assumptions.reserve(mElemArray.size());
    ymuint elem_pos = mElemPosMap[fault->id()];
    for (ymuint i = 0; i < mElemVarArray.size(); ++ i) {
      SatVarId evar = mElemVarArray[i];
      bool inv = (i != elem_pos);
      assumptions.push_back(SatLiteral(evar, inv));
    }
  }

  //timer.stop();

  // 故障に対するテスト生成を行なう．
  SatBool3 ans = solve(assumptions, assign_list, fault, nodeval_list, stats);

  return ans;
}

// @brief MFFC 内部の故障伝搬条件を表すCNFを作る．
void
DtpgM::make_mffc_condition()
{
  // 各FFRの根にXORゲートを挿入した故障回路を作る．
  // そのXORをコントロールする入力変数を作る．
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    mElemVarArray[i] = solver().new_var();

#if DEBUG_DTPGM
    cout << "cvar(Elem#" << i << ") = " << mElemVarArray[i] << endl;
#endif
  }

  // mElemArray[] に含まれるノードと root の間にあるノードを
  // 求め，同時に変数を割り当てる．
  vector<const TpgNode*> node_list;
  vector<int> elem_map(max_node_id(), -1);
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    const TpgNode* node = mElemArray[i];
    elem_map[node->id()] = i;
    if ( node == root_node() ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_var();
	set_fvar(onode, var);
	node_list.push_back(onode);

#if DEBUG_DTPGM
	cout << "fvar(Node#" << onode->id() << ") = " << var << endl;
#endif
      }
    }
  }
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    if ( node == root_node() ) {
      continue;
    }
    ymuint nfo = node->fanout_num();
    for (ymuint i = 0; i < nfo; ++ i) {
      const TpgNode* onode = node->fanout(i);
      if ( fvar(onode) == gvar(onode) ) {
	SatVarId var = solver().new_var();
	set_fvar(onode, var);
	node_list.push_back(onode);

#if DEBUG_DTPGM
	cout << "fvar(Node#" << onode->id() << ") = " << var << endl;
#endif
      }
    }
  }
  node_list.push_back(root_node());

  // 最も入力よりにある FFR の根のノードの場合
  // 正常回路と制御変数のXORをとったものを故障値とする．
  for (ymuint i = 0; i < mElemArray.size(); ++ i) {
    const TpgNode* node = mElemArray[i];
    if ( fvar(node) != gvar(node) ) {
      // このノードは入力側ではない．
      continue;
    }

    SatVarId fvar = solver().new_var();
    set_fvar(node, fvar);

    inject_fault(i, gvar(node));
  }

  // node_list に含まれるノードの入出力の関係を表すCNF式を作る．
  for (ymuint rpos = 0; rpos < node_list.size(); ++ rpos) {
    const TpgNode* node = node_list[rpos];
    ymuint ni = node->fanin_num();
    vector<SatVarId> ivars(ni);
    for (ymuint i = 0; i < ni; ++ i) {
      const TpgNode* inode = node->fanin(i);
      ivars[i] = fvar(inode);
    }
    SatVarId ovar = fvar(node);
    ymuint elem_pos = elem_map[node->id()];
    if ( elem_pos != -1 ) {
      // 実際のゲートの出力と ovar の間に XOR ゲートを挿入する．
      // XORの一方の入力は mElemVarArray[elem_pos]
      ovar = solver().new_var();
      inject_fault(elem_pos, ovar);
    }
    node->make_cnf(solver(), VectLitMap(ivars, ovar));

#if DEBUG_DTPGM
    cout << "Node#" << node->id() << ": ofvar("
	 << ovar << ") := " << node->gate_type()
	 << "(";
    for (ymuint i = 0; i < ni; ++ i) {
      cout << " " << ivars[i];
    }
    cout << ")" << endl;
#endif
  }
}

// @brief 故障挿入回路のCNFを作る．
// @param[in] elem_pos 要素番号
// @param[in] ovar ゲートの出力の変数
void
DtpgM::inject_fault(ymuint elem_pos,
		    SatVarId ovar)
{
  SatLiteral lit1(ovar);
  SatLiteral lit2(mElemVarArray[elem_pos]);
  const TpgNode* node = mElemArray[elem_pos];
  SatLiteral olit(fvar(node));
  solver().add_clause( lit1,  lit2, ~olit);
  solver().add_clause(~lit1, ~lit2, ~olit);
  solver().add_clause(~lit1,  lit2,  olit);
  solver().add_clause( lit1, ~lit2,  olit);

#if DEBUG_DTPGM
  cout << "inject fault: " << ovar << " -> " << fvar(node)
       << " with cvar = " << mElemVarArray[elem_pos] << endl;
#endif
}

END_NAMESPACE_YM_SATPG_SA