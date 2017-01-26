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

#include "sa/TestVector.h"
#include "sa/NodeValList.h"
#include "sa/DetectOp.h"

#include "SimNode.h"
#include "SimFFR.h"
#include "ym/HashSet.h"
#include "ym/StopWatch.h"


BEGIN_NAMESPACE_YM_SATPG_SA

Fsim*
new_Fsim2()
{
  return new nsFsim2::Fsim2();
}


//////////////////////////////////////////////////////////////////////
// Fsim の実装コード
// 他に適当な場所がなかったもので．
//////////////////////////////////////////////////////////////////////

// @brief 複数の故障にスキップマークをつける．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは消される．
void
Fsim::set_skip(const vector<const TpgFault*>& fault_list)
{
  clear_skip_all();
  for (ymuint i = 0; i < fault_list.size(); ++ i) {
    const TpgFault* f = fault_list[i];
    set_skip(f);
  }
}

// @brief 複数の故障のスキップマークを消す．
// @param[in] fault_list 故障のリスト
//
// fault_list に含まれない故障のスキップマークは付けられる．
void
Fsim::clear_skip(const vector<const TpgFault*>& fault_list)
{
  set_skip_all();
  for (ymuint i = 0; i < fault_list.size(); ++ i) {
    const TpgFault* f = fault_list[i];
    clear_skip(f);
  }
}


END_NAMESPACE_YM_SATPG_SA


BEGIN_NAMESPACE_YM_SATPG_FSIM

//////////////////////////////////////////////////////////////////////
// Fsim2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Fsim2::Fsim2()
{
  mPatMap = kPvAll0;
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

  ymuint nn = network.node_num();
  ymuint ni = network.ppi_num();
  ymuint no = network.ppo_num();

  // 対応付けを行うマップの初期化
  vector<SimNode*> simmap(nn);
  mInputArray.resize(ni);
  mOutputArray.resize(no);

  ymuint nf = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    nf += network.node_fault_num(tpgnode->id());

    SimNode* node = nullptr;

    if ( tpgnode->is_ppi() ) {
      // 外部入力に対応する SimNode の生成
      node = make_input();
      mInputArray[tpgnode->input_id()] = node;
    }
    else if ( tpgnode->is_ppo() ) {
      // 外部出力に対応する SimNode の生成
      SimNode* inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(kGateBUFF, vector<SimNode*>(1, inode));
      node->set_output();
      mOutputArray[tpgnode->output_id()] = node;
    }
    else if ( tpgnode->is_dff_clock() ||
	      tpgnode->is_dff_clear() ||
	      tpgnode->is_dff_preset() ) {
      // DFFの制御端子に対応する SimNode の生成
      SimNode* inode = simmap[tpgnode->fanin(0)->id()];
      // 実際にはバッファタイプのノードに出力の印をつけるだけ．
      node = make_gate(kGateBUFF, vector<SimNode*>(1, inode));
      node->set_output();
    }
    else if ( tpgnode->is_logic() ) {
      // 論理ノードに対する SimNode の作成
      ymuint ni = tpgnode->fanin_num();

      // ファンインに対応する SimNode を探す．
      vector<SimNode*> inputs(ni);
      for (ymuint i = 0; i < ni; ++ i) {
	const TpgNode* itpgnode = tpgnode->fanin(i);
	SimNode* inode = simmap[itpgnode->id()];
	ASSERT_COND(inode );
	inputs[i] = inode;
      }

      // 出力の論理を表す SimNode を作る．
      GateType type = tpgnode->gate_type();
      node = make_gate(type, inputs);
    }
    // 対応表に登録しておく．
    simmap[tpgnode->id()] = node;
  }

  // 各ノードのファンアウトリストの設定
  ymuint node_num = mNodeArray.size();
  {
    vector<vector<SimNode*> > fanout_lists(node_num);
    vector<ymuint> ipos(node_num);
    for (vector<SimNode*>::iterator p = mNodeArray.begin();
	 p != mNodeArray.end(); ++ p) {
      SimNode* node = *p;
      ymuint ni = node->fanin_num();
      for (ymuint i = 0; i < ni; ++ i) {
	SimNode* inode = node->fanin(i);
	fanout_lists[inode->id()].push_back(node);
	ipos[inode->id()] = i;
      }
    }
    for (ymuint i = 0; i < node_num; ++ i) {
      SimNode* node = mNodeArray[i];
      node->set_fanout_list(fanout_lists[i], ipos[i]);
    }
  }

  // FFR の設定
  mFFRMap.resize(mNodeArray.size());
  ymuint ffr_num = 0;
  for (ymuint i = node_num; i > 0; ) {
    -- i;
    SimNode* node = mNodeArray[i];
    if ( node->is_output() || node->fanout_num() != 1 ) {
      ++ ffr_num;
    }
  }
  mFFRArray.resize(ffr_num);
  ffr_num = 0;
  for (ymuint i = node_num; i > 0; ) {
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
      SimNode* fo_node = node->fanout_top();
      SimFFR* ffr = mFFRMap[fo_node->id()];
      mFFRMap[node->id()] = ffr;
    }
  }

  // 最大レベルを求め，イベントキューを初期化する．
  ymuint max_level = 0;
  for (ymuint i = 0; i < no; ++ i) {
    SimNode* inode = mOutputArray[i];
    if ( max_level < inode->level() ) {
      max_level = inode->level();
    }
  }
  mEventQ.init(max_level, mNodeArray.size());


  //////////////////////////////////////////////////////////////////////
  // 故障リストの設定
  //////////////////////////////////////////////////////////////////////

  // 同時に各 SimFFR 内の故障リストも再構築する．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    p->clear_fault_list();
  }

  mSimFaults.resize(nf);
  mDetFaultArray.resize(nf);
  mFaultArray.resize(network.max_fault_id());
  ymuint fid = 0;
  for (ymuint i = 0; i < nn; ++ i) {
    const TpgNode* tpgnode = network.node(i);
    SimNode* simnode = simmap[tpgnode->id()];
    SimFFR* ffr = mFFRMap[simnode->id()];
    ymuint nf1 = network.node_fault_num(tpgnode->id());
    for (ymuint j = 0; j < nf1; ++ j) {
      const TpgFault* fault = network.node_fault(tpgnode->id(), j);
      SimNode* isimnode = nullptr;
      ymuint ipos = 0;
      if ( fault->is_branch_fault() ) {
	ipos = fault->tpg_pos();
	const TpgNode* inode = tpgnode->fanin(ipos);
	isimnode = simmap[inode->id()];
      }
      else {
	isimnode = simnode;
      }
      mSimFaults[fid].set(fault, simnode, ipos, isimnode);
      SimFault* ff = &mSimFaults[fid];
      mFaultArray[fault->id()] = ff;
      ff->mSkip = false;
      ffr->add_fault(ff);
      ++ fid;
    }
  }
}

