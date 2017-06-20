
/// @file PrintFaultCmd.cc
/// @brief PrintFaultCmd の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "PrintFaultCmd.h"
#include "AtpgMgr.h"
#include "TpgFault.h"
#include "TpgFaultMgr.h"
#include "ym/TclPopt.h"


BEGIN_NAMESPACE_YM_SATPG

//////////////////////////////////////////////////////////////////////
// 情報を表示するコマンド
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
PrintFaultCmd::PrintFaultCmd(AtpgMgr* mgr) :
  AtpgCmd(mgr)
{
  mPoptType = new TclPoptStr(this, "type",
			     "spefify fault type "
			     "(detected, untestable, remain)");
  set_usage_string("?filename?");
}

// @brief デストラクタ
PrintFaultCmd::~PrintFaultCmd()
{
}

// コマンド処理関数
int
PrintFaultCmd::cmd_proc(TclObjVector& objv)
{
  // このコマンドはファイル名のみを引数に取る．
  // 引数がなければ標準出力に出す．
  ymuint objc = objv.size();
  if ( objc > 2 ) {
    print_usage();
    return TCL_ERROR;
  }

  // 出力先のストリームを開く
  ostream* osp = &cout;
  ofstream ofs;
  if ( objc == 2 ) {
    string filename = objv[1];
    if ( !open_ofile(ofs, filename) ) {
      // ファイルが開けなかった．
      return TCL_ERROR;
    }
    osp = &ofs;
  }

  // 参照を使いたいのでめんどくさいことをやっている．
  ostream& out = *osp;

  FaultStatus type = kFsUndetected;
  if ( mPoptType->is_specified() ) {
    string tmp = mPoptType->val();
    if ( tmp == "detected" ) {
      type = kFsDetected;
    }
    else if ( tmp == "untestable" ) {
      type = kFsUntestable;
    }
    else if ( tmp == "remain" ) {
      type = kFsUndetected;
    }
    else {
      print_usage();
    }
  }

  TpgFaultMgr& fmgr = _fault_mgr();

  const TpgNetwork& network = _network();
  ymuint n = network.rep_fault_num();
  for (ymuint i = 0; i < n; ++ i) {
    const TpgFault* f = network.rep_fault(i);
    if ( fmgr.status(f) == type ) {
      out << f->str() << endl;
    }
  }

  return TCL_OK;
}

END_NAMESPACE_YM_SATPG
