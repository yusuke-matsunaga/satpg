﻿
/// @File TpgNetwork.cc
/// @brief TpgNetwork の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "TpgNetwork.h"
#include "TpgNode.h"
#include "TpgNodeFactory.h"
#include "TpgGateInfo.h"
#include "TpgFault.h"
#include "TpgDff.h"
#include "TpgMFFC.h"
#include "TpgFFR.h"

#include "GateType.h"

#include "NodeMap.h"
#include "AuxNodeInfo.h"

#include "ym/BnNetwork.h"
#include "ym/BnPort.h"
#include "ym/BnDff.h"
#include "ym/BnNode.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_SATPG

BEGIN_NONAMESPACE

// BnFuncType を GateType に変換する．
inline
GateType
conv_to_gate_type(BnNodeType type)
{
  switch ( type ) {
  case BnNodeType::C0:   return GateType::Const0;
  case BnNodeType::C1:   return GateType::Const1;
  case BnNodeType::Buff: return GateType::Buff;
  case BnNodeType::Not:  return GateType::Not;
  case BnNodeType::And:  return GateType::And;
  case BnNodeType::Nand: return GateType::Nand;
  case BnNodeType::Or:   return GateType::Or;
  case BnNodeType::Nor:  return GateType::Nor;
  case BnNodeType::Xor:  return GateType::Xor;
  case BnNodeType::Xnor: return GateType::Xnor;
  default: break;
  }
  ASSERT_NOT_REACHED;
  return GateType::Const0;
}

// immediate dominator リストをマージする．
const TpgNode*
merge(const TpgNode* node1,
      const TpgNode* node2)
{
  for ( ; ; ) {
    if ( node1 == node2 ) {
      return node1;
    }
    if ( node1 == nullptr || node2 == nullptr ) {
      return nullptr;
    }
    int id1 = node1->id();
    int id2 = node2->id();
    if ( id1 < id2 ) {
      node1 = node1->imm_dom();
    }
    else if ( id1 > id2 ) {
      node2 = node2->imm_dom();
    }
  }
}

// pair の最初の要素で比較を行なうファンクター
// こういうのは lambda 表記があると簡潔にすむ．
struct Lt
{
  bool
  operator()(const pair<int, int>& left,
	     const pair<int, int>& right)
  {
    return left.first < right.first;
  }

};


