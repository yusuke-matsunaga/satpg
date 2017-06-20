
/// @file Rtpg1Cmd.cc
/// @brief Rtpg1Cmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "Rtpg1Cmd.h"
#include "TpgFaultMgr.h"
#include "td/Rtpg.h"
#include "td/RtpgStats.h"
#include "Fsim.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "ym/TclPopt.h"
#include "ym/RandGen.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// 故障シミュレーションを実行するコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Rtpg1Cmd::Rtpg1Cmd(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  mPoptNpat = new TclPoptInt(this, "npat",
			     "specify number of patterns to be simulated");
  mPoptMin = new TclPoptInt(this, "min_fault",
			    "specify the minimum limit of detected faults at a time");
  mPoptMax = new TclPoptInt(this, "max_interval",
			    "specify the maximum limit of non-detecting interval");
  mPoptSeed = new TclPoptUint(this, "seed",
			      "sepcify the random seed");
  mPoptWsa = new TclPoptDouble(this, "wsa_limit",
			       "sepcify the WSA limit (double)");
  mPoptMcmc = new TclPoptUint(this, "mcmc",
			      "MCMC mode");
  mPoptFile = new TclPoptStr(this, "file",
			     "specify the file name containg patterns");
  mPoptPrintStats = new TclPopt(this, "print_stats",
				"print statistics");
}

// @brief デストラクタ
Rtpg1Cmd::~Rtpg1Cmd()
{
}

// コマンド処理関数
int
Rtpg1Cmd::cmd_proc(TclObjVector& objv)
{
  ymuint objc = objv.size();
  if ( objc != 1 ) {
    print_usage();
    return TCL_ERROR;
  }

  nsTd::Rtpg* rtpg = nullptr;
  if ( mPoptMcmc->is_specified() ) {
    ymuint nbits = mPoptMcmc->val();
    rtpg = nsTd::new_RtpgP2(nbits);
  }
  else {
    rtpg = nsTd::new_RtpgP1();
  }

  bool n_flag = false;
  ymuint max_pat = 100000;
  ymuint max_i = 4;
  ymuint min_f = 0;
  ymuint wsa_limit = 0;

  if ( mPoptNpat->is_specified() ) {
    max_pat = mPoptNpat->val();
    n_flag = true;
  }

  if ( mPoptMin->is_specified() ) {
    min_f = mPoptMin->val();
  }

  if ( mPoptMax->is_specified() ) {
    max_i = mPoptMax->val();
  }

  if ( mPoptSeed->is_specified() ) {
    rtpg->init(mPoptSeed->val());
  }

  if ( mPoptWsa->is_specified() ) {
    double wsa_ratio = mPoptWsa->val();
    if ( wsa_ratio <= 0.0 ) {
      // エラーだけど今は無視
      wsa_ratio = 0.0;
    }
    // 平均の WSA を求める．
    RandGen randgen;
    ymuint n_count = 10000;
    Fsim& fsim = _fsim2();
    TvMgr& tvmgr = _tv_mgr();
    TestVector* tv = tvmgr.new_td_vector();
    ymuint wsa_sum = 0;
    ymuint wsa_max = 0;
    ymuint wsa_min = 0;
    for (ymuint i = 0; i < n_count; ++ i) {
      tv->set_from_random(randgen);
      //ymuint wsa1 = fsim.calc_wsa(tv);
      ymuint wsa1 = 0;
      wsa_sum += wsa1;
      if ( wsa_max < wsa1 ) {
	wsa_max = wsa1;
      }
      if ( wsa_min == 0 || wsa_min > wsa1 ) {
	wsa_min = wsa1;
      }
    }
    double wsa_ave = static_cast<double>(wsa_sum) / static_cast<double>(n_count);
    wsa_limit = static_cast<ymuint>(wsa_ave * wsa_ratio);
    cout << "WSA ave.  = " << wsa_ave << endl
	 << "WSA min   = " << wsa_min << endl
	 << "WSA max   = " << wsa_max << endl
	 << "WSA limit = " << wsa_limit << endl;
  }

  if ( mPoptFile->is_specified() ) {
    // 未実装
  }

  bool print_stats = mPoptPrintStats->is_specified();

  if ( n_flag ) {
    min_f = 0;
    max_i = 0;
  }

  TpgFaultMgr& fmgr = _fault_mgr();
  Fsim& fsim = _fsim2();
  TvMgr& tvmgr = _tv_mgr();

  vector<const TestVector*>& tv_list = _td_tv_list();
  nsTd::RtpgStats stats;

  rtpg->run(fmgr, tvmgr, fsim, min_f, max_i, max_pat, wsa_limit, tv_list, stats);

  after_update_faults();

  delete rtpg;

  if ( print_stats ) {
    cout << "********** rtpg **********" << endl
	 << setw(10) << stats.detected_faults()
	 << ": # of detected faults" << endl
	 << setw(10) << stats.simulated_patterns()
	 << ": # of simulated patterns" << endl
	 << setw(10) << stats.effective_patterns()
	 << ": # of effective patterns" << endl
	 << stats.time() << endl;
  }

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
