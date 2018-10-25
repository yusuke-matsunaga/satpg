
/// @file RandWsaCmd.cc
/// @brief RandWsaCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "RandWsaCmd.h"
#include "Fsim.h"
#include "TpgNetwork.h"
#include "TestVector.h"
#include "InputVector.h"
#include "DffVector.h"
#include "ym/RandGen.h"
#include "ym/TclPopt.h"


BEGIN_NAMESPACE_SATPG

// @brief コンストラクタ
RandWsaCmd::RandWsaCmd(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  mPoptFsm = new TclPopt(this, "fsm",
			 "FSM mode");

  mPoptBs = new TclPopt(this, "bs",
			"Broad Side mode");

  mPoptCount = new TclPoptInt(this, "count",
			      "Specifies the pattern counts to be simulated");

  mPoptWarmup = new TclPoptInt(this, "warmup",
			       "Specifies the 'warm-up' counts to be discarded");

  mPoptWeighted = new TclPopt(this, "weighted",
			      "Specifies 'weighted' mode for signal activity calculation");

  new_popt_group(mPoptFsm, mPoptBs);
}

// @brief デストラクタ
RandWsaCmd::~RandWsaCmd()
{
}

// コマンド処理関数
int
RandWsaCmd::cmd_proc(TclObjVector& objv)
{
  int objc = objv.size();
  if ( objc != 1 ) {
    print_usage();
    return TCL_ERROR;
  }

  bool fsm = mPoptFsm->is_specified();
  int count = 10000;
  int warmup = 0;
  bool weighted = mPoptWeighted->is_specified();

  if ( mPoptCount->is_specified() ) {
    count = mPoptCount->val();
  }
  if ( mPoptWarmup->is_specified() ) {
    warmup = mPoptWarmup->val();
  }

  int input_num = _network().input_num();
  int dff_num = _network().dff_num();
  InputVector iv(input_num);
  DffVector dv(dff_num);
  InputVector av(input_num);
  Fsim& fsim = _td_fsim2();

  double total_wsa = 0.0;
  RandGen rg;
  if ( fsm ) {
    // FSM モード
    iv.set_from_random(rg);
    dv.set_from_random(rg);
    fsim.set_state(iv, dv);

    for ( int i = 0; i < warmup; ++ i ) {
      // このシミュレーション結果は捨てる．
      // 状態を遷移させることが目的
      iv.set_from_random(rg);
      fsim.calc_wsa(iv, weighted);
    }
    for ( int i = 0; i < count; ++ i ) {
      iv.set_from_random(rg);
      int wsa1 = fsim.calc_wsa(iv, weighted);
      total_wsa += wsa1;
    }
  }
  else {
    // BS モード
    for ( int i = 0; i < count; ++ i ) {
      iv.set_from_random(rg);
      dv.set_from_random(rg);
      av.set_from_random(rg);
      fsim.set_state(iv, dv);
      int wsa1 = fsim.calc_wsa(av, weighted);
      total_wsa += wsa1;
    }
  }

  double ave_wsa = total_wsa / count;

  // 結果をセットする．
  set_result(ave_wsa);

  return 0;
}

END_NAMESPACE_SATPG
