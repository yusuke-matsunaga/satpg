
/// @file TpgFFR.cc
/// @brief TpgFFR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// クラス TpgFFR
//////////////////////////////////////////////////////////////////////

// @brief 内容を設定する．
// @param[in] root 根のノード
// @param[in] fault_list 故障のリスト
// @param[in] alloc メモリアロケータ
void
TpgFFR::set(TpgNode* root,
	    vector<TpgFault*>& fault_list,
	    Alloc& alloc)
{
  mRoot = root;

  ymuint nf = fault_list.size();
  mFaultNum = nf;
  void* q = alloc.get_memory(sizeof(const TpgFault*) * nf);
  mFaultList = new (q) const TpgFault*[nf];

  for (ymuint i = 0; i < nf; ++ i) {
    TpgFault* fault = fault_list[i];
    mFaultList[i] = fault;
    fault->set_ffr(this);
  }
}

END_NAMESPACE_YM_SATPG
