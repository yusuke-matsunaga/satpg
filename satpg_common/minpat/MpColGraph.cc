
/// @file MpColGraph.cc
/// @brief MpColGraph の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MpColGraph.h"
#include "TestVector.h"
#include "ym/Range.h"
#include "ym/HashSet.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] tv_list テストパタンのリスト
MpColGraph::MpColGraph(const vector<TestVector>& tv_list) :
  mTvList(tv_list),
  mNodeNum(mTvList.size()),
  mVectorSize(0),
  mOidListArray(mNodeNum),
  mColNum(0),
  mColorMap(mNodeNum, 0)
{
  if ( mNodeNum > 0 ) {
    TestVector tv0 = mTvList[0];
    mVectorSize = tv0.vector_size();
    mNodeListArray.resize(mVectorSize * 2);

    gen_conflict_list();
  }
}

// @brief デストラクタ
MpColGraph::~MpColGraph()
{
}

// @brief 衝突リストを作る．
void
MpColGraph::gen_conflict_list()
{
  for ( auto bit: Range(mVectorSize) ) {
    int oid0 = bit * 2 + 0;
    int oid1 = bit * 2 + 1;
    vector<int>& list0 = mNodeListArray[oid0];
    vector<int>& list1 = mNodeListArray[oid1];
    for ( auto id: Range(mNodeNum) ) {
      const TestVector& tv = mTvList[id];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	list0.push_back(id);
      }
      else if ( val == Val3::_1 ) {
	list1.push_back(id);
      }
    }
    if ( !list0.empty() && !list1.empty() ) {
      for ( auto id: list0 ) {
	mOidListArray[id].push_back(oid1);
      }
      for ( auto id: list1 ) {
	mOidListArray[id].push_back(oid0);
      }
    }
  }

  for ( auto id: Range(mNodeNum) ) {
    vector<int>& list = mOidListArray[id];
    sort(list.begin(), list.end());
  }
}

// @brief node1 の衝突集合が node2 の衝突集合に含まれていたら true を返す．
bool
MpColGraph::containment_check(int node1,
			      int node2) const
{
  // まず mOidListArray[node1] と mOidListArray[node2] を比較する．
  // 共通に含まれる oid は削除する．
  const vector<int>& src_list1 = mOidListArray[node1];
  const vector<int>& src_list2 = mOidListArray[node2];
  vector<int> tmp_list1; tmp_list1.reserve(src_list1.size());
  vector<int> tmp_list2; tmp_lsit2.reserve(src_list2.size());
  int rpos1 = 0;
  int rpos2 = 0;
  int n1 = src_list1.size();
  int n2 = src_list2.size();
  while ( rpos1 < n1 && rpos2 < n2 ) {
    int oid1 = src_list1[rpos1];
    int oid2 = src_list2[rpos2];
    if ( oid1 < oid2 ) {
      tmp_list1.push_back(oid1);
      ++ rpos1;
    }
    else if ( oid1 > oid2 ) {
      tmp_list2.push_back(oid2);
      ++ rpos2;
    }
    else {
      ++ rpos1;
      ++ rpos2;
    }
  }
  for ( ; rpos1 < n1; ++ rpos1 ) {
    int oid1 = src_list1[rpos1];
    tmp_list1.push_back(oid1);
  }
  for ( ; rpos2 < n2; ++ rpos2 ) {
    int oid2 = src_list2[rpos2];
    tmp_lsit2.push_back(oid2);
  }

  // tmp_list1 に含まれる
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

END_NAMESPACE_YM_SATPG