void
check_network_connection(const TpgNetwork& network)
{
  // fanin/fanout の sanity check
  bool error = false;

  int nn = network.node_num();
  for ( int i = 0; i < nn; ++ i ) {
    const TpgNode* node = network.node(i);
    int nfi = node->fanin_num();
    for ( int j = 0; j < nfi; ++ j ) {
      const TpgNode* inode = node->fanin(j);
      int nfo = inode->fanout_num();
      bool found = false;
      for ( int k = 0; k < nfo; ++ k ) {
	if ( inode->fanout(k) == node ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	cout << "Error: inode(" << inode->id() << ") is a fanin of "
	     << "node(" << node->id() << "), but "
	     << "node(" << node->id() << ") is not a fanout of "
	     << "inode(" << inode->id() << ")" << endl;
      }
    }
    int nfo = node->fanout_num();
    for ( int j = 0; j < nfo; ++ j ) {
      const TpgNode* onode = node->fanout(j);
      int nfi = onode->fanin_num();
      bool found = false;
      for ( int k = 0; k < nfi; ++ k ) {
	if ( onode->fanin(k) == node ) {
	  found = true;
	  break;
	}
      }
      if ( !found ) {
	error = true;
	cout << "Error: onode(" << onode->id() << ") is a fanout of "
	     << "node(" << node->id() << "), but "
	     << "node(" << node->id() << ") is not a fanin of "
	     << "onode(" << onode->id() << ")" << endl;
      }
    }
  }
  if ( error ) {
    cout << "network connectivity check failed" << endl;
    abort();
  }
}

END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// クラス TpgNetwork
//////////////////////////////////////////////////////////////////////

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @return 読み込みが成功したら true を返す．
bool
TpgNetwork::read_blif(const string& filename)
{
  BnNetwork network;
  bool stat = nsBnet::read_blif(network, filename);
  if ( stat ) {
    set(network);
  }

  return stat;
}

// @brief blif ファイルを読み込む．
// @param[in] filename ファイル名
// @param[in] cell_library セルライブラリ
// @return 読み込みが成功したら true を返す．
bool
TpgNetwork::read_blif(const string& filename,
		      const ClibCellLibrary& cell_library)
{
  BnNetwork network;
  bool stat = nsBnet::read_blif(network, filename, cell_library);
  if ( stat ) {
    set(network);
  }

  return stat;
}

// @brief iscas89 形式のファイルを読み込む．
// @param[in] filename ファイル名
// @return 読み込みが成功したら true を返す．
bool
TpgNetwork::read_iscas89(const string& filename)
{
  BnNetwork network;
  bool stat = nsBnet::read_iscas89(network, filename);
  if ( stat ) {
    set(network);
  }

  return stat;
}

BEGIN_NONAMESPACE

// @brief ノードの TFI にマークをつける．
int
tfimark(const TpgNode* node,
	vector<bool>& mark)
{
  if ( mark[node->id()] ) {
    return 0;
  }
  mark[node->id()] = true;

  int ni = node->fanin_num();
  int n = 1;
  for ( int i = 0; i < ni; ++ i ) {
    n += tfimark(node->fanin(i), mark);
  }
  return n;
}

END_NONAMESPACE


// @brief 内容を設定する．
// @param[in] network 設定元のネットワーク
void
TpgNetwork::set(const BnNetwork& network)
{
  // まずクリアしておく．
  clear();

  //////////////////////////////////////////////////////////////////////
  // NodeInfoMgr にノードの論理関数を登録する．
  //////////////////////////////////////////////////////////////////////
  TpgGateInfoMgr node_info_mgr;
  int nexpr = network.expr_num();
  vector<const TpgGateInfo*> node_info_list(nexpr);
  int extra_node_num = 0;
  for ( int i = 0; i < nexpr; ++ i ) {
    Expr expr = network.expr(i);
    int ni = expr.input_size();
    const TpgGateInfo* node_info = node_info_mgr.complex_type(ni, expr);
    node_info_list[i] = node_info;
  }


  //////////////////////////////////////////////////////////////////////
  // 追加で生成されるノード数を数える．
  //////////////////////////////////////////////////////////////////////
  int nl = network.logic_num();
  for ( int i = 0; i < nl; ++ i ) {
    const BnNode* src_node = network.logic(i);
    BnNodeType logic_type = src_node->type();
    if ( logic_type == BnNodeType::Expr ) {
      const TpgGateInfo* node_info = node_info_list[src_node->func_id()];
      extra_node_num += node_info->extra_node_num();
    }
    else if ( logic_type == BnNodeType::Xor || logic_type == BnNodeType::Xnor ) {
      int ni = src_node->fanin_num();
      extra_node_num += (ni - 2);
    }
  }


  //////////////////////////////////////////////////////////////////////
  // 要素数を数え，必要なメモリ領域を確保する．
  //////////////////////////////////////////////////////////////////////

  // BnPort は複数ビットの場合があり，さらに入出力が一緒なのでめんどくさい
  vector<int> input_map;
  vector<int> output_map;
  int np = network.port_num();
  for ( int i = 0; i < np; ++ i ) {
    const BnPort* port = network.port(i);
    int nb = port->bit_width();
    for ( int j = 0; j < nb; ++ j ) {
      int id = port->bit(j);
      const BnNode* node = network.node(id);
      if ( node->is_input() ) {
	input_map.push_back(id);
      }
      else if ( node->is_output() ) {
	output_map.push_back(id);
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }
  }
  mInputNum = input_map.size();
  mOutputNum = output_map.size();
  mDffNum = network.dff_num();

  int dff_control_num = 0;
  for ( int i = 0; i < mDffNum; ++ i ) {
    const BnDff* dff = network.dff(i);
    // まずクロックで一つ
    ++ dff_control_num;
    if ( dff->clear() != kBnNullId ) {
      // クリア端子で一つ
      ++ dff_control_num;
    }
    if ( dff->preset() != kBnNullId ) {
      // プリセット端子で一つ
      ++ dff_control_num;
    }
  }

  mDffArray = new TpgDff[mDffNum];
  for ( int i = 0; i < mDffNum; ++ i ) {
    mDffArray[i].mId = i;
  }

  int nn = mInputNum + mOutputNum + mDffNum * 2 + nl + extra_node_num + dff_control_num;
  mNodeArray = new TpgNode*[nn];
  mAuxInfoArray = new AuxNodeInfo[nn];

  int nppi = mInputNum + mDffNum;
  mPPIArray = new TpgNode*[nppi];

  int nppo = mOutputNum + mDffNum;
  mPPOArray = new TpgNode*[nppo];
  mPPOArray2 = new TpgNode*[nppo];

  NodeMap node_map;

  mNodeNum = 0;
  mFaultNum = 0;


  //////////////////////////////////////////////////////////////////////
  // 入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( int i = 0; i < mInputNum; ++ i ) {
    int id = input_map[i];
    const BnNode* src_node = network.node(id);
    ASSERT_COND( src_node->is_input() );
    int nfo = src_node->fanout_num();
    TpgNode* node = make_input_node(i, src_node->name(), nfo);
    mPPIArray[i] = node;

    node_map.reg(id, node);
  }


  //////////////////////////////////////////////////////////////////////
  // DFFの出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( int i = 0; i < mDffNum; ++ i ) {
    const BnDff* src_dff = network.dff(i);
    const BnNode* src_node = network.node(src_dff->output());
    ASSERT_COND( src_node->is_input() );
    int nfo = src_node->fanout_num();
    TpgDff* dff = &mDffArray[i];
    int iid = i + mInputNum;
    TpgNode* node = make_dff_output_node(iid, dff, src_node->name(), nfo);
    mPPIArray[iid] = node;
    dff->mOutput = node;

    node_map.reg(src_node->id(), node);
  }


  //////////////////////////////////////////////////////////////////////
  // 論理ノードを作成する．
  // BnNetwork::logic() はトポロジカルソートされているので
  // 結果として TpgNode もトポロジカル順に並べられる．
  //////////////////////////////////////////////////////////////////////
  for ( int i = 0; i < nl; ++ i ) {
    const BnNode* src_node = network.logic(i);
    const TpgGateInfo* node_info = nullptr;
    BnNodeType logic_type = src_node->type();
    if ( logic_type == BnNodeType::Expr ) {
      node_info = node_info_list[src_node->func_id()];
    }
    else {
      ASSERT_COND( logic_type != BnNodeType::TvFunc );
      GateType gate_type = conv_to_gate_type(logic_type);
      node_info = node_info_mgr.simple_type(gate_type);
    }

    // ファンインのノードを取ってくる．
    int ni = src_node->fanin_num();
    vector<TpgNode*> fanin_array(ni);
    for ( int j = 0; j < ni; ++ j ) {
      fanin_array[j] = node_map.get(src_node->fanin(j));
    }
    int nfo = src_node->fanout_num();
    TpgNode* node = make_logic_node(src_node->name(), node_info, fanin_array, nfo);

    // ノードを登録する．
    node_map.reg(src_node->id(), node);
  }


  //////////////////////////////////////////////////////////////////////
  // 出力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( int i = 0; i < mOutputNum; ++ i ) {
    int id = output_map[i];
    const BnNode* src_node = network.node(id);
    ASSERT_COND( src_node->is_output() );
    TpgNode* inode = node_map.get(src_node->fanin());
    string buf = "*";
    buf += src_node->name();
    TpgNode* node = make_output_node(i, buf, inode);
    mPPOArray[i] = node;
  }


  //////////////////////////////////////////////////////////////////////
  // DFFの入力ノードを作成する．
  //////////////////////////////////////////////////////////////////////
  for ( int i = 0; i < mDffNum; ++ i ) {
    const BnDff* src_dff = network.dff(i);
    const BnNode* src_node = network.node(src_dff->input());

    TpgNode* inode = node_map.get(src_node->fanin());
    string dff_name = src_dff->name();
    string input_name = dff_name + ".input";
    TpgDff* dff = &mDffArray[i];
    int oid = i + mOutputNum;
    TpgNode* node = make_dff_input_node(oid, dff, input_name, inode);
    mPPOArray[oid] = node;
    dff->mInput = node;

    // クロック端子を作る．
    const BnNode* src_clock = network.node(src_dff->clock());
    TpgNode* clock_fanin = node_map.get(src_clock->fanin());
    string clock_name = dff_name + ".clock";
    TpgNode* clock = make_dff_clock_node(dff, clock_name, clock_fanin);
    dff->mClock = clock;

    // クリア端子を作る．
    if ( src_dff->clear() != kBnNullId ) {
      const BnNode* src_clear = network.node(src_dff->clear());
      TpgNode* clear_fanin = node_map.get(src_clear->fanin());
      string clear_name = dff_name + ".clear";
      TpgNode* clear = make_dff_clear_node(dff, clear_name, clear_fanin);
      dff->mClear = clear;
    }

    // プリセット端子を作る．
    if ( src_dff->preset() != kBnNullId ) {
      const BnNode* src_preset = network.node(src_dff->preset());
      TpgNode* preset_fanin = node_map.get(src_preset->fanin());
      string preset_name = dff_name + ".preset";
      TpgNode* preset = make_dff_preset_node(dff, preset_name, preset_fanin);
      dff->mPreset = preset;
    }
  }

  ASSERT_COND( mNodeNum == nn );


  //////////////////////////////////////////////////////////////////////
  // ファンアウトをセットする．
  //////////////////////////////////////////////////////////////////////
  vector<int> nfo_array(mNodeNum, 0);
  for ( int i = 0; i < mNodeNum; ++ i ) {
    TpgNode* node = mNodeArray[i];
    int ni = node->fanin_num();
    for ( int j = 0; j < ni; ++ j ) {
      TpgNode* inode = node->fanin(j);
      int& fo_pos = nfo_array[inode->id()];
      inode->set_fanout(fo_pos, node);
      ++ fo_pos;
    }
  }
  { // 検証
    int error = 0;
    for ( int i = 0; i < mNodeNum; ++ i ) {
      TpgNode* node = mNodeArray[i];
      if ( nfo_array[node->id()] != node->fanout_num() ) {
	if ( error == 0 ) {
	  cerr << "Error in TpgNetwork()" << endl;
	}
	cerr << "nfo_array[Node#" << node->id() << "] = " << nfo_array[node->id()] << endl
	     << "node->fanout_num()    = " << node->fanout_num() << endl;
	++ error;
      }
    }
    if ( error ) {
      abort();
    }
    // 接続が正しいかチェックする．
    check_network_connection(*this);
  }


  //////////////////////////////////////////////////////////////////////
  // データ系のノードに印をつける．
  //////////////////////////////////////////////////////////////////////
  vector<bool> dmarks(mNodeNum, false);
  for ( int i = 0; i < ppo_num(); ++ i ) {
    const TpgNode* node = ppo(i);
    tfimark(node, dmarks);
  }


  //////////////////////////////////////////////////////////////////////
  // 代表故障を求める．
  //////////////////////////////////////////////////////////////////////
  mRepFaultNum = 0;
  for ( int i = 0; i < mNodeNum; ++ i ) {
    // ノードごとに代表故障を設定する．
    // この処理は出力側から行う必要がある．
    TpgNode* node = mNodeArray[mNodeNum - i - 1];
    if ( dmarks[node->id()] ) {
      int nf = set_rep_faults(node);
      mRepFaultNum += nf;
    }
  }

  mRepFaultArray = new const TpgFault*[mRepFaultNum];
  int wpos = 0;
  for ( int i = 0; i < mNodeNum; ++ i ) {
    const AuxNodeInfo& aux_node_info = mAuxInfoArray[i];
    int nf = aux_node_info.fault_num();
    for ( int j = 0; j < nf; ++ j, ++ wpos ) {
      const TpgFault* fault = aux_node_info.fault(j);
      mRepFaultArray[wpos] = fault;
    }
  }


  //////////////////////////////////////////////////////////////////////
  // TFI のサイズの昇順に並べた出力順を
  // mPPOArray2 に記録する．
  //////////////////////////////////////////////////////////////////////
  int npo = ppo_num();
  vector<pair<int, int> > tmp_list(npo);
  for ( int i = 0; i < npo; ++ i ) {
    const TpgNode* onode = ppo(i);
    // onode の TFI のノード数を計算する．
    vector<bool> mark(nn, false);
    int n = tfimark(onode, mark);
    tmp_list[i] = make_pair(n, i);
  }

  // TFI のサイズの昇順にソートする．
  sort(tmp_list.begin(), tmp_list.end(), Lt());
  // tmp_list の順に mPPOArray2 にセットする．
  for ( int i = 0; i < npo; ++ i ) {
    int opos = tmp_list[i].second;
    TpgNode* onode = mPPOArray[opos];
    mPPOArray2[i] = onode;
    onode->set_output_id2(i);
  }

  // immediate dominator を求める．
  for ( int i = 0; i < mNodeNum; ++ i ) {
    TpgNode* node = mNodeArray[mNodeNum - i - 1];
    const TpgNode* imm_dom = nullptr;
    if ( !node->is_ppo() ) {
      int nfo = node->fanout_num();
      if ( nfo > 0 ) {
	imm_dom = node->fanout(0);
	for ( int i = 1; imm_dom != nullptr && i < nfo; ++ i ) {
	  imm_dom = merge(imm_dom, node->fanout(i));
	}
      }
    }
    node->set_imm_dom(imm_dom);
  }


  //////////////////////////////////////////////////////////////////////
  // FFR と MFFC の根のノードを求める．
  //////////////////////////////////////////////////////////////////////
  vector<TpgNode*> ffr_root_list;
  vector<TpgNode*> mffc_root_list;
  for ( int i = 0; i < mNodeNum; ++ i ) {
    TpgNode* node = mNodeArray[i];
    if ( !dmarks[node->id()] ) {
      // データ系のノードでなければスキップ
      continue;
    }
    if ( node->ffr_root() == node ) {
      ffr_root_list.push_back(node);

      // MFFC の根は必ず FFR の根でもある．
      if ( node->imm_dom() == nullptr ) {
	mffc_root_list.push_back(node);
      }
    }
  }


  //////////////////////////////////////////////////////////////////////
  // FFR の情報を作る．
  //////////////////////////////////////////////////////////////////////
  mFfrNum = ffr_root_list.size();
  mFfrArray = new TpgFFR[mFfrNum];
  for ( int i = 0; i < mFfrNum; ++ i ) {
    TpgNode* node = ffr_root_list[i];
    TpgFFR* ffr = &mFfrArray[i];
    set_ffr(node, ffr);
  }


  //////////////////////////////////////////////////////////////////////
  // MFFC の情報を作る．
  //////////////////////////////////////////////////////////////////////
  mMffcNum = mffc_root_list.size();
  mMffcArray = new TpgMFFC[mMffcNum];
  for ( int i = 0; i < mMffcNum; ++ i ) {
    TpgNode* node = mffc_root_list[i];
    TpgMFFC* mffc = &mMffcArray[i];
    set_mffc(node, mffc);
  }
}