// @brief 全ての故障にスキップマークをつける．
void
Fsim2::set_skip_all()
{
  for (ymuint i = 0; i < mSimFaults.size(); ++ i) {
    mSimFaults[i].mSkip = true;
  }
}

// @brief 故障にスキップマークをつける．
// @param[in] f 対象の故障
void
Fsim2::set_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = true;
}

// @brief 全ての故障のスキップマークを消す．
void
Fsim2::clear_skip_all()
{
  for (ymuint i = 0; i < mSimFaults.size(); ++ i) {
    mSimFaults[i].mSkip = false;
  }
}

// @brief 故障のスキップマークを消す．
// @param[in] f 対象の故障
void
Fsim2::clear_skip(const TpgFault* f)
{
  mFaultArray[f->id()]->mSkip = false;
}

// @brief SPSFP故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim2::spsfp(const TestVector* tv,
	     const TpgFault* f)
{
  _set_sp(tv);
  return _spsfp(f);
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
  _set_sp(assign_list);
  return _spsfp(f);
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] tv テストベクタ
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
ymuint
Fsim2::sppfp(const TestVector* tv)
{
  _set_sp(tv);
  return _sppfp();
}

// @brief ひとつのパタンで故障シミュレーションを行う．
// @param[in] assign_list 値の割当リスト
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．
ymuint
Fsim2::sppfp(const NodeValList& assign_list)
{
  _set_sp(assign_list);
  return _sppfp();
}

// @brief ppsfp 用のパタンバッファをクリアする．
void
Fsim2::clear_patterns()
{
  mPatMap = kPvAll0;
}

// @brief ppsfp 用のパタンを設定する．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
// @param[in] tv テストベクタ
void
Fsim2::set_pattern(ymuint pos,
		   const TestVector* tv)
{
  ASSERT_COND( pos < kPvBitLen );
  mPatBuff[pos] = tv;
  mPatMap |= (1ULL << pos);
}

// @brief 設定した ppsfp 用のパタンを読み出す．
// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
const TestVector*
Fsim2::get_pattern(ymuint pos)
{
  ASSERT_COND( pos < kPvBitLen );
  if ( mPatMap & (1ULL << pos) ) {
    return mPatBuff[pos];
  }
  else {
    return nullptr;
  }
}

