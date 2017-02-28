#ifndef FSIMX_H
#define FSIMX_H

/// @file FsimX.h
/// @brief FsimX のヘッダファイル
///
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2012-2013, 2016, 2017 Yusuke Matsunaga
/// All rights reserved.


#include "Fsim.h"

#include "fsim_nsdef.h"
#include "PackedVal.h"
#include "EventQ.h"
#include "SimFault.h"
#include "TpgNode.h"


BEGIN_NAMESPACE_YM_SATPG_FSIM

class SimFFR;
class SimNode;

//////////////////////////////////////////////////////////////////////
/// @class FSIM_CLASSNAME FsimX.h "FsimX.h"
/// @brief 故障シミュレーションを行うモジュール
///
/// 実際のクラス名は Fsim2 か Fsim3 である．
//////////////////////////////////////////////////////////////////////
class FSIM_CLASSNAME :
  public Fsim
{
public:

  /// @brief コンストラクタ
  FSIM_CLASSNAME ();

  /// @brief デストラクタ
  virtual
  ~FSIM_CLASSNAME ();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ネットワークをセットする．
  /// @param[in] network ネットワーク
  ///
  /// 全ての故障のスキップマークはクリアされる．
  virtual
  void
  set_network(const TpgNetwork& network);

  /// @brief 全ての故障にスキップマークをつける．
  virtual
  void
  set_skip_all();

  /// @brief 故障にスキップマークをつける．
  /// @param[in] f 対象の故障
  virtual
  void
  set_skip(const TpgFault* f);

  /// @brief 全ての故障のスキップマークを消す．
  virtual
  void
  clear_skip_all();

  /// @brief 故障のスキップマークを消す．
  /// @param[in] f 対象の故障
  virtual
  void
  clear_skip(const TpgFault* f);


public:
  //////////////////////////////////////////////////////////////////////
  // 縮退故障用の故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  sa_spsfp(const TestVector* tv,
	   const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  sa_spsfp(const NodeValList& assign_list,
	   const TpgFault* f);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  sa_sppfp(const TestVector* tv);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  sa_sppfp(const NodeValList& assign_list);

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  ymuint
  sa_ppsfp();


public:
  //////////////////////////////////////////////////////////////////////
  // 遷移故障用の故障シミュレーションを行う関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  td_spsfp(const TestVector* tv,
	   const TpgFault* f);

  /// @brief SPSFP故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  virtual
  bool
  td_spsfp(const NodeValList& assign_list,
	   const TpgFault* f);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] tv テストベクタ
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  td_sppfp(const TestVector* tv);

  /// @brief ひとつのパタンで故障シミュレーションを行う．
  /// @param[in] assign_list 値の割当リスト
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．
  virtual
  ymuint
  td_sppfp(const NodeValList& assign_list);

  /// @brief 複数のパタンで故障シミュレーションを行う．
  /// @return 検出された故障数を返す．
  ///
  /// 検出された故障は det_fault() で取得する．<br>
  /// 最低1つのパタンが set_pattern() で設定されている必要がある．<br>
  virtual
  ymuint
  td_ppsfp();


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp のテストパタンを設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief ppsfp 用のパタンバッファをクリアする．
  virtual
  void
  clear_patterns();

  /// @brief ppsfp 用のパタンを設定する．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  /// @param[in] tv テストベクタ
  virtual
  void
  set_pattern(ymuint pos,
	      const TestVector* tv);

  /// @brief 設定した ppsfp 用のパタンを読み出す．
  /// @param[in] pos 位置番号 ( 0 <= pos < kPvBitLen )
  virtual
  const TestVector*
  get_pattern(ymuint pos);


