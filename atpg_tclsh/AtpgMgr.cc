
/// @file AtpgMgr.cc
/// @brief AtpgMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012 Yusuke Matsunaga
/// All rights reserved.


#include "AtpgMgr.h"
#include "TpgNetwork.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"

#include "TvMgr.h"
#include "Fsim.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// AtpgMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
AtpgMgr::AtpgMgr() :
  mTimer(TM_SIZE, TM_MISC)
{
  mFsim2 = nullptr;
  mFsim3 = nullptr;
  mSaFaultMgr = nullptr;
  mSaTvMgr = nullptr;
  mTdFaultMgr = nullptr;
  mTdTvMgr = nullptr;
}

// @brief デストラクタ
AtpgMgr::~AtpgMgr()
{
  delete mFsim2;
  delete mFsim3;
  delete mSaFaultMgr;
  delete mSaTvMgr;
  delete mTdFaultMgr;
  delete mTdTvMgr;
}

// @brief ファイル読み込みに関わる時間を得る．
USTime
AtpgMgr::read_time() const
{
  return mTimer.time(TM_READ);
}

// @brief DTPG に関わる時間を得る．
USTime
AtpgMgr::dtpg_time() const
{
  return mTimer.time(TM_DTPG);
}

// @brief 故障シミュレーションに関わる時間を得る．
USTime
AtpgMgr::fsim_time() const
{
  return mTimer.time(TM_FSIM);
}

// @brief SAT に関わる時間を得る．
USTime
AtpgMgr::sat_time() const
{
  return mTimer.time(TM_SAT);
}

// @brief その他の時間を得る．
USTime
AtpgMgr::misc_time() const
{
  return mTimer.time(TM_MISC);
}

// @brief ネットワークをセットした後に呼ぶ関数
void
AtpgMgr::after_set_network()
{
  delete mFsim2;
  delete mFsim3;
  delete mSaFaultMgr;
  delete mSaTvMgr;
  delete mTdFaultMgr;
  delete mTdTvMgr;

  mSaTvList.clear();
  mTdTvList.clear();

  mFsim2 = Fsim::new_Fsim2(_network(), kFtStuckAt);
  mFsim3 = Fsim::new_Fsim3(_network(), kFtStuckAt);
  mSaFaultMgr = new TpgFaultMgr(_network());
  mSaTvMgr = new TvMgr(_network(), kFtStuckAt);
  mTdFaultMgr = new TpgFaultMgr(_network());
  mTdTvMgr = new TvMgr(_network(), kFtTransitionDelay);
}

END_NAMESPACE_YM_SATPG
