
/// @file RandWsaCmd.cc
/// @brief RandWsaCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "RandWsaCmd.h"
#include "TvMgr.h"
#include "Fsim.h"
#include "InputVector.h"
#include "DffVector.h"
#include "ym/RandGen.h"
#include "ym/TclPopt.h"


BEGIN_NAMESPACE_YM_SATPG

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

  TvMgr& tv_mgr = _td_tv_mgr();
  InputVector* i_vect = tv_mgr.new_input_vector();
  InputVector* a_vect = tv_mgr.new_input_vector();
  DffVector* f_vect = tv_mgr.new_dff_vector();
  Fsim& fsim = _td_fsim2();

  double total_wsa = 0.0;
  RandGen rg;
  if ( fsm ) {
    // FSM モード
    i_vect->set_from_random(rg);
    f_vect->set_from_random(rg);
    fsim.set_state(*i_vect, *f_vect);

    for (int i = 0; i < warmup; ++ i) {
      // このシミュレーション結果は捨てる．
      // 状態を遷移させることが目的
      i_vect->set_from_random(rg);
      fsim.calc_wsa(*i_vect, weighted);
    }
    for (int i = 0; i < count; ++ i) {
      i_vect->set_from_random(rg);
      int wsa1 = fsim.calc_wsa(*i_vect, weighted);
      total_wsa += wsa1;
    }
  }
  else {
    // BS モード
    for (int i = 0; i < count; ++ i) {
      i_vect->set_from_random(rg);
      f_vect->set_from_random(rg);
      a_vect->set_from_random(rg);
      fsim.set_state(*i_vect, *f_vect);
      int wsa1 = fsim.calc_wsa(*a_vect, weighted);
      total_wsa += wsa1;
    }
  }

  tv_mgr.delete_vector(i_vect);
  tv_mgr.delete_vector(f_vect);

  double ave_wsa = total_wsa / count;

  // 結果をセットする．
  set_result(ave_wsa);

  return 0;
}

END_NAMESPACE_YM_SATPG