// @brief 複数のパタンで故障シミュレーションを行う．
// @return 検出された故障数を返す．
//
// 検出された故障は det_fault() で取得する．<br>
// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
ymuint
Fsim2::ppsfp()
{
  mDetNum = 0;

  if ( mPatMap == kPvAll0 ) {
    // パタンが一つも設定されていない．
    return 0;
  }

  _set_pp();

  // 正常値の計算を行う．
  _calc_gval();

  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    const SimFFR& ffr = *p;
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault::mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = ffr.fault_prop();

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    // FFR の出力の故障伝搬を行う．
    SimNode* root = ffr.root();
    PackedVal obs = kPvAll0;
    if ( root->is_output() ) {
      obs = kPvAll1;
    }
    else {
      mEventQ.put_trigger(root, ffr_req);
      obs = mEventQ.simulate();
    }
    obs &= mPatMap;

    _fault_sweep(ffr.fault_list(), obs);
  }

  return mDetNum;
}

// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
ymuint
Fsim2::det_fault_num()
{
  return mDetNum;
}

// @brief 直前の sppfp/ppsfp で検出された故障を返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
const TpgFault*
Fsim2::det_fault(ymuint pos)
{
  ASSERT_COND( pos < det_fault_num() );
  return mDetFaultArray[pos].mFault;
}

// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
PackedVal
Fsim2::det_fault_pat(ymuint pos)
{
  ASSERT_COND( pos < det_fault_num() );
  return mDetFaultArray[pos].mPat;
}

// @brief 一つのパタンを全ビットに展開して設定する．
// @param[in] tv 設定するテストベクタ
void
Fsim2::_set_sp(const TestVector* tv)
{
  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = mInputArray[i];
    PackedVal val = (tv->val3(i) == kVal1) ? kPvAll1 : kPvAll0;
    simnode->set_gval(val);
  }
}

// @brief 一つのパタンを全ビットに展開して設定する．
// @param[in] assign_list 設定する値の割り当てリスト
void
Fsim2::_set_sp(const NodeValList& assign_list)
{
  // デフォルトで 0 にする．
  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
    SimNode* simnode = mInputArray[i];
    simnode->set_gval(kPvAll0);
  }

  ymuint n = assign_list.size();
  for (ymuint i = 0; i < n; ++ i) {
    NodeVal nv = assign_list[i];
    if ( nv.val() ) {
      SimNode* simnode = mInputArray[nv.node()->input_id()];
      simnode->set_gval(kPvAll1);
    }
  }
}

// @brief 複数のパタンを設定する．
void
Fsim2::_set_pp()
{
  // 設定されていないビットはどこか他の設定されているビットをコピーする．
  // 設定されている最初のビット位置を求める．
  ymuint first = 0;
  for ( ; first < kPvBitLen; ++ first) {
    if ( mPatMap & (1ULL << first) ) {
      break;
    }
  }
  ASSERT_COND( first < kPvBitLen );

  ymuint npi = mInputArray.size();
  for (ymuint i = 0; i < npi; ++ i) {
    PackedVal val = kPvAll0;
    PackedVal bit = 1ULL;
    for (ymuint j = 0; j < kPvBitLen; ++ j, bit <<= 1) {
      ymuint pos = (mPatMap & bit) ? j : first;
      if ( mPatBuff[pos]->val3(i) == kVal1 ) {
	val |= bit;
      }
    }
    SimNode* simnode = mInputArray[i];
    simnode->set_gval(val);
  }
}

// @brief SPPFP故障シミュレーションの本体
// @return 検出された故障数を返す．
ymuint
Fsim2::_sppfp()
{
  mDetNum = 0;

  // 正常値の計算を行う．
  _calc_gval();

  ymuint bitpos = 0;
  SimFFR* ffr_buff[kPvBitLen];
  // FFR ごとに処理を行う．
  for (vector<SimFFR>::iterator p = mFFRArray.begin();
       p != mFFRArray.end(); ++ p) {
    SimFFR* ffr = &(*p);
    // FFR 内の故障伝搬を行う．
    // 結果は SimFault.mObsMask に保存される．
    // FFR 内の全ての obs マスクを ffr_req に入れる．
    PackedVal ffr_req = ffr->fault_prop();

    // ffr_req が 0 ならその後のシミュレーションを行う必要はない．
    if ( ffr_req == kPvAll0 ) {
      continue;
    }

    SimNode* root = ffr->root();
    if ( root->is_output() ) {
      // 常に観測可能
      _fault_sweep(ffr->fault_list());
      continue;
    }

    // キューに積んでおく
    PackedVal bitmask = 1UL << bitpos;
    mEventQ.put_trigger(root, bitmask);
    ffr_buff[bitpos] = ffr;

    ++ bitpos;
    if ( bitpos == kPvBitLen ) {
      PackedVal obs = mEventQ.simulate();
      for (ymuint i = 0; i < kPvBitLen; ++ i, obs >>= 1) {
	if ( obs & 1UL ) {
	  _fault_sweep(ffr_buff[i]->fault_list());
	}
      }
      bitpos = 0;
    }
  }
  if ( bitpos > 0 ) {
    PackedVal obs = mEventQ.simulate();
    for (ymuint i = 0; i < bitpos; ++ i, obs >>= 1) {
      if ( obs & 1UL ) {
	_fault_sweep(ffr_buff[i]->fault_list());
      }
    }
  }

  return mDetNum;
}

