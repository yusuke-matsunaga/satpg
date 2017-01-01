﻿
/// @file Fsim2.cc
/// @brief Fsim2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Fsim2.h"

#include "TpgNetwork.h"
#include "TpgDff.h"
#include "TpgNode.h"
#include "TpgFault.h"

#include "sa/FsimOp.h"
#include "sa/TestVector.h"
#include "sa/NodeValList.h"
#include "sa/DetectOp.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "ym/HashSet.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_SA

Fsim*
new_Fsim2New()
{
  return new nsFsim2New::Fsim2();
}

END_NAMESPACE_YM_SATPG_SA


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// Fsim2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Fsim2::Fsim2()
{
}

// @brief デストラクタ
Fsim2::~Fsim2()
{
  clear();
}

// @brief ネットワークをセットする関数
// @param[in] network ネットワーク
void
Fsim2::set_network(const TpgNetwork& network)
{
  clear();

  mNetwork = &network;

  ymuint nn = mNetwork->node_num();
  ymuint ni = mNetwork->ppi_num();
  ymuint no = mNetwork->ppo_num();

  // SimNode の生成
  // 対応付けを行うマップの初期化
  mSimMap.resize(nn);
  mInputArray.resize(ni);
  vector<SimNode*> output_array(no);

  vector<vector<SimNode*> > fanout_lists(nn);
  vector<ymuint> ipos(nn);

  ymuint nf = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = mNetwork->node(i);
    nf += network.node_fault_num(tpgnode->id());

    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mInputArray[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      SimNode* inode = find_simnode(tpgnode->fanin(0));
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(kGateBUFF, vector<SimNode*>(1, inode));
      node->set_output();
      output_array[tpgnode->output_id()] = node;

      fanout_lists[inode->id()].push_back(node);
      ipos[inode->id()] = 0;
    }
    else if ( tpgnode->is_dff_clock() ||
	      tpgnode->is_dff_clear() ||
	      tpgnode->is_dff_preset() ) {
      // DFFの制御端子に対応する SimNode の生成
      SimNode* inode = find_simnode(tpgnode->fanin(0));
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(kGateBUFF, vector<SimNode*>(1, inode));
      node->set_output();

      fanout_lists[inode->id()].push_back(node);
      ipos[inode->id()] = 0;
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      ymuint ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs(ni);
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* itpgnode = tpgnode->fanin(i);
	SimNode* inode = find_simnode(itpgnode);
	ASSERT_COND(inode );
	inputs[i] = inode;
      }

      // 出力の論理を表す SimNode を作る．
      GateType type = tpgnode->gate_type();
      node = make_gate(type, inputs);

      // ファンアウトリストを作る．
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* itpgnode = tpgnode->fanin(i);
	SimNode* inode = find_simnode(itpgnode);
	ASSERT_COND(inode );
	fanout_lists[inode->id()].push_back(node);
	ipos[inode->id()] = i;
      }
    }
    // 対応表に登録しておく．
    mSimMap[tpgnode->id()] = node;
  }
  ASSERT_COND( mNodeArray.size() == nn );

  // 各ノードのファンアウトリストの設定
  for (ymuint i = 0; i < nn; ++ i) {
    SimNode* node = mNodeArray[i];
    node->set_fanout_list(fanout_lists[i], ipos[i]);
  }

  // FFR の設定
  mFFRMap.resize(mNodeArray.size());
  ymuint ffr_num = 0;
  for (ymuint i = nn; i > 0; ) {
    -- i;
    SimNode* node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      ++ ffr_num;
    }
  }
  mFFRArray.resize(ffr_num);
  ffr_num = 0;
  for (ymuint i = nn; i > 0; ) {
    -- i;
    SimNode* node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      SimFFR* ffr = &mFFRArray[ffr_num];
      node->set_ffr_root();
      mFFRMap[node->id()] = ffr;
      ffr->set_root(node);
      ++ ffr_num;
    }
    else {
      SimFFR* ffr = mFFRMap[node->fanout(0)->id()];
      mFFRMap[node->id()] = ffr;
    }
  }

  // 消去用の配列の大きさはノード数を越えない．
  mClearArray.reserve(mNodeArray.size());

  // 最大レベルを求め，イベントキューを初期化する．
  ymuint max_level = 0;
  for (ymuint i = 0; i < no; ++ i) {
    SimNode* node = output_array[i];
    if ( max_level < node->level() ) {
      max_level = node->level();
    }
  }
  mEventQ.init(max_level);


  //////////////////////////////////////////////////////////////////////
  // 故障リストの設定
  //////////////////////////////////////////////////////////////////////
  mSimFaults.resize(nf);
  mFaultArray.resize(network.max_fault_id());
  ymuint fid = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    ymuint nf1 = network.node_fault_num(tpgnode->id());
    for (ymuint j = 0; j < nf1; ++ j) {
      const TpgFault* fault = network.node_fault(tpgnode->id(), j);
      const TpgNode* tpgnode = fault->tpg_onode();
      SimNode* simnode = find_simnode(tpgnode);
      SimNode* isimnode = nullptr;
      ymuint ipos = 0;
      if ( fault->is_branch_fault() ) {
	ipos = fault->tpg_pos();
	const TpgNode* inode = tpgnode->fanin(ipos);
	isimnode = find_simnode(inode);
      }
      else {
	isimnode = simnode;
      }
      mSimFaults[fid].set(fault, simnode, ipos, isimnode);
      SimFault* ff = &mSimFaults[fid];
      mFaultArray[fault->id()] = ff;
      ++ fid;
    }
  }
}

