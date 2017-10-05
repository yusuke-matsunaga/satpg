
/// @file Dtpg2.cc
/// @brief Dtpg2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Dtpg2.h"

#include "Dtpg2Impl.h"
#include "TpgFaultMgr.h"
#include "TvMgr.h"
#include "TestVector.h"
#include "Fsim.h"
#include "DopList.h"
#include "UntestOp.h"

#include "TpgMFFC.h"
#include "TpgFFR.h"
#include "TpgFault.h"


BEGIN_NAMESPACE_YM_SATPG

// @brief コンストラクタ
// @param[in] sat_type SATソルバの種類を表す文字列
// @param[in] sat_option SATソルバに渡すオプション文字列
// @param[in] sat_outp SATソルバ用の出力ストリーム
// @param[in] bt バックトレーサー
Dtpg2::Dtpg2(const string& sat_type,
	     const string& sat_option,
	     ostream* sat_outp,
	     BackTracer& bt) :
  mSatType(sat_type),
  mSatOption(sat_option),
  mSatOutP(sat_outp),
  mBackTracer(bt)
{
}

// @brief デストラクタ
Dtpg2::~Dtpg2()
{
}

// @brief テスト生成を行う．
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] fmgr 故障マネージャ
// @param[in] fsim 故障シミュレーター
// @param[in] network 対象のネットワーク
// @param[in] dop 故障検出時に実行されるファンクター
// @param[in] uop 故障が冗長故障の時に実行されるファンクター
// @param[inout] stats DTPGの統計情報
void
Dtpg2::run(TvMgr& tvmgr,
	   TpgFaultMgr& fmgr,
	   Fsim& fsim,
	   const TpgNetwork& network,
	   bool use_xorsampling,
	   bool use_rtpg,
	   double wsa_ratio,
	   DetectOp& dop,
	   UntestOp& uop,
	   DtpgStats& stats)
{
  ymuint wsa_limit = 0;
  { // 順序回路としてランダムに動かした時の
    // 平均の信号遷移回数を得る．
    ymuint count = 10000;
    ymuint warmup = 100;
    bool weighted = false;

    InputVector* i_vect = tvmgr.new_input_vector();
    DffVector* f_vect = tvmgr.new_dff_vector();
    double total_wsa = 0.0;
    RandGen rg;

    // 初期状態
    i_vect->set_from_random(rg);
    f_vect->set_from_random(rg);
    fsim.set_state(*i_vect, *f_vect);

    for (ymuint i = 0; i < warmup; ++ i) {
      // このシミュレーション結果は捨てる．
      // 状態を遷移させることが目的
      i_vect->set_from_random(rg);
      fsim.calc_wsa(*i_vect, weighted);
    }
    for (ymuint i = 0; i < count; ++ i) {
      i_vect->set_from_random(rg);
      ymuint wsa1 = fsim.calc_wsa(*i_vect, weighted);
      total_wsa += wsa1;
    }
    double ave_wsa = total_wsa / count;

    tvmgr.delete_vector(i_vect);
    tvmgr.delete_vector(f_vect);

    wsa_limit = static_cast<ymuint>(ave_wsa * wsa_ratio);
  }

#if 0
  make_input_constraint(tvmgr, fsim, wsa_limit);
#endif

  mPatNum = 0;
  mExceedNum = 0;
  mTotalCount = 0;
  mTotalFound = 0;
  mTotalSampling = 0;
  mTotalOver = 0;
  mFinalExceedNum = 0;

  if ( use_rtpg ) {
    rtpg(tvmgr, fmgr, fsim, wsa_limit, dop);
  }

  TestVector* tv = tvmgr.new_vector();
  ymuint nf = network.rep_fault_num();
  for (ymuint i = 0; i < nf; ++ i) {
    const TpgFault* fault = network.rep_fault(i);
    if ( fmgr.status(fault) == kFsUndetected ) {
      SatBool3 stat = dtpg(tvmgr, fsim, network, fault, use_xorsampling, wsa_limit, tv, stats);
      if ( stat == kB3True ) {
	++ mPatNum;
	dop(fault, tv);
      }
      else if ( stat == kB3False ) {
	uop(fault);
      }
    }
  }
  {
    double ave_loop = static_cast<double>(mTotalCount) / static_cast<double>(mExceedNum);
    double ave_hit = static_cast<double>(mTotalFound) / static_cast<double>(mTotalSampling);
    double ave_sample = static_cast<double>(mTotalOver) / static_cast<double>(mExceedNum);
    cout << "Total Detected Faults: " << mPatNum << endl
	 << "WSA limit:             " << wsa_limit << endl
	 << "Initial Exceed Pats:   " << mExceedNum << endl
	 << "Final Exceed Pats:     " << mFinalExceedNum << endl
	 << "Ave. loop counts:      " << ave_loop << endl
	 << "Ave. hit counts:       " << ave_hit << endl
	 << "Ave. samples:          " << ave_sample << endl;
  }
}

