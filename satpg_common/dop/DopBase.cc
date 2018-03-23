
/// @file DopBase.cc
/// @brief DopBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "DopBase.h"
#include "TpgFaultMgr.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief 'base' タイプを生成する．
// @param[in] fmgr FaultMgr
DetectOp*
new_DopBase(TpgFaultMgr& fmgr)
{
  return new DopBase(fmgr);
}


//////////////////////////////////////////////////////////////////////
// クラス DopBase
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] fmgr FaultMgr
DopBase::DopBase(TpgFaultMgr& fmgr) :
  mMgr(fmgr)
{
}

// @brief デストラクタ
DopBase::~DopBase()
{
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] assign_list 値割当のリスト
void
DopBase::operator()(const TpgFault* f,
		    const NodeValList& assign_list)

{
  mMgr.set_status(f, FaultStatus::Detected);
}

// @brief テストパタンが見つかった時の処理
// @param[in] f 故障
// @param[in] tv テストベクタ
void
DopBase::operator()(const TpgFault* f,
		    const TestVector* tv)
{
  mMgr.set_status(f, FaultStatus::Detected);
}

END_NAMESPACE_YM_SATPG