// @brief SPSFP故障シミュレーションの本体
// @param[in] f 対象の故障
// @retval true 故障の検出が行えた．
// @retval false 故障の検出が行えなかった．
bool
Fsim2::_spsfp(const TpgFault* f)
{
  // 正常値の計算を行う．
  _calc_gval();

  // FFR 内の故障伝搬を行う．
  SimFault* ff = mFaultArray[f->id()];

  SimNode* simnode = ff->mNode;

  PackedVal lobs = kPvAll1;
  for (SimNode* node = simnode; !node->is_ffr_root(); ) {
    SimNode* onode = node->fanout_top();
    ymuint pos = node->fanout_ipos();
    lobs &= onode->_calc_gobs(pos);
    node = onode;
  }

  PackedVal valdiff = ff->mInode->gval();
  if ( f->is_branch_fault() ) {
    // 入力の故障
    ymuint ipos = ff->mIpos;
    lobs &= simnode->_calc_gobs(ipos);
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
    root = root->fanout_top();
  }

  if ( root->is_output() ) {
    return true;
  }

  mEventQ.put_trigger(root, kPvAll1);
  PackedVal obs = mEventQ.simulate();
  return (obs != kPvAll0);
}

// @brief 正常値の計算を行う．
//
// 入力ノードに gval の設定は済んでいるものとする．
void
Fsim2::_calc_gval()
{
  for (vector<SimNode*>::iterator q = mLogicArray.begin();
       q != mLogicArray.end(); ++ q) {
    SimNode* node = *q;
    node->calc_gval();
  }
}

// @brief 故障をスキャンして結果をセットする(sppfp用)
// @param[in] fault_list 故障のリスト
void
Fsim2::_fault_sweep(const vector<SimFault*>& fault_list)
{
  for (vector<SimFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    SimFault* ff = *p;
    if ( ff->mSkip || ff->mObsMask == kPvAll0 ) {
      continue;
    }
    const TpgFault* f = ff->mOrigF;
    mDetFaultArray[mDetNum].mFault = f;
    mDetFaultArray[mDetNum].mPat = kPvAll1; // ダミー
    ++ mDetNum;
  }
}

// @brief 故障をスキャンして結果をセットする(ppsfp用)
// @param[in] fault_list 故障のリスト
// @param[in] pat 検出パタン
void
Fsim2::_fault_sweep(const vector<SimFault*>& fault_list,
		    PackedVal mask)
{
  for (vector<SimFault*>::const_iterator p = fault_list.begin();
       p != fault_list.end(); ++ p) {
    SimFault* ff = *p;
    if ( ff->mSkip ) {
      continue;
    }
    PackedVal pat = ff->mObsMask & mask;
    if ( pat != kPvAll0 ) {
      const TpgFault* f = ff->mOrigF;
      mDetFaultArray[mDetNum].mFault = f;
      mDetFaultArray[mDetNum].mPat = pat;
      ++ mDetNum;
    }
  }
}

// @brief 現在保持している SimNode のネットワークを破棄する．
void
Fsim2::clear()
{
  // mNodeArray が全てのノードを持っている
  for (vector<SimNode*>::iterator p = mNodeArray.begin();
       p != mNodeArray.end(); ++ p) {
    delete *p;
  }
  mNodeArray.clear();
  mInputArray.clear();
  mOutputArray.clear();
  mLogicArray.clear();

  mFFRArray.clear();
  mFFRMap.clear();

  mSimFaults.clear();
  mFaultArray.clear();
}

// @brief 外部入力ノードを作る．
SimNode*
Fsim2::make_input()
{
  ymuint id = mNodeArray.size();
  SimNode* node = SimNode::new_input(id);
  mNodeArray.push_back(node);
  return node;
}

// @brief 単純な logic ノードを作る．
SimNode*
Fsim2::make_gate(GateType type,
		 const vector<SimNode*>& inputs)
{
  ymuint id = mNodeArray.size();
  SimNode* node = SimNode::new_gate(id, type, inputs);
  mNodeArray.push_back(node);
  mLogicArray.push_back(node);
  return node;
}

END_NAMESPACE_YM_SATPG_FSIM