// @brief 代表故障を設定する．
// @param[in] node 対象のノード
int
TpgNetwork::set_rep_faults(TpgNode* node)
{
  vector<TpgFault*> fault_list;

  if ( node->fanout_num() == 1 ) {
    TpgNode* onode = node->fanout(0);
    // ファンアウト先が一つならばそのファンイン
    // ブランチの故障と出力の故障は等価
    int ni = onode->fanin_num();
    int ipos = ni;
    for ( int i = 0; i < ni; ++ i ) {
      if ( onode->fanin(i) == node ) {
	ipos = i;
	break;
      }
    }
    ASSERT_COND( ipos < ni );

    TpgFault* rep0 = _node_input_fault(onode->id(), 0, ipos);
    TpgFault* of0 = _node_output_fault(node->id(), 0);
    if ( of0 != nullptr ) {
      of0->set_rep(rep0);
    }

    TpgFault* rep1 = _node_input_fault(onode->id(), 1, ipos);
    TpgFault* of1 = _node_output_fault(node->id(), 1);
    if ( of1 != nullptr ){
      of1->set_rep(rep1);
    }
  }

  if ( !node->is_ppo() ) {
    TpgFault* of0 = _node_output_fault(node->id(), 0);
    if ( of0 != nullptr ) {
      TpgFault* rep0 = of0->_rep_fault();
      if ( rep0 == nullptr ) {
	of0->set_rep(of0);
	fault_list.push_back(of0);
      }
      else {
	of0->set_rep(rep0->_rep_fault());
      }
    }

    TpgFault* of1 = _node_output_fault(node->id(), 1);
    if ( of1 != nullptr ) {
      TpgFault* rep1 = of1->_rep_fault();
      if ( rep1 == nullptr ) {
	of1->set_rep(of1);
	fault_list.push_back(of1);
      }
      else {
	of1->set_rep(rep1->_rep_fault());
      }
    }
  }

  int ni = node->fanin_num();
  for ( int i = 0; i < ni; ++ i ) {
    TpgFault* if0 = _node_input_fault(node->id(), 0, i);
    if ( if0 != nullptr ) {
      TpgFault* rep0 = if0->_rep_fault();
      if ( rep0 == nullptr ) {
	if0->set_rep(if0);
	fault_list.push_back(if0);
      }
      else {
	if0->set_rep(rep0->_rep_fault());
      }
    }

    TpgFault* if1 = _node_input_fault(node->id(), 1, i);
    if ( if1 != nullptr ) {
      TpgFault* rep1 = if1->_rep_fault();
      if ( rep1 == nullptr ) {
	if1->set_rep(if1);
	fault_list.push_back(if1);
      }
      else {
	if1->set_rep(rep1->_rep_fault());
      }
    }
  }

  // node の代表故障をセットする．
  mAuxInfoArray[node->id()].set_fault_list(fault_list, mAlloc);

  return fault_list.size();
}