public:
  //////////////////////////////////////////////////////////////////////
  // ppsfp の結果を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 直前の sppfp/ppsfp で検出された故障数を返す．
  virtual
  ymuint
  det_fault_num();

  /// @brief 直前の sppfp/ppsfp で検出された故障を返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  const TpgFault*
  det_fault(ymuint pos);

  /// @brief 直前の ppsfp で検出された故障の検出ビットパタンを返す．
  /// @param[in] pos 位置番号 ( 0 <= pos < det_fault_num() )
  virtual
  PackedVal
  det_fault_pat(ymuint pos);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 一つのパタンを全ビットに展開して設定する．
  /// @param[in] tv 設定するテストベクタ
  void
  _set_sp(const TestVector* tv);

  /// @brief 一つのパタンを全ビットに展開して設定する．
  /// @param[in] assign_list 設定する値の割り当てリスト
  void
  _set_sp(const NodeValList& assign_list);

  /// @brief 一つのパタンを全ビットに展開して設定する．
  /// @param[in] tv 設定するテストベクタ
  ///
  /// こちらは遷移故障の２時刻目用
  void
  _set_sp2(const TestVector* tv);

  /// @brief 一つのパタンを全ビットに展開して設定する．
  /// @param[in] assign_list 設定する値の割り当てリスト
  ///
  /// こちらは遷移故障の２時刻目用
  void
  _set_sp2(const NodeValList& assign_list);

  /// @brief 複数のパタンを設定する．
  void
  _set_pp();

  /// @brief SPSFP故障シミュレーションの本体
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _sa_spsfp(const TpgFault* f);

  /// @brief SPPFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  ymuint
  _sa_sppfp();

  /// @brief SPSFP故障シミュレーションの本体
  /// @param[in] f 対象の故障
  /// @retval true 故障の検出が行えた．
  /// @retval false 故障の検出が行えなかった．
  bool
  _td_spsfp(const TpgFault* f);

  /// @brief SPPFP故障シミュレーションの本体
  /// @return 検出された故障数を返す．
  ymuint
  _td_sppfp();

  /// @brief 正常値の計算を行う．
  ///
  /// 入力ノードに gval の設定は済んでいるものとする．
  void
  _calc_gval();

  /// @brief 1時刻前の正常値の計算を行う．
  ///
  /// 結果は mPrevValArray に格納される．
  /// と同時に現在の時刻のDFFの出力も設定される．
  void
  _calc_pval();

  /// @brief FFR の根から故障伝搬シミュレーションを行う．
  /// @param[in] root FFRの根のノード
  /// @return 伝搬したビットに1を立てたビットベクタ
  PackedVal
  _prop_sim(SimNode* root);

  /// @brief FFR内の故障シミュレーションを行う．
  /// @param[in] fault 対象の故障
  PackedVal
  _fault_prop(SimFault* fault);

  /// @brief FFR内の故障シミュレーションを行う．
  /// @param[in] fault_list 故障のリスト
  PackedVal
  _fault_prop(const vector<SimFault*>& fault_list);

  /// @brief 故障の活性化条件を求める．(縮退故障用)
  /// @param[in] fault 対象の故障
  PackedVal
  _sa_fault_act(SimFault* fault);

  /// @brief 故障の活性化条件を求める．(遷移故障用)
  /// @param[in] fault 対象の故障
  PackedVal
  _td_fault_act(SimFault* fault);

  /// @brief 故障をスキャンして結果をセットする(sppfp用)
  /// @param[in] fault_list 故障のリスト
  void
  _fault_sweep(const vector<SimFault*>& fault_list);

  /// @brief 故障をスキャンして結果をセットする(ppsfp用)
  /// @param[in] fault_list 故障のリスト
  /// @param[in] pat 検出パタン
  void
  _fault_sweep(const vector<SimFault*>& fault_list,
	       PackedVal pat);


private:
  //////////////////////////////////////////////////////////////////////
  // SimNode / SimFault の設定に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在保持している SimNode のネットワークを破棄する．
  /// 内部で clear_faults() を呼ぶ．
  void
  clear();

  /// @brief 外部入力ノードを作る．
  SimNode*
  make_input();

  /// @brief logic ノードを作る．
  SimNode*
  make_gate(GateType type,
	    const vector<SimNode*>& inputs);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  // ppsfp の結果を格納する構造体
  struct FaultPat
  {
    // 故障
    const TpgFault* mFault;
    // 検出したビットパタン
    PackedVal mPat;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 外部入力数
  ymuint mInputNum;

  // 外部出力数
  ymuint mOutputNum;

  // DFF数
  ymuint mDffNum;

  // 全ての SimNode を納めた配列
  vector<SimNode*> mNodeArray;

  // PPIに対応する SimNode を納めた配列
  // サイズは mInputNum + mDffNum
  vector<SimNode*> mPPIArray;

  // PPOに対応する SimNode を納めた配列
  // サイズは mOutputNum + mDffNum
  vector<SimNode*> mPPOArray;

  // 入力からのトポロジカル順に並べた logic ノードの配列
  vector<SimNode*> mLogicArray;

  // ブロードサイド方式用の１時刻前の値を保持する配列
  // サイズは mNodeArray.size()
  vector<FSIM_VALTYPE> mPrevValArray;

  // FFR を納めた配列
  vector<SimFFR> mFFRArray;

  // SimNode->id() をキーにして所属する FFR を納めた配列
  vector<SimFFR*> mFFRMap;

  // パタンの設定状況を表すビットベクタ
  PackedVal mPatMap;

  // パタンバッファ
  const TestVector* mPatBuff[kPvBitLen];

  // イベントキュー
  EventQ mEventQ;

  // 故障シミュレーション用の故障の配列
  vector<SimFault> mSimFaults;

  // TpgFault::id() をキーとして SimFault を格納する配列
  vector<SimFault*> mFaultArray;

  // 検出された故障を格納する配列
  // サイズは常に mSimFaults.size();
  vector<FaultPat> mDetFaultArray;

  // 検出された故障数
  ymuint mDetNum;

};

END_NAMESPACE_YM_SATPG_FSIM

#endif // FSIMX_H