// @brief 故障にスキップマークをつける．
void
Fsim2::set_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = true;
}

// @brief 故障リストを設定する．
// @param[in] fault_list 対象の故障リスト
//
// スキップマークは消される．
void
Fsim2::set_faults(const vector<const TpgFault*>& fault_list)
{
  // 同時に各 SimFFR 内の故障リストも再構築する．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    p->clear_fault_list();
  }
  for (ymuint i = 0; i < fault_list.size(); ++ i) {
    const TpgFault* f = fault_list[i];
    SimFault* ff = mFaultArray[f->id()];
    ff->mSkip = false;
    SimNode* simnode = ff->mNode;
    SimFFR* ffr = mFFRMap[simnode->id()];
    ffr->add_fault(ff);
  }
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] op 検出した時に起動されるファンクタオブジェクト
void
Fsim2::sppfp(TestVector* tv,
	     FsimOp& op)
{
  _set_sp2(tv);
  _sppfp2(op);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] op 検出した時に起動されるファンクタオブジェクト
void
Fsim2::sppfp(const NodeValList& assign_list,
	     FsimOp& op)
{
  _set_sp2(assign_list);
  _sppfp2(op);
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @param[in] tv_array テストベクタの配列
// @param[in] op 検出した時に起動されるファンクタオブジェクト
void
Fsim2::ppsfp(const vector<TestVector*>& tv_array,
	     FsimOp& op)
{
  _set_pp2(tv_array);

  // 正常値の計算を行う．
  _calc_gval2();

  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    SimFFR* ffr = &(*p);
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = ffr->fault_prop2();

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    SimNode* root = ffr->root();
    PackedVal obs = kPvAll0;
    if ( root->is_output() ) {
      obs = kPvAll1;
    }
    else {
      eventq_put2(root, ffr_req);
      obs = eventq_simulate2();
    }

    ffr->fault_sweep(op, obs);
  }
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim2::spsfp(TestVector* tv,
	     const TpgFault* f)
{
  _set_sp2(tv);
  return _spsfp2(f);
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim2::spsfp(const NodeValList& assign_list,
	     const TpgFault* f)
{
  _set_sp2(assign_list);
  return _spsfp2(f);
}

// @brief 一つのパタンを全ビットに展開して設定する．
// @param[in] tv 設定するテストベクタ
void
Fsim2::_set_sp2(TestVector* tv)
{
  ymuint npi = mNetwork->ppi_num();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = mInputArray[i];
    PackedVal val = (tv->val3(i) == kVal1) ? kPvAll1 : kPvAll0;
    simnode->gval()->set_val(val);
  }
}

// @brief 一つのパタンを全ビットに展開して設定する．
// @param[in] assign_list 設定する値の割り当てリスト
void
Fsim2::_set_sp2(const NodeValList& assign_list)
{
  ymuint npi = mNetwork->ppi_num();

  // デフォルトで 0 にする．
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = mInputArray[i];
    simnode->gval()->set_val(kPvAll0);
  }

  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    if ( nv.val() ) {
      SimNode* simnode = mInputArray[nv.node()->input_id()];
      simnode->gval()->set_val(kPvAll1);
    }
  }
}