// @brief FFR の情報を設定する．
// @param[in] root FFR の根のノード
// @param[in] ffr 対象の FFR
void
TpgNetwork::set_ffr(TpgNode* root,
		    TpgFFR* ffr)
{
  // root を根とするFFRの故障リストを求める．
  vector<TpgNode*> node_list;
  vector<TpgFault*> fault_list;

  node_list.push_back(root);
  while ( !node_list.empty() ) {
    TpgNode* node = node_list.back();
    node_list.pop_back();

    mAuxInfoArray[node->id()].fault_list(fault_list);

    int ni = node->fanin_num();
    for ( int i = 0; i < ni; ++ i ) {
      TpgNode* inode = node->fanin(i);
      if ( inode->ffr_root() != inode ) {
	node_list.push_back(inode);
      }
    }
  }

  mAuxInfoArray[root->id()].set_ffr(ffr);
  ffr->set(root, fault_list, mAlloc);
}

// @brief MFFC の情報を設定する．
// @param[in] root MFFCの根のノード
// @param[in] mffc 対象のMFFC
void
TpgNetwork::set_mffc(TpgNode* root,
		     TpgMFFC* mffc)
{
  // root を根とする MFFC の情報を得る．
  vector<bool> mark(node_num());
  vector<TpgNode*> node_list;
  vector<const TpgFFR*> ffr_list;
  vector<TpgFault*> fault_list;

  node_list.push_back(root);
  mark[root->id()] = true;
  while ( !node_list.empty() ) {
    TpgNode* node = node_list.back();
    node_list.pop_back();

    if ( node->ffr_root() == node ) {
      ffr_list.push_back(mAuxInfoArray[node->id()].ffr());
    }

    mAuxInfoArray[node->id()].fault_list(fault_list);

    int ni = node->fanin_num();
    for ( int i = 0; i < ni; ++ i ) {
      TpgNode* inode = node->fanin(i);
      if ( !mark[inode->id()] &&
	   inode->imm_dom() != nullptr ) {
	mark[inode->id()] = true;
	node_list.push_back(inode);
      }
    }
  }

  mAuxInfoArray[root->id()].set_mffc(mffc);
  mffc->set(root, ffr_list, fault_list, mAlloc);
}

END_NAMESPACE_YM_SATPG
