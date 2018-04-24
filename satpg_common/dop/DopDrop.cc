
/// @file DopDrop.cc
/// @brief DopDrop の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopDrop.h"
#include "FaultStatusMgr.h"
#include "Fsim.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'drop' タイプを生成する．
// @param[in] fsmgr 故障マネージャ
// @param[in] fsim 故障シミュレータ
DetectOp*
new_DopDrop(FaultStatusMgr& fsmgr,
	    Fsim& fsim)
{
  return new DopDrop(fsmgr, fsim);
}


//////////////////////////////////////////////////////////////////////
// クラス DopDrop
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fsmgr 故障の状態を管理するクラス
// @param[in] fsim 故障シミュレータ
DopDrop::DopDrop(FaultStatusMgr& fsmgr,
		 Fsim& fsim) :
  mFaultStatusMgr(fsmgr),
  mFsim(fsim)
{
}

// @brief デストラクタ
DopDrop::~DopDrop()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopDrop::operator()(const TpgFault* f,
		    const NodeValList& assign_list)
{
  int n = mFsim.sppfp(assign_list);
  common(n);
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストベクタ
void
DopDrop::operator()(const TpgFault* f,
		    const TestVector& tv)
{
  int n = mFsim.sppfp(tv);
  common(n);
}

// @brief 検出された故障に印をつける．
// @param[in] n 検出された故障数
void
DopDrop::common(int n)
{
  for ( auto f: mFsim.det_fault_list() ) {
    mFaultStatusMgr.set(f, FaultStatus::Detected);
    mFsim.set_skip(f);
  }
}

END_NAMESPACE_YM_SATPG