// @brief テスト生成を行なう．
// @param[in] network 対象のネットワーク
// @param[in] tvmgr テストベクタのマネージャ
// @param[in] fault 対象の故障
// @param[out] nodeval_list テストパタンの値割り当てを格納するリスト
// @param[inout] stats DTPGの統計情報
// @return 結果を返す．
//
// 直前にどちらのモードでCNFを作っていたかで動作は異なる．<br>
// どちらの関数も呼んでいなければなにもしないで kB3X を返す．
SatBool3
Dtpg2::dtpg(TvMgr& tvmgr,
	    Fsim& fsim,
	    const TpgNetwork& network,
	    const TpgFault* fault,
	    bool use_xorsampling,
	    ymuint wsa_limit,
	    TestVector* tv,
	    DtpgStats& stats)
{
  Dtpg2Impl impl(mSatType, mSatOption, mSatOutP, mBackTracer, network, fault->ffr()->root());
  impl.gen_cnf(stats);

  NodeValList nodeval_list;
  SatBool3 ans = impl.dtpg(fault, nodeval_list, stats);
  if ( ans != kB3True ) {
    return ans;
  }

  ymuint wsa = optimize(tvmgr, fsim, wsa_limit, nodeval_list, tv);
  if ( wsa <= wsa_limit ) {
    return kB3True;
  }

  ++ mExceedNum;

  if ( !use_xorsampling ) {
    return kB3True;
  }

  // 今の故障に関係のある PPI の数を数える．
  ymuint xor_num = impl.make_xor_list();
  if ( xor_num > 30 ) {
    xor_num -= 20;
  }
  else if ( xor_num > 20 ) {
    xor_num -= 10;
  }
  if ( xor_num > 5 ) {
    xor_num = 5;
  }
  // とりあえず xor_num ぐらいの制約をつける．

  ymuint count_limit = 50;
  ymuint fcount_limit = 20;
  ymuint count = 0;
  ymuint fcount = 0;
  bool exit = false;
  TestVector* tv1 = tvmgr.new_vector();
  ymuint wsa_min = UINT_MAX;
  for ( ; count < count_limit; ++ count) {
    ++ mTotalCount;
    Dtpg2Impl impl2(mSatType, mSatOption, mSatOutP, mBackTracer, network, fault->ffr()->root());
    impl2.gen_cnf(stats);
    impl2.make_xor_list();
    impl2.add_xor_constraint(xor_num, mRandGen);

    ymuint xn_exp = 1U << xor_num;
    for (ymuint p = 0U; p < xn_exp; ++ p) {
      ++ mTotalSampling;
      NodeValList nodeval_list1;
      SatBool3 ans = impl2.dtpg_with_xor(fault, p, nodeval_list1, stats);
      if ( ans != kB3True ) {
	continue;
      }

      ++ mTotalFound;

      wsa = optimize(tvmgr, fsim, wsa_limit, nodeval_list1, tv);
      if ( wsa <= wsa_limit ) {
	exit = true;
	break;
      }
      if ( wsa_min > wsa ) {
	wsa_min = wsa;
	tv1->copy(*tv);
      }

      ++ mTotalOver;

      ++ fcount;
      if ( fcount > fcount_limit ) {
	exit = true;
	break;
      }
    }
    if ( exit ) {
      break;
    }
  }

  if ( wsa > wsa_limit ) {
    ++ mFinalExceedNum;
    tv->copy(*tv1);
  }

  return kB3True;
}

