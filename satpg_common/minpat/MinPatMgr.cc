
/// @file MinPatMgr.cc
/// @brief MinPatMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatMgr.h"
#include "Fsim.h"
#include "TestVector.h"
#include "TpgFault.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス MinPatMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatMgr::MinPatMgr()
{
}

// @brief デストラクタ
MinPatMgr::~MinPatMgr()
{
}

// @brief 初期化する．
// @param[in] fault_list 故障のリスト
// @param[in] tv_list テストパタンのリスト
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
void
MinPatMgr::init(const vector<const TpgFault*>& fault_list,
		const vector<TestVector>& tv_list,
		const TpgNetwork& network,
		FaultType fault_type)
{
  mFaultList = fault_list;
  mOrigTvList = tv_list;

  gen_covering_matrix(network, fault_type);

  gen_conflict_list();

}

// @brief 故障シミュレーションを行い被覆表を作る．
// @param[in] network 対象のネットワーク
// @param[in] fault_type 故障の種類
void
MinPatMgr::gen_covering_matrix(const TpgNetwork& network,
			       FaultType fault_type)
{
  Fsim fsim;
  fsim.init_fsim3(network, fault_type);

  mElemList.clear();
  int wpos = 0;
  fsim.clear_patterns();
  int tv_base = 0;
  for ( auto tv: mOrigTvList ) {
    fsim.set_pattern(wpos, tv);
    ++ wpos;
    if ( wpos == kPvBitLen ) {
      int ndet = fsim.ppsfp();
      for ( auto i: Range(ndet) ) {
	const TpgFault* fault = fsim.det_fault(i);
	PackedVal dbits = fsim.det_fault_pat(i);
	int fid = fault->id();
	for ( auto bit: Range(kPvBitLen) ) {
	  if ( dbits & (1UL << bit) ) {
	    int tvid = tv_base + bit;
	    // (fid, tvid) を記録
	    mElemList.push_back(make_pair(fid, tvid));
	  }
	}
      }
      fsim.clear_patterns();
      wpos = 0;
      tv_base += kPvBitLen;
    }
  }
  if ( wpos > 0 ) {
    int ndet = fsim.ppsfp();
    for ( auto i: Range(ndet) ) {
      const TpgFault* fault = fsim.det_fault(i);
      PackedVal dbits = fsim.det_fault_pat(i);
      int fid = fault->id();
      for ( auto bit: Range(kPvBitLen) ) {
	if ( dbits & (1UL << bit) ) {
	  int tvid = tv_base + bit;
	  // (fid, tvid) を記録
	  mElemList.push_back(make_pair(fid, tvid));
	}
      }
    }
  }
}

// @brief テストパタンの衝突リストを作る．
void
MinPatMgr::gen_conflict_list()
{
  if ( mOrigTvList.empty() ) {
    return;
  }

  int nv = mOrigTvList.size();
  int vs = mOrigTvList[0].vector_size();
  mConflictPairList.resize(vs * 2);
  mConflictListArray.resize(nv);
  for ( auto bit: Range(vs) ) {
    int tvid = 0;
    vector<int>& list0 = mConflictPairList[bit * 2 + 0];
    vector<int>& list1 = mConflictPairList[bit * 2 + 1];
    for ( auto tvid: Range(nv) ) {
      TestVector tv = mOrigTvList[tvid];
      Val3 val = tv.val(bit);
      if ( val == Val3::_0 ) {
	list0.push_back(tvid);
      }
      else if ( val == Val3::_1 ) {
	list1.push_back(tvid);
      }
    }
    if ( !list0.empty() && !list1.empty() ) {
      for ( auto tvid: list0 ) {
	mConflictListArray[tvid].push_back(bit * 2 + 1);
      }
      for ( auto tvid: list1 ) {
	mConflictListArray[tvid].push_back(bit * 2 + 0);
      }
    }
  }

  mConflictListArray2.resize(nv);
  for ( auto tvid: Range(nv) ) {
    cout << "TestVector#" << tvid << endl;
    vector<int>& conflict_list = mConflictListArray2[tvid];
    const vector<int>& idlist_list = mConflictListArray[tvid];
    vector<bool> mark(nv, false);
    for ( auto idlist: idlist_list ) {
      for ( auto id: mConflictPairList[idlist] ) {
	if ( !mark[id] ) {
	  mark[id] = true;
	  conflict_list.push_back(id);
	}
      }
    }
    cout << "  " << conflict_list.size() << endl;
  }
}

END_NAMESPACE_YM_SATPG
