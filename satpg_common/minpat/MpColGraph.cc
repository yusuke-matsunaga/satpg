
/// @file MpColGraph.cc
/// @brief MpColGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MpColGraph.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TestVector.h"
#include "Fsim.h"
#include "ym/Range.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
// @param[in] network ネットワーク
// @param[in] fault_type 故障の種類
MpColGraph::MpColGraph(const vector<const TpgFault*>& fault_list,
		       const vector<TestVector>& tv_list,
		       const TpgNetwork& network,
		       FaultType fault_type)
{
  init(fault_list, tv_list, network, fault_type);
}

// @brief デストラクタ
MpColGraph::~MpColGraph()
{
  delete [] mRowIdMap;
  delete [] mCoverFlag;
  delete [] mConflictPairArray;
  delete [] mAdjListArray;
  delete [] mCoverListArray;
  delete [] mColorMap;
}

// @brief 初期化する
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
// @param[in] network ネットワーク
// @param[in] fault_type 故障の種類
void
MpColGraph::init(const vector<const TpgFault*>& fault_list,
		 const vector<TestVector>& tv_list,
		 const TpgNetwork& network,
		 FaultType fault_type)
{
  mFaultNum = fault_list.size();
  mRowIdMap = new int[network.max_fault_id()];
  for ( auto fault_id: Range(network.max_fault_id()) ) {
    mRowIdMap[fault_id] = -1;
  }
  mCoverFlag = new bool[mFaultNum];
  for ( auto row_id: Range(mFaultNum) ) {
    auto fault = fault_list[row_id];
    mRowIdMap[fault->id()] = row_id;
    mCoverFlag[row_id] = false;
  }

  mColNum = 0;
  mNodeNum = tv_list.size();
  if ( mNodeNum == 0 ) {
    mAdjListArray = nullptr;
    mCoverListArray = nullptr;
    mColorMap = nullptr;
    return;
  }

  mAdjListArray = new IdList[mNodeNum];
  mCoverListArray = new IdList[mNodeNum];
  mColorMap = new int[mNodeNum];
  for ( auto node_id: Range(mNodeNum) ) {
    mColorMap[node_id] = 0;
  }

  gen_cover_list(fault_list, tv_list, network, fault_type);
  gen_conflict_list(tv_list);
}

// @brief 被覆リストを作る．
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
/// @param[in] network ネットワーク
// @param[in] fault_type 故障の種類
void
MpColGraph::gen_cover_list(const vector<const TpgFault*>& fault_list,
			   const vector<TestVector>& tv_list,
			   const TpgNetwork& network,
			   FaultType fault_type)
{
  Fsim fsim;
  fsim.init_fsim3(network, fault_type);

  int wpos = 0;
  fsim.clear_patterns();
  int tv_base = 0;
  for ( auto tv: tv_list ) {
    fsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      do_fsim(fsim, tv_base, wpos);
      fsim.clear_patterns();
      wpos = 0;
      tv_base += kPvBitLen;
    }
  }
  if ( wpos > 0 ) {
    do_fsim(fsim, tv_base, wpos);
  }
}

// @brief 故障シミュレーションを行う．
void
MpColGraph::do_fsim(Fsim& fsim,
		    int tv_base,
		    int num)
{
  int ndet = fsim.ppsfp();
  vector<int> det_list[kPvBitLen];
  for ( auto i: Range(ndet) ) {
    const TpgFault* fault = fsim.det_fault(i);
    PackedVal dbits = fsim.det_fault_pat(i);
    int row_id = mRowIdMap[fault->id()];
    for ( auto bit: Range(num) ) {
      if ( dbits & (1UL << bit) ) {
	int tvid = tv_base + bit;
	det_list[bit].push_back(row_id);
      }
    }
  }
  for ( auto bit: Range(num) ) {
    mCoverListArray[tv_base + bit].init(det_list[bit]);
  }
}

// @brief 衝突リストを作る．
// @param[in] tv_list テストパタンのリスト
void
MpColGraph::gen_conflict_list(const vector<TestVector>& tv_list)
{
  int vs = tv_list[0].vector_size();
  vector<vector<int>> conflict_pos_array(mNodeNum);
  mConflictPairArray = new IdList[vs * 2];
  for ( auto bit: Range(vs) ) {
    vector<int> list0;
    vector<int> list1;
    for ( auto id: Range(mNodeNum) ) {
      const TestVector& tv = tv_list[id];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	list0.push_back(id);
      }
      else if ( val == Val3::_1 ) {
	list1.push_back(id);
      }
    }
    int pos0 = bit * 2 + 0;
    int pos1 = bit * 2 + 1;
    mConflictPairArray[pos0].init(list0);
    mConflictPairArray[pos1].init(list1);
    if ( !list0.empty() && !list1.empty() ) {
      for ( auto id: list0 ) {
	conflict_pos_array[id].push_back(pos1);
      }
      for ( auto id: list1 ) {
	conflict_pos_array[id].push_back(pos0);
      }
    }
  }

  for ( auto id: Range(mNodeNum) ) {
    mAdjListArray[id].init(conflict_pos_array[id]);
  }
}

// @brief color_map を作る．
int
MpColGraph::get_color_map(vector<int>& color_map) const
{
  color_map.clear();
  color_map.resize(node_num());
  for ( auto node_id: Range(node_num()) ) {
    color_map[node_id] = mColorMap[node_id];
  }
  return color_num();
}

// @brief すべての行が彩色された列で被覆されているとき true を返す．
bool
MpColGraph::is_covered() const
{
  vector<bool> mark(fault_num(), false);
  for ( auto node_id: Range(node_num()) ) {
    if ( color(node_id) != 0 ) {
      for ( auto row_id: cover_list(node_id) ) {
	mark[row_id] = true;
      }
    }
  }
  for ( auto row_id: Range(fault_num()) ) {
    if ( !mark[row_id] ) {
      return false;
    }
  }

  return true;
}

// @brief 隣接しているノード対に同じ色が割り当てられていないか確認する．
bool
MpColGraph::verify() const
{
  for ( auto node_id: Range(node_num()) ) {
    int c = color(node_id);
    if ( c == 0 ) {
      continue;
    }
    for ( auto node1_id: adj_list(node_id) ) {
      int c1 = color(node1_id);
      if ( c1 != 0 && c1 == c ) {
	return false;
      }
    }
  }
  return true;
}

MpColGraph::IdList::IdList() :
  mBody(nullptr),
  mNum(0)
{
}

MpColGraph::IdList::~IdList()
{
  delete [] mBody;
}

void
MpColGraph::IdList::init(const vector<int>& src_list)
{
  delete [] mBody;
  mNum = src_list.size();
  mBody = new int[mNum];
  for ( auto i: Range(mNum) ) {
    mBody[i] = src_list[i];
  }
}

END_NAMESPACE_YM_SATPG
