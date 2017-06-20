
/// @file MinPatCmd.cc
/// @brief MinPatCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "MinPatCmd.h"
#include "sa/MinPat.h"
#include "sa/MinPatStats.h"
#include "ym/TclPopt.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// 故障シミュレーションを実行するコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
MinPatCmd::MinPatCmd(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  mPoptSimple = new TclPopt(this, "simple",
			    "simple heuristic");
  mPoptSimple2 = new TclPopt(this, "simple2",
			     "simple2 heuristic");
  mPoptDsatur = new TclPopt(this, "dsatur",
			    "Dsatur heuristic");
  mPoptDsatur2 = new TclPopt(this, "dsatur2",
			     "Dsatur-2 heuristic");
  mPoptGroupDominance = new TclPopt(this, "group-dominance",
				    "use group dominance");
  mPoptFaultDominance = new TclPopt(this, "fault-dominance",
				    "use fault dominance");
  mPoptPrintStats = new TclPopt(this, "print_stats",
				"print statistics");
  mPoptVerbose = new TclPoptInt(this, "verbose",
				"<int>: specify verbose-level");
  mPoptExact = new TclPopt(this, "exact",
			   "exact fault grouping");
  mPoptCompaction = new TclPopt(this, "compaction",
				"do compaction");
  mPoptFastCompaction = new TclPopt(this, "fast-compaction",
				    "do fast compaction");
  mPoptMcCompaction = new TclPopt(this, "mc-compaction",
				  "do mincov compaction");
  mPoptThVal = new TclPoptInt(this, "thval",
			      "specify threshold value");
  mPoptRepFaults = new TclPopt(this, "rep-faults",
			       "get representative faults");
}

// @brief デストラクタ
MinPatCmd::~MinPatCmd()
{
}

// コマンド処理関数
int
MinPatCmd::cmd_proc(TclObjVector& objv)
{
  ymuint objc = objv.size();
  if ( objc != 1 ) {
    print_usage();
    return TCL_ERROR;
  }

  bool print_stats = mPoptPrintStats->is_specified();

  bool simple = mPoptSimple->is_specified();
  bool simple2 = mPoptSimple2->is_specified();
  bool dsatur = mPoptDsatur->is_specified();
  bool dsatur2 = mPoptDsatur2->is_specified();
  int verbose = mPoptVerbose->is_specified() ? mPoptVerbose->val() : 0;
  bool group_dominance = mPoptGroupDominance->is_specified();
  bool exact = mPoptExact->is_specified();
  bool compaction = mPoptCompaction->is_specified();
  bool fast_compaction = mPoptFastCompaction->is_specified();
  bool mc_compaction = mPoptMcCompaction->is_specified();
  bool has_thval = mPoptThVal->is_specified();
  ymuint thval = mPoptThVal->val();
  bool rep_faults = mPoptRepFaults->is_specified();

  nsSa::MinPat* minpat = nullptr;

  if ( simple ) {
    minpat = nsSa::new_MinPatSimple(group_dominance, rep_faults);
  }
  else if ( simple2 ) {
    minpat = nsSa::new_MinPatSimple2(group_dominance);
  }
  else if ( dsatur ) {
    minpat = nsSa::new_MinPatDsatur();
  }
  else if ( dsatur2 ) {
    minpat = nsSa::new_MinPatDsatur2();
  }
  else {
    minpat = nsSa::new_MinPat(group_dominance);
  }

  minpat->set_verbose(verbose);

  USTime time;
  minpat->run(_network(), _fault_mgr(), _tv_mgr(),  _fsim2(), _fsim3(),
	      exact, compaction, fast_compaction, mc_compaction, has_thval, thval,
	      _sa_tv_list(), time);

  delete minpat;

  if ( print_stats ) {
    cout << " # of patterns: "
	 << setw(10) << _sa_tv_list().size()
	 << ": " << time << endl;
  }

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