ymuint
Dtpg2::optimize(TvMgr& tvmgr,
		Fsim& fsim,
		ymuint wsa_limit,
		const NodeValList& nodeval_list,
		TestVector* tv)
{
  ymuint ni = tvmgr.input_num();
  ymuint nd = tvmgr.dff_num();
  ymuint nall = ni + ni + nd;
  ymuint n = nodeval_list.size();
  ymuint nx = nall - n;
  vector<ymuint> x_list;
  { // nodeval_list に現れない入力の番号を x_list に入れる．
    vector<bool> i0_map(ni, true);
    vector<bool> i1_map(ni, true);
    vector<bool> d0_map(nd, true);
    for (ymuint i = 0; i < n; ++ i) {
      NodeVal nv = nodeval_list[i];
      const TpgNode* node = nv.node();
      if ( node->is_primary_input() ) {
	ymuint id = node->input_id();
	if ( nv.time() == 1 ) {
	  i1_map[id] = false;
	}
	else {
	  i0_map[id] = false;
	}
      }
      else if ( node->is_dff_output() ) {
	ASSERT_COND( nv.time() == 0 );
	ymuint id = node->dff()->id();
	d0_map[id] = false;
      }
      else {
	ASSERT_NOT_REACHED;
      }
    }
    x_list.reserve(nx);
    for (ymuint i = 0; i < ni; ++ i) {
      if ( i0_map[i] ) {
	x_list.push_back(i);
      }
    }
    for (ymuint i = 0; i < nd; ++ i) {
      if ( d0_map[i] ) {
	x_list.push_back(i + ni);
      }
    }
    for (ymuint i = 0; i < ni; ++ i) {
      if ( i1_map[i] ) {
	x_list.push_back(i + ni + nd);
      }
    }
    ASSERT_COND( x_list.size() == nx );
  }

  ymuint count_limit = nx * 1;

  tv->init();
  tv->set_from_assign_list(nodeval_list);
  tv->fix_x_from_random(mRandGen2);
  ymuint wsa = fsim.calc_wsa(tv, false);
  TestVector* tv1 = tvmgr.new_vector();
  //cout << "  wsa = " << wsa << endl;
  for (ymuint count = 0; wsa > wsa_limit && count < count_limit; ++ count) {
    // tv の一つのビットをランダムに反転させる．
    tv1->copy(*tv);
    ymuint pos = mRandGen2.int32() % nx;
    ymuint id = x_list[pos];
    if ( id < ni ) {
      // 0時刻の入力
      Val3 val = tv1->input_val(id);
      tv1->set_input_val(id, ~val);
    }
    else if ( id < (ni + nd) ) {
      // 0時刻のフリップフロップ
      id -= ni;
      Val3 val = tv1->dff_val(id);
      tv1->set_dff_val(id, ~val);
    }
    else {
      // 1時刻の入力
      id -= (ni + nd);
      Val3 val = tv1->aux_input_val(id);
      tv1->set_aux_input_val(id, ~val);
    }
    ymuint wsa1 = fsim.calc_wsa(tv1, false);
    //cout << "\r    wsa1 = " << wsa1 << endl;
    if ( wsa1 <= wsa ) {
      tv->copy(*tv1);
      wsa = wsa1;
      if ( wsa1 <= wsa_limit ) {
	break;
      }
    }
    else {
      double r = (wsa - wsa1) / static_cast<double>(wsa);
      double p = exp(r);
      double th = mRandGen2.real1();
      if ( p < th ) {
	tv->copy(*tv1);
      }
    }
  }
  //cout << endl;
  tvmgr.delete_vector(tv1);
  return wsa;
}