// @brief 複数のパタンを設定する．
// @param[in] tv_array テストベクタの配列
void
Fsim2::_set_pp2(const vector<TestVector*>& tv_array)
{
  ymuint npi = mNetwork->ppi_num();
  ymuint nb = tv_array.size();

  // tv_array を入力ごとに固めてセットしていく．
  for (ymuint i = 0; i < npi; ++ i) {
    PackedVal val = kPvAll0;
    PackedVal bit = 1UL;
    for (ymuint j = 0; j < nb; ++ j, bit <<= 1) {
      if ( tv_array[j]->val3(i) == kVal1 ) {
	val |= bit;
      }
    }
    // 残ったビットには 0 番めのパタンを詰めておく．
    if ( tv_array[0]->val3(i) == kVal1 ) {
      for (ymuint j = nb; j < kPvBitLen; ++ j, bit <<= 1) {
	val |= bit;
      }
    }
    SimNode* simnode = mInputArray[i];
    simnode->gval()->set_val(val);
  }
}

// @brief SPPFP故障シミュレーションの本体
// @param[in] op 検出した時に起動されるファンクタオブジェクト
void
Fsim2::_sppfp2(FsimOp& op)
{
  // 正常値の計算を行う．
  _calc_gval2();

  ymuint bitpos = 0;
  SimFFR* ffr_buff[kPvBitLen];
  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    SimFFR* ffr = &(*p);
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = ffr->fault_prop2();

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    SimNode* root = ffr->root();
    if ( root->is_output() ) {
      // 常に観測可能
      ffr->fault_sweep(op, kPvAll1);
      continue;
    }

    // キューに積んでおく
    PackedVal bitmask = 1UL << bitpos;
    eventq_put2(root, bitmask);
    ffr_buff[bitpos] = ffr;

    ++ bitpos;
    if ( bitpos == kPvBitLen ) {
      PackedVal obs = eventq_simulate2();
      for (ymuint i = 0; i < kPvBitLen; ++ i, obs >>= 1) {
	if ( obs & 1UL ) {
	  ffr_buff[i]->fault_sweep(op, kPvAll1);
	}
      }
      bitpos = 0;
    }
  }
  if ( bitpos > 0 ) {
    PackedVal obs = eventq_simulate2();
    for (ymuint i = 0; i < bitpos; ++ i, obs >>= 1) {
      if ( obs & 1UL ) {
	ffr_buff[i]->fault_sweep(op, kPvAll1);
      }
    }
  }
}

// @brief SPSFP故障シミュレーションの本体
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim2::_spsfp2(const TpgFault* f)
{
  // 正常値の計算を行う．
  _calc_gval2();

  // FFR 内の故障伝搬を行う．
  SimFault* ff = mFaultArray[f->id()];

  SimNode* simnode = ff->mNode;

  PackedVal lobs = kPvAll1;
  for (SimNode* node = simnode; !node->is_ffr_root(); ) {
    SimNode* onode = node->fanout(0);
    ymuint pos = node->fanout_ipos();
    lobs &= onode->gval()->_calc_gobs(pos);
    node = onode;
  }

  PackedVal valdiff = ff->mInode->gval()->val();
  if ( f->is_branch_fault() ) {
    // 入力の故障
    ymuint ipos = ff->mIpos;
    lobs &= simnode->gval()->_calc_gobs(ipos);
  }
  if ( f->val() == 1 ) {
    valdiff = ~valdiff;
  }
  lobs &= valdiff;

  // lobs が 0 ならその後のシミュレーションを行う必要はない．
  if ( lobs == kPvAll0 ) {
    return false;
  }

  // FFR の根のノードを求める．
  SimNode* root = ff->mNode;
  while ( !root->is_ffr_root() ) {
    root = root->fanout(0);
  }

  if ( root->is_output() ) {
    return true;
  }

  eventq_put2(root, kPvAll1);
  PackedVal obs = eventq_simulate2();
  return (obs != kPvAll0);
}