void
Dtpg2::rtpg(TvMgr& tvmgr,
	    TpgFaultMgr& fmgr,
	    Fsim& fsim,
	    ymuint wsa_limit,
	    DetectOp& dop)
{
  ymuint max_pat = 10000;
  ymuint min_f = 0;
  ymuint max_i = 4;

  ymuint fnum = 0;
  ymuint undet_i = 0;
  ymuint epat_num = 0;
  ymuint total_det_count = 0;

  fsim.set_skip_all();
  for (ymuint i = 0; i < fmgr.max_fault_id(); ++ i) {
    const TpgFault* f = fmgr.fault(i);
    if ( f != nullptr && fmgr.status(f) == kFsUndetected ) {
      fsim.clear_skip(f);
      ++ fnum;
    }
  }

  TestVector* tv_array[kPvBitLen];
  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    tv_array[i] = tvmgr.new_vector();
  }

  fsim.clear_patterns();
  ymuint pat_num = 0;
  ymuint wpos = 0;
  for ( ; ; ) {
    if ( pat_num < max_pat ) {
      TestVector* tv = tv_array[wpos];
      for ( ; ; ) {
	tv->set_from_random(mRandGen);
	ymuint wsa = fsim.calc_wsa(tv, false);
	if ( wsa <= wsa_limit ) {
	  break;
	}
      }
      fsim.set_pattern(wpos, tv);
      ++ pat_num;
      ++ wpos;
      if ( wpos < kPvBitLen ) {
	continue;
      }
    }
    else if ( wpos == 0 ) {
      break;
    }

    ymuint det_count = fsim.ppsfp();

    const TpgFault* det_flags[kPvBitLen];
    for (ymuint i = 0; i < kPvBitLen; ++ i) {
      det_flags[i] = nullptr;
    }
    ymuint num = wpos;
    for (ymuint i = 0; i < det_count; ++ i) {
      const TpgFault* f = fsim.det_fault(i);
      fmgr.set_status(f, kFsDetected);
      fsim.set_skip(f);
      PackedVal dpat = fsim.det_fault_pat(i);
      // dpat の最初の1のビットを調べる．
      ymuint first = 0;
      for ( ; first < num; ++ first) {
	if ( dpat & (1ULL << first) ) {
	  break;
	}
      }
      ASSERT_COND( first < num );
      det_flags[first] = f;
    }
    for (ymuint i = 0; i < num; ++ i) {
      const TpgFault* f = det_flags[i];
      if ( f != nullptr ) {
	// 検出できたパタンは tvlist に移す．
	const TestVector* tv = tv_array[i];
	dop(f, tv);
	++ epat_num;
      }
    }
    fsim.clear_patterns();
    wpos = 0;

    total_det_count += det_count;

    if ( total_det_count == fnum ) {
      // すべての故障を検出した．
      break;
    }
    if ( det_count < min_f ) {
      // 検出故障数の下限を下回った
      break;
    }
    if ( det_count > 0 ) {
      undet_i = 0;
    }
    else {
      ++ undet_i;
      if ( undet_i > max_i ) {
	// 未検出の回数が max_i を越えた．
	break;
      }
    }
  }

  for (ymuint i = 0; i < kPvBitLen; ++ i) {
    tvmgr.delete_vector(tv_array[i]);
  }
}

Val3
read_val(const TestVector* tv,
	 ymuint pos)
{
  ymuint ni = tv->input_num();
  ymuint nd = tv->dff_num();
  if ( pos < ni ) {
    return tv->input_val(pos);
  }
  pos -= ni;
  if ( pos < nd ) {
    return tv->dff_val(pos);
  }
  pos -= nd;
  return tv->aux_input_val(pos);
}

void
Dtpg2::make_input_constraint(TvMgr& tvmgr,
			     Fsim& fsim,
			     ymuint wsa_limit)
{
  ymuint count_limit = 10000;
  ymuint warm_up = 10;
  RandGen rg;
  vector<TestVector*> on_list;
  vector<TestVector*> off_list;
  for (ymuint i = 0; i < count_limit; ++ i) {
    TestVector* tv = tvmgr.new_vector();
    tv->set_from_random(rg);
    ymuint wsa1 = fsim.calc_wsa(tv, false);
    if ( wsa1 <= wsa_limit ) {
      on_list.push_back(tv);
    }
    else {
      off_list.push_back(tv);
    }
  }
  InputVector* i_vect = tvmgr.new_input_vector();
  DffVector* d_vect = tvmgr.new_dff_vector();
  InputVector* a_vect = tvmgr.new_input_vector();
  for (ymuint i = 0; i < count_limit; ++ i) {
    i_vect->set_from_random(rg);
    d_vect->set_from_random(rg);
    fsim.set_state(*i_vect, *d_vect);
    for (ymuint j = 0; j < warm_up; ++ j) {
      i_vect->set_from_random(rg);
      fsim.calc_wsa(*i_vect, false);
    }
    fsim.get_state(*i_vect, *d_vect);
    a_vect->set_from_random(rg);
    ymuint wsa1 = fsim.calc_wsa(*i_vect, false);
    TestVector* tv = tvmgr.new_vector();
    tv->set(*i_vect, *d_vect, *a_vect);
    if ( wsa1 <= wsa_limit ) {
      on_list.push_back(tv);
    }
    else {
      off_list.push_back(tv);
    }
  }
  tvmgr.delete_vector(i_vect);
  tvmgr.delete_vector(d_vect);
  tvmgr.delete_vector(a_vect);

  {
    cout << "wsa_limit = " << wsa_limit << endl
	 << " ON patterns:  " << on_list.size() << endl
	 << " OFF patterns: " << off_list.size() << endl;
  }

  ymuint ni = tvmgr.input_num();
  ymuint nd = tvmgr.dff_num();
  ymuint nall = ni + ni + nd;
  for (ymuint i1 = 0; i1 < nall; ++ i1) {
    for (ymuint i2 = i1 + 1; i2 < nall; ++ i2) {
      for (ymuint i3 = i2 + 1; i3 < nall; ++ i3) {
	ymuint n000 = 0;
	ymuint n001 = 0;
	ymuint n010 = 0;
	ymuint n011 = 0;
	ymuint n100 = 0;
	ymuint n101 = 0;
	ymuint n110 = 0;
	ymuint n111 = 0;
	for (ymuint i = 0; i < on_list.size(); ++ i) {
	  TestVector* tv = on_list[i];
	  Val3 v1 = read_val(tv, i1);
	  Val3 v2 = read_val(tv, i2);
	  Val3 v3 = read_val(tv, i3);
	  if ( v1 == kVal0 ) {
	    if ( v2 == kVal0 ) {
	      if ( v3 == kVal0 ) {
		++ n000;
	      }
	      else {
		++ n001;
	      }
	    }
	    else {
	      if ( v3 == kVal0 ) {
		++ n010;
	      }
	      else {
		++ n011;
	      }
	    }
	  }
	  else {
	    if ( v2 == kVal0 ) {
	      if ( v3 == kVal0 ) {
		++ n100;
	      }
	      else {
		++ n101;
	      }
	    }
	    else {
	      if ( v3 == kVal0 ) {
		++ n110;
	      }
	      else {
		++ n111;
	      }
	    }
	  }
	}
	if ( n000 == 0 ) {
	  cout << " ~" << i1 << ": ~" << i2 << ": ~" << i3 << endl;
	}
	if ( n001 == 0 ) {
	  cout << " ~" << i1 << ": ~" << i2 << ":  " << i3 << endl;
	}
	if ( n010 == 0 ) {
	  cout << " ~" << i1 << ":  " << i2 << ": ~" << i3 << endl;
	}
	if ( n011 == 0 ) {
	  cout << " ~" << i1 << ":  " << i2 << ":  " << i3 << endl;
	}
	if ( n100 == 0 ) {
	  cout << "  " << i1 << ": ~" << i2 << ": ~" << i3 << endl;
	}
	if ( n101 == 0 ) {
	  cout << "  " << i1 << ": ~" << i2 << ":  " << i3 << endl;
	}
	if ( n110 == 0 ) {
	  cout << "  " << i1 << ":  " << i2 << ": ~" << i3 << endl;
	}
	if ( n111 == 0 ) {
	  cout << "  " << i1 << ":  " << i2 << ":  " << i3 << endl;
	}
      }
    }
  }


  for (ymuint i = 0; i < on_list.size(); ++ i) {
    tvmgr.delete_vector(on_list[i]);
  }
  for (ymuint i = 0; i < off_list.size(); ++ i) {
    tvmgr.delete_vector(off_list[i]);
  }
}

END_NAMESPACE_YM_SATPG