// @brief 正常値の計算を行う．
//
// 入力ノードに gval の設定は済んでいるものとする．
void
Fsim2::_calc_gval2()
{
  for (vector<SimPrim*>::iterator q = mGvalArray.begin();
       q != mGvalArray.end(); ++ q) {
    SimPrim* prim = *q;
    prim->calc_val();
  }
  for (vector<SimNode*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    SimNode* node = *p;
    node->clear_fval();
  }
}

// @brief イベントキューにイベントを追加する．
// @param[in] node イベントの起こったノード
// @param[in] mask 反転マスク
void
Fsim2::eventq_put2(SimNode* node,
		   PackedVal mask)
{
  node->fval()->flip_val(mask);
  mClearArray.push_back(node);
  ymuint no = node->fanout_num();
  for (ymuint i = 0; i < no; ++ i) {
    mEventQ.put(node->fanout(i));
  }
}

// @brief イベントキューを用いてシミュレーションを行う．
PackedVal
Fsim2::eventq_simulate2()
{
  // どこかの外部出力で検出されたことを表すビット
  PackedVal obs = kPvAll0;
  for ( ; ; ) {
    SimNode* node = mEventQ.get();
    if ( node == nullptr ) break;
    // すでに検出済みのビットはマスクしておく
    // これは無駄なイベントの発生を抑える．
    PackedVal diff = node->fval()->calc_val(~obs);
    if ( diff != kPvAll0 ) {
      mClearArray.push_back(node);
      if ( node->is_output() ) {
	obs |= diff;
      }
      else {
	ymuint no = node->fanout_num();
	for (ymuint i = 0; i < no; ++ i) {
	  mEventQ.put(node->fanout(i));
	}
      }
    }
  }
  // 今の故障シミュレーションで値の変わったノードを元にもどしておく
  for (vector<SimNode*>::iterator p = mClearArray.begin();
       p != mClearArray.end(); ++ p) {
    (*p)->clear_fval();
  }
  mClearArray.clear();
  return obs;
}

// @brief 現在保持している SimNode のネットワークを破棄する．
void
Fsim2::clear()
{
  mSimMap.clear();

  // mNodeArray が全てのノードを持っている
  for (vector<SimNode*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    delete *p;
  }
  mNodeArray.clear();
  mInputArray.clear();
  mGvalArray.clear();

  mFFRArray.clear();
  mFFRMap.clear();

  mClearArray.clear();

  mSimFaults.clear();
  mFaultArray.clear();

  // 念のため
  mNetwork = nullptr;
}

// @brief 外部入力ノードを作る．
SimNode*
Fsim2::make_input()
{
  ymuint id = mNodeArray.size();
  SimPrim* gval = SimPrim::new_input();
  SimPrim* fval = SimPrim::new_input();
  SimNode* node = new SimNode(id, gval, fval);
  mNodeArray.push_back(node);
  return node;
}

// @brief 単純な logic ノードを作る．
SimNode*
Fsim2::make_gate(GateType type,
		 const vector<SimNode*>& inputs)
{
  ymuint ni = inputs.size();
  vector<SimPrim*> igvals(ni);
  vector<SimPrim*> ifvals(ni);
  for (ymuint i = 0; i < ni; ++ i) {
    SimNode* inode = inputs[i];
    igvals[i] = inode->gval();
    ifvals[i] = inode->fval();
  }
  SimPrim* gval = SimPrim::new_gate(type, igvals);
  SimPrim* fval = SimPrim::new_gate(type, ifvals);
  ymuint id = mNodeArray.size();
  SimNode* node = new SimNode(id, gval, fval);
  mNodeArray.push_back(node);
  mGvalArray.push_back(gval);
  return node;
}

// @brief node に対応する SimNode* を得る．
SimNode*
Fsim2::find_simnode(const TpgNode* node) const
{
  return mSimMap[node->id()];
}

END_NAMESPACE_YM_SATPG_FSIM